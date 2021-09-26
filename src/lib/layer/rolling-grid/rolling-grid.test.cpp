// GPL v3 (c) 2021, Daniel Williams 

#include <cmath>
#include <iostream>
#include <sstream>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
using Catch::Approx;

#include "chart-box/geometry/bound-box.hpp"
#include "chart-box/geometry/polygon.hpp"
#include "rolling-grid-index.hpp"
#include "rolling-grid-layer.hpp"
 
using chartbox::geometry::BoundBox;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::Polygon;
using chartbox::geometry::UTMLocation;
 
using chartbox::layer::CellInSectorIndex;
using chartbox::layer::CellInViewIndex;
using chartbox::layer::Index2u32;
using chartbox::layer::SectorInViewIndex;

using chartbox::layer::Index2u32;
using chartbox::layer::RollingGridLayer;

// ============ ============  Rolling-Grid-Index-Tests  ============ ============
TEST_CASE( "RollingGridIndex Simple Initialization" ){
    const CellInViewIndex<2,2> viewIndex = {1,1};
    CHECK( viewIndex.offset() == 5 );
} // TEST_CASE

TEST_CASE( "RollingGridIndex can divide into sub-index "){
    const CellInViewIndex<2,2> viewIndex = {2,3};
    const auto sectorIndex = viewIndex.divide();
    CHECK( Index2u32(1,1) == sectorIndex.data() );
    CHECK( sectorIndex.offset() == 3 );
} // TEST_CASE

TEST_CASE( "RollingGridIndex can clamp into sub-index "){
    const CellInViewIndex<2,2> viewIndex = {2,3};
    const auto sectorIndex = viewIndex.zoom();
    CHECK( Index2u32(0,1) == sectorIndex.data() );
    CHECK( sectorIndex.offset() == 2);
} // TEST_CASE

TEST_CASE( "RollingGridIndex calculates storage offset "){
    CHECK(  0 == CellInSectorIndex<4>(0,0).offset() );
    CHECK(  1 == CellInSectorIndex<4>(1,0).offset() );
    CHECK(  2 == CellInSectorIndex<4>(2,0).offset() );
    CHECK(  3 == CellInSectorIndex<4>(3,0).offset() );
    CHECK(  4 == CellInSectorIndex<4>(0,1).offset() );
    CHECK(  5 == CellInSectorIndex<4>(1,1).offset() );
    CHECK( 10 == CellInSectorIndex<4>(2,2).offset() );
    CHECK( 15 == CellInSectorIndex<4>(3,3).offset() );
} // TEST_CASE

TEST_CASE( "RollingGridIndex wraps correctly in both directions "){
    const CellInViewIndex<2,3> viewIndex(2,3);

    CHECK( Index2u32(1,1) == viewIndex.divide().data() );

    //                           vvv Anchor vvv
    REQUIRE( Index2u32(1,1) == (SectorInViewIndex<3>(0,0) + viewIndex.divide()).data() );
    REQUIRE( Index2u32(2,2) == (SectorInViewIndex<3>(1,1) + viewIndex.divide()).data() );
    REQUIRE( Index2u32(3,3) == (SectorInViewIndex<3>(2,2) + viewIndex.divide()).data() );

} // TEST_CASE


