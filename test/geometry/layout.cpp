// GPL v3 (c) 2020

#include <cmath>
#include <iostream>

#include "gtest/gtest.h"

#include <nlohmann/json.hpp>

#include "geometry/layout.hpp"

using std::cerr;
using std::endl;
using std::isnan;
using yggdrasil::geometry::Layout;

TEST(LayoutTest, ConstructDefault) {
    Layout layout;

    EXPECT_DOUBLE_EQ(layout.get_precision(), 1.0);
    EXPECT_DOUBLE_EQ(layout.get_x(), 0);
    EXPECT_DOUBLE_EQ(layout.get_y(), 0);
    EXPECT_DOUBLE_EQ(layout.get_width(), 1.0);

    EXPECT_EQ( layout.get_padding(),         64);
    EXPECT_EQ( layout.get_dimension(),        1);
    EXPECT_DOUBLE_EQ(layout.get_half_width(), 0.5);
    EXPECT_EQ( layout.get_size(),             1);

    EXPECT_DOUBLE_EQ(layout.get_center()[0],  0);
    EXPECT_DOUBLE_EQ(layout.get_center()[1],  0);

    EXPECT_DOUBLE_EQ(layout.get_anchor()[0], -0.5);
    EXPECT_DOUBLE_EQ(layout.get_anchor()[1], -0.5);

    EXPECT_DOUBLE_EQ(layout.get_x_max(),      0.5);
    EXPECT_DOUBLE_EQ(layout.get_x_min(),     -0.5);
    EXPECT_DOUBLE_EQ(layout.get_y_max(),      0.5);
    EXPECT_DOUBLE_EQ(layout.get_y_min(),     -0.5);

    // i.e. memory size:
    EXPECT_EQ( sizeof(layout), 64);
}

TEST(LayoutTest, ConstructExplicitValues) {
    Layout layout(1.,   // precision
                  3,    // center.x
                  1,    // center.y
                  2.);  // width

    EXPECT_DOUBLE_EQ(layout.get_precision(),   1.);
    EXPECT_DOUBLE_EQ(layout.get_x(),           3.);
    EXPECT_DOUBLE_EQ(layout.get_y(),           1.);
    EXPECT_DOUBLE_EQ(layout.get_width(),       2.);

    EXPECT_EQ(layout.get_padding(),           62);
    EXPECT_EQ( layout.get_dimension(),         2);
    EXPECT_DOUBLE_EQ(layout.get_half_width(),  1.);
    EXPECT_EQ( layout.get_size(),              4);
    EXPECT_DOUBLE_EQ(layout.get_center().x(),  3.);
    EXPECT_DOUBLE_EQ(layout.get_center().y(),  1.);
    EXPECT_DOUBLE_EQ(layout.get_anchor().x(),  2.);
    EXPECT_DOUBLE_EQ(layout.get_anchor().y(),  0.);
}

TEST(LayoutTest, ConstructExplicitEvenNumbers) {
    Layout layout(2., 0, 0, 8);

    EXPECT_DOUBLE_EQ(layout.get_precision(), 2.0);
    EXPECT_DOUBLE_EQ(layout.get_x(), 0);
    EXPECT_DOUBLE_EQ(layout.get_y(), 0);
    EXPECT_DOUBLE_EQ(layout.get_width(), 8.0);
}

TEST(LayoutTest, ConstructExplicitUneven_p2_w5) {
    Layout layout(2., 0, 0, 5.);

    EXPECT_DOUBLE_EQ(layout.get_precision(),  2.);
    EXPECT_DOUBLE_EQ(layout.get_x(),          0.);
    EXPECT_DOUBLE_EQ(layout.get_y(),          0.);
    EXPECT_DOUBLE_EQ(layout.get_width(),      8.0);

    EXPECT_EQ(layout.get_padding(),          60);
    EXPECT_EQ(layout.get_dimension(),         4);
    EXPECT_EQ(layout.get_size(),             16);
}

