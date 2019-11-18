// GPL v3 (c) 2020

#include <cmath>
#include <cstddef>
#include <iostream>
#include <sstream>

#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

#include "geometry/polygon.hpp"
#include "io/readers.hpp"
#include "io/writers.hpp"
#include "node/tile_node.hpp"
#include "node/tile_cache_generated.h"

using std::cerr;
using std::endl;
using std::isnan;
using std::string;

using Eigen::Vector2d;
using nlohmann::json;


namespace yggdrasil::node {

TEST(TileTest, ConstructDefault) {
    Tile1k tile;

    EXPECT_EQ( tile.size(), 1024);
    EXPECT_EQ( tile.dimension(), 32);

    const auto& layout = tile.get_layout();
    EXPECT_DOUBLE_EQ( layout.get_precision(),  1.);
    EXPECT_DOUBLE_EQ( layout.get_x(),          0.);
    EXPECT_DOUBLE_EQ( layout.get_y(),          0.);
    EXPECT_DOUBLE_EQ( layout.get_width(),     32.);
    EXPECT_DOUBLE_EQ( layout.get_dimension(), 32.);
    EXPECT_DOUBLE_EQ( layout.get_size(),    1024.);
    EXPECT_DOUBLE_EQ( layout.get_x_min(),    -16.);
    EXPECT_DOUBLE_EQ( layout.get_y_min(),    -16.);
    EXPECT_DOUBLE_EQ( layout.get_x_max(),     16.);
    EXPECT_DOUBLE_EQ( layout.get_y_max(),     16.);
}

TEST(TileTest, ConstructWithSizeSpacingCenter) {
    Tile1k tile({ 1., 3., 3., 4. });

    EXPECT_EQ( tile.size(), 1024);
    EXPECT_EQ( tile.dimension(), 32);

    const auto& layout = tile.get_layout();
    EXPECT_DOUBLE_EQ( layout.get_precision(), 1.0);
    EXPECT_DOUBLE_EQ( layout.get_x(), 3.);
    EXPECT_DOUBLE_EQ( layout.get_y(), 3.);
    EXPECT_DOUBLE_EQ( layout.get_width(), 4.);
    EXPECT_DOUBLE_EQ( layout.get_half_width(), 2.);
    EXPECT_DOUBLE_EQ( layout.get_x_min(), 1.);
    EXPECT_DOUBLE_EQ( layout.get_y_min(), 1.);
    EXPECT_DOUBLE_EQ( layout.get_x_max(), 5.);
    EXPECT_DOUBLE_EQ( layout.get_y_max(), 5.);

}

static const json up_arrow_grid = {
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99}};

TEST(TileTest, LoadGridFromJSON) {
    Tile1k tile;

    EXPECT_EQ( tile.size(), 1024);
    EXPECT_EQ( tile.dimension(), 32);

    const auto& layout = tile.get_layout();
    EXPECT_DOUBLE_EQ( layout.get_x(),       0.);
    EXPECT_DOUBLE_EQ( layout.get_y(),       0.);
    EXPECT_DOUBLE_EQ( layout.get_width(),  32.);
    EXPECT_DOUBLE_EQ( layout.get_x_min(), -16.);
    EXPECT_DOUBLE_EQ( layout.get_y_min(), -16.);
    EXPECT_DOUBLE_EQ( layout.get_x_max(),  16.);
    EXPECT_DOUBLE_EQ( layout.get_y_max(),  16.);

    // test target
    EXPECT_TRUE(yggdrasil::io::load_grid_from_json(tile, up_arrow_grid));

    // // DEBUG
    // yggdrasil::io::to_console(tile);

    ASSERT_EQ( tile.size(), 1024);
    ASSERT_EQ( tile.dimension(), 32);
    
    
}

