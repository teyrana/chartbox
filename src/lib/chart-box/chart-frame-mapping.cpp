// GPL v3 (c) 2021, Daniel Williams 

#include <cmath>

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>

#include <fmt/core.h>

#include <gdal.h>
#include <ogr_spatialref.h>

#include "chart-frame-mapping.hpp"


using chartbox::FrameMapping;

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

bool FrameMapping::move_local_bounds( const Eigen::Vector2d& min_lon_lat, const Eigen::Vector2d& max_lon_lat ){
    if( (nullptr== global_to_utm_transform_) || (nullptr==utm_to_global_transform_) ){
        fmt::print( stderr, "XXX null transformations.  aborting.\n");
        return false;
    }

    double xs[] = { min_lon_lat.y(), max_lon_lat.y() };
    double ys[] = { min_lon_lat.x(), max_lon_lat.x() };
    if( ! global_to_utm_transform_->Transform( 2, xs, ys ) ){
        fmt::print( stderr, "<< !! Transformations failed.\n" );
        return false;

    }else{
        xs[0] = std::floor(xs[0]);
        ys[0] = std::floor(ys[0]);
        const double raw_width = std::max( std::abs(xs[1]-xs[0]), std::abs(ys[1]-ys[0]) );
        if( max_local_width_ < raw_width ){
            // this is a hard error!  Refuse to load this bounds.
            fprintf( stderr, "!! Maximum chart dimension exceeded!  ( %f > %f ) Aborting! \n", raw_width, max_local_width_ );
            return false;
        }

        const double snap_width = snap_power_2( (raw_width<min_local_width_) ? min_local_width_ : raw_width );

        const Eigen::Vector2d accept_min_xy = Eigen::Vector2d( xs[0], ys[0] );; // this is just an alias
        const Eigen::Vector2d accept_max_xy( xs[0] + snap_width, ys[0] + snap_width );

        double xs[] = { accept_max_xy.x() };
        double ys[] = { accept_max_xy.y() };
        if( ! utm_to_global_transform_->Transform( 1, xs, ys ) ){
            fmt::print( stderr, "<< !! Global <= Local transform failed !!\n" );
            return false;
        }else{
            global_bounds_.min() = min_lon_lat;
            global_bounds_.max() = Eigen::Vector2d( ys[0], xs[0] );
            utm_bounds_.min() = accept_min_xy;
            utm_bounds_.max() = accept_max_xy;
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

OGRPoint* FrameMapping::to_utm( const double longitude, const double latitude ){
    // WGS-84 and other Latitude-Longitude Frames use a non-intuitive axis order
    // -- and this order gets the correct answers.
    double xs[] = { latitude };
    double ys[] = { longitude };

    if( global_to_utm_transform_->Transform( 1, xs, ys ) ){
        return new OGRPoint( xs[0] - utm_bounds_.min().x(), ys[0] - utm_bounds_.min().y() );
    }

    return nullptr;
}

Eigen::Vector2d FrameMapping::to_local( const Eigen::Vector2d& from_lat_lon ){
    // WGS-84 and other Latitude-Longitude Frames use a non-intuitive axis order
    // -- and this order gets the correct answers.
    double xs[] = { from_lat_lon.y() };
    double ys[] = { from_lat_lon.x() };
    if( global_to_utm_transform_->Transform( 1, xs, ys ) ){
        return Eigen::Vector2d( xs[0] - utm_bounds_.min().x(), ys[0] - utm_bounds_.min().y() );
    }

    return {NAN,NAN};
}

OGRPoint* FrameMapping::to_global( const double easting, const double northing ){
    double xs[] = { easting + utm_bounds_.min().x() };
    double ys[] = { northing + utm_bounds_.min().y() };

    if( utm_to_global_transform_->Transform( 1, xs, ys ) ){
        return new OGRPoint( xs[0], ys[0] );
    }

    return nullptr;
}

Eigen::Vector2d FrameMapping::to_global( const Eigen::Vector2d& from_local ){
    double xs[] = { from_local.x() + utm_bounds_.min().x() };
    double ys[] = { from_local.y() + utm_bounds_.min().y() };
    if( utm_to_global_transform_->Transform( 1, xs, ys ) ){
        return Eigen::Vector2d( xs[0], ys[0] );
    }
    return {NAN,NAN};
}

Eigen::Vector2d FrameMapping::to_utm( const Eigen::Vector2d& from_lat_lon ){
    // WGS-84 and other Latitude-Longitude Frames use a non-intuitive axis order
    // -- and this order gets the correct answers.
    double xs[] = { from_lat_lon.y() };
    double ys[] = { from_lat_lon.x() };
    if( global_to_utm_transform_->Transform( 1, xs, ys ) ){
        return Eigen::Vector2d( xs[0], ys[0] );
    }

    return {NAN,NAN};
}

void FrameMapping::print() const {
    fmt::print( "============ ============ Frame Mapping Information ============ ============ \n" );
    fmt::print( "::Bounding Box: (Global Frame) \n" );
    fmt::print( "      min:      {:12.6f} N Latitude,    {:12.6f} W Longitude\n", global_bounds_.min().y(), global_bounds_.min().x() );
    fmt::print( "      max:      {:12.6f} N Latitude,    {:12.6f} W Longitude\n", global_bounds_.max().y(), global_bounds_.max().x() );
    fmt::print( "::Bounding Box: (UTM Frame) \n" );
    fmt::print( "      min:      {:12.6f} Easting,    {:12.6f} Northing\n", utm_bounds_.min().x(), utm_bounds_.min().y() );
    fmt::print( "      max:      {:12.6f} Easting,    {:12.6f} Northing\n", utm_bounds_.max().x(), utm_bounds_.max().y() );
    fmt::print( "::Width:        {:8.1f} / {:8.1f} / {:8.1f} \n", min_local_width_, current_local_width_, max_local_width_ );
    fmt::print( "::Bounding Box: (Local Frame) \n" );
    fmt::print( "      min:      {:12.6f} Easting,    {:12.6f} Northing\n", 0.0f, 0.0f );
    fmt::print( "      max:      {:12.6f} Easting,    {:12.6f} Northing\n", current_local_width_, current_local_width_ );
    fmt::print( "============ ============ ============ ============ ============ ============ \n" );
}
