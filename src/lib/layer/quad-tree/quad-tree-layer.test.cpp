// GPL v3 (c) 2021, Daniel Williams 

#include <cmath>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
using Catch::Approx;

#include "quad-tree-layer.hpp"

// using chartbox::geometry::BoundBox;
using chartbox::geometry::LocalLocation;
//using chartbox::geometry::UTMLocation;
 
using chartbox::layer::QuadTreeLayer;
using chartbox::layer::QuadNode;
using chartbox::layer::LeafNode;


namespace chartbox::layer {

// ============ ============ QuadTreeLayer Tests  ============ ============

TEST_CASE( "QuadTreeLayer placeholder " ){
    //const LeafNode<4> leaf({0,0});
    // CHECK( leaf.get( 00 .offset() == 5 );
} // TEST_CASE

}   // namespace

// static const Vector2d default_anchor = {4.4, 4.6};
// static const std::vector<uint8_t> default_tile_contents = {
//     66, 67, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 68, 69, 65, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 70, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 0,  99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 0,  0,  0,  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 0,  0,  0,
//     0,  0,  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 0,  0,  0,  0,  0,  0,  0,  99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 0,  0,  0,  0,  0,  0,  0,  0,  0,  99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 0,  0,  0,  0,  0,
//     0,  0,  0,  0,  0,  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 0,  0,  0,  0,  0,  0,  0,
//     0,  0,  0,  0,  0,  0,  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 0,  0,
//     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 0,  0,  0,  0,  0,  0,  0,  0,  0,
//     0,  0,  0,  0,  0,  0,  0,  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//     0,  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 0,  0,  0,  0,
//     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//     0,  0,  0,  0,  0,  0,  0,  0,  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 0,  0,  0,  0,  0,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 0,  0,  0,  0,  0,  99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 0,  0,  0,  0,  0,  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 0,  0,  0,  0,
//     0,  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 0,  0,  0,  0,  0,  99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 0,  0,  0,  0,  0,  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 0,  0,  0,
//     0,  0,  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 0,  0,  0,  0,  0,  99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 0,  0,  0,  0,  0,  99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 0,  0,
//     0,  0,  0,  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 0,  0,  0,  0,  0,  99, 99, 99,
//     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 76, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 99, 99, 0,  0,  0,  0,  0,  99, 99, 99, 99, 99, 99, 99, 99, 99,
//     99, 99, 99, 71, 75, 74, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 0,
//     0,  0,  0,  0,  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 73, 72};

// TEST(Tile, ContainsPoints) {
//     // step 1:
//     Tile1k tile(default_anchor);
//     tile.fill(default_tile_contents);

//     // cerr << tile.to_string() << endl; // DEBUG

//     ASSERT_NEAR(tile.anchor.x(), 4.4, 1e-6);
//     ASSERT_NEAR(tile.anchor.y(), 4.6, 1e-6);

//     EXPECT_FALSE(tile.contains({0., 0.}));
//     EXPECT_FALSE(tile.contains({1., 1.}));
//     EXPECT_FALSE(tile.contains({2., 2.}));
//     EXPECT_FALSE(tile.contains({3., 3.}));
//     EXPECT_FALSE(tile.contains({4., 4.}));
//     EXPECT_FALSE(tile.contains({4.5, 4.5}));
//     // lower-left corner
//     EXPECT_TRUE(tile.contains({4.6, 4.6}));
//     EXPECT_TRUE(tile.contains({5., 5.}));
//     EXPECT_TRUE(tile.contains({10., 10.}));
//     EXPECT_TRUE(tile.contains({20., 20.}));
//     EXPECT_TRUE(tile.contains({30., 30.}));
//     EXPECT_TRUE(tile.contains({35., 35.}));
//     EXPECT_TRUE(tile.contains({36.2, 36.2}));
//     // upper-right corner
//     ASSERT_FALSE(tile.contains({36.4, 35.4}));
//     ASSERT_FALSE(tile.contains({36.5, 35.5}));
//     ASSERT_FALSE(tile.contains({37.6, 37.6}));
//     ASSERT_FALSE(tile.contains({37., 37}));
//     ASSERT_FALSE(tile.contains({40., 40.}));
// }

// TEST(Tile, Classify) {
//     Tile1k tile(default_anchor);
//     tile.fill(default_tile_contents);

//     // cerr << tile.to_string() << endl; // DEBUG

//     EXPECT_EQ(tile.status(), NodeStatus::Mixed);
//     EXPECT_EQ(tile.get_value(), 'K');

//     ASSERT_NEAR(tile.anchor.x(), 4.4, 1e-6);
//     ASSERT_NEAR(tile.anchor.y(), 4.6, 1e-6);