TEST(TileTest, CacheRoundTrip) {
    ASSERT_EQ( sizeof(uint8_t), sizeof(std::byte) );
    ASSERT_EQ( sizeof(uint8_t*), sizeof(std::byte*) );

    Tile1k write_tile({ 2., 4., 4., 64 });

    // setup
    EXPECT_TRUE(yggdrasil::io::load_grid_from_json(write_tile, up_arrow_grid));

    // target first half: write to cache
    std::byte* bufp = write_tile.cache_write();
    EXPECT_NE( nullptr, bufp );

    // Perform intermediate tests on the buffer
    auto tile_cache = GetTileCache( bufp );
    EXPECT_EQ( tile_cache->dimension(), 32);
    EXPECT_DOUBLE_EQ( tile_cache->width(), 64.);
    EXPECT_DOUBLE_EQ( tile_cache->x(), 4.);
    EXPECT_DOUBLE_EQ( tile_cache->y(), 4.);
    EXPECT_EQ( tile_cache->grid()->size(), 1024 );

    // // DEBUG
    // yggdrasil::io::to_console( write_tile );

    // // Not implemented.  Not sure how the builder knows how many total bytes are available, in this case.
    // EXPECT_EQ( tile_cache->GetSize(), 1072);  // serialized size, including packing

    // target second half: write to cache
    Tile1k read_tile;
    read_tile.fill(0);
    bool success = read_tile.cache_read( bufp );
    EXPECT_TRUE( success );

    const auto& layout = read_tile.get_layout();
    EXPECT_DOUBLE_EQ( layout.get_precision(), 2.);
    EXPECT_DOUBLE_EQ( layout.get_x(),       4.);
    EXPECT_DOUBLE_EQ( layout.get_y(),       4.);
    EXPECT_DOUBLE_EQ( layout.get_width(),  64.);
    EXPECT_DOUBLE_EQ( layout.get_y_min(), -28.);
    EXPECT_DOUBLE_EQ( layout.get_x_min(), -28.);
    EXPECT_DOUBLE_EQ( layout.get_x_max(),  36.);
    EXPECT_DOUBLE_EQ( layout.get_y_max(),  36.);

    // // DEBUG
    // yggdrasil::io::to_console( read_tile );
}

// TEST(TileTest, XYToIndex_OriginCentered) {
//     Tile1k tile;

//     EXPECT_EQ( tile.size(), 1024);
//     EXPECT_EQ( tile.dimension(), 32);

//     const auto& l = g.get_layout();
//     EXPECT_DOUBLE_EQ( l.get_precision(), 1.);
//     EXPECT_DOUBLE_EQ( l.get_half_width(), 32.);
//     EXPECT_DOUBLE_EQ( l.get_x(), 0.);
//     EXPECT_DOUBLE_EQ( l.get_y(), 0.);

//     EXPECT_EQ(l.x_to_index(-33.),  0); // <- past-the-post attempt

//     EXPECT_EQ(g.x_to_index(1.0),  0); // <- actual first cell
//     EXPECT_EQ(g.x_to_index(1.1),  0); // <- actual first cell
//     EXPECT_EQ(g.x_to_index(1.49), 0); // <- actual first cell
//     EXPECT_EQ(g.x_to_index(1.6),  1);
//     EXPECT_EQ(g.x_to_index(2.1),  2);
//     EXPECT_EQ(g.x_to_index(2.6),  3);
//     EXPECT_EQ(g.x_to_index(3.1),  4);
//     EXPECT_EQ(g.x_to_index(3.6),  5);
//     EXPECT_EQ(g.x_to_index(4.1),  6);
//     EXPECT_EQ(g.x_to_index(4.6),  7);
//     EXPECT_EQ(g.x_to_index(5.1),  8);
//     EXPECT_EQ(g.x_to_index(5.6),  9);
//     EXPECT_EQ(g.x_to_index(6.1), 10);
//     EXPECT_EQ(g.x_to_index(6.6), 11);
//     EXPECT_EQ(g.x_to_index(7.1), 12);
//     EXPECT_EQ(g.x_to_index(7.6), 13);
//     EXPECT_EQ(g.x_to_index(8.1), 14);
//     EXPECT_EQ(g.x_to_index(8.5), 15); // <- actual last cell 
//     EXPECT_EQ(g.x_to_index(8.6), 15); // <- actual last cell 
//     EXPECT_EQ(g.x_to_index(8.99), 15); // <- actual last cell 
//     EXPECT_EQ(g.x_to_index(9.1), 15); // <- past-the-post attempt
// }

