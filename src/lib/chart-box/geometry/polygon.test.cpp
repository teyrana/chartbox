// GPL v3 (c) 2021, Daniel Williams 

#include <iostream>
#include <memory>
#include <vector>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
using Catch::Approx;

#include "polygon.hpp"
#include "xy-location.hpp"

using chartbox::geometry::XYLocation;
using chartbox::geometry::BoundBox;
using chartbox::geometry::Polygon;

TEST_CASE( "PolygonTests" ){

    SECTION( "Initialize Simple Polygon"){
        const Polygon<XYLocation> shape( {{0,0}, {1,0}, {1,1}, {0,1}} );

        CHECK( shape.size() == 4 );        
        CHECK( shape[0] == XYLocation(0, 0) );
        CHECK( shape[1] == XYLocation(1, 0) );
        CHECK( shape[2] == XYLocation(1, 1) );
        CHECK( shape[3] == XYLocation(0, 1) );

    } // SECTION

    SECTION( "Initialize from initialize list" ){
        const Polygon<XYLocation> shape({{3, 4}, {5, 6},{9, 5}, {12, 8}, {5, 11}}); 

        REQUIRE( shape.size() == 5 );
        CHECK( shape[0] == XYLocation(3, 4) );
        CHECK( shape[1] == XYLocation(5, 6) );
        CHECK( shape[2] == XYLocation(9, 5) );
        CHECK( shape[3] == XYLocation(12, 8) );
        CHECK( shape[4] == XYLocation(5, 11) );
    } // SECTION

    SECTION( "Polygons Accurately populate their bounds on load" ){
        const Polygon<XYLocation> shape({{3, 4}, {5, 6},{9, 5}, {12, 8}, {5, 11}}); 

        REQUIRE( shape.size() == 5 );

        const auto& bounds = shape.bounds();
        CHECK( bounds.west()  == Approx( 3.0) );
        CHECK( bounds.south() == Approx( 4.0) );
        CHECK( bounds.east()  == Approx(12.0) );
        CHECK( bounds.north() == Approx(11.0) );
    }

    SECTION( "Polygons can detect overlap" ){
        const Polygon<XYLocation> target_shape({{0, 0}, {4,0}, {4,4}, {0,4}});
        const Polygon<XYLocation> object_shape({{2, 2}, {6,2}, {6,6}, {2,6}});

        REQUIRE( target_shape.overlap(object_shape) );
    }

    SECTION( "Polygons can be loaded from CPLJSONArray objects" ){
        const Polygon<XYLocation> target_shape({{0,0}, {4,0}, {4,4}, {0,4}});
        const Polygon<XYLocation> object_shape({{6,6}, {9,6}, {9,9}, {6,9}});

        REQUIRE( not target_shape.overlap(object_shape) );
    }

} // TEST_CASE