TEST(LayoutTest, ConstructExplictUneven_p04_w16) {
    Layout layout(0.4, 8, 8, 16.);

    EXPECT_DOUBLE_EQ(layout.get_precision(),  1.0);
    EXPECT_DOUBLE_EQ(layout.get_x(),          8.);
    EXPECT_DOUBLE_EQ(layout.get_y(),          8.);
    EXPECT_DOUBLE_EQ(layout.get_width(),     16.0);

    EXPECT_EQ(layout.get_padding(),          56);
    EXPECT_EQ( layout.get_dimension(),       16);
    EXPECT_EQ( layout.get_size(),           256);
}

TEST(LayoutTest, ConstructExplictUneven_p7_w32) {
    Layout layout(7, 0, 0, 32.);

    EXPECT_DOUBLE_EQ(layout.get_precision(),  4.0);
    EXPECT_DOUBLE_EQ(layout.get_x(),          0.);
    EXPECT_DOUBLE_EQ(layout.get_y(),          0.);
    EXPECT_DOUBLE_EQ(layout.get_width(),     32.0);

    EXPECT_EQ(layout.get_padding(),         58);
    EXPECT_EQ(layout.get_dimension(),        8);
    EXPECT_EQ(layout.get_size(),            64);
}

TEST(LayoutTest, ConstructExplictUneven_p32_w4096) {
    Layout layout(32, 0, 0, 4096);

    EXPECT_DOUBLE_EQ(layout.get_precision(), 32.0);
    EXPECT_DOUBLE_EQ(layout.get_x(),          0.);
    EXPECT_DOUBLE_EQ(layout.get_y(),          0.);
    EXPECT_DOUBLE_EQ(layout.get_width(),   4096.0);

    EXPECT_EQ(layout.get_padding(),          50);
    EXPECT_EQ(layout.get_dimension(),       128);
    EXPECT_EQ(layout.get_size(),          16384);
}

TEST(LayoutTest, ConstructExplictUneven_p40_w4096) {
    Layout layout(40, 0, 0, 4096);

    EXPECT_DOUBLE_EQ(layout.get_precision(), 32.0);
    EXPECT_DOUBLE_EQ(layout.get_x(),          0.);
    EXPECT_DOUBLE_EQ(layout.get_y(),          0.);
    EXPECT_DOUBLE_EQ(layout.get_width(),   4096.0);

    EXPECT_EQ(layout.get_padding(),          50);
    EXPECT_EQ(layout.get_dimension(),       128);
    EXPECT_EQ(layout.get_size(),          16384);
}

TEST(LayoutTest, ContainPoints) {
    Layout l(1.,   // precision
             2,    // center.x
             3,    // center.y
             2.);  // width
    // x bounds -> [1, 3]
    // y bounds -> [2, 4]

    EXPECT_TRUE(  l.contains({  2  , 3  }));
    EXPECT_TRUE(  l.contains({  1.5, 2.5}));
    EXPECT_TRUE(  l.contains({  2.5, 2.5}));
    EXPECT_TRUE(  l.contains({  1.5, 3.5}));
    EXPECT_TRUE(  l.contains({  2.5, 3.5}));
    
    // borders contain points _on_ the borders
    EXPECT_TRUE( l.contains({2.0, 2.0}));
    EXPECT_TRUE( l.contains({1.0, 3.0}));

    EXPECT_FALSE( l.contains({0  , 0  }));
    EXPECT_FALSE( l.contains({1.0, 1.0}));
    
    EXPECT_FALSE( l.contains({2.0, 1.5}));
    EXPECT_FALSE( l.contains({2.0, 4.5}));
    EXPECT_FALSE( l.contains({0.5, 3.0}));
    EXPECT_FALSE( l.contains({3.5, 3.0}));
}