//     EXPECT_EQ(tile.classify({0., 0.}), 42);
//     EXPECT_EQ(tile.classify({1., 1.}), 42);
//     EXPECT_EQ(tile.classify({2., 2.}), 42);
//     EXPECT_EQ(tile.classify({3., 3.}), 42);
//     EXPECT_EQ(tile.classify({4., 4.}), 42);
//     EXPECT_EQ(tile.classify({4.5, 4.5}), 42);
//     // lower-left corner
//     EXPECT_EQ(tile.classify({4.6, 4.6}), 75);
//     EXPECT_EQ(tile.classify({5., 5.}), 75);
//     EXPECT_EQ(tile.classify({10., 10.}), 99);
//     EXPECT_EQ(tile.classify({15., 15.}), 99);
//     EXPECT_EQ(tile.classify({18., 18.}), 0);
//     EXPECT_EQ(tile.classify({20., 20.}), 0);
//     EXPECT_EQ(tile.classify({25., 25.}), 0);
//     EXPECT_EQ(tile.classify({22., 22.}), 0);
//     EXPECT_EQ(tile.classify({30., 30.}), 99);
//     EXPECT_EQ(tile.classify({35., 35.}), 99);
//     EXPECT_EQ(tile.classify({36., 36.}), 69);
//     EXPECT_EQ(tile.classify({36.3, 36.3}), 69);
//     // upper-right corner
//     EXPECT_EQ(tile.classify({36.4, 35.4}), 42);
//     EXPECT_EQ(tile.classify({36.5, 35.5}), 42);
//     EXPECT_EQ(tile.classify({37.6, 37.6}), 42);
//     EXPECT_EQ(tile.classify({37., 37.}), 42);
//     EXPECT_EQ(tile.classify({40., 40.}), 42);
// }

// TEST(Tile, FlatbufferRoundTrip) {
//     ASSERT_EQ(sizeof(uint8_t), sizeof(std::byte));
//     ASSERT_EQ(sizeof(uint8_t*), sizeof(std::byte*));

//     // step 1:
//     Tile1k write_tile(default_anchor);
//     write_tile.fill(default_tile_contents);

//     // // DEBUG
//     // cerr << write_tile.to_string() << endl;

//     { // quickly test the original tile
//         EXPECT_NEAR(write_tile.anchor.x(), 4.4, 1e-6);
//         EXPECT_NEAR(write_tile.anchor.y(), 4.6, 1e-6);

//         EXPECT_EQ(write_tile.status(), NodeStatus::Mixed);
    
//         ASSERT_EQ(write_tile.classify({0., 0.}), 42);
//         ASSERT_EQ(write_tile.classify({10., 10.}), 99);
//         ASSERT_EQ(write_tile.classify({20., 20.}), 0);
//         ASSERT_EQ(write_tile.classify({30., 30.}), 99);
//         ASSERT_EQ(write_tile.classify({40., 40.}), 42);
//     }

//     // target first half: write to cache
//     auto* buffer = write_tile.to_flatbuffer();
//     ASSERT_NE(nullptr, buffer);

//     // Perform intermediate tests on the buffer
//     auto tile_cache = GetTileCache(buffer);
//     ASSERT_NEAR(tile_cache->x(), 4.4, 1e-6);
//     ASSERT_NEAR(tile_cache->y(), 4.6, 1e-6);
    
//     Tile1k read_tile({tile_cache->x(), tile_cache->y()});
//     ASSERT_NEAR(read_tile.anchor.x(), 4.4, 1e-6);
//     ASSERT_NEAR(read_tile.anchor.y(), 4.6, 1e-6);

//     // // Not implemented.  Not sure how the builder knows how many total bytes
//     // are available, in this case. EXPECT_EQ( tile_cache->GetSize(), 1072);  //
//     // serialized size, including packing

//     // target second half: write to cache
//     ASSERT_TRUE( read_tile.load_from_flatbuffer(buffer) );

//     // // DEBUG
//     // cerr << read_tile.to_string() << endl;

//     { // quickly test the loaded tile
//         EXPECT_EQ(read_tile.status(), NodeStatus::Mixed);

//         EXPECT_EQ(read_tile.classify({0., 0.}), 42);
//         EXPECT_EQ(read_tile.classify({10., 10.}), 99);
//         EXPECT_EQ(read_tile.classify({20., 20.}), 0);
//         EXPECT_EQ(read_tile.classify({30., 30.}), 99);
//         EXPECT_EQ(read_tile.classify({40., 40.}), 42);
//     }
// }

// TEST(Tile, JSONRoundTrip) {
//     ASSERT_EQ(sizeof(uint8_t), sizeof(std::byte));
//     ASSERT_EQ(sizeof(uint8_t*), sizeof(std::byte*));

//     // step 1:
//     Tile1k write_tile(default_anchor);
//     write_tile.fill(default_tile_contents);

//     // // DEBUG
//     // cerr << write_tile->to_string() << endl;

//     { // quickly test the original tile
//         EXPECT_NEAR(write_tile.anchor.x(), 4.4, 1e-6);
//         EXPECT_NEAR(write_tile.anchor.y(), 4.6, 1e-6);

//         EXPECT_EQ(write_tile.status(), NodeStatus::Mixed);

//         ASSERT_EQ(write_tile.classify({0., 0.}), 42);
//         ASSERT_EQ(write_tile.classify({10., 10.}), 99);
//         ASSERT_EQ(write_tile.classify({20., 20.}), 0);
//         ASSERT_EQ(write_tile.classify({30., 30.}), 99);
//         ASSERT_EQ(write_tile.classify({40., 40.}), 42);
//     }

