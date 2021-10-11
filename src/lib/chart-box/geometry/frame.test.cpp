// GPL v3 (c) 2021, Daniel Williams 

#include <cmath>
#include <cstddef>
#include <iostream>
#include <sstream>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
using Catch::Approx;

#include "bound-box.hpp"
#include "frame-mapping.hpp"
#include "utm-location.hpp"
#include "global-location.hpp"
#include "xy-location.hpp"

using namespace chartbox::geometry;

TEST_CASE( "FrameTests" ){

    SECTION( " UTMLocation Vectors Store Correctly" ) {
        const UTMLocation loc( 1.2, 3.4 );
        CHECK( loc.easting == Approx(1.2) );
        CHECK( loc.northing == Approx(3.4) );
        CHECK( loc[0] == Approx(1.2) );
        CHECK( loc[1] == Approx(3.4) );
    }

    SECTION( "UTMLocation BoundBox works correctly" ) {
        BoundBox<UTMLocation> box( {0,0}, {2.1, 3.5} );

        CHECK( box.max.easting == Approx(2.1) );
        CHECK( box.max.northing == Approx(3.5) );
        CHECK( box.min.easting == Approx(0.0) );
        CHECK( box.min.northing == Approx(0.0) );

        CHECK( box.height() == Approx(3.5) );
        CHECK( box.width() == Approx(2.1) );
    }

    SECTION( "GlobalLocation Vectors Store Correctly" ) {
        const GlobalLocation loc( 41.174943073807306, -71.55655137368608 );

        CHECK( loc.latitude  == Approx( 41.1749).margin(0.0001) );
        CHECK( loc.longitude == Approx(-71.5565).margin(0.0001) );
        
        // not that the order switches => longitude, latitude
        CHECK( loc[0] == Approx(-71.5565).margin(0.0001) );
        CHECK( loc[1]  == Approx( 41.1749).margin(0.0001) );

    }

    SECTION( "XYLocation Vectors Store Correctly" ) {
        const XYLocation loc( 1.2, 3.4 );

        CHECK( loc.x == Approx(1.2) );
        CHECK( loc.y == Approx(3.4) );
        CHECK( loc[0] == Approx(1.2) );
        CHECK( loc[1] == Approx(3.4) );
    }

    SECTION( "Global to UTM Frame Translation Initializes." ) {
        FrameMapping map;

        CHECK( map.utm_bounds().min.easting   == Approx(0.0) );
        CHECK( map.utm_bounds().min.northing  == Approx(0.0) );
        CHECK( map.utm_bounds().max.easting   == Approx(32.0) );
        CHECK( map.utm_bounds().max.northing  == Approx(32.0) );
        CHECK( map.global_bounds().min.latitude  == Approx(0) );
        CHECK( map.global_bounds().min.longitude == Approx(0) );
        CHECK( map.global_bounds().max.latitude  == Approx(1) );
        CHECK( map.global_bounds().max.longitude == Approx(1) );
    }

    SECTION( "Global to UTM Frame Initializes To a Square Box" ) {
        FrameMapping map;

        // UTM / Zone 19-N:  Nantucket
        map.move_to_corners( BoundBox<GlobalLocation>( GlobalLocation( 41.2, -70.2),
                                                       GlobalLocation( 41.3, -70.3) ));

        CHECK( map.utm_bounds().min.easting   == Approx(  399383.0) );
        CHECK( map.utm_bounds().min.northing  == Approx( 4561653.0) );
        CHECK( map.utm_bounds().max.easting   == Approx(  415766.0) ); 
        CHECK( map.utm_bounds().max.northing  == Approx( 4578037.0) );
        CHECK( map.utm_bounds().width() == map.utm_bounds().height()  );
        CHECK( map.utm_bounds().width() == Approx( 16384.0) );

        CHECK( map.global_bounds().min.latitude  == Approx(  41.2) );
        CHECK( map.global_bounds().min.longitude == Approx( -70.3).margin(0.1) );
        CHECK( map.global_bounds().max.latitude  == Approx(  41.3494) );
        CHECK( map.global_bounds().max.longitude == Approx( -70.0069) );
   
        CHECK( map.local_bounds().min.easting   == Approx(       0.0) );
        CHECK( map.local_bounds().min.northing  == Approx(       0.0) );
        CHECK( map.local_bounds().max.easting   == Approx(   16384.0) ); 
        CHECK( map.local_bounds().max.northing  == Approx(   16384.0) );
        CHECK( map.local_bounds().width() == map.utm_bounds().height()  );
        CHECK( map.local_bounds().width() == Approx( 16384.0) );
    }


    SECTION( "Global to UTM Frame Initializes To a Square Box" ) {
        FrameMapping map;

        // UTM / Zone 19-N:  Nantucket
        map.move_to_corners( BoundBox<GlobalLocation>( GlobalLocation( 41.2, -70.2),
                                                       GlobalLocation( 41.3, -70.3) ));

        CHECK( map.utm_bounds().min.easting   == Approx(  399383.0).margin(0.1) );
        CHECK( map.utm_bounds().min.northing  == Approx( 4561653.0).margin(0.1) );
        CHECK( map.utm_bounds().max.easting   == Approx(  415766.0).margin(0.1) ); 
        CHECK( map.utm_bounds().max.northing  == Approx( 4578037.0).margin(0.1) );

        { // case 1: Southwest corner should map correctly:
            const auto& input_location = map.global_bounds().southwest();

            const auto& utm_location = map.map_to_utm( input_location );
            CHECK( utm_location.easting  == Approx(map.utm_bounds().min.easting).margin(0.1) );
            CHECK( utm_location.northing == Approx(map.utm_bounds().min.northing).margin(0.1) );

            const auto& local_location = map.map_to_local( input_location );
            CHECK( local_location.easting  == Approx( 0.0 ).margin(0.1) );
            CHECK( local_location.northing == Approx( 0.0 ).margin(0.1) );
        }
        { // case 2: Northwest corner should map correctly:
            const auto& input_location = map.global_bounds().northeast();

            const auto& utm_location = map.map_to_utm( input_location );
            CHECK( utm_location.easting  == Approx(  415766.0 ).margin(0.1) );
            CHECK( utm_location.northing == Approx( 4578037.0 ).margin(0.1) );

            const auto& local_location = map.map_to_local( input_location );
            CHECK( local_location.easting  == Approx( 16384.0 ).margin(0.1) );
            CHECK( local_location.northing == Approx( 16384.0 ).margin(0.1) );
        }
    }

}