TEST(LayoutTest, LimitX) {
    Layout l(1.,   // precision
             2,    // center.x
             3,    // center.y
             2.);  // width
    // x bounds -> [1, 3]

    EXPECT_DOUBLE_EQ(  l.limitx(  0.5),  1.);
    EXPECT_DOUBLE_EQ(  l.limitx(  0.99), 1.);
    EXPECT_DOUBLE_EQ(  l.limitx(  1.0),  1.);
    EXPECT_DOUBLE_EQ(  l.limitx(  1.01), 1.01);
    EXPECT_DOUBLE_EQ(  l.limitx(  1.5),  1.5);
    
    EXPECT_DOUBLE_EQ(  l.limitx(  2.5),  2.5);
    EXPECT_DOUBLE_EQ(  l.limitx(  2.99), 2.99);
    EXPECT_DOUBLE_EQ(  l.limitx(  3.0),  3.);
    EXPECT_DOUBLE_EQ(  l.limitx(  3.01), 3.);
    EXPECT_DOUBLE_EQ(  l.limitx(  3.5),  3.);
}

TEST(LayoutTest, LimitY) {
    Layout l(1.,   // precision
             2,    // center.x
             3,    // center.y
             2.);  // width
    // y bounds -> [2, 4]

    EXPECT_DOUBLE_EQ(  l.limity(  1.5),  2.);
    EXPECT_DOUBLE_EQ(  l.limity(  1.99), 2.);
    EXPECT_DOUBLE_EQ(  l.limity(  2.0),  2.);
    EXPECT_DOUBLE_EQ(  l.limity(  2.01), 2.01);
    EXPECT_DOUBLE_EQ(  l.limity(  2.5),  2.5);
    
    EXPECT_DOUBLE_EQ(  l.limity(  3.5),  3.5);
    EXPECT_DOUBLE_EQ(  l.limity(  3.99), 3.99);
    EXPECT_DOUBLE_EQ(  l.limity(  4.0),  4.0);
    EXPECT_DOUBLE_EQ(  l.limity(  4.01), 4.0);
    EXPECT_DOUBLE_EQ(  l.limity(  4.5),  4.0);

}

TEST(LayoutTest, ManufactureFromJSON) {
    nlohmann::json source = {{"precision", 1},
                             {"x", 7.},
                             {"y", 8.},
                             {"width", 2.}};

    // vvvv Target vvvv
    auto opt = Layout::make_from_json(source);
    // ^^^^ Target ^^^^

    ASSERT_TRUE(opt);

    Layout& layout = *opt;
    EXPECT_DOUBLE_EQ(layout.get_precision(),  1.);
    EXPECT_DOUBLE_EQ(layout.get_x(),          7.);
    EXPECT_DOUBLE_EQ(layout.get_y(),          8.);
    EXPECT_DOUBLE_EQ(layout.get_width(),      2.);

    EXPECT_EQ(layout.get_padding(),          62);
    EXPECT_EQ(layout.get_dimension(),         2);
    EXPECT_EQ(layout.get_size(),              4);
}

TEST(LayoutTest, HashBy_RowMajor_p1_w4) {
    ASSERT_EQ( sizeof(uint64_t), 8);  // should be guaranteed, by spec
    ASSERT_EQ( sizeof(size_t), 8);    // not strictly necessary

    Layout layout(1.,  2.,  2., 4);

    EXPECT_DOUBLE_EQ(layout.get_precision(),  1.);
    EXPECT_DOUBLE_EQ(layout.get_x(),          2.);
    EXPECT_DOUBLE_EQ(layout.get_y(),          2.);
    EXPECT_DOUBLE_EQ(layout.get_width(),      4.);
    EXPECT_EQ(layout.get_dimension(),         4.);

    EXPECT_EQ( layout.hashr(0.5, 0.5),  0);
    EXPECT_EQ( layout.hashr(1.5, 0.5),  1);
    EXPECT_EQ( layout.hashr(2.5, 0.5),  2);
    EXPECT_EQ( layout.hashr(3.5, 0.5),  3);

    ASSERT_EQ( layout.hashr(0.5, 1.5),  4);
    ASSERT_EQ( layout.hashr(1.5, 1.5),  5);
    ASSERT_EQ( layout.hashr(2.5, 1.5),  6);
    ASSERT_EQ( layout.hashr(3.5, 1.5),  7);

    ASSERT_EQ( layout.hashr(0.5, 2.5),  8);
    ASSERT_EQ( layout.hashr(1.5, 2.5),  9);
    ASSERT_EQ( layout.hashr(2.5, 2.5), 10);
    ASSERT_EQ( layout.hashr(3.5, 2.5), 11);

    ASSERT_EQ( layout.hashr(0.5, 3.5), 12);
    ASSERT_EQ( layout.hashr(1.5, 3.5), 13);
    ASSERT_EQ( layout.hashr(2.5, 3.5), 14);
    ASSERT_EQ( layout.hashr(3.5, 3.5), 15);
}

