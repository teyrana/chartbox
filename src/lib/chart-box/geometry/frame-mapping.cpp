// GPL v3 (c) 2021, Daniel Williams 

#include <cmath>

#include <fmt/core.h>

#include <gdal.h>

#include "frame-mapping.hpp"

using chartbox::geometry::BoundBox;
using chartbox::geometry::FrameMapping;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::GlobalLocation;
using chartbox::geometry::Polygon;
using chartbox::geometry::UTMLocation;

FrameMapping::FrameMapping()
    : global_bounds_( Eigen::Vector2d(0,0), Eigen::Vector2d(1,1) )
    , global_frame_( nullptr )
    , global_to_utm_transform_(nullptr)
    , utm_to_global_transform_(nullptr)
    , utm_bounds_( Eigen::Vector2d(0,0), Eigen::Vector2d(32,32) )
    , utm_frame_( nullptr ) 
{

    if( 0 != setenv("PROJ_LIB", "/home/teyrana/.conan/data/proj/8.0.1/_/_/package/8041ed4ccd88c797ec193c798bee4643ca247118/res/", true) ){
        fmt::print( stderr, "!? Failed to set env variable: PROJ_LIB \n");
        return;
    }

    // Zone:  WGS 84
    // EPSG:4326
    // https://www.spatialreference.org/ref/epsg/4326/
    global_frame_.SetWellKnownGeogCS( "EPSG:4326" );

    // Hard code our local UTM zone, for now.
    // https://en.wikipedia.org/wiki/Universal_Transverse_Mercator_coordinate_system
    //
    // UTM Zone:          19
    // Start Longitude:  -72
    // Start Latitude:    0
    // Hemisphere:        N
    // Central Meridian: -69
    // EPSG:32619
    // https://www.spatialreference.org/ref/epsg/wgs-84-utm-zone-19n/
    utm_frame_.SetProjCS( "UTM 19 in northern hemisphere." );
    utm_frame_.SetWellKnownGeogCS( "WGS84" );
    utm_frame_.SetUTM(  19,      // zone number
                        true );  // bool( hemisphere=="north" )

    global_to_utm_transform_ = OGRCreateCoordinateTransformation( &global_frame_, &utm_frame_ );
    utm_to_global_transform_ = OGRCreateCoordinateTransformation( &utm_frame_,  &global_frame_ );
}

// BoundBox<GlobalLocation> FrameMapping::map_to_global( const BoundBox<UTMLocation>& from ) const { 
//     const GlobalLocation bounds_min_local = map_to_global( bounds_lat_lon.min );
//     const GlobalLocation bounds_max_local = map_to_global( bounds_lat_lon.max );
//     const BoundBox<GlobalLocation> bounds_local( bounds_min_local, bounds_max_local );
//     return std::move( bounds_local );
// }

BoundBox<LocalLocation> FrameMapping::local_bounds() const {
    return { LocalLocation(0,0), LocalLocation( utm_bounds_.width(), utm_bounds_.height()) };
}

GlobalLocation FrameMapping::map_to_global( const UTMLocation& from ) const {
    double xs[] = { from.easting };
    double ys[] = { from.northing };
    if( utm_to_global_transform_->Transform( 1, xs, ys ) ){
        return GlobalLocation( xs[0], ys[0] );
    }
    return GlobalLocation( NAN,NAN );
}

BoundBox<LocalLocation> FrameMapping::map_to_local( const BoundBox<GlobalLocation>& from ) const { 
    const LocalLocation min_local = map_to_local( from.min );
    const LocalLocation max_local = map_to_local( from.max );
    return { min_local, max_local };
}

LocalLocation FrameMapping::map_to_local( const GlobalLocation& from ) const {
    // WGS-84 and other Latitude-Longitude Frames use a non-intuitive axis order
    // -- and this order gets the correct answers.
    double xs[] = { from.latitude };
    double ys[] = { from.longitude };
    if( global_to_utm_transform_->Transform( 1, xs, ys ) ){
        return LocalLocation( xs[0] - utm_bounds_.min.easting, ys[0] - utm_bounds_.min.northing );
    }
    return {NAN, NAN};
}

Polygon<LocalLocation> FrameMapping::map_to_local( const Polygon<GlobalLocation>& global_polygon ) const { 
    Polygon<LocalLocation> local_polygon( global_polygon.size() );

    size_t index = 0;
    for( auto& each_global_point : global_polygon ){

        fmt::print( stderr, "                    >>> each-point: latitude:  {} < {}\n", each_global_point.latitude,  each_global_point.latitude );
        fmt::print( stderr, "                    >>> each-point: longitude: {} < {}\n", each_global_point.longitude, each_global_point.longitude );

        const LocalLocation each_local_point = map_to_local(each_global_point);
        fmt::print( stderr, "                    <<< each-point: easting:  {} < {}\n", each_local_point.easting, each_local_point.easting );
        fmt::print( stderr, "                    <<< each-point: northing: {} < {}\n", each_local_point.northing, each_local_point.northing );

        break;
        local_polygon.set( index, map_to_local(each_global_point) );
        ++index;
    }

    return local_polygon;
}

