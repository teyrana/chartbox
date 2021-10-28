// GPL v3 (c) 2021, Daniel Williams 

// #include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <memory>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
using Catch::Approx;

#include <fmt/core.h>

#include "geometry/bound-box.hpp"
#include "geometry/polygon.hpp"
#include "simple-grid-layer.hpp"

using chartbox::geometry::BoundBox;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::Polygon;
using chartbox::geometry::UTMLocation;
 
using chartbox::layer::simple::SimpleGridLayer;

// ============ ============  Static-Grid-Index-Tests  ============ ============
TEST_CASE( "SimpleGridLayer Default Template Initialization" ){
    SimpleGridLayer layer;

    REQUIRE( 1024 == layer.cells_across_view() );
    REQUIRE( 1.0 == Approx(layer.meters_across_cell()) );
} // TEST_CASE

TEST_CASE( "SimpleGridLayer Simple Template Initialization" ){
    SimpleGridLayer<uint32_t, 8, 16000> layer;

    REQUIRE( 8 == layer.cells_across_view() );
    REQUIRE( 16.0 == Approx(layer.meters_across_cell()) );
} // TEST_CASE

TEST_CASE( "SimpleGridLayer can test bounds" ){
    SimpleGridLayer<uint32_t,32,2000> layer;
    REQUIRE( 32 == layer.cells_across_view() );
    REQUIRE( 2.0 == Approx(layer.meters_across_cell()) );
    REQUIRE( LocalLocation(    0.0,    0.0) == layer.tracked().min );
    REQUIRE( LocalLocation(   64.0,   64.0) == layer.tracked().max );
    REQUIRE( LocalLocation(    0.0,    0.0) == layer.visible().min );
    REQUIRE( LocalLocation(   64.0,   64.0) == layer.visible().max );

    CHECK( not layer.visible( LocalLocation( -1.0, -1.0 )) );
    CHECK(     layer.visible( LocalLocation(  0.0,  0.0 )) );
    CHECK(     layer.visible( LocalLocation( 10.0, 10.0 )) );
    CHECK(     layer.visible( LocalLocation( 60.0, 60.0 )) );
    CHECK(     layer.visible( LocalLocation( 64.0, 64.0 )) );
    CHECK( not layer.visible( LocalLocation( 65.0, 65.0 )) );
}

TEST_CASE( "SimpleGridLayer can remember new bounds" ){
    SimpleGridLayer<uint32_t,32,1000> layer;
    layer.view( BoundBox<LocalLocation>( {8,16}, {40,48}) );
    REQUIRE( 32 == layer.cells_across_view() );
    REQUIRE( 1.0 == Approx(layer.meters_across_cell()) );
    REQUIRE( LocalLocation(    8.0,   16.0) == layer.tracked().min );
    REQUIRE( LocalLocation(   40.0,   48.0) == layer.tracked().max );
    REQUIRE( LocalLocation(    8.0,   16.0) == layer.visible().min );
    REQUIRE( LocalLocation(   40.0,   48.0) == layer.visible().max );

    CHECK( not layer.visible( LocalLocation(  5.0,  5.0 )) );
    CHECK(     layer.visible( LocalLocation( 20.0, 20.0 )) );
    CHECK(     layer.visible( LocalLocation( 30.0, 30.0 )) );
    CHECK(     layer.visible( LocalLocation( 40.0, 40.0 )) );
    CHECK(     layer.visible( LocalLocation( 40.0, 45.0 )) );
    CHECK( not layer.visible( LocalLocation( 50.0, 50.0 )) );
}

TEST_CASE( "SimpleGridLayer Construct from Uneven & Under-sized bounds" ){
    SimpleGridLayer<uint32_t,32,1000> layer;
    layer.view( BoundBox<LocalLocation>( {4,4}, {20,24}) );
    
    REQUIRE( 32 == layer.cells_across_view() );
    REQUIRE( 1.0 == Approx(layer.meters_across_cell()) );
    REQUIRE( LocalLocation(    4.0,    4.0) == layer.tracked().min );
    REQUIRE( LocalLocation(   36.0,   36.0) == layer.tracked().max );
    REQUIRE( LocalLocation(    4.0,    4.0) == layer.visible().min );
    REQUIRE( LocalLocation(   36.0,   36.0) == layer.visible().max );
}

TEST_CASE( "SimpleGridLayer Construct from Over-sized bounds" ){
    SimpleGridLayer<uint32_t,32,1000> layer;
    layer.view( BoundBox<LocalLocation>( {2,2}, {40,48}) );
    
    REQUIRE( 32 == layer.cells_across_view() );
    REQUIRE( 1.0 == Approx(layer.meters_across_cell()) );
    REQUIRE( LocalLocation(    0.0,    0.0) == layer.tracked().min );
    REQUIRE( LocalLocation(   32.0,   32.0) == layer.tracked().max );
    REQUIRE( LocalLocation(    0.0,    0.0) == layer.visible().min );
    REQUIRE( LocalLocation(   32.0,   32.0) == layer.visible().max );
}

TEST_CASE( "SimpleGridLayer can set and retrive " ){
    SimpleGridLayer<uint32_t, 8, 1000> layer;
    layer.fill( 0 );

    REQUIRE( 8 == layer.cells_across_view() );
    REQUIRE( 1.0 == Approx(layer.meters_across_cell()) );

    CHECK( 0 == layer.get({ 0.0, 0.0}));
    CHECK( 0 == layer.get({ 1.0, 1.0}));
    CHECK( 0 == layer.get({ 4.0, 6.0}));
    CHECK( 0 == layer.get({ 1.0, 7.0}));

} // TEST_CASE

TEST_CASE( "SimpleGridLayer Populates From Array" ){
    const std::array<uint32_t,8*8> contents {
        6666, 6667, 6668, 6669, 6670, 6671, 6672, 6673,
        6766, 6767, 6768,    0,    0,  0, 69, 70,
        6866, 6867, 6868,    0,    0, 69,  0, 70,
        6966, 6966, 6968, 6969, 6970,  0,  0, 70,
        7066, 7066, 7068, 7069, 7070,  0,  0, 71,
        7166, 7166, 7168, 7169,    0, 72,  0, 71,
        7266, 7266, 7268, 7269,    0,  0, 72, 71,
        7366, 7366, 7368, 7369,   73, 73, 73, 72};
    SimpleGridLayer<uint32_t, 8, 1000> layer;
    layer.fill( contents.data(), contents.size() );

    REQUIRE( 8 == layer.cells_across_view() );
    REQUIRE( 1.0 == Approx(layer.meters_across_cell()) );

    CHECK( 6666 == layer.get({ 0.5, 0.5}));
    CHECK( 6667 == layer.get({ 1.5, 0.5}));
    CHECK( 6668 == layer.get({ 2.5, 0.5}));
    CHECK( 6669 == layer.get({ 3.5, 0.5}));

    CHECK( 6767 == layer.get({ 1.5, 1.5}));
    CHECK( 6868 == layer.get({ 2.5, 2.5}));
    CHECK( 6969 == layer.get({ 3.5, 3.5}));
    CHECK( 7070 == layer.get({ 4.5, 4.5}));

} // TEST_CASE