//     // target first half: write to cache
//     const std::string& buffer = write_tile.to_json();

//     // target second half: write to cache
//     const auto read_tile = Tile1k::build_from_json(buffer);
//     ASSERT_TRUE(read_tile);

//     EXPECT_NEAR(read_tile->anchor.x(), 4.4, 1e-6);
//     EXPECT_NEAR(read_tile->anchor.y(), 4.6, 1e-6);

//     // // DEBUG
//     // cerr << read_tile->to_string() << endl;

//     { // quickly test the loaded tile
//         EXPECT_EQ(read_tile->status(), NodeStatus::Mixed);

//         EXPECT_EQ(read_tile->classify({0., 0.}), 42);
//         EXPECT_EQ(read_tile->classify({10., 10.}), 99);
//         EXPECT_EQ(read_tile->classify({20., 20.}), 0);
//         EXPECT_EQ(read_tile->classify({30., 30.}), 99);
//         EXPECT_EQ(read_tile->classify({40., 40.}), 42);
//     }
// }

// TEST(Tile, LoadPolygonFromVector) {
//     Tile1k tile;
//     EXPECT_DOUBLE_EQ(tile.anchor.x(), 0.);
//     EXPECT_DOUBLE_EQ(tile.anchor.y(), 0.);

//     // reset grid
//     tile.fill(88); // == 'X'

//     Polygon shape = {{2, 2}, {12, 12}, {24, 6}, {30, 16}, {20, 30}, {5, 24}};
//     tile.fill(shape, 46); // == '.'

//     // // DEBUG
//     // cerr << tile.to_string() << endl;

//     EXPECT_EQ(tile.status(), NodeStatus::Mixed);

//     EXPECT_EQ(tile.get_cell(0, 0), 88);
//     EXPECT_EQ(tile.get_cell(1, 1), 88);
//     EXPECT_EQ(tile.get_cell(2, 2), 46);
//     EXPECT_EQ(tile.get_cell(3, 3), 46);
//     EXPECT_EQ(tile.get_cell(4, 4), 46);

//     EXPECT_EQ(tile.get_cell(12, 10), 88);
//     EXPECT_EQ(tile.get_cell(12, 11), 88);
//     EXPECT_EQ(tile.get_cell(12, 12), 46);
//     EXPECT_EQ(tile.get_cell(12, 13), 46);
//     EXPECT_EQ(tile.get_cell(12, 14), 46);

//     // EXPECT_EQ( tile.get_cell( 9, 9), 88);
//     // EXPECT_EQ( tile.get_cell(15,15), 88);
// }

// TEST(Tile, LoadShapefile) {
//     // fyi, this is not a _great_ teste, because a single tile is small,
//     // compared to the whole map...
//     const Vector2d anchor{771635, 2961225};
//     Tile1k tile(anchor);

//     const string shapefile("data/Somerville/CityLimits.shp");

//     // vvvv test target vvvv
//     ASSERT_TRUE(tile.load_from_shapefile(shapefile));
//     // ^^^^ test target ^^^^

//     // // DEBUG
//     // cerr << tile.to_string() << endl;

//     { // quickly test the loaded tile
//         EXPECT_EQ(tile.status(), NodeStatus::Mixed);

//         EXPECT_EQ(tile.get_cell(0, 0), 153);
//         EXPECT_EQ(tile.get_cell(1, 1), 153);
//         EXPECT_EQ(tile.get_cell(2, 2), 153);
//         EXPECT_EQ(tile.get_cell(3, 3), 153);

//         EXPECT_EQ(tile.get_cell(4, 4), 153);
//         EXPECT_EQ(tile.get_cell(4, 5), 153);
//         EXPECT_EQ(tile.get_cell(4, 6), 0);
//         EXPECT_EQ(tile.get_cell(4, 7), 0);
//         EXPECT_EQ(tile.get_cell(4, 8), 0);
//         EXPECT_EQ(tile.get_cell(4, 9), 0);
//         EXPECT_EQ(tile.get_cell(4, 10), 0);

//         EXPECT_EQ(tile.get_cell(20, 15), 0);
//         EXPECT_EQ(tile.get_cell(21, 15), 0);
//         EXPECT_EQ(tile.get_cell(22, 15), 153);
//         EXPECT_EQ(tile.get_cell(23, 15), 153);
//         EXPECT_EQ(tile.get_cell(24, 15), 153);
//     }
// }

// // TEST(Tile, ToPNG) {
// //     // step 1:
// //     Tile1k tile(default_anchor);
// //     tile.fill(default_tile_contents);

// //     // DEBUG
// //     cerr << tile->to_string() << endl; // DEBUG

// //     const std::vector<std::byte> data = tile->to_raster();
// //     const string outpath("test.tile.png");
// //     ASSERT_TRUE( chart::io::to_png( data, outpath ) );
// // }

// }; // namespace chart::node

// TEST(QuadTreeTest, CalculateMemoryUsage) {
//     Tree tree;
//     Terrain terrain(tree);

//     assert_layouts_match(terrain.get_layout(), default_layout);