TEST(LayoutTest, HashBy_RowMajor_p05_w8) {
    ASSERT_EQ( sizeof(uint64_t), 8);  // should be guaranteed, by spec
    ASSERT_EQ( sizeof(size_t), 8);    // not strictly necessary

    Layout layout(0.5,  4.,  4., 8.);

    EXPECT_DOUBLE_EQ(layout.get_precision(),  1.);
    EXPECT_DOUBLE_EQ(layout.get_x(),          4.);
    EXPECT_DOUBLE_EQ(layout.get_y(),          4.);
    EXPECT_DOUBLE_EQ(layout.get_width(),      8.);
    EXPECT_EQ(layout.get_dimension(),         8);

    EXPECT_EQ( layout.hashr(0.25, 0.25),  0);
    EXPECT_EQ( layout.hashr(0.75, 0.25),  0);
    EXPECT_EQ( layout.hashr(1.25, 0.25),  1);
    EXPECT_EQ( layout.hashr(1.75, 0.25),  1);

    ASSERT_EQ( layout.hashr(0.25, 0.75),  0);
    ASSERT_EQ( layout.hashr(0.75, 0.75),  0);
    ASSERT_EQ( layout.hashr(1.25, 0.75),  1);
    ASSERT_EQ( layout.hashr(1.75, 0.75),  1);

    ASSERT_EQ( layout.hashr(2.5, 0.5), 2);
    ASSERT_EQ( layout.hashr(3.5, 0.5), 3);

    ASSERT_EQ( layout.hashr(2.5, 1.5), 10);
    ASSERT_EQ( layout.hashr(3.5, 1.5), 11);
}

TEST(LayoutTest, XHashCenter) {
    const Layout l(1., 0., 0., 32.);

    ASSERT_DOUBLE_EQ(l.get_precision(),  1.);
    ASSERT_DOUBLE_EQ(l.get_x(),          0.);
    ASSERT_DOUBLE_EQ(l.get_y(),          0.);
    ASSERT_DOUBLE_EQ(l.get_half_width(), 16.);

    EXPECT_EQ(l.hashx(-25.),  0); // <- out-of-bounds. undefined behavior.
    EXPECT_EQ(l.hashx(-20.),  0); // <- out-of-bounds. undefined behavior.
    EXPECT_EQ(l.hashx(-16.5), 0); // <- out-of-bounds. undefined behavior.

    EXPECT_EQ(l.hashx(-16.0), 0); // <- actual first cell
    EXPECT_EQ(l.hashx(-15.8), 0); // <- actual first cell
    EXPECT_EQ(l.hashx(-15.5), 0); // <- actual first cell
    EXPECT_EQ(l.hashx(-14.5), 1);
    EXPECT_EQ(l.hashx(-13.5), 2);
    EXPECT_EQ(l.hashx(-12.5), 3);
    EXPECT_EQ(l.hashx(-11.5), 4);
    EXPECT_EQ(l.hashx(-10.5), 5);
    EXPECT_EQ(l.hashx( -5.5), 10);
    EXPECT_EQ(l.hashx( -0.5), 15);
    EXPECT_EQ(l.hashx(  0.5), 16);
    EXPECT_EQ(l.hashx(  5.5), 21);
    EXPECT_EQ(l.hashx( 10.5), 26);
    EXPECT_EQ(l.hashx( 11.5), 27);
    EXPECT_EQ(l.hashx( 12.5), 28);
    EXPECT_EQ(l.hashx( 13.5), 29);
    EXPECT_EQ(l.hashx( 14.5), 30);
    EXPECT_EQ(l.hashx( 15.5), 31); // <- actual last cell
    EXPECT_EQ(l.hashx( 15.9), 31); // <- actual last cell

    EXPECT_EQ(l.hashx( 16.0), 31); // <- out-of-bounds (rounding). undefined behavior.
    EXPECT_EQ(l.hashx( 16.5), 31); // <- out-of-bounds. undefined behavior.
}