// TEST(TileTest, XYToIndex_Offset) {
//     Tile1k tile({ 0.1, 800., 800., 1024});

//     EXPECT_EQ( tile.size(), 1024);
//     EXPECT_EQ( tile.dimension(), 32);

//     const auto& l = g.get_layout();
//     EXPECT_DOUBLE_EQ( l.get_precision(), 1.0);
//     EXPECT_DOUBLE_EQ( l.half_width, 32.);
//     EXPECT_DOUBLE_EQ( l.center.x, 800.);
//     EXPECT_DOUBLE_EQ( l.center.y, 800.);


//     EXPECT_EQ(l.x_to_index(0.5),  0); // <- past-the-post attempt

//     EXPECT_EQ(g.x_to_index(1.0),  0); // <- actual first cell
//     EXPECT_EQ(g.x_to_index(1.1),  0); // <- actual first cell
//     EXPECT_EQ(g.x_to_index(1.49),  0); // <- actual first cell
//     EXPECT_EQ(g.x_to_index(1.6),  1);
//     EXPECT_EQ(g.x_to_index(2.1),  2);
//     EXPECT_EQ(g.x_to_index(2.6),  3);
//     EXPECT_EQ(g.x_to_index(3.1),  4);
//     EXPECT_EQ(g.x_to_index(3.6),  5);
//     EXPECT_EQ(g.x_to_index(4.1),  6);
//     EXPECT_EQ(g.x_to_index(4.6),  7);
//     EXPECT_EQ(g.x_to_index(5.1),  8);
//     EXPECT_EQ(g.x_to_index(5.6),  9);
//     EXPECT_EQ(g.x_to_index(6.1), 10);
//     EXPECT_EQ(g.x_to_index(6.6), 11);
//     EXPECT_EQ(g.x_to_index(7.1), 12);
//     EXPECT_EQ(g.x_to_index(7.6), 13);
//     EXPECT_EQ(g.x_to_index(8.1), 14);
//     EXPECT_EQ(g.x_to_index(8.5), 15); // <- actual last cell 
//     EXPECT_EQ(g.x_to_index(8.6), 15); // <- actual last cell 
//     EXPECT_EQ(g.x_to_index(8.99), 15); // <- actual last cell 
//     EXPECT_EQ(g.x_to_index(9.1), 15); // <- past-the-post attempt
// }


// TEST(TileTest, LoadFromJSONDocument) {
//     json source = {{"layout", {{"x", 4.5}, {"y", 4.5}, {"width", 9}}},
//                    {"precision", 1},
//                    {"grid",{{99, 99, 99, 99,  0, 99, 99, 99, 99},
//                             {99, 99, 99,  0,  0,  0, 99, 99, 99},
//                             {99, 99,  0,  0,  0,  0,  0, 99, 99},
//                             {99,  0,  0,  0,  0,  0,  0,  0, 99},
//                             { 0,  0,  0,  0,  0,  0,  0,  0,  0},
//                             {99, 99, 99,  0,  0,  0, 99, 99, 99},
//                             {99, 99, 99,  0,  0,  0, 99, 99, 99},
//                             {99, 99, 99,  0,  0,  0, 99, 99, 99},
//                             {99, 99, 99,  0,  0,  0, 99, 99, 99}}}};
//     json& layout_doc = source["layout"];
//     // json& precision_doc = source["precision"];
//     json& grid_doc = source["grid"];

//     string source_json_string = source.dump();

//     constexpr size_t dim = 9;
//     layout b;
//     ASSERT_TRUE(b.load(layout_doc));
//     Grid<cell_value_t,dim> g(b);
//     Terrain terrain(g);

//     // step 1:
//     g.load_layout(layout_doc);
//     const auto& layout = terrain.get_layout();
//     EXPECT_DOUBLE_EQ( layout.center.x,    4.5);
//     EXPECT_DOUBLE_EQ( layout.center.y,    4.5);
//     EXPECT_DOUBLE_EQ( layout.half_width,  4.5);
//     EXPECT_DOUBLE_EQ( layout.get_x_min(),   0.);
//     EXPECT_DOUBLE_EQ( layout.get_y_min(),   0.);
//     EXPECT_DOUBLE_EQ( layout.get_x_max(),   9.);
//     EXPECT_DOUBLE_EQ( layout.get_y_max(),   9.);