//     EXPECT_EQ(sizeof(Terrain<Tree>), 32);
//     EXPECT_EQ(sizeof(Layout), 64);
//     EXPECT_EQ(sizeof(Tree), 72); // composed of: Layout, root-pointer
//     EXPECT_EQ(sizeof(Vector2d), 16);
//     EXPECT_EQ(sizeof(Node), 40);
// }

// TEST(QuadTreeTest, ConstructDefault) {
//     Tree tree;
//     Terrain terrain(tree);

//     assert_layouts_match(terrain.get_layout(), default_layout);

//     EXPECT_TRUE(tree.root);
//     EXPECT_TRUE(tree.root->is_leaf());
// }

// TEST(QuadTreeTest, ConstructByCenterAndSize) {
//     Tree tree({1., 1, 1, 256});
//     Terrain terrain(tree);

//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_precision(), 1.);
//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_x(), 1.);
//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_y(), 1.);
//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_width(), 256.);
//     EXPECT_EQ(terrain.get_layout().get_dimension(), 256);
//     EXPECT_EQ(terrain.get_layout().get_size(), 65536);

//     // functional tests:
//     // (1) in bounds
//     ASSERT_TRUE(tree.contains({0, 0}));
//     // (2) out of bounds in x
//     ASSERT_FALSE(tree.contains({200, 0}));
//     // (3) out of bounds in y
//     ASSERT_FALSE(tree.contains({0, 200}));
//     // (4) out of bounds in both x AND y
//     ASSERT_FALSE(tree.contains({130, 130}));
// }

// TEST(QuadTreeTest, ConstructAndSetBounds) {
//     Tree tree({1., 5, 3, 17});
//     Terrain terrain(tree);

//     // pre-conditions
//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_precision(), 1.);
//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_x(), 5.);
//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_y(), 3.);
//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_width(), 32.);
//     EXPECT_EQ(terrain.get_layout().get_dimension(), 32);
//     EXPECT_EQ(terrain.get_layout().get_size(), 1024);

//     const Layout new_layout = {32, 1., 1, 256};

//     // test target
//     tree.reset(new_layout);
//     // test target

//     // post-conditions
//     assert_layouts_match(terrain.get_layout(), new_layout);
// }

// TEST(QuadTreeTest, LoadMalformedSource) {
//     Tree tree;
//     Terrain terrain(tree);

//     assert_layouts_match(terrain.get_layout(), default_layout);

//     // this is simply a malformed document.  It should not parse.
//     std::istringstream source(R"({"bounds": {"x": 100, "y": 100, "width": )");

//     // this should fail. Gracefully.
//     EXPECT_FALSE(yggdrasil::io::load_from_json_stream(terrain, source));

//     // these tests should be *exactly* the same as before the 'load' call
//     assert_layouts_match(terrain.get_layout(), default_layout);
// }

// TEST(QuadTreeTest, LoadBoundsOnly) {
//     Tree tree;
//     Terrain terrain(tree);

//     assert_layouts_match(terrain.get_layout(), default_layout);

//     // construct a valid document, with correct fields, but simply .. missing
//     // things:
//     std::stringstream stream(
//         R"({"bounds": {"x": 100, "y": 100, "width": 64}} )");

//     // this should fail. (gracefully)
//     EXPECT_FALSE(yggdrasil::io::load_from_json_stream(terrain, stream));

//     // these tests should be *exactly* the same as before the 'load' call
//     assert_layouts_match(terrain.get_layout(), default_layout);
// }

// TEST(QuadTreeTest, LoadValidTree) {
//     Tree tree;
//     Terrain terrain(tree);
//     assert_layouts_match(terrain.get_layout(), default_layout);

//     const json source = {
//         {"NE", {{"NE", 0.0}, {"NW", 0.0}, {"SE", 0.0}, {"SW", 0.0}}},
//         {"NW", 0.0},
//         {"SE", 0.0},
//         {"SW", 0.0}};

//     tree.load_tree(source);

//     // // debug
//     // tree.debug_tree();

//     // test shape
//     ASSERT_FALSE(tree.root->is_leaf());
//     ASSERT_FALSE(tree.root->get_northeast()->is_leaf());
//     ASSERT_TRUE(tree.root->get_southwest()->is_leaf());
// }

// TEST(QuadTreeTest, CalculateFullLoading) {
//     EXPECT_EQ(Tree::calculate_complete_tree(0), 1);
//     EXPECT_EQ(Tree::calculate_complete_tree(1), 5);
//     EXPECT_EQ(Tree::calculate_complete_tree(2), 21);
//     ASSERT_EQ(Tree::calculate_complete_tree(3), 85);
//     ASSERT_EQ(Tree::calculate_complete_tree(4), 341);
//     ASSERT_EQ(Tree::calculate_complete_tree(5), 1365);
// }

// TEST(QuadTreeTest, CalculateLoadFactor) {
//     Tree tree;
//     Terrain terrain(tree);

//     assert_layouts_match(terrain.get_layout(), default_layout);