TEST_CASE( "RollingGridTests" ){

    SECTION( "Initialize Simple Polygon"){
        

    } // SECTION


//     auto& bds = g.bounds();
//     EXPECT_DOUBLE_EQ( bds.max().x(),  4.);
//     EXPECT_DOUBLE_EQ( bds.max().y(),  4.);
//     EXPECT_DOUBLE_EQ( bds.min().x(), -4.);
//     EXPECT_DOUBLE_EQ( bds.min().y(), -4.);

//     EXPECT_DOUBLE_EQ( bds.size().x(), 8);
//     EXPECT_DOUBLE_EQ( bds.size().y(), 8);

//     EXPECT_DOUBLE_EQ( g.precision(), 1.0);
// }

// TEST( RollingGrid, ConstructDefault1k) {
//     RollingGrid1k g;
 
//     auto& bds = g.bounds();
//     EXPECT_DOUBLE_EQ( bds.max().x(),  16.);
//     EXPECT_DOUBLE_EQ( bds.max().y(),  16.);
//     EXPECT_DOUBLE_EQ( bds.min().x(), -16.);
//     EXPECT_DOUBLE_EQ( bds.min().y(), -16.);

//     EXPECT_DOUBLE_EQ( bds.size().x(), 32);
//     EXPECT_DOUBLE_EQ( bds.size().y(), 32);

//     EXPECT_DOUBLE_EQ( g.precision(), 1.0);
// }

// TEST( RollingGrid, ConstructDefault4k) {
//     RollingGrid4k g;
 
//     auto& bds = g.bounds();
//     EXPECT_DOUBLE_EQ( bds.max().x(),  32.);
//     EXPECT_DOUBLE_EQ( bds.max().y(),  32.);
//     EXPECT_DOUBLE_EQ( bds.min().x(), -32.);
//     EXPECT_DOUBLE_EQ( bds.min().y(), -32.);

//     EXPECT_DOUBLE_EQ( bds.size().x(), 64);
//     EXPECT_DOUBLE_EQ( bds.size().y(), 64);

//     EXPECT_DOUBLE_EQ( g.precision(), 1.0);
// }

// TEST( RollingGrid, ConstructFromSquareBounds) {
//     RollingGrid1k g(Bounds({0., 1.},{32., 33.}));

//     auto& bds = g.bounds();
//     EXPECT_DOUBLE_EQ( bds.max().x(), 32.);
//     EXPECT_DOUBLE_EQ( bds.max().y(), 33.);
//     EXPECT_DOUBLE_EQ( bds.min().x(), 0.);
//     EXPECT_DOUBLE_EQ( bds.min().y(), 1.);

//     EXPECT_DOUBLE_EQ( bds.size().x(), 32.);
//     EXPECT_DOUBLE_EQ( bds.size().y(), 32.);

//     EXPECT_DOUBLE_EQ( g.precision(), 1.0);
// }

// TEST( RollingGrid, ConstructFromUnevenBounds) {
//     RollingGrid1k g(Bounds({-2., -4.},{2., 4.}));

//     auto& bds = g.bounds();
//     EXPECT_DOUBLE_EQ( bds.max().x(), 16.);
//     EXPECT_DOUBLE_EQ( bds.max().y(), 16.);
//     EXPECT_DOUBLE_EQ( bds.min().x(), -16.);
//     EXPECT_DOUBLE_EQ( bds.min().y(), -16.);
    
//     EXPECT_DOUBLE_EQ( bds.size().x(), 32.);
//     EXPECT_DOUBLE_EQ( bds.size().y(), 32.);

//     EXPECT_DOUBLE_EQ( g.precision(), 1.0);
// }

// TEST( RollingGrid, ConstructFromOversizeBounds) {
//     RollingGrid1k g(Bounds({-2,-55},{2., 55.}));

//     EXPECT_DOUBLE_EQ( g.precision(), 1.0);

//     auto& b = g.bounds();
//     EXPECT_DOUBLE_EQ( b.max().x(), 16.);
//     EXPECT_DOUBLE_EQ( b.max().y(), 16.);
//     EXPECT_DOUBLE_EQ( b.min().x(), -16.);
//     EXPECT_DOUBLE_EQ( b.min().y(), -16.);

//     EXPECT_DOUBLE_EQ( b.size().x(), 32.);
//     EXPECT_DOUBLE_EQ( b.size().y(), 32.);
// }

// static const std::vector<uint8_t> test_pattern = chart::geometry::vflip<uint8_t>({
//     88,  0,  0,  0,  88, 88, 88, 88,
//     88, 88,  0,  0,  88, 88, 88,  0,
//     88, 88, 88,  0,  88, 88,  0,  0,
//     88, 88, 88, 88,  88,  0,  0,  0,

//      0,  0,  0, 88,  88, 88, 88, 88,
//      0,  0, 88, 88,   0, 88, 88, 88,
//      0, 88, 88, 88,   0,  0, 88, 88,
//     88, 88, 88, 88,   0,  0,  0, 88,}, 8);

// TEST( RollingGrid, UpdateBounds) {
//     const Bounds b0({-2., -4.},{2., 4.});
//     RollingGrid64 g(b0);

    // SECTION( "Verify RollingGrid Can Refocus" ){
    //     // tracks the interval [ 0.0, 12.0 ]
    //     const BoundBox<UTMLocation> utm_bounds( {0,0}, {32,32} );
    //     RollingGridLayer layer( utm_bounds );

    //     REQUIRE( 0.0 == Approx(layer.tracked().min.easting) );
    //     REQUIRE( 0.0 == Approx(layer.tracked().min.northing) );
    //     REQUIRE(32.0 == Approx(layer.tracked().max.easting) );
    //     REQUIRE(32.0 == Approx(layer.tracked().max.northing) );

    //     const auto view0 = layer.visible();
    //     REQUIRE( 10.0 == Approx(view0.min.easting) );
    //     REQUIRE( 10.0 == Approx(view0.min.northing) );
    //     REQUIRE( 22.0 == Approx(view0.max.easting) );
    //     REQUIRE( 22.0 == Approx(view0.max.northing) );

    //     // layer.scroll( 2, 0 );
    //     // std::cout << layer.print_contents() << std::endl;
    //     // layer.scroll( -2, 0 );

    //     // layer.scroll( 0, 4 );
    //     // // std::cout << layer.print_contents() << std::endl;
    //     // layer.scroll( 0, -4 );

    //     // // layer.scroll_right();
    //     // // layer.scroll_lateral(false);  // roll left
    //     // std::cerr << "=========================\n";

    //     // std::cout << layer.print_contents() << std::endl;

    //     // roll_vertical(true); // roll up one row // verified
    //     //     REQUIRE( 0 == Approx(layer.anchor().row) );
    //     // roll_vertical(true); // roll up one row // verified
    //     //     REQUIRE( 1 == Approx(layer.anchor().row) );
    //     // roll_vertical(true); // roll up one row // verified
    //     //     REQUIRE( 2 == Approx(layer.anchor().row) );
    //     // roll_vertical(true); // roll up one row // verified
    //     //     REQUIRE( 0 == Approx(layer.anchor().row) );
        
    //     // {
    //     //     layer.focus( {{0,5}, {20,16}} );

    //     //     const auto result_view= layer.visible();
    //     //     REQUIRE(  0.0 == Approx(result_view.min.easting) );
    //     //     REQUIRE(  4.0 == Approx(result_view.min.northing) );
    //     //     REQUIRE( 12.0 == Approx(result_view.max.easting) );
    //     //     REQUIRE( 16.0 == Approx(result_view.max.northing) );
    //     // }

    // }

//     auto& b1 = g.bounds();
//     EXPECT_DOUBLE_EQ( b1.min().x(), -16.);
//     EXPECT_DOUBLE_EQ( b1.min().y(), -16.);
//     EXPECT_DOUBLE_EQ( b1.max().x(), 16.);
//     EXPECT_DOUBLE_EQ( b1.max().y(), 16.);
    
//     g.set_bounds(Bounds({10,12},{42,44}));

//     EXPECT_DOUBLE_EQ( g.precision(), 1.);
//     auto& b3 = g.bounds();
//     EXPECT_DOUBLE_EQ( b3.min().x(), 10.);
//     EXPECT_DOUBLE_EQ( b3.min().y(), 12.);
//     EXPECT_DOUBLE_EQ( b3.max().x(), 42.);
//     EXPECT_DOUBLE_EQ( b3.max().y(), 44.);
// }


// TEST( RollingGrid, MoveGrid) {
//     const Bounds b0({-2., -4.},{2., 4.});
//     RollingGrid64 g(b0);
    
//     ASSERT_TRUE(g.fill(test_pattern));

//     // DEBUG
//     cerr << g.to_string() << endl;

//     // TODO: NYI: Implement rolling
//     ASSERT_TRUE(false);

//     // DEBUG
//     cerr << g.to_string() << endl;
    
//     auto& b1 = g.bounds();
//     EXPECT_DOUBLE_EQ( b1.min().x(), -16.);
//     EXPECT_DOUBLE_EQ( b1.min().y(), -16.);
//     EXPECT_DOUBLE_EQ( b1.max().x(), 16.);
//     EXPECT_DOUBLE_EQ( b1.max().y(), 16.);
    
//     g.move({10,12});

//     EXPECT_DOUBLE_EQ( g.precision(), 1.);
//     auto& b3 = g.bounds();
//     EXPECT_DOUBLE_EQ( b3.min().x(), -6.);
//     EXPECT_DOUBLE_EQ( b3.min().y(), -4.);
//     EXPECT_DOUBLE_EQ( b3.max().x(), 26.);
//     EXPECT_DOUBLE_EQ( b3.max().y(), 28.);
// }

// TEST( RollingGrid, DeflateRoundTrip) {
//     RollingGrid1k g(Bounds({5.5,2.5}, {37.5, 34.5}));
//     ASSERT_DOUBLE_EQ( g.precision(), 1.0);
//     ASSERT_DOUBLE_EQ( g.bounds().min().x(), 5.5);
//     ASSERT_DOUBLE_EQ( g.bounds().min().y(), 2.5);

//     { // case #0
//         const Vector2d p_global = {6,3};

//         const Index2u p_local = g.as_index(p_global);
//         EXPECT_EQ( p_local.i, 0 );
//         EXPECT_EQ( p_local.j, 0 );

//         const Vector2d p_final = g.as_location(p_local);
//         EXPECT_DOUBLE_EQ(p_final.x(), 6.0 );
//         EXPECT_DOUBLE_EQ(p_final.y(), 3.0 );

//     }{ // case #1
//         const Vector2d p_global = { 8.1, 8.1};
        
//         const Index2u p_local = g.as_index(p_global);
//         ASSERT_EQ( p_local.i, 2 );
//         ASSERT_EQ( p_local.j, 5 );

//         const Vector2d p_final = g.as_location(p_local);
//         ASSERT_DOUBLE_EQ(p_final.x(), 8 );
//         ASSERT_DOUBLE_EQ(p_final.y(), 8 );
    
//     }{ // case #3
//         const Vector2d p_global = { 8.3, 8.9};

//         const Index2u p_local = g.as_index(p_global);
//         ASSERT_EQ( p_local.i, 2 );
//         ASSERT_EQ( p_local.j, 6 );

//         const Vector2d p_final = g.as_location(p_local);
//         ASSERT_DOUBLE_EQ(p_final.x(), 8 );
//         ASSERT_DOUBLE_EQ(p_final.y(), 9 );
//     }
// }

// static const std::vector<uint8_t> default_tile = chart::geometry::vflip<uint8_t>({
//     66, 67, 67, 67,  68, 68, 68, 69,
//     65, 66,  0,  0,   0,  0, 69, 70,
//     65,  0, 66,  0,   0, 69,  0, 70,
//     65,  0,  0, 66,  69,  0,  0, 70,

//     76,  0,  0, 75,  72,  0,  0, 71,
//     76,  0, 75,  0,   0, 72,  0, 71,
//     76, 75,  0,  0,   0,  0, 72, 71,
//     75, 74, 74, 74,  73, 73, 73, 72}, 8);

// TEST( RollingGrid, FillFromVector) {
//     RollingGrid64 g;

//     ASSERT_EQ(default_tile.size(), g.size());

//     ASSERT_EQ( 0, g.fill(default_tile) );

//     // // DEBUG
//     // cerr << g.to_string() << endl;

//     ASSERT_EQ( g.get_cell(0,0), 75);
//     ASSERT_EQ( g.get_cell(1,1), 75);
//     ASSERT_EQ( g.get_cell(2,2), 75);
//     ASSERT_EQ( g.get_cell(3,3), 75);
//     ASSERT_EQ( g.get_cell(4,4), 69);
//     ASSERT_EQ( g.get_cell(5,5), 69);
//     ASSERT_EQ( g.get_cell(6,6), 69);
//     ASSERT_EQ( g.get_cell(7,7), 69);

//     ASSERT_EQ( g.get_cell(3,0), 74);
//     ASSERT_EQ( g.get_cell(3,1),  0);
//     ASSERT_EQ( g.get_cell(3,2),  0);
//     ASSERT_EQ( g.get_cell(3,3), 75);
//     ASSERT_EQ( g.get_cell(3,4), 66);
//     ASSERT_EQ( g.get_cell(3,5),  0);
//     ASSERT_EQ( g.get_cell(3,6),  0);
//     ASSERT_EQ( g.get_cell(3,7), 67);

//     ASSERT_EQ( g.get_cell(4,4), 69);
//     ASSERT_EQ( g.get_cell(4,5),  0);
//     ASSERT_EQ( g.get_cell(4,6),  0);
//     ASSERT_EQ( g.get_cell(4,7), 68);
// }


// TEST( RollingGrid, FillFromPath) {
//     RollingGrid64 g(Bounds({10.2,10.2}, {18.2,18.2}));

//     // preconditions
//     ASSERT_TRUE( g.fill(126) ); // == '~'

//     // draw path
//     const uint8_t fill_value = 111;
//     const Path path = {{11,18}, {11,11}, {18,18}};
//     ASSERT_TRUE( g.fill(path, fill_value) ); // == 'o'

//     // // DEBUG
//     // cerr << g.to_string() << endl;

//     // segment 0:
//     ASSERT_EQ( g.classify({ 11.0, 18.0}), fill_value);
//     ASSERT_EQ( g.classify({ 11.0, 17.0}), fill_value);
//     ASSERT_EQ( g.classify({ 11.0, 16.0}), fill_value);
//     ASSERT_EQ( g.classify({ 11.0, 15.0}), fill_value);
//     ASSERT_EQ( g.classify({ 11.0, 14.0}), fill_value);
//     ASSERT_EQ( g.classify({ 11.0, 13.0}), fill_value);
//     ASSERT_EQ( g.classify({ 11.0, 12.0}), fill_value);
//     ASSERT_EQ( g.classify({ 11.0, 11.0}), fill_value);

//     // segment 1:
//     ASSERT_EQ( g.classify({ 11.0, 11.0}), fill_value);
//     ASSERT_EQ( g.classify({ 12.0, 12.0}), fill_value);
//     ASSERT_EQ( g.classify({ 13.0, 13.0}), fill_value);
//     ASSERT_EQ( g.classify({ 14.0, 14.0}), fill_value);
//     ASSERT_EQ( g.classify({ 15.0, 15.0}), fill_value);
//     ASSERT_EQ( g.classify({ 16.0, 16.0}), fill_value);
//     ASSERT_EQ( g.classify({ 17.0, 17.0}), fill_value);
//     ASSERT_EQ( g.classify({ 18.0, 18.0}), fill_value);
// }

// TEST( RollingGrid, ClassifyIntoVector) {
//     RollingGrid64 g;

//     g.set_bounds({{32,32}, {64,64}});

//     EXPECT_DOUBLE_EQ( g.precision(), 4. );
//     EXPECT_DOUBLE_EQ( g.width(), 32.0 );
//     EXPECT_DOUBLE_EQ( g.bounds().min().x(), 32.);
//     EXPECT_DOUBLE_EQ( g.bounds().min().y(), 32.);
//     EXPECT_DOUBLE_EQ( g.bounds().max().x(), 64.);
//     EXPECT_DOUBLE_EQ( g.bounds().max().y(), 64.);

//     ASSERT_EQ( 0, g.fill(default_tile) );

//     // // DEBUG
//     // cerr << g.to_string() << endl;

//     // OOB
//     ASSERT_EQ( g.classify({  1.0, -1.0}), 255);
//     ASSERT_EQ( g.classify({ -1.0,  1.0}), 255);
//     ASSERT_EQ( g.classify({ -1.0, -1.0}), 255);

//     // first cell (first in memory; NW in nav-space)
//     // Coordinates are in x,y, from the SouthWest corner...
//     ASSERT_EQ( g.classify({ 32.01, 32.01}), 75);
//     ASSERT_EQ( g.classify({ 32.1,  32.1}),  75);
//     ASSERT_EQ( g.classify({ 33.0,  33.5}),  75);
//     ASSERT_EQ( g.classify({ 34.0,  34.5}),  75);
//     ASSERT_EQ( g.classify({ 35.9,  35.9}),  75);
//     ASSERT_EQ( g.classify({ 35.99, 35.99}), 75);

//     // cell at (2,0)
//     ASSERT_EQ( g.classify({ 43.9, 36.1}), 0);
//     ASSERT_EQ( g.classify({ 43.0, 37.0}), 0);
//     ASSERT_EQ( g.classify({ 42.0, 38.0}), 0);
//     ASSERT_EQ( g.classify({ 41.0, 39.0}), 0);
//     ASSERT_EQ( g.classify({ 40.1, 39.9}), 0);

//     // cell @ 4,3
//     ASSERT_EQ( g.classify({ 50.0, 46}), 72);

//     // cell @ 3,7
//     ASSERT_EQ( g.classify({ 45, 62}), 67);
// }

// TEST( RollingGrid, StoreReadLoop) {
//     RollingGrid64 g({{32,32}, {64,64}});

//     g.fill(55);
//     EXPECT_EQ( g.classify({50,46}), 55);

//     EXPECT_TRUE( g.store({50, 46}, 88) );
//     ASSERT_EQ( g.classify({50, 46}), 88);
// }

// TEST( RollingGrid, LoadMalformedSource){
//     RollingGrid64 g;

//     EXPECT_DOUBLE_EQ( g.precision(),  1.);
//     EXPECT_DOUBLE_EQ( g.bounds().max().x(),  4.);
//     EXPECT_DOUBLE_EQ( g.bounds().max().y(),  4.);
//     EXPECT_DOUBLE_EQ( g.bounds().min().x(), -4.);
//     EXPECT_DOUBLE_EQ( g.bounds().min().y(), -4.);

//     // this is simply a malformed document.  It should not parse.
//     std::string source(R"({"bounds": {"x": 100, "y": 100, "width": )");
    
//     // this should fail. Gracefully.
//     EXPECT_FALSE( g.load_json(source));

//     // these tests should be *exactly* the same as before the 'load' call
//     EXPECT_DOUBLE_EQ( g.precision(),  1.);
//     EXPECT_DOUBLE_EQ( g.bounds().max().x(),  4.);
//     EXPECT_DOUBLE_EQ( g.bounds().max().y(),  4.);
//     EXPECT_DOUBLE_EQ( g.bounds().min().x(), -4.);
//     EXPECT_DOUBLE_EQ( g.bounds().min().y(), -4.);
// }

// TEST( RollingGrid, LoadValidBoundsFromJSON){
//     RollingGrid1k g;

//     EXPECT_DOUBLE_EQ( g.bounds().max().x(),  16.);
//     EXPECT_DOUBLE_EQ( g.bounds().max().y(),  16.);
//     EXPECT_DOUBLE_EQ( g.bounds().min().x(), -16.);
//     EXPECT_DOUBLE_EQ( g.bounds().min().y(), -16.);

//     // construct a valid document, with correct fields, but missing required fields:
//     std::string source(R"({"bounds": {"center": [100, 100], "width": 64}} )");
    
//     EXPECT_TRUE( g.load_json(source) );

//     // these tests should be *exactly* the same as before the 'load' call
//     EXPECT_DOUBLE_EQ( g.bounds().max().x(), 132.);
//     EXPECT_DOUBLE_EQ( g.bounds().max().y(), 132.);
//     EXPECT_DOUBLE_EQ( g.bounds().min().x(),  68.);
//     EXPECT_DOUBLE_EQ( g.bounds().min().y(),  68.);
//     EXPECT_DOUBLE_EQ( g.precision(), 2 );
// }

// TEST( RollingGrid, LoadGridFromJSON) {
//     RollingGrid64 g;

//     std::string text(R"({
//         "grid": [ 88, 88, 88, 88, 88, 88, 88, 88,
//                   88, 88, 88,  0,  0, 88, 88, 88,
//                   88, 88,  0,  0,  0,  0, 88, 88,
//                   88,  0,  0,  0,  0,  0,  0, 88,
//                   88, 88, 88, 88,  0,  0,  0, 88,
//                   88, 88, 88, 88,  0,  0, 88, 88,
//                   88, 88, 88, 88,  0, 88, 88, 88,
//                   88, 88, 88, 88, 88, 88, 88, 88 ]})");

//     ASSERT_EQ( 0, g.load_json(text) );

//     // // DEBUG
//     // cerr << g.to_string() << endl;

//     ASSERT_EQ( g.get_cell(0,0), 88);
//     ASSERT_EQ( g.get_cell(1,1), 88);
//     ASSERT_EQ( g.get_cell(2,2), 88);
//     ASSERT_EQ( g.get_cell(2,3), 88);
//     ASSERT_EQ( g.get_cell(2,4),  0);

//     ASSERT_EQ( g.get_cell(3,0), 88);
//     ASSERT_EQ( g.get_cell(3,1), 88);
//     ASSERT_EQ( g.get_cell(3,2), 88);
//     ASSERT_EQ( g.get_cell(3,3), 88);
//     ASSERT_EQ( g.get_cell(3,4),  0);
//     ASSERT_EQ( g.get_cell(3,5),  0);
//     ASSERT_EQ( g.get_cell(3,6),  0);
//     ASSERT_EQ( g.get_cell(3,7), 88);

//     ASSERT_EQ( g.get_cell(0,7), 88);
//     ASSERT_EQ( g.get_cell(1,6), 88);
//     ASSERT_EQ( g.get_cell(2,5),  0);
// }

// TEST( RollingGrid, FillSimplePolygon) {
//     RollingGrid64 g(Bounds({0,0},{8,8}));
//     EXPECT_DOUBLE_EQ( g.bounds().max().x(), 8.);
//     EXPECT_DOUBLE_EQ( g.bounds().max().y(), 8.);
//     EXPECT_DOUBLE_EQ( g.bounds().min().x(), 0.);
//     EXPECT_DOUBLE_EQ( g.bounds().min().y(), 0.);
//     EXPECT_DOUBLE_EQ( g.precision(), 1. );

//     ASSERT_TRUE( g.fill(0x99) );

//     constexpr double diamond_width = 6.;
//     Polygon diamond = geometry::Polygon::make_diamond(diamond_width)
//                                         .move(g.bounds().center());
//     ASSERT_TRUE( g.fill(diamond, (uint8_t)0) );

//     // // DEBUG
//     // cerr << diamond.to_yaml() << endl;
//     // cerr << g.to_string() << endl;

//     ASSERT_EQ( g.get_cell(3, 7), 0x99);
//     ASSERT_EQ( g.get_cell(3, 6), 0x99);
//     ASSERT_EQ( g.get_cell(3, 5),    0);
//     ASSERT_EQ( g.get_cell(3, 4),    0);
//     ASSERT_EQ( g.get_cell(3, 3),    0);
//     ASSERT_EQ( g.get_cell(3, 2),    0);
//     ASSERT_EQ( g.get_cell(3, 1), 0x99);
//     ASSERT_EQ( g.get_cell(3, 0), 0x99);

//     ASSERT_EQ( g.get_cell( 0, 5), 0x99);
//     ASSERT_EQ( g.get_cell( 1, 5), 0x99);
//     ASSERT_EQ( g.get_cell( 2, 5), 0x99);
//     ASSERT_EQ( g.get_cell( 3, 5),    0);
//     ASSERT_EQ( g.get_cell( 4, 5),    0);
//     ASSERT_EQ( g.get_cell( 5, 5),    0);
//     ASSERT_EQ( g.get_cell( 6, 5), 0x99);
//     ASSERT_EQ( g.get_cell( 7, 5), 0x99);
// }

// TEST( RollingGrid, FillHoledPolygon) {
//     RollingGrid1k g;
//     EXPECT_DOUBLE_EQ( g.bounds().max().x(),  16.);
//     EXPECT_DOUBLE_EQ( g.bounds().max().y(),  16.);
//     EXPECT_DOUBLE_EQ( g.bounds().min().x(), -16.);
//     EXPECT_DOUBLE_EQ( g.bounds().min().y(), -16.);
//     EXPECT_DOUBLE_EQ( g.precision(), 1.0);

//     ASSERT_TRUE( g.fill(126) );

//     const std::string source = R"({
//         "bounds": { "southwest": [0,0],
//                     "northeast": [32,32]},
//         "allow": [[ [  1,  1],
//                     [  1, 28],
//                     [ 28, 28],
//                     [ 28,  1]]],
//         "block": [[ [  4,  4],
//                     [  4, 24],
//                     [  8, 24],
//                     [  8,  4]],
//                   [ [ 12, 12],
//                     [ 12, 20],
//                     [ 20, 20],
//                     [ 20, 12]]]})";

//     EXPECT_TRUE( g.load_json(source) );

//     EXPECT_DOUBLE_EQ( g.bounds().max().x(), 32.);
//     EXPECT_DOUBLE_EQ( g.bounds().max().y(), 32.);
//     EXPECT_DOUBLE_EQ( g.bounds().min().x(),  0.);
//     EXPECT_DOUBLE_EQ( g.bounds().min().y(),  0.);
//     EXPECT_DOUBLE_EQ( g.precision(), 1. );

    // // DEBUG
    // cerr << g.to_string() << endl;

//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_precision(), 1.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_x(),         4.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_y(),         4.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_width(),     8.);
//     EXPECT_EQ( terrain.get_layout().get_dimension(), 8);
//     EXPECT_EQ( terrain.get_layout().get_size(),     64);

//     ASSERT_EQ( terrain.classify({ 0.2, 0.2}), 0x99);
//     ASSERT_EQ( terrain.classify({ 0.8, 0.8}), 0x99);
//     ASSERT_EQ( terrain.classify({ 1.1, 1.1}), 0);
//     ASSERT_EQ( terrain.classify({ 2.2, 2.2}), 0);
//     ASSERT_EQ( terrain.classify({ 3.2, 3.2}), 0);
//     ASSERT_EQ( terrain.classify({ 4.2, 4.2}), 0x99);
//     ASSERT_EQ( terrain.classify({ 4.2, 4.7}), 0x99);
//     ASSERT_EQ( terrain.classify({ 5.2, 5.2}), 0x99);
//     ASSERT_EQ( terrain.classify({ 5.2, 5.7}), 0x99);
//     ASSERT_EQ( terrain.classify({ 6.2, 6.2}), 0);
//     ASSERT_EQ( terrain.classify({ 6.7, 6.7}), 0);
//     ASSERT_EQ( terrain.classify({ 7.2, 7.2}), 0x99);
//     ASSERT_EQ( terrain.classify({ 7.7, 7.7}), 0x99);

//     // above-diagonal square
//     ASSERT_EQ( terrain.classify({ 2.2, 5.2}), 0x99);
//     ASSERT_EQ( terrain.classify({ 2.2, 5.7}), 0x99);
}

