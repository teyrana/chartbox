// GPL v3 (c) 2020, Daniel Williams 

#include <cmath>
#include <cstddef>
#include <iostream>
#include <sstream>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
using Catch::Approx;

#include "bound-box.hpp"
#include "location2en.hpp"
#include "location2ll.hpp"
#include "location2xy.hpp"
#include "frame-mapping.hpp"

using namespace chartbox::frame;

TEST_CASE( "FrameTests" ){

    SECTION( " Location2EN Vectors Store Correctly" ) {
        const Location2EN loc( 1.2, 3.4 );
        CHECK( loc.easting == Approx(1.2) );
        CHECK( loc.northing == Approx(3.4) );
        CHECK( loc[0] == Approx(1.2) );
        CHECK( loc[1] == Approx(3.4) );
 
        auto vec = loc.toVector();
        CHECK( vec.x() == Approx(1.2) );
        CHECK( vec.y() == Approx(3.4) );

        const Location2EN loc2( vec );
        CHECK( loc2.easting == Approx(1.2) );
        CHECK( loc2.northing == Approx(3.4) );
    }

    SECTION( "Location2EN BoundBox works correctly" ) {
        BoundBox<Location2EN> box( {0,0}, {2.1, 3.5} );

        CHECK( box.max.easting == Approx(2.1) );
        CHECK( box.max.northing == Approx(3.5) );
        CHECK( box.min.easting == Approx(0.0) );
        CHECK( box.min.northing == Approx(0.0) );

        CHECK( box.height() == Approx(3.5) );
        CHECK( box.width() == Approx(2.1) );
    }

    SECTION( "Location2LL Vectors Store Correctly" ) {
        const Location2LL loc( 41.174943073807306, -71.55655137368608 );

        CHECK( loc.latitude  == Approx( 41.1749).margin(0.0001) );
        CHECK( loc.longitude == Approx(-71.5565).margin(0.0001) );
        CHECK( loc.latitude  == Approx( 41.1749).margin(0.0001) );
        CHECK( loc.longitude == Approx(-71.5565).margin(0.0001) );

        auto vec = loc.toVector();
        CHECK( loc.latitude  == Approx( 41.1749).margin(0.0001) );
        CHECK( loc.longitude == Approx(-71.5565).margin(0.0001) );
 
        const Location2LL loc2( vec );
        CHECK( loc2.latitude  == Approx( 41.1749).margin(0.0001) );
        CHECK( loc2.longitude == Approx(-71.5565).margin(0.0001) );
    }

    SECTION( "Location2xy Vectors Store Correctly" ) {
        const Location2xy loc( 1.2, 3.4 );

        CHECK( loc.x == Approx(1.2) );
        CHECK( loc.y == Approx(3.4) );
        CHECK( loc[0] == Approx(1.2) );
        CHECK( loc[1] == Approx(3.4) );
 
        auto vec = loc.toVector();
        CHECK( vec.x() == Approx(1.2) );
        CHECK( vec.y() == Approx(3.4) );

        const Location2xy loc2( vec );
        CHECK( loc2.x == Approx(1.2) );
        CHECK( loc2.y == Approx(3.4) );
    }

    SECTION( "Global to Local (UTM) Frame Translation Initializes." ) {
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

    SECTION( "Global to Local (UTM) Frame Initializes To a Square Box" ) {
        FrameMapping map;

        // UTM / Zone 19-N:  Nantucket
        map.move_to_corners( BoundBox<Location2LL>( Location2LL( 41.2, -70.2),
                                                    Location2LL( 41.3, -70.3) ));

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
    }

}