//     const json source = {
//         {"NE",
//          {{"NE", 0.0},
//           {"NW", 0.0},
//           {"SE", {{"NE", 0.0}, {"NW", 0.0}, {"SE", 0.0}, {"SW", 0.0}}},
//           {"SW", 0.0}}},
//         {"NW", 0.0},
//         {"SE", 0.0},
//         {"SW", 0.0}};

//     tree.load_tree(source);

//     // // debug
//     // tree.debug_tree();

//     // test shape
//     ASSERT_FALSE(tree.root->is_leaf());
//     ASSERT_FALSE(tree.root->get_northeast()->is_leaf());
//     ASSERT_TRUE(tree.root->get_northeast()
//                     ->get_southeast()
//                     ->get_southeast()
//                     ->is_leaf());
//     ASSERT_TRUE(tree.root->get_southwest()->is_leaf());

//     ASSERT_EQ(tree.get_height(), 3);
//     ASSERT_EQ(tree.size(), 13);
//     ASSERT_NEAR(tree.get_load_factor(), 0.03812, 1e-4);
// }

// TEST(QuadTreeTest, LoadGridFromJSON) {
//     Tree tree;
//     Terrain terrain(tree);

//     assert_layouts_match(terrain.get_layout(), default_layout);

//     std::istringstream stream(R"(
//         {"layout": {"precision": 32.0, "x": 1, "y": 1, "width": 256},
//          "grid":[[88, 88, 88, 88,  0, 88, 88, 88],
//                  [88, 88, 88,  0,  0,  0, 88, 88],
//                  [88, 88,  0,  0,  0,  0,  0, 88],
//                  [88,  0,  0,  0,  0,  0,  0,  0],
//                  [ 0,  0,  0,  0, 88, 88, 88, 88],
//                  [88,  0,  0,  0, 88, 88, 88, 88],
//                  [88, 88,  0,  0, 88, 88, 88, 88],
//                  [88, 88, 88,  0, 88, 88, 88, 88]]} )");

//     // test target
//     ASSERT_TRUE(yggdrasil::io::load_from_json_stream(terrain, stream));
//     // test target

//     // // DEBUG
//     // terrain.debug();

//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_precision(), 32.);
//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_x(), 1.);
//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_y(), 1.);
//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_width(), 256.);
//     EXPECT_EQ(terrain.get_layout().get_dimension(), 8);
//     EXPECT_EQ(terrain.get_layout().get_size(), 64);

//     { // test tree shape
//         EXPECT_EQ(tree.get_height(), 3);
//         EXPECT_EQ(tree.size(), 41);
//         EXPECT_FALSE(tree.root->is_leaf());

//         // spot check #1: RT-NE-SW-quadrant
//         const auto* r_ne_sw = tree.root->get_northeast()->get_southwest();
//         ASSERT_TRUE(r_ne_sw->is_leaf());
//         ASSERT_DOUBLE_EQ(r_ne_sw->get_value(), 0);

//         // spot check #2: RT-NW-NE-NW quadrant
//         const auto* r_ne_nw =
//             tree.root->get_northwest()->get_northeast()->get_northwest();
//         ASSERT_TRUE(r_ne_nw->is_leaf());
//         ASSERT_DOUBLE_EQ(r_ne_nw->get_value(), 88);
//     }

//     const Tree& frozen = tree;
//     EXPECT_EQ(frozen.classify({16, 16}), 0);
//     EXPECT_EQ(frozen.classify({48, 48}), 0);
//     EXPECT_EQ(frozen.classify({72, 72}), 88);
//     EXPECT_EQ(frozen.classify({104, 104}), 88);

//     // [-127, -95, -63, -31, 1, 33, 65, 97, 129]
//     EXPECT_EQ(frozen.classify({-70, 130}), 88);
//     EXPECT_EQ(frozen.classify({-70, 129}), 88);
//     EXPECT_EQ(frozen.classify({-70, 97}), 88);
//     EXPECT_EQ(frozen.classify({-70, 65}), 88);
//     EXPECT_EQ(frozen.classify({-70, 33}), 0);
//     EXPECT_EQ(frozen.classify({-70, 1}), 0);
//     EXPECT_EQ(frozen.classify({-70, -31}), 0);
//     EXPECT_EQ(frozen.classify({-70, -63}), 88);
//     EXPECT_EQ(frozen.classify({-70, -95}), 88);
//     EXPECT_EQ(frozen.classify({-70, -127}), 88);
//     EXPECT_EQ(frozen.classify({-70, -130}), 88);

//     EXPECT_EQ(frozen.classify({15, 130}), 0);
//     EXPECT_EQ(frozen.classify({15, 129}), 0);
//     EXPECT_EQ(frozen.classify({15, 97}), 0);
//     EXPECT_EQ(frozen.classify({15, 65}), 0);
//     EXPECT_EQ(frozen.classify({15, 33}), 0);
//     EXPECT_EQ(frozen.classify({15, 1}), 88);
//     EXPECT_EQ(frozen.classify({15, -31}), 88);
//     EXPECT_EQ(frozen.classify({15, -63}), 88);
//     EXPECT_EQ(frozen.classify({15, -95}), 88);
//     EXPECT_EQ(frozen.classify({15, -127}), 88);
//     EXPECT_EQ(frozen.classify({15, -130}), 88);
// }