// TEST(Grid, LoadOffsetPolygon) {
//     Terrain<Grid> terrain;

//     // =====
//     const json source = {
//             {"layout",  {{"precision", 32},
//                          {"x", 232000},
//                          {"y", 811000},
//                          {"width", 1024}}},
//             {"allow", {{{231600, 810800},
//                         {231800, 810600},
//                         {232440, 810600},
//                         {232440, 810850},
//                         {232220, 811000},
//                         {232440, 811150},
//                         {232440, 811400},
//                         {232200, 811400},
//                         {231600, 810800}}}}};
//     std::istringstream stream(source.dump());

//     EXPECT_TRUE( terrain::io::load_from_json_stream(terrain, stream) );
//     // print error, if it is set:
//     ASSERT_TRUE( terrain.get_error().empty() ) << terrain.get_error();

//     // DEBUG
//     // terrain.debug();
//     // terrain.print();

//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_precision(),    32.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_x(),        232000.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_y(),        811000.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_width(),      1024.);
//     EXPECT_EQ( terrain.get_layout().get_dimension(),  32);
//     EXPECT_EQ( terrain.get_layout().get_size(),     1024);

//     ASSERT_EQ( terrain.classify({ 231500, 811500}), 0x99);
//     ASSERT_EQ( terrain.classify({ 231600, 811400}), 0x99);
//     ASSERT_EQ( terrain.classify({ 231700, 811300}), 0x99);
//     ASSERT_EQ( terrain.classify({ 231800, 811200}), 0x99);
//     ASSERT_EQ( terrain.classify({ 231900, 811100}), 0x99);
//     ASSERT_EQ( terrain.classify({ 232000, 811000}), 0);
//     ASSERT_EQ( terrain.classify({ 232100, 810900}), 0);
//     ASSERT_EQ( terrain.classify({ 232200, 810800}), 0);
//     ASSERT_EQ( terrain.classify({ 232300, 810700}), 0);
//     ASSERT_EQ( terrain.classify({ 232400, 810600}), 0);
//     ASSERT_EQ( terrain.classify({ 232500, 810500}), 0x99);
    
