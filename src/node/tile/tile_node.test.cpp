// GPL v3 (c) 2020

#include <cmath>
#include <cstddef>
#include <iostream>
#include <sstream>

#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

#include "geometry/polygon.hpp"
#include "tile_node.hpp"
#include "tile_cache_generated.h"

using std::cerr;
using std::endl;
using std::isnan;
using std::string;

using Eigen::Vector2d;
using nlohmann::json;


namespace yggdrasil::node {

TEST(Tile, ConstructDefault) {
    TileNode tile;

    EXPECT_EQ( TileNode::size, 1024);
    EXPECT_EQ( TileNode::dimension, 32);
    EXPECT_DOUBLE_EQ( TileNode::scale,       1.);
    EXPECT_DOUBLE_EQ( TileNode::width,      32.);

    EXPECT_DOUBLE_EQ( tile.anchor.x(),  0.);
    EXPECT_DOUBLE_EQ( tile.anchor.y(),  0.);
}

TEST(Tile, ConstructAtLocation) {
    TileNode tile({6.,7.7});

    EXPECT_EQ( TileNode::size, 1024);
    EXPECT_EQ( TileNode::dimension, 32);
    EXPECT_DOUBLE_EQ( TileNode::scale,       1.);
    EXPECT_DOUBLE_EQ( TileNode::width,      32.);

    EXPECT_DOUBLE_EQ( tile.anchor.x(),  6.0);
    EXPECT_DOUBLE_EQ( tile.anchor.y(),  7.7);
}

static const Vector2d default_anchor = {4.4, 4.6};
static const std::vector<TileNode::cell_t> default_tile_contents = {
        66, 67, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 68, 69,
        65, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 70,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        76, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 71,
        75, 74, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 73, 72};

TEST(Tile, ContainsPoints) {
    // step 1:
    TileNode tile(default_anchor);
    tile.fill(default_tile_contents);

    // cerr << tile.to_string() << endl; // DEBUG
    
    ASSERT_NEAR( tile.anchor.x(), 4.4, 1e-6);
    ASSERT_NEAR( tile.anchor.y(), 4.6, 1e-6);

    EXPECT_FALSE( tile.contains({  0.,  0.}) );
    EXPECT_FALSE( tile.contains({  1.,  1.}) );
    EXPECT_FALSE( tile.contains({  2.,  2.}) );
    EXPECT_FALSE( tile.contains({  3.,  3.}) );
    EXPECT_FALSE( tile.contains({  4.,  4.}) );
    EXPECT_FALSE( tile.contains({  4.5, 4.5}) );
    // lower-left corner
    EXPECT_TRUE( tile.contains({   4.6,  4.6}) );
    EXPECT_TRUE( tile.contains({   5.,   5.}) );
    EXPECT_TRUE( tile.contains({  10.,  10.}) );
    EXPECT_TRUE( tile.contains({  20.,  20.}) );
    EXPECT_TRUE( tile.contains({  30.,  30.}) );
    EXPECT_TRUE( tile.contains({  35.,  35.}) );
    EXPECT_TRUE( tile.contains({  36.2, 36.2}) );
    // upper-right corner
    ASSERT_FALSE( tile.contains({  36.4, 35.4}) );
    ASSERT_FALSE( tile.contains({  36.5, 35.5}) );
    ASSERT_FALSE( tile.contains({  37.6, 37.6}) );
    ASSERT_FALSE( tile.contains({  37.,  37}) );
    ASSERT_FALSE( tile.contains({  40.,  40.}) );

}

TEST(Tile, Classify){
    TileNode tile(default_anchor);
    tile.fill(default_tile_contents);

    // cerr << tile.to_string() << endl; // DEBUG

    ASSERT_NEAR( tile.anchor.x(), 4.4, 1e-6);
    ASSERT_NEAR( tile.anchor.y(), 4.6, 1e-6);

    EXPECT_EQ( tile.classify({  0.,   0.}),   42);
    EXPECT_EQ( tile.classify({  1.,   1.}),   42);
    EXPECT_EQ( tile.classify({  2.,   2.}),   42);
    EXPECT_EQ( tile.classify({  3.,   3.}),   42);
    EXPECT_EQ( tile.classify({  4.,   4.}),   42);
    EXPECT_EQ( tile.classify({  4.5,  4.5}),  42);
    // lower-left corner
    EXPECT_EQ( tile.classify({  4.6,  4.6}),  75);
    EXPECT_EQ( tile.classify({  5.,   5.}),   75);
    EXPECT_EQ( tile.classify({ 10.,  10.}),   99);
    EXPECT_EQ( tile.classify({ 15.,  15.}),   99);
    EXPECT_EQ( tile.classify({ 18.,  18.}),    0);
    EXPECT_EQ( tile.classify({ 20.,  20.}),    0);
    EXPECT_EQ( tile.classify({ 25.,  25.}),    0);
    EXPECT_EQ( tile.classify({ 22.,  22.}),    0);
    EXPECT_EQ( tile.classify({ 30.,  30.}),   99);
    EXPECT_EQ( tile.classify({ 35.,  35.}),   99);
    EXPECT_EQ( tile.classify({ 36.,  36.}),   69);
    EXPECT_EQ( tile.classify({ 36.3, 36.3}),  69);
    // upper-right corner
    EXPECT_EQ( tile.classify({ 36.4, 35.4}),  42);
    EXPECT_EQ( tile.classify({ 36.5, 35.5}),  42);
    EXPECT_EQ( tile.classify({ 37.6, 37.6}),  42);
    EXPECT_EQ( tile.classify({ 37.,  37.}),   42);
    EXPECT_EQ( tile.classify({ 40.,  40.}),   42);

}

TEST(Tile, CacheRoundTrip) {
    ASSERT_EQ( sizeof(uint8_t), sizeof(std::byte) );
    ASSERT_EQ( sizeof(uint8_t*), sizeof(std::byte*) );

     // step 1:
    TileNode write_tile(default_anchor);
    write_tile.fill(default_tile_contents);

    // // DEBUG
    // cerr << write_tile->to_string() << endl;

    { // quickly test the original tile
        EXPECT_NEAR( write_tile.anchor.x(),  4.4, 1e-6);
        EXPECT_NEAR( write_tile.anchor.y(),  4.6, 1e-6);

        ASSERT_EQ( write_tile.classify({  0.,   0.}), 42);
        ASSERT_EQ( write_tile.classify({ 10.,  10.}), 99);
        ASSERT_EQ( write_tile.classify({ 20.,  20.}),  0);
        ASSERT_EQ( write_tile.classify({ 30.,  30.}), 99);
        ASSERT_EQ( write_tile.classify({ 40.,  40.}), 42);
    }

    // target first half: write to cache
    const std::byte* bufp = write_tile.cache_write();
    EXPECT_NE( nullptr, bufp );

    // Perform intermediate tests on the buffer
    auto tile_cache = GetTileCache( bufp );
    EXPECT_NEAR( tile_cache->x(), 4.4, 1e-6);
    EXPECT_NEAR( tile_cache->y(), 4.6, 1e-6);

    // // Not implemented.  Not sure how the builder knows how many total bytes are available, in this case.
    // EXPECT_EQ( tile_cache->GetSize(), 1072);  // serialized size, including packing

    // target second half: write to cache
    const auto read_tile = TileNode::make(bufp);
    ASSERT_TRUE( read_tile );  

    EXPECT_NEAR( read_tile->anchor.x(), 4.4, 1e-6);
    EXPECT_NEAR( read_tile->anchor.y(), 4.6, 1e-6);

    // // DEBUG
    // cerr << read_tile->to_string() << endl;

    { // quickly test the loaded tile
        EXPECT_EQ( read_tile->classify({  0.,   0.}), 42);
        EXPECT_EQ( read_tile->classify({ 10.,  10.}), 99);
        EXPECT_EQ( read_tile->classify({ 20.,  20.}),  0);
        EXPECT_EQ( read_tile->classify({ 30.,  30.}), 99);
        EXPECT_EQ( read_tile->classify({ 40.,  40.}), 42);
    }
}

TEST(Tile, JSONRoundTrip) {
    ASSERT_EQ( sizeof(uint8_t), sizeof(std::byte) );
    ASSERT_EQ( sizeof(uint8_t*), sizeof(std::byte*) );

     // step 1:
    TileNode write_tile(default_anchor);
    write_tile.fill(default_tile_contents);

    // // DEBUG
    // cerr << write_tile->to_string() << endl;

    { // quickly test the original tile
        EXPECT_NEAR( write_tile.anchor.x(),  4.4, 1e-6);
        EXPECT_NEAR( write_tile.anchor.y(),  4.6, 1e-6);

        ASSERT_EQ( write_tile.classify({  0.,   0.}), 42);
        ASSERT_EQ( write_tile.classify({ 10.,  10.}), 99);
        ASSERT_EQ( write_tile.classify({ 20.,  20.}),  0);
        ASSERT_EQ( write_tile.classify({ 30.,  30.}), 99);
        ASSERT_EQ( write_tile.classify({ 40.,  40.}), 42);
    }

    // target first half: write to cache
    const std::string& buffer = write_tile.to_json();

    // target second half: write to cache
    const auto read_tile = TileNode::make(buffer);
    ASSERT_TRUE( read_tile );  

    EXPECT_NEAR( read_tile->anchor.x(), 4.4, 1e-6);
    EXPECT_NEAR( read_tile->anchor.y(), 4.6, 1e-6);

    // // DEBUG
    // cerr << read_tile->to_string() << endl;

    { // quickly test the loaded tile
        EXPECT_EQ( read_tile->classify({  0.,   0.}), 42);
        EXPECT_EQ( read_tile->classify({ 10.,  10.}), 99);
        EXPECT_EQ( read_tile->classify({ 20.,  20.}),  0);
        EXPECT_EQ( read_tile->classify({ 30.,  30.}), 99);
        EXPECT_EQ( read_tile->classify({ 40.,  40.}), 42);
    }
}

// TEST(Tile, LoadPolygonFromVector) {
//     grid::Grid<cell_t,16> g({{8.,8.}, 8});
//     Terrain terrain(g);

//     EXPECT_EQ( g.size(), 256);
//     EXPECT_EQ( g.dim(),   16);
//     EXPECT_EQ( g.width(),  8);

//     EXPECT_DOUBLE_EQ( terrain.get_precision(), 0.5);

//     const auto& layout = terrain.get_layout();
//     EXPECT_DOUBLE_EQ( layout.center.x,    8.);
//     EXPECT_DOUBLE_EQ( layout.center.y,    8.);
//     EXPECT_DOUBLE_EQ( layout.half_width,  4.);

//     EXPECT_DOUBLE_EQ( layout.get_x_min(),   4.);
//     EXPECT_DOUBLE_EQ( layout.get_y_min(),   4.);
//     EXPECT_DOUBLE_EQ( layout.get_x_max(),  12.);
//     EXPECT_DOUBLE_EQ( layout.get_y_max(),  12.);

//     // reset grid
//     g.fill(99);


TEST(Tile, ToPNG) {
    // step 1:
    TileNode tile(default_anchor);
    tile.fill(default_tile_contents);

    // // DEBUG
    // cerr << tile->to_string() << endl; // DEBUG
    
    // const std::vector<std::byte> data = tile->to_raster();
    // const string outpath("test.tile.png");
    // ASSERT_TRUE( yggdrasil::io::to_png( data, outpath ) );
}

}; // namespace yggdrasil::node