// TEST(QuadTreeTest, LoadPolygonFromJSON) {
//     quadtree::Tree tree;
//     Terrain terrain(tree);

//     constexpr double desired_width = 16.; // overall boundary
//     constexpr double desired_precision = 1.0;
//     // =====
//     json source = generate_diamond(desired_width, desired_precision);

//     std::istringstream stream(source.dump());

//     // // DEBUG
//     // cerr << "======\n" << source.dump(4) << "\n======\n" << endl;

//     ASSERT_TRUE(yggdrasil::io::load_from_json_stream(terrain, stream));

//     // // DEBUG
//     // terrain.debug();

//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_precision(), 1.);
//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_x(), desired_width / 2);
//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_y(), desired_width / 2);
//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_width(), desired_width);
//     EXPECT_EQ(terrain.get_layout().get_dimension(), 16);
//     EXPECT_EQ(terrain.get_layout().get_size(), 256);

//     ASSERT_EQ(tree.classify({4.5, 15.5}), 0x99);
//     ASSERT_EQ(tree.classify({4.5, 14.5}), 0x99);
//     ASSERT_EQ(tree.classify({4.5, 13.5}), 0x99);
//     ASSERT_EQ(tree.classify({4.5, 12.5}), 0);
//     ASSERT_EQ(tree.classify({4.5, 11.5}), 0);
//     ASSERT_EQ(tree.classify({4.5, 10.5}), 0);
//     ASSERT_EQ(tree.classify({4.5, 9.5}), 0);
//     ASSERT_EQ(tree.classify({4.5, 8.5}), 0);
//     ASSERT_EQ(tree.classify({4.5, 7.5}), 0);
//     ASSERT_EQ(tree.classify({4.5, 6.5}), 0);
//     ASSERT_EQ(tree.classify({4.5, 5.5}), 0);
//     ASSERT_EQ(tree.classify({4.5, 4.5}), 0);
//     ASSERT_EQ(tree.classify({4.5, 3.5}), 0);
//     ASSERT_EQ(tree.classify({4.5, 2.5}), 0x99);
//     ASSERT_EQ(tree.classify({4.5, 1.5}), 0x99);
//     ASSERT_EQ(tree.classify({4.5, 0.5}), 0x99);

//     ASSERT_EQ(tree.classify({0.5, 5.5}), 0x99);
//     ASSERT_EQ(tree.classify({1.5, 5.5}), 0x99);
//     ASSERT_EQ(tree.classify({2.5, 5.5}), 0);
//     ASSERT_EQ(tree.classify({3.5, 5.5}), 0);
//     ASSERT_EQ(tree.classify({4.5, 5.5}), 0);
//     ASSERT_EQ(tree.classify({5.5, 5.5}), 0);
//     ASSERT_EQ(tree.classify({6.5, 5.5}), 0);
//     ASSERT_EQ(tree.classify({7.5, 5.5}), 0);
//     ASSERT_EQ(tree.classify({8.5, 5.5}), 0);
//     ASSERT_EQ(tree.classify({9.5, 5.5}), 0);
//     ASSERT_EQ(tree.classify({10.5, 5.5}), 0);
//     ASSERT_EQ(tree.classify({11.5, 5.5}), 0);
//     ASSERT_EQ(tree.classify({12.5, 5.5}), 0);
//     ASSERT_EQ(tree.classify({13.5, 5.5}), 0x99);
//     ASSERT_EQ(tree.classify({14.5, 5.5}), 0x99);
//     ASSERT_EQ(tree.classify({15.5, 5.5}), 0x99);
// }

// TEST(QuadTreeTest, SearchExplicitTree) {
//     Tree tree({50, 0, 0, 100});
//     Terrain terrain(tree);
//     tree.root->split(50, 100);

//     ASSERT_FALSE(tree.root->is_leaf());

//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_precision(), 32.);
//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_x(), 0.);
//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_y(), 0.);
//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_width(), 128);
//     EXPECT_EQ(terrain.get_layout().get_dimension(), 4);
//     EXPECT_EQ(terrain.get_layout().get_size(), 16);

//     cell_value_t false_value = 5;
//     cell_value_t true_value = 14;

//     // Set Quadrant I:
//     tree.root->get_northeast()->set_value(true_value);
//     // Set Quadrdant II:
//     tree.root->get_northwest()->set_value(false_value);
//     // Set Quadrant III:
//     tree.root->get_southwest()->set_value(true_value);
//     // Set Quadrant IV:
//     tree.root->get_southeast()->set_value(false_value);

//     // // DEBUG
//     // tree.debug();

//     // .... Out Of Bounds: snaps to nearest node...
//     ASSERT_EQ(tree.classify({110, 110}), true_value);

//     // functional tests:
//     // .... Quadrant I:
//     EXPECT_EQ(tree.classify({25, 25}), true_value);
//     // .... Quadrant II:
//     EXPECT_EQ(tree.classify({-25, 25}), false_value);
//     // .... Quadrant III:
//     EXPECT_EQ(tree.classify({-25, -25}), true_value);
//     // .... Quadrant IV:
//     EXPECT_EQ(tree.classify({25, -25}), false_value);
// }

