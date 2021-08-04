// GPL v3 (c) 2021, Daniel Williams 

#include <cmath>

#include <fmt/core.h>

#include <gdal.h>

#include "frame-mapping.hpp"

using chartbox::frame::FrameMapping;
using chartbox::frame::Location2EN;
using chartbox::frame::Location2LL;
using chartbox::frame::Location2xy;

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


bool FrameMapping::move_to_corners( const BoundBox<Location2LL>& bounds ){
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
        fmt::print( stderr, "    ::from:    lat: {}    lon: {} \n", bounds.min.latitude, bounds.min.longitude );
        fmt::print( stderr, "    ::to:raw:    x: {}      y: {} \n", xs[0], ys[0] );

        xs[0] = std::floor(xs[0]);
        ys[0] = std::floor(ys[0]);
        
        fmt::print( stderr, "    :: floor-limited:  x: {}  y: {} \n", xs[0], ys[0] );
        
        const double raw_width = std::max( std::abs(xs[1]-xs[0]), std::abs(ys[1]-ys[0]) );
        
        if( max_local_width_ < raw_width ){
            // this is a hard error!  Refuse to load this bounds.
            fprintf( stderr, "!! Maximum chart dimension exceeded!  ( %f > %f ) Aborting! \n", raw_width, max_local_width_ );
            return false;
        }

        const double snap_width = snap_power_2( (raw_width<min_local_width_) ? min_local_width_ : raw_width );

        const Location2EN accept_min_xy( xs[0], ys[0] ); // this is just an alias
        const Location2EN accept_max_xy( xs[0] + snap_width, ys[0] + snap_width );

        double xs[] = { accept_max_xy.easting };
        double ys[] = { accept_max_xy.northing };
        // TODO: swap this out with `this->to_global(accept_max_xy)`? 
        if( ! utm_to_global_transform_->Transform( 1, xs, ys ) ){
            fmt::print( stderr, "<< !! Global <= Local transform failed !!\n" );
            return false;
        }else{
            global_bounds_.min = bounds.min;
            global_bounds_.max = Location2LL( xs[0], ys[0] );
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

Location2LL FrameMapping::to_global( const Location2EN& from ){
    double xs[] = { from.easting };
    double ys[] = { from.northing };
    if( utm_to_global_transform_->Transform( 1, xs, ys ) ){
        return Location2LL( xs[0], ys[0] );
    }
    return Location2LL( NAN,NAN );
}

Location2xy FrameMapping::to_local( const Location2LL& from ){
    // WGS-84 and other Latitude-Longitude Frames use a non-intuitive axis order
    // -- and this order gets the correct answers.
    double xs[] = { from.latitude };
    double ys[] = { from.longitude };
    if( global_to_utm_transform_->Transform( 1, xs, ys ) ){
        return Location2xy( xs[0] - utm_bounds_.min.easting, ys[0] - utm_bounds_.min.northing );
    }
    return Location2xy( NAN, NAN );
}

Location2EN FrameMapping::to_utm( const Location2LL& from ){
    // WGS-84 and other Latitude-Longitude Frames use a non-intuitive axis order
    // -- and this order gets the correct answers.
    double xs[] = { from.latitude };
    double ys[] = { from.longitude };
    if( global_to_utm_transform_->Transform( 1, xs, ys ) ){
        return Location2EN( xs[0], ys[0] );
    }
    return Location2EN( NAN, NAN );
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