//     // // step 2:
//     // const size_t precision = precision_doc.get<size_t>();
    
//     EXPECT_EQ( g.size(), 81);
//     EXPECT_EQ( g.width(), 9);
//     EXPECT_DOUBLE_EQ( terrain.get_precision(), 1.0);

//     // step 3:
//     ASSERT_TRUE(g.load_grid(grid_doc));

//     // eventual goal
//     // terrain.load(source_json_string);

//     // DEBUG
//     terrain.debug();

//     ASSERT_EQ( g(0,0), 99);
//     ASSERT_EQ( g(1,1), 99);
//     ASSERT_EQ( g(2,2), 99);
//     ASSERT_EQ( g(2,3), 99);
//     ASSERT_EQ( g(2,4),  0);

//     ASSERT_EQ( g(3,0),  0);
//     ASSERT_EQ( g(3,1),  0);
//     ASSERT_EQ( g(3,2),  0);
//     ASSERT_EQ( g(3,3),  0);
//     ASSERT_EQ( g(3,4),  0);
//     ASSERT_EQ( g(3,5),  0);
//     ASSERT_EQ( g(3,6),  0);
//     ASSERT_EQ( g(3,7),  0);
//     ASSERT_EQ( g(3,8), 99);

//     ASSERT_EQ( g(0,8), 99);
//     ASSERT_EQ( g(1,7), 99);
//     ASSERT_EQ( g(2,6),  0);

// }

// TEST(TileTest, LoadPolygonFromVector) {
//     grid::Grid<cell_value_t,16> g({{8.,8.}, 8});
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

// TEST( TileTest, LoadValidlayoutFromJSON){
//     grid::Grid g;
//     Terrain terrain(g);

//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_precision(), 1.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_x(),         0.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_y(),         0.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_width(),     1.);

//     // construct a valid document, with correct fields, but missing required fields:
//     std::istringstream source(R"({"layout": {"precision": 1.0, "x": 100, "y": 100, "width": 64}} )");
//     // this should fail. gracefully.
//     EXPECT_FALSE(terrain.load_from_json_stream(source));

//     // these tests should be *exactly* the same as before the 'load' call
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_precision(), 1.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_x(),         0.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_y(),         0.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_width(),     1.);
// }

// TEST(TileTest, LoadGridFromJSON) {
//     grid::Grid g;
//     Terrain terrain(g);
//     std::istringstream stream(R"(
//         {"layout": {"precision": 1.0, "x": 4, "y": 4, "width": 8},
//          "grid": [[88, 88, 88, 88, 88, 88, 88, 88],
//                   [88, 88, 88,  0,  0, 88, 88, 88],
//                   [88, 88,  0,  0,  0,  0, 88, 88],
//                   [88,  0,  0,  0,  0,  0,  0, 88],
//                   [88, 88, 88, 88,  0,  0,  0, 88],
//                   [88, 88, 88, 88,  0,  0, 88, 88],
//                   [88, 88, 88, 88,  0, 88, 88, 88],
//                   [88, 88, 88, 88, 88, 88, 88, 88]]})");

//     nlohmann::json source  = {{242, 242, 242, 242,  0, 242, 242, 242, 242},
//                               {242, 242, 242,   0,  0,   0, 242, 242, 242},
//                               {242, 242,   0,   0,  0,   0,   0, 242, 242},
//                               {242,   0,   0,   0,  0,   0,   0,   0, 242},
//                               {  0,   0,   0,   0,  0,   0,   0,   0,   0},
//                               {242,   0,   0,   0,  0, 242, 242, 242, 242},
//                               {242, 242,   0,   0,  0, 242, 242, 242, 242},
//                               {242, 242, 242,   0,  0, 242, 242, 242, 242},
//                               {242, 242, 242, 242,  0, 242, 242, 242, 242}};
//     EXPECT_DOUBLE_EQ( terrain.get_precision(), 1.0);
	
//     const auto& layout = terrain.get_layout();
//     EXPECT_DOUBLE_EQ( layout.center.x,    4.5);
//     EXPECT_DOUBLE_EQ( layout.center.y,    4.5);
//     EXPECT_DOUBLE_EQ( layout.half_width,  4.5);