TEST(LayoutTest, YHashCenter) {
    const Layout l({ 1., 0., 0., 32});

    ASSERT_DOUBLE_EQ(l.get_precision(),  1.);
    ASSERT_DOUBLE_EQ(l.get_x(),          0.);
    ASSERT_DOUBLE_EQ(l.get_y(),          0.);
    ASSERT_DOUBLE_EQ(l.get_half_width(), 16.);

    EXPECT_EQ(l.hashy(-20.),  0); // <- out-of-bounds. undefined behavior.
    EXPECT_EQ(l.hashy(-16.5), 0); // <- out-of-bounds. undefined behavior.

    EXPECT_EQ(l.hashy(-16.0), 0); // <- actual first cell
    EXPECT_EQ(l.hashy(-15.8), 0); // <- actual first cell
    EXPECT_EQ(l.hashy(-15.5), 0); // <- actual first cell
    EXPECT_EQ(l.hashy(-14.5), 1);
    EXPECT_EQ(l.hashy(-13.5), 2);
    EXPECT_EQ(l.hashy(-12.5), 3);
    EXPECT_EQ(l.hashy(-11.5), 4);
    EXPECT_EQ(l.hashy(-10.5), 5);
    EXPECT_EQ(l.hashy( -5.5), 10);
    EXPECT_EQ(l.hashy( -0.5), 15);
    EXPECT_EQ(l.hashy(  0.5), 16);
    EXPECT_EQ(l.hashy(  5.5), 21);
    EXPECT_EQ(l.hashy( 10.5), 26);
    EXPECT_EQ(l.hashy( 11.5), 27);
    EXPECT_EQ(l.hashy( 12.5), 28);
    EXPECT_EQ(l.hashy( 13.5), 29);
    EXPECT_EQ(l.hashy( 14.5), 30);
    EXPECT_EQ(l.hashy( 15.5), 31); // <- actual last cell
    EXPECT_EQ(l.hashy( 15.9), 31); // <- actual last cell

    EXPECT_EQ(l.hashy( 16.0), 31); // <- out-of-bounds (rounding). undefined behavior.
    EXPECT_EQ(l.hashy( 16.5), 31); // <- out-of-bounds. undefined behavior.
}