// TEST(QuadTreeTest, SampleTree) {
//     quadtree::Tree tree;
//     Terrain terrain(tree);

//     std::istringstream stream(R"(
//         {"layout": {"precision": 1, "x": 2, "y": 2, "width": 4},
//          "grid":[[  1,  2,  3,  4],
//                  [  5,  6,  7,  8],
//                  [  9, 10, 11, 12],
//                  [ 13, 14, 15, 16]]} )");

//     EXPECT_TRUE(yggdrasil::io::load_from_json_stream(terrain, stream));

//     // // DEBUG
//     // tree.debug_tree(true);
//     // terrain.debug();

//     // Out-Of-Bounds call -- returns nearest node.
//     const Sample s0 = tree.sample({5, 5});
//     ASSERT_TRUE(Vector2d(3.5, 3.5) == s0.at);
//     ASSERT_EQ(s0.is, 4);

//     const Sample s1 = tree.sample({0, 0});
//     ASSERT_TRUE(Vector2d(0.5, 0.5) == s1.at);
//     ASSERT_EQ(s1.is, 13);

//     const Sample s2 = tree.sample({0.9, 0.9});
//     ASSERT_TRUE(Vector2d(0.5, 0.5) == s2.at);
//     ASSERT_EQ(s2.is, 13);

//     const Sample s3 = tree.sample({2.4, 2.7});
//     ASSERT_TRUE(Vector2d(2.5, 2.5) == s3.at);
//     ASSERT_EQ(s3.is, 7);

//     const Sample s4 = tree.sample({1.7, 3.3});
//     ASSERT_TRUE(Vector2d(1.5, 3.5) == s4.at);
//     ASSERT_EQ(s4.is, 2);
// }

// // TEST( QuadTreeTest, InterpolateTree){
// //     Tree tree({{1,1}, 64}, 1.0);
// //     Terrain terrain(tree);
// //     tree.root->split();

// //     // Set Quadrant I:
// //     tree.root->get_northeast()->set_value(0);
// //     // Set Quadrdant II:
// //     tree.root->get_northwest()->set_value(50);
// //     // Set Quadrant III:
// //     tree.root->get_southwest()->set_value(100);
// //     // Set Quadrant IV:
// //     tree.root->get_southeast()->set_value(50);

// //     vector<TestPoint> test_cases;
// //     test_cases.emplace_back( -35,    4, cell_default_value);  // Start out of
// //     bounds test_cases.emplace_back( -33,    4, cell_default_value);
// //     test_cases.emplace_back( -32,    4, cell_default_value);
// //     test_cases.emplace_back( -31,    4,   70);  // border of tree
// //     test_cases.emplace_back( -30.9,  4,   70);
// //     test_cases.emplace_back( -30,    4,   70);
// //     test_cases.emplace_back( -20,    4,   70);
// //     test_cases.emplace_back( -17,    4,   70);
// //     test_cases.emplace_back( -16,    4,   70);
// //     test_cases.emplace_back( -15.1,  4,   70);
// //     test_cases.emplace_back( -15.0,  4,   70);  // breakpoint - center of
// //     outer cell test_cases.emplace_back( -14.9,  4,   70);
// //     test_cases.emplace_back( -10,    4,   62);
// //     test_cases.emplace_back( - 5,    4,   54);
// //     test_cases.emplace_back(   0,    4,   47);
// //     test_cases.emplace_back(   1,    4,   45);  // midpoint
// //     test_cases.emplace_back(   2,    4,   43);
// //     test_cases.emplace_back(  10,    4,   31);
// //     test_cases.emplace_back(  14,    4,   25);
// //     test_cases.emplace_back(  15,    4,   23);
// //     test_cases.emplace_back(  16,    4,   22);  // breakpoint - center of
// //     outer cell test_cases.emplace_back(  17,    4,   20);
// //     test_cases.emplace_back(  20,    4,   20);
// //     test_cases.emplace_back(  30,    4,   20);
// //     test_cases.emplace_back(  31,    4,   20);
// //     test_cases.emplace_back(  32,    4,   20);
// //     test_cases.emplace_back(  33,    4,   20);  // border of tree
// //     test_cases.emplace_back(  34,    4, cell_default_value);
// //     test_cases.emplace_back(  35,    4, cell_default_value);

// //     // =====================================
// //     for( const TestPoint& expect : test_cases){
// //         const auto& actual_value = tree.interp({expect.x, expect.y});

// //         std::ostringstream buf;
// //         buf << "@@  x=" << expect.x << "  y=" << expect.y << "  v=" <<
// //         expect.value << endl;

// //         ASSERT_EQ(actual_value, expect.value) << buf.str();
// //     }
// // }

// TEST(QuadTreeTest, SavePNG) {
//     Terrain<Tree> terrain;
//     const json source = generate_diamond(16.,  // boundary_width
//                                          0.4); // desired_precision);

//     std::istringstream stream(source.dump());

//     ASSERT_TRUE(yggdrasil::io::load_from_json_stream(terrain, stream));