//     EXPECT_DOUBLE_EQ( layout.get_x_min(), 0.0);
//     EXPECT_DOUBLE_EQ( layout.get_y_min(), 0.0);
//     EXPECT_DOUBLE_EQ( layout.get_x_max(), 9.0);
//     EXPECT_DOUBLE_EQ( layout.get_y_max(), 9.0);

    
//     // Test Target
//     ASSERT_TRUE(terrain.load_from_json_stream(stream));
//     // Test Target
    
//     // // DEBUG
//     // terrain.debug();

//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_precision(), 1.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_x(),         4.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_y(),         4.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_width(),     8.);

//     EXPECT_EQ( terrain.get_layout().get_dimension(), 8);
//     EXPECT_EQ( terrain.get_layout().get_size(),     64);

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

// TEST(TileTest, FillGradient) {
//     grid::Grid<cell_value_t,512> g({{0.,0.}, 256});
//     Terrain terrain(g);

//     EXPECT_EQ( g.size(), 512*512);
//     EXPECT_EQ( g.width(), 256);

//     std::istringstream stream(source.dump());

//     const auto& layout = terrain.get_layout();
//     EXPECT_DOUBLE_EQ( layout.center.x,       0.);
//     EXPECT_DOUBLE_EQ( layout.center.y,       0.);
//     EXPECT_DOUBLE_EQ( layout.width(),      256.);

//     // // DEBUG
//     // terrain.debug();

//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_precision(), 1.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_x(),         8.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_y(),         8.);
//     EXPECT_DOUBLE_EQ( terrain.get_layout().get_width(),    16.);

//     EXPECT_EQ( terrain.get_layout().get_dimension(), 16);
//     EXPECT_EQ( terrain.get_layout().get_size(),     256);

//     ASSERT_EQ( g.get_cell(4,15), 0x99);
//     ASSERT_EQ( g.get_cell(4,14), 0x99);
//     ASSERT_EQ( g.get_cell(4,13), 0x99);
//     ASSERT_EQ( g.get_cell(4,12),    0);
//     ASSERT_EQ( g.get_cell(4,11),    0);
//     ASSERT_EQ( g.get_cell(4, 9),    0);
//     ASSERT_EQ( g.get_cell(4, 8),    0);
//     ASSERT_EQ( g.get_cell(4, 7),    0);
//     ASSERT_EQ( g.get_cell(4, 6),    0);
//     ASSERT_EQ( g.get_cell(4, 5),    0);
//     ASSERT_EQ( g.get_cell(4, 4),    0);
//     ASSERT_EQ( g.get_cell(4, 3),    0);
//     ASSERT_EQ( g.get_cell(4, 2), 0x99);
//     ASSERT_EQ( g.get_cell(4, 1), 0x99);
//     ASSERT_EQ( g.get_cell(4, 0), 0x99);

//     ASSERT_EQ( g.get_cell( 0, 5), 0x99);
//     ASSERT_EQ( g.get_cell( 1, 5), 0x99);
//     ASSERT_EQ( g.get_cell( 2, 5),    0);
//     ASSERT_EQ( g.get_cell( 3, 5),    0);
//     ASSERT_EQ( g.get_cell( 4, 5),    0);
//     ASSERT_EQ( g.get_cell( 5, 5),    0);
//     ASSERT_EQ( g.get_cell( 6, 5),    0);
//     ASSERT_EQ( g.get_cell( 7, 5),    0);
//     ASSERT_EQ( g.get_cell( 8, 5),    0);
//     ASSERT_EQ( g.get_cell( 9, 5),    0);
//     ASSERT_EQ( g.get_cell(10, 5),    0);
//     ASSERT_EQ( g.get_cell(11, 5),    0);
//     ASSERT_EQ( g.get_cell(12, 5),    0);
//     ASSERT_EQ( g.get_cell(13, 5), 0x99);
//     ASSERT_EQ( g.get_cell(14, 5), 0x99);
//     ASSERT_EQ( g.get_cell(15, 5), 0x99);
// }


}; // namespace yggdrasil::node
