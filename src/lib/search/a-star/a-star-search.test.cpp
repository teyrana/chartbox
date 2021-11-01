// GPL v3 (c) 2020, Daniel Williams 

#include <cmath>
#include <iostream>
#include <sstream>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
using Catch::Approx;

#include <fmt/core.h>

#include "geometry/bound-box.hpp"
#include "geometry/path.hpp"
#include "layer/simple-grid/simple-grid-layer.hpp"

#include "a-star-search.hpp"

using chartbox::layer::simple::SimpleGridLayer;
using chartbox::search::AStarSearch;

using namespace chartbox;

// ============ ============  A*-Search-Tests  ============ ============
TEST_CASE( "A* Default constructor" ){
    const SimpleGridLayer<uint8_t, 24, 1000> g;
    search::AStarSearch search(g);

    CHECK(  1.0 == search.precision() );
    REQUIRE( LocalLocation(  0.0,  0.0) == search.searchable().min );
    REQUIRE( LocalLocation( 24.0, 24.0) == search.searchable().max );
}

TEST_CASE( "A* Search Structure can adjust to different terrain sizes" ){
    const SimpleGridLayer<uint8_t, 256, 1000> g;
    search::AStarSearch search(g);

    REQUIRE(   1.0 == search.precision() );
    REQUIRE( LocalLocation(  0.0,  0.0) == search.searchable().min );
    REQUIRE( LocalLocation( 256.0, 256.0) == search.searchable().max );
}

TEST_CASE( "A* Search Structure does not support different terrain resolutions" ){
    const SimpleGridLayer<uint8_t, 32, 500> g;
    search::AStarSearch search(g);

    // 1/2 meter resolution
    REQUIRE(  0.5 != search.precision() );
}

TEST_CASE( "A* Search: Verify Cost Function" ){
    const SimpleGridLayer<uint8_t, 24, 1000> g;
    AStarSearch search(g);

    CHECK( 0.0    == Approx(search.cost( {0,0}, {0,0} )));
    CHECK( 1.0    == Approx(search.cost( {0,0}, {0,1} )));
    CHECK( 1.4142 == Approx(search.cost( {0,0}, {1,1} )));
    CHECK( 2.8284 == Approx(search.cost( {2,2}, {4,4} )));
    CHECK( 5.0    == Approx(search.cost( {2,2}, {5,6} )));

} // placeholder

TEST_CASE( "A* Search: Encode Adjacency Flags", "DEV-ONLY" ){
    const SimpleGridLayer<uint8_t, 24, 1000> g;
    AStarSearch search(g);

    CHECK( 0x32 == static_cast<int>(search.encode_adjacency_flags( { 1, 0} )));
    CHECK( 0x36 == static_cast<int>(search.encode_adjacency_flags( { 1,-1} )));
    CHECK( 0x34 == static_cast<int>(search.encode_adjacency_flags( { 0,-1} )));
    CHECK( 0x35 == static_cast<int>(search.encode_adjacency_flags( {-1,-1} )));
    CHECK( 0x31 == static_cast<int>(search.encode_adjacency_flags( {-1, 0} )));
    CHECK( 0x39 == static_cast<int>(search.encode_adjacency_flags( {-1, 1} )));
    CHECK( 0x38 == static_cast<int>(search.encode_adjacency_flags( { 0, 1} )));
    CHECK( 0x3A == static_cast<int>(search.encode_adjacency_flags( { 1, 1} )));
}

TEST_CASE( "A* Search: Decode Adjacency Flags", "DEV-ONLY" ){
    const SimpleGridLayer<uint8_t, 24, 1000> g;
    AStarSearch search(g);

    // error condition -- not visited
    CHECK( search.decode_adjacency_flags( 0x00 ).isnan() );

    // Sentinel marker -- start point
    CHECK( LocalLocation(  0,  0 ) == search.decode_adjacency_flags( 0xCF ));

    CHECK( LocalLocation(  1,  0 ) == search.decode_adjacency_flags( 0x32 ));
    CHECK( LocalLocation(  1, -1 ) == search.decode_adjacency_flags( 0x36 ));
    CHECK( LocalLocation(  0, -1 ) == search.decode_adjacency_flags( 0x34 ));
    CHECK( LocalLocation( -1, -1 ) == search.decode_adjacency_flags( 0x35 ));
    CHECK( LocalLocation( -1,  0 ) == search.decode_adjacency_flags( 0x31 ));
    CHECK( LocalLocation( -1,  1 ) == search.decode_adjacency_flags( 0x39 ));
    CHECK( LocalLocation(  0,  1 ) == search.decode_adjacency_flags( 0x38 ));
    CHECK( LocalLocation(  1,  1 ) == search.decode_adjacency_flags( 0x3A ));
}

TEST_CASE( "A* Can find a simple, direct path" ){
    SimpleGridLayer<uint8_t, 24, 1000> g;
    g.fill(0);

    AStarSearch search(g);
    REQUIRE( 1.0 == search.precision() );
    REQUIRE( LocalLocation(  0.0,  0.0) == search.searchable().min );
    REQUIRE( LocalLocation( 24.0, 24.0) == search.searchable().max );

    const auto found_path = search.compute( {2,2}, {12,12} );

    CHECK( not found_path.empty() );
    REQUIRE( 6 == found_path.size() );

    CHECK( LocalLocation(  2,  2 ) == found_path[0] );
    CHECK( LocalLocation(  4,  4 ) == found_path[1] );
    CHECK( LocalLocation(  6,  6 ) == found_path[2] );
    CHECK( LocalLocation(  8,  8 ) == found_path[3] );
    CHECK( LocalLocation( 10, 10 ) == found_path[4] );
    CHECK( LocalLocation( 12, 12 ) == found_path[5] );
}


// a 32x32 grid of cells, containing a few interesting islands
// Note: this will be vertical-swapped
static const std::array<uint8_t, 32*32> islands = {
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

TEST_CASE( "A* can navigate a more complicated path" ){
    SimpleGridLayer<uint8_t, 32, 1000> g;
    
    CHECK( g.fill( islands.data(), islands.size() ) );

    AStarSearch search(g);
    REQUIRE(  1.0 == search.precision() );
    REQUIRE( LocalLocation(  0.0,  0.0) == search.searchable().min );
    REQUIRE( LocalLocation( 32.0, 32.0) == search.searchable().max );

    const auto found_path = search.compute( {3,12}, {24,28} );
    CHECK( not found_path.empty() );

    // // DEBUG
    // fmt::print( "====== Found Search Path: ====== \n{}\n", found_path.str() );
    // // DEBUG
    // fmt::print( "====== Terrain (After):  ====== \n" );
    // g.fill( found_path, g.visible(), 'o');
    // fmt::print( "{}\n", g.to_location_content_string(4) );
    // // DEBUG

    CHECK( 15 == found_path.size() );

    CHECK( LocalLocation(  3, 12 ) == found_path[ 0] );
    CHECK( LocalLocation(  7, 19 ) == found_path[ 5] );
    CHECK( LocalLocation( 11, 26 ) == found_path[ 8] );
    CHECK( LocalLocation( 14, 28 ) == found_path[10] );
    CHECK( LocalLocation( 16, 26 ) == found_path[11] );
    CHECK( LocalLocation( 19, 26 ) == found_path[12] );
    CHECK( LocalLocation( 24, 28 ) == found_path[14] );
}
