// GPL v3 (c) 2021, Daniel Williams 

#include <cstddef>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
using Catch::Approx;

#include "bound-box.hpp"
#include "global-location.hpp"
#include "local-location.hpp"
#include "utm-location.hpp"
#include "xy-location.hpp"

using chartbox::geometry::GlobalLocation;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::UTMLocation;
using chartbox::geometry::XYLocation;
using chartbox::geometry::BoundBox;

TEST_CASE( "BoundTests" ){

    SECTION( "Default Construct Works Normally" ) {
        const BoundBox<XYLocation> bounds;
        CHECK( bounds.min.x == Approx(std::numeric_limits<double>::max()) );
        CHECK( bounds.min.y == Approx(std::numeric_limits<double>::max()) );
        CHECK( bounds.max.x == Approx(-std::numeric_limits<double>::max()) );
        CHECK( bounds.max.y == Approx(-std::numeric_limits<double>::max()) );

        CHECK( bounds.southwest().x == Approx( std::numeric_limits<double>::max()) );
        CHECK( bounds.southwest().y == Approx( std::numeric_limits<double>::max()) );
        CHECK( bounds.northeast().x == Approx( -std::numeric_limits<double>::max()) );
        CHECK( bounds.northeast().y == Approx( -std::numeric_limits<double>::max()) );
    }

    SECTION( "Bounds can be constructed in all supported vector-spaces" ){
        // if it compiles, then the test passes.
        const BoundBox<GlobalLocation> global_bounds({41.0, 10.0}, {42.0, 11.0});
        const BoundBox<UTMLocation>    utm_bounds({5,6}, {22,57});
        const BoundBox<LocalLocation>  local_bounds({5,6}, {22,57});
        const BoundBox<XYLocation>     xy_bounds({5,6}, {22,57});
    }

    SECTION( "Bounds can be constructed from an initializer lists" ){
        const BoundBox<XYLocation> bounds({5,6}, {22,57});

        CHECK( bounds.southwest().x == Approx(5) );
        CHECK( bounds.southwest().y == Approx(6) );
        CHECK( bounds.northeast().x == Approx(22) );
        CHECK( bounds.northeast().y == Approx(57) );
    }

    SECTION( "Bounds can indicate if it is a square (true)" ){
        const BoundBox<XYLocation> bounds({0,0}, {22,22});
        REQUIRE( bounds.is_square() );
    }

    SECTION( "Bounds can indicate if it is a square (false)" ){
        const BoundBox<XYLocation> bounds({0,0}, {22,57});
        REQUIRE( not bounds.is_square() );
    }

    SECTION( "Bounds can grow to accomodate new points" ){
        BoundBox<XYLocation> bounds;
        bounds.grow({3, 4});
        bounds.grow({5, 6});
        bounds.grow({9, 5});
        bounds.grow({12, 8});
        bounds.grow({5, 11});

        CHECK( bounds.west()  == Approx( 3.0) );
        CHECK( bounds.south() == Approx( 4.0) );
        CHECK( bounds.east()  == Approx(12.0) );
        CHECK( bounds.north() == Approx(11.0) );
    }
}