//     // // DEBUG
//     // terrain.debug();

//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_precision(), 1.);
//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_x(), 8.);
//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_y(), 8.);
//     EXPECT_DOUBLE_EQ(terrain.get_layout().get_width(), 16);
//     EXPECT_EQ(terrain.get_layout().get_dimension(), 16);
//     EXPECT_EQ(terrain.get_layout().get_size(), 256);

//     // // Because this manually tested, comment this block until needed:
//     // const string filename("tree.test.png");
//     // terrain.to_png(filename);
// }

// TEST(QuadTreeTest, WriteLoadCycle) {
//     Tree source_tree({32, 11, 11, 128});
//     Terrain source_terrain(source_tree);

//     EXPECT_DOUBLE_EQ(source_terrain.get_layout().get_precision(), 32.);
//     EXPECT_DOUBLE_EQ(source_terrain.get_layout().get_x(), 11.);
//     EXPECT_DOUBLE_EQ(source_terrain.get_layout().get_y(), 11.);
//     EXPECT_DOUBLE_EQ(source_terrain.get_layout().get_width(), 128);
//     EXPECT_EQ(source_terrain.get_layout().get_dimension(), 4);
//     EXPECT_EQ(source_terrain.get_layout().get_size(), 16);

//     source_tree.root->split(32, 128);

//     // Set interesting values
//     source_tree.root->get_northeast()->get_northeast()->set_value(21);
//     source_tree.root->get_northeast()->get_northwest()->set_value(22);
//     source_tree.root->get_northeast()->get_southeast()->set_value(23);
//     source_tree.root->get_northeast()->get_southwest()->set_value(24);

//     source_tree.root->get_northwest()->get_northeast()->set_value(11);
//     source_tree.root->get_northwest()->get_northwest()->set_value(11);
//     source_tree.root->get_northwest()->get_southeast()->set_value(11);
//     source_tree.root->get_northwest()->get_southwest()->set_value(11);

//     source_tree.root->get_southwest()->get_northeast()->set_value(31);
//     source_tree.root->get_southwest()->get_northwest()->set_value(32);
//     source_tree.root->get_southwest()->get_southeast()->set_value(33);
//     source_tree.root->get_southwest()->get_southwest()->set_value(34);

//     source_tree.root->get_southeast()->get_northeast()->set_value(55);
//     source_tree.root->get_southeast()->get_northwest()->set_value(55);
//     source_tree.root->get_southeast()->get_southeast()->set_value(55);
//     source_tree.root->get_southeast()->get_southwest()->set_value(55);

//     // // DEBUG
//     // source_tree.debug_tree();

//     source_tree.root->prune();

//     // // DEBUG
//     // source_tree.debug_tree();

//     // write tree #1 to the serialization buffer
//     std::stringstream buffer;
//     ASSERT_TRUE(yggdrasil::io::to_json(source_terrain, buffer));

//     // // DEBUG
//     // cerr << buffer.str() << endl;

//     // load contents into another tree
//     Tree load_tree;
//     Terrain load_terrain(load_tree);

//     buffer.seekg(0);
//     ASSERT_TRUE(yggdrasil::io::load_from_json_stream(load_terrain, buffer));

//     // // DEBUG
//     // load_tree.debug_tree();

//     // test contents of test_tree
//     { // test layout:
//         EXPECT_DOUBLE_EQ(load_terrain.get_layout().get_precision(), 32.);
//         EXPECT_DOUBLE_EQ(load_terrain.get_layout().get_x(), 11.);
//         EXPECT_DOUBLE_EQ(load_terrain.get_layout().get_y(), 11.);
//         EXPECT_DOUBLE_EQ(load_terrain.get_layout().get_width(), 128);
//         EXPECT_EQ(load_terrain.get_layout().get_dimension(), 4);
//         EXPECT_EQ(load_terrain.get_layout().get_size(), 16);
//     }
//     { // test tree shape
//         auto& root = load_tree.root;
//         ASSERT_FALSE(root->is_leaf());
//         {
//             auto* ne_quad = root->get_northeast();
//             ASSERT_FALSE(ne_quad->is_leaf());
//             ASSERT_TRUE(ne_quad->get_northeast()->is_leaf());
//             ASSERT_TRUE(ne_quad->get_northwest()->is_leaf());
//             ASSERT_TRUE(ne_quad->get_southwest()->is_leaf());
//             ASSERT_TRUE(ne_quad->get_southeast()->is_leaf());
//         }
//         ASSERT_TRUE(root->get_northwest()->is_leaf());
//         ASSERT_TRUE(root->get_southeast()->is_leaf());
//         {
//             auto* sw_quad = root->get_northeast();
//             ASSERT_FALSE(sw_quad->is_leaf());
//             ASSERT_TRUE(sw_quad->get_northeast()->is_leaf());
//             ASSERT_TRUE(sw_quad->get_northwest()->is_leaf());
//             ASSERT_TRUE(sw_quad->get_southwest()->is_leaf());
//             ASSERT_TRUE(sw_quad->get_southeast()->is_leaf());
//         }
//     }
// }

// } // namespace yggdrasil::quadtree
