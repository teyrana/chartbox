// GPL v3 (c) 2020, Daniel Williams 

#include <cmath>
#include <iostream>
#include <sstream>

#include <gtest/gtest.h>

#include "geometry/bounds.hpp"
#include "geometry/path.hpp"

#include "grid/fixed-grid.hpp"
#include "search/a-star.hpp"

using std::cerr;
using std::endl;
using std::isnan;
using std::string;

using chart::geometry::Path;
using chart::grid::FixedGrid1k;
using chart::search::RRTStar;

TEST( SearchRRTStar, ConstructDefault64) {
    FixedGrid1k g;
    RRTStar<FixedGrid1k> search(g);
}

TEST(SearchRRTStar, DirectPath){
    FixedGrid1k g(Bounds({0.5,0.5}, {32.5,32.5}));  // start with a 1-cell-to-1-meter spacing.
    RRTStar<FixedGrid1k> search(g);

    g.fill(' ');

    Path foundPath = search.compute( {2,2}, {24,24} );

    // // DEBUG
    // cerr << g.to_string() << endl;

    // // debug
    // g.fill(foundPath, 'o');

    // // DEBUG
    // cerr << g.to_string() << endl;

    ASSERT_FALSE( foundPath.empty() );
    ASSERT_EQ( foundPath.size(), 12);

    const auto& p0 = foundPath[0];
    ASSERT_DOUBLE_EQ( p0.x(), 2);
    ASSERT_DOUBLE_EQ( p0.y(), 2);

    const auto& p5 = foundPath[5];
    ASSERT_DOUBLE_EQ( p5.x(), 12);
    ASSERT_DOUBLE_EQ( p5.y(), 12);

    const auto& p8 = foundPath[8];
    ASSERT_DOUBLE_EQ( p8.x(), 18);
    ASSERT_DOUBLE_EQ( p8.y(), 18);

    const auto& p9 = foundPath[9];
    ASSERT_DOUBLE_EQ( p9.x(), 20);
    ASSERT_DOUBLE_EQ( p9.y(), 20);

    const auto& pf = foundPath[11];
    ASSERT_DOUBLE_EQ( pf.x(), 24);
    ASSERT_DOUBLE_EQ( pf.y(), 24);
}


// a 32x32 grid of cells, containing a few interesting islands
static const std::vector<uint8_t> islands = chart::geometry::vflip<uint8_t>({
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
}, 32);

TEST(SearchRRTStar, RouteIslands){
    FixedGrid1k g(Bounds({0.5,0.5}, {32.5,32.5}));  // start with a 1-cell-to-1-meter spacing.
    ASSERT_DOUBLE_EQ( g.precision(), 1.0 );

    RRTStar<FixedGrid1k> search(g);

    ASSERT_EQ( g.size(), islands.size() );

    ASSERT_EQ( 0, g.fill(islands) );

    // // DEBUG
    // cerr << g.to_string() << endl;

    Path foundPath = search.compute( {6,6}, {24,30} );
    ASSERT_FALSE( foundPath.empty() );

    // // debug
    // g.fill(foundPath, 'o');

    // // DEBUG
    // cerr << g.to_string() << endl;

    ASSERT_EQ( foundPath.size(), 25);

    const auto& p0 = foundPath[0];
    ASSERT_DOUBLE_EQ(  p0.x(),  6);
    ASSERT_DOUBLE_EQ(  p0.y(),  6);

    const auto& p5 = foundPath[5];
    ASSERT_DOUBLE_EQ(  p5.x(), 15);
    ASSERT_DOUBLE_EQ(  p5.y(), 13);

    const auto& p8 = foundPath[8];
    ASSERT_DOUBLE_EQ(  p8.x(), 11);
    ASSERT_DOUBLE_EQ(  p8.y(), 18);

    const auto& p9 = foundPath[9];
    ASSERT_DOUBLE_EQ(  p9.x(),  9);
    ASSERT_DOUBLE_EQ(  p9.y(), 20);

    const auto& p12 = foundPath[12];
    ASSERT_DOUBLE_EQ( p12.x(),  8);
    ASSERT_DOUBLE_EQ( p12.y(), 24);

    const auto& p20 = foundPath[20];
    ASSERT_DOUBLE_EQ( p20.x(), 19);
    ASSERT_DOUBLE_EQ( p20.y(), 27);

    const auto& pf = foundPath[24];
    ASSERT_DOUBLE_EQ( pf.x(), 24);
    ASSERT_DOUBLE_EQ( pf.y(), 30);
}