TEST(LayoutTest, YHashOffset) {
    const Layout l({ 1.0, 68., 68., 64});

    ASSERT_DOUBLE_EQ(l.get_precision(),   1.);
    ASSERT_DOUBLE_EQ(l.get_x(),          68.);
    ASSERT_DOUBLE_EQ(l.get_y(),          68.);
    ASSERT_DOUBLE_EQ(l.get_half_width(), 32.);

    // 68 +-32 => [ 36, 100]
    EXPECT_EQ(l.hashx(30.5), 0); // <- out-of-bounds. undefined behavior.
    EXPECT_EQ(l.hashx(34.5), 0); // <- out-of-bounds. undefined behavior.
    EXPECT_EQ(l.hashy(35.9), 0); // <- out-of-bounds. undefined behavior.
    EXPECT_EQ(l.hashy(36.0), 0); // <- out-of-bounds. undefined behavior.

    EXPECT_EQ(l.hashy(36.1), 0); // <- actual first cell
    EXPECT_EQ(l.hashy(36.5), 0); // <- actual first cell
    EXPECT_EQ(l.hashy(37.5), 1);
    EXPECT_EQ(l.hashy(38.6), 2);
    EXPECT_EQ(l.hashy(39.5), 3);
    EXPECT_EQ(l.hashy(40.5), 4);
    EXPECT_EQ(l.hashy(45.5), 9);
    EXPECT_EQ(l.hashy(50.5), 14);
    EXPECT_EQ(l.hashy(55.5), 19);
    EXPECT_EQ(l.hashy(60.5), 24);
    EXPECT_EQ(l.hashy(65.5), 29);
    EXPECT_EQ(l.hashy(70.5), 34);
    EXPECT_EQ(l.hashy(75.5), 39);
    EXPECT_EQ(l.hashy(80.5), 44);
    EXPECT_EQ(l.hashy(85.5), 49);
    EXPECT_EQ(l.hashy(90.5), 54);
    EXPECT_EQ(l.hashy(95.5), 59);
    EXPECT_EQ(l.hashy(96.5), 60);
    EXPECT_EQ(l.hashy(97.5), 61);
    EXPECT_EQ(l.hashy(98.5), 62);
    EXPECT_EQ(l.hashy(99.1), 63); // <- actual last cell
    EXPECT_EQ(l.hashy(99.5), 63); // <- actual last cell
    EXPECT_EQ(l.hashy(99.9), 63); // <- actual last cell

    EXPECT_EQ(l.hashy(100.), 63); // <- out-of-bounds (rounding). undefined behavior.
    EXPECT_EQ(l.hashy(101.), 63); // <- out-of-bound. undefined behavior.
}

TEST(LayoutTest, HashBy_ZOrderCurve) {
    ASSERT_EQ( sizeof(uint64_t), 8);  // should be guaranteed, by spec
    ASSERT_EQ( sizeof(size_t), 8);    // not strictly necessary

    Layout layout(1,  2,  2, 4);

    EXPECT_EQ(layout.get_padding(),             60);
    EXPECT_DOUBLE_EQ(layout.get_precision(),     1.);
    EXPECT_DOUBLE_EQ(layout.get_x(),             2.);
    EXPECT_DOUBLE_EQ(layout.get_y(),             2.);
    EXPECT_DOUBLE_EQ(layout.get_width(),         4.);
    EXPECT_EQ(layout.get_dimension(),            4);

    EXPECT_EQ( layout.hashz(0.5, 0.5), 0x0000000000000000);
    EXPECT_EQ( layout.hashz(1.5, 0.5), 0x1000000000000000);
    ASSERT_EQ( layout.hashz(0.5, 1.5), 0x2000000000000000);
    ASSERT_EQ( layout.hashz(1.5, 1.5), 0x3000000000000000);
    
    EXPECT_EQ( layout.hashz(2.5, 0.5), 0x4000000000000000);
    EXPECT_EQ( layout.hashz(3.5, 0.5), 0x5000000000000000);
    ASSERT_EQ( layout.hashz(2.5, 1.5), 0x6000000000000000);
    ASSERT_EQ( layout.hashz(3.5, 1.5), 0x7000000000000000);

    ASSERT_EQ( layout.hashz(0.5, 2.5), 0x8000000000000000);
    ASSERT_EQ( layout.hashz(1.5, 2.5), 0x9000000000000000);
    ASSERT_EQ( layout.hashz(0.5, 3.5), 0xa000000000000000);
    ASSERT_EQ( layout.hashz(1.5, 3.5), 0xb000000000000000);

    ASSERT_EQ( layout.hashz(2.5, 2.5), 0xc000000000000000);
    ASSERT_EQ( layout.hashz(3.5, 2.5), 0xd000000000000000);
    ASSERT_EQ( layout.hashz(2.5, 3.5), 0xe000000000000000);
    ASSERT_EQ( layout.hashz(3.5, 3.5), 0xf000000000000000);
}