UTMLocation FrameMapping::map_to_utm( const GlobalLocation& from ) const {
    // WGS-84 and other Latitude-Longitude Frames use a non-intuitive axis order
    // -- and this order gets the correct answers.
    double xs[] = { from.latitude };
    double ys[] = { from.longitude };
    if( global_to_utm_transform_->Transform( 1, xs, ys ) ){
        return UTMLocation( xs[0], ys[0] );
    }
    return UTMLocation( NAN, NAN );
}

bool FrameMapping::move_to_corners( const BoundBox<GlobalLocation>& bounds ){
    if( (nullptr== global_to_utm_transform_) || (nullptr==utm_to_global_transform_) ){
        fmt::print( stderr, "XXX null transformations.  aborting.\n");
        return false;
    }

    double xs[] = { bounds.min.latitude, bounds.max.latitude };
    double ys[] = { bounds.min.longitude, bounds.max.longitude };
    if( ! global_to_utm_transform_->Transform( 2, xs, ys ) ){
        fmt::print( stderr, "<< !! Transformations failed.\n" );
        return false;

    }else{
        xs[0] = xs[0];
        ys[0] = ys[0];

        const double raw_width = std::max( std::abs(xs[1]-xs[0]), std::abs(ys[1]-ys[0]) );
        
        if( max_local_width_ < raw_width ){
            // this is a hard error!  Refuse to load this bounds.
            fprintf( stderr, "!! Maximum chart dimension exceeded!  ( %f > %f ) Aborting! \n", raw_width, max_local_width_ );
            return false;
        }

        const double snap_width = snap_power_2( (raw_width<min_local_width_) ? min_local_width_ : raw_width );

        const UTMLocation accept_min_xy( xs[0], ys[0] ); // this is just an alias
        const UTMLocation accept_max_xy( xs[0] + snap_width, ys[0] + snap_width );

        double xs[] = { accept_max_xy.easting };
        double ys[] = { accept_max_xy.northing };
        // TODO: swap this out with `this->to_global(accept_max_xy)`? 
        if( ! utm_to_global_transform_->Transform( 1, xs, ys ) ){
            fmt::print( stderr, "<< !! Global <= Local transform failed !!\n" );
            return false;
        }else{
            global_bounds_.min = bounds.min;
            global_bounds_.max = GlobalLocation( xs[0], ys[0] );
            utm_bounds_.min = accept_min_xy;
            utm_bounds_.max = accept_max_xy;
            current_local_width_ = snap_width;
            
            return true;
        }

        return false;
    }
}

constexpr size_t FrameMapping::snap_power_2(const size_t target){
    size_t threshold = 2;

    while( threshold < target ){
        threshold <<= 1;
    }

    return threshold;
}

void FrameMapping::print() const {
    fmt::print( "============ ============ Frame Mapping Information ============ ============ \n" );
    fmt::print( "::Bounding Box: (Global Frame) \n" );
    fmt::print( "      min:      {:12.6f} N Latitude,    {:12.6f} W Longitude\n", global_bounds_.min.latitude, global_bounds_.min.longitude );
    fmt::print( "      max:      {:12.6f} N Latitude,    {:12.6f} W Longitude\n", global_bounds_.max.latitude, global_bounds_.max.longitude );
    fmt::print( "::Bounding Box: (UTM Frame) \n" );
    fmt::print( "      min:      {:12.6f} Easting,    {:12.6f} Northing\n", utm_bounds_.min.easting, utm_bounds_.min.northing );
    fmt::print( "      max:      {:12.6f} Easting,    {:12.6f} Northing\n", utm_bounds_.max.easting, utm_bounds_.max.northing );
    fmt::print( "::Width:        {:8.1f} / {:8.1f} / {:8.1f} \n", min_local_width_, current_local_width_, max_local_width_ );
    fmt::print( "::Bounding Box: (Local Frame) \n" );
    fmt::print( "      min:      {:12.6f} Easting,    {:12.6f} Northing\n", 0.0f, 0.0f );
    fmt::print( "      max:      {:12.6f} Easting,    {:12.6f} Northing\n", current_local_width_, current_local_width_ );
    fmt::print( "============ ============ ============ ============ ============ ============ \n" );
}
