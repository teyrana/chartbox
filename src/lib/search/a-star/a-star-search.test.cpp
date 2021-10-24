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
// #include "layer/dynamic-grid/dynamic-grid-layer.hpp"
#include "layer/simple-grid/simple-grid-layer.hpp"

#include "a-star-search.hpp"

using chartbox::layer::simple::SimpleGridLayer;
using chartbox::search::AStarSearch;

using namespace chartbox;

// ============ ============  A*-Search-Tests  ============ ============
TEST_CASE( "A* Default constructor" ){
    const SimpleGridLayer<uint8_t, 24, 1000> g;
    search::AStarSearch search(g);

    REQUIRE(  1.0 == search.precision() );
    REQUIRE( 24   == search.dimension() );
}

// TEST_CASE( "A* Search Structure can adjust to different terrain sizes" ){
//     const SimpleGridLayer<uint8_t, 256, 1000> g;
//     search::AStarSearch search(g);

//     REQUIRE(   1.0 == search.precision() );
//     REQUIRE( 256   == search.dimension() );
// }

// TEST_CASE( "A* Search Structure can adjust to different terrain resolutions" ){
//     const SimpleGridLayer<uint8_t, 32, 500> g;
//     search::AStarSearch search(g);

//     REQUIRE(  0.5 == search.precision() ); // 1/2 meter resolution
//     REQUIRE( 32   == search.dimension() );
// }

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
    REQUIRE(  1.0 == search.precision() );
    REQUIRE( 24   == search.dimension() );

    const auto found_path = search.compute( {2,2}, {12,12} );

    // DEBUG
    fmt::print( "====== Found Search Path: ====== \n{}\n", found_path.str() );
    // g.fill(found, g.visible(), 'o');
    // fmt::print( "{}\n", g.print_contents_by_cell(4) );
    // fmt::print( "====== ====== ====== ====== \n" );
    // DEBUG

    CHECK( not found_path.empty() );
    REQUIRE( 11 == found_path.size() );

    CHECK( LocalLocation(  2,  2 ) == found_path[ 0] );
    CHECK( LocalLocation(  4,  4 ) == found_path[ 2] );
    CHECK( LocalLocation(  7,  7 ) == found_path[ 5] );
    CHECK( LocalLocation( 10, 10 ) == found_path[ 8] );
    CHECK( LocalLocation( 11, 11 ) == found_path[ 9] );
    CHECK( LocalLocation( 12, 12 ) == found_path[10] );
}


// a 32x32 grid of cells, containing a few interesting islands
static const std::array<uint8_t, 32*32> islands = {
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0, 65, 65, 65, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0, 65, 65, 65, 65, 65,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

TEST_CASE( "A* Can navigate a more direct path" ){
    SimpleGridLayer<uint8_t, 24, 1000> g;
    
    g.fill(0);
    // CHECK( g.fill( islands.data(), islands.size() ) );

    AStarSearch search(g);
    REQUIRE(  1.0 == search.precision() );
    REQUIRE( 24   == search.dimension() );

//     FixedGrid1k g(Bounds({0.5,0.5}, {32.5,32.5}));  // start with a 1-cell-to-1-meter spacing.

    // // DEBUG
    // cerr << g.to_string() << endl;

//     const auto found_path = search.compute( {6,6}, {24,30} );
//     ASSERT_FALSE( foundPath.empty() );

//     // // debug
//     // g.fill(foundPath, 'o');

//     // // DEBUG
//     // cerr << g.to_string() << endl;

//     ASSERT_EQ( foundPath.size(), 25);

//     const auto& p0 = foundPath[0];
//     ASSERT_DOUBLE_EQ(  p0.x(),  6);
//     ASSERT_DOUBLE_EQ(  p0.y(),  6);

//     const auto& p5 = foundPath[5];
//     ASSERT_DOUBLE_EQ(  p5.x(), 15);
//     ASSERT_DOUBLE_EQ(  p5.y(), 13);

//     const auto& p8 = foundPath[8];
//     ASSERT_DOUBLE_EQ(  p8.x(), 11);
//     ASSERT_DOUBLE_EQ(  p8.y(), 18);

//     const auto& p9 = foundPath[9];
//     ASSERT_DOUBLE_EQ(  p9.x(),  9);
//     ASSERT_DOUBLE_EQ(  p9.y(), 20);

//     const auto& p12 = foundPath[12];
//     ASSERT_DOUBLE_EQ( p12.x(),  8);
//     ASSERT_DOUBLE_EQ( p12.y(), 24);

//     const auto& p20 = foundPath[20];
//     ASSERT_DOUBLE_EQ( p20.x(), 19);
//     ASSERT_DOUBLE_EQ( p20.y(), 27);

//     const auto& pf = foundPath[24];
//     ASSERT_DOUBLE_EQ( pf.x(), 24);
//     ASSERT_DOUBLE_EQ( pf.y(), 30);

}
