// GPL v3 (c) 2021, Daniel Williams 

#include <iostream>
#include <memory>
#include <vector>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
using Catch::Approx;

#include <geometry/xy-location.hpp>

#include "path.hpp"

using chartbox::geometry::XYLocation;
using chartbox::geometry::Path;

TEST_CASE("Initialize Path with defaults"){
    const Path<XYLocation> path;
    CHECK( 0 == path.size() );
    CHECK( path.empty() );
}

TEST_CASE( "Initialize Path with initialization list" ){
    const Path<XYLocation> path({{3, 4}, {5, 6},{9, 5}, {12, 8}, {5, 11}});

    CHECK( 5 == path.size() );
    CHECK( path[0] == XYLocation(3, 4) );
    CHECK( path[1] == XYLocation(5, 6) );
    CHECK( path[2] == XYLocation(9, 5) );
    CHECK( path[3] == XYLocation(12, 8) );
    CHECK( path[4] == XYLocation(5, 11) );
}

TEST_CASE( "Create Path and emplace-points-back" ){
    Path<XYLocation> path;
    REQUIRE( path.empty() );
    REQUIRE( 0 == path.size() );

    path.emplace_back(0,0);
    CHECK( 1 == path.size() );
    CHECK( path[0] == XYLocation( 0, 0));

    path.emplace_back(2,2);
    CHECK( 2 == path.size() );
    CHECK( path[1] == XYLocation( 2, 2));

    path.emplace_back(2,0);
    CHECK( 3 == path.size() );
    CHECK( path[2] == XYLocation( 2, 0));

    path.emplace_back(3,0);
    CHECK( 4 == path.size() );
    CHECK( path[3] == XYLocation( 3, 0));
}

TEST_CASE( "Path can calculate its length" ){
    const Path<XYLocation> path({{3, 4}, {5, 6},{9, 5}, {12, 8}, {5, 11}});

    CHECK( 5 == path.size() );
    CHECK( path[0] == XYLocation(3, 4) );
    CHECK( path[1] == XYLocation(5, 6) );
    CHECK( path[2] == XYLocation(9, 5) );
    CHECK( path[3] == XYLocation(12, 8) );
    CHECK( path[4] == XYLocation(5, 11) );

    CHECK( Approx(17.0) == path.length() );
}

TEST_CASE( "Users can iterate over path points" ){
    Path<XYLocation> path = {{0,0}, {1,0}, {1,1}, {0,1}};

    auto iter = path.begin();
    CHECK( *iter == path[0] );

    ++iter;
    CHECK( *iter == path[1] );

    ++iter;
    CHECK( *iter == path[2] );

    ++iter;
    CHECK( *iter == path[3] );
}