// }

// TEST(Grid, LoadSomervilleShapeFile) {
//     Terrain<Grid> terrain;

//     string shapefile("data/Somerville/CityLimits.shp");

//     const bool load_result = terrain::io::load_shape_from_file(terrain, shapefile);
//     // terrain.debug();
//     // terrain.print();

//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_precision(),    16.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_x(),        763251.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_y(),       2969340.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_width(),     32768.);
//     EXPECT_EQ( terrain.get_layout().get_dimension(),  2048);
//     EXPECT_EQ( terrain.get_layout().get_size(),    4194304);

//     // Because this manually tested, comment this block until needed:
//     const string filename("test.somerville.shapefile.png");
//     terrain::io::to_png(terrain, filename);

//     ASSERT_TRUE( load_result );
// }

// TEST(Grid, LoadMassachusettsShapeFile) {
//     Terrain<Grid> terrain;

//     string shapefile("data/massachusetts/navigation_area_100k.shp");

//     const bool load_result = terrain::io::load_shape_from_file(terrain, shapefile);
//     // terrain.debug();
//     // terrain.print();

//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_precision(),  64.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_x(),        305000.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_y(),        861400.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_width(),    262144.);
//     // EXPECT_EQ( terrain.get_layout().get_dimension(),  1024);
//     // EXPECT_EQ( terrain.get_layout().get_size(),    1048576);
//     EXPECT_EQ( terrain.get_layout().get_dimension(),  4096);
//     EXPECT_EQ( terrain.get_layout().get_size(),   16777216);

//     // Because this manually tested, comment this block until needed:
//     const string filename("shapefile.test.png");
//     terrain::io::to_png(terrain, filename);

//     ASSERT_TRUE( load_result );
// }

// TEST(Grid, SavePNG) {
//     Terrain<Grid> terrain;
//     const json source = generate_diamond(  64.,   // boundary_width
//                                             1.0);  // desired_precision);
//     std::istringstream stream(source.dump());
//     ASSERT_TRUE( terrain::io::load_from_json_stream(terrain, stream) );

//     // // DEBUG
//     // terrain.debug();

//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_precision(), 1.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_x(),        32.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_y(),        32.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_width(),    64.);
//     EXPECT_EQ( terrain.get_layout().get_dimension(), 64);
//     EXPECT_EQ( terrain.get_layout().get_size(),    4096);

//     // Because this manually tested, comment this block until needed:
//     const string filename("grid.test.png");
//     terrain::io::to_png(terrain, filename);
// }