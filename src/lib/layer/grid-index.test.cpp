// GPL v3 (c) 2021, Daniel Williams 

#include <cmath>
#include <iostream>
#include <sstream>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
using Catch::Approx;

#include "grid-index.hpp"

using chartbox::layer::GridIndex;

// ============ ============  Rolling-Grid-Index-Tests  ============ ============
TEST_CASE( "GridIndex Default Initialization" ){
    const GridIndex index;
    CHECK( 0 == index.column );
    CHECK( 0 == index.row );
} // TEST_CASE

TEST_CASE( "GridIndex Simple Initialization" ){
    const GridIndex index(1,2);
    CHECK( 1 == index.column );
    CHECK( 2 == index.row );
} // TEST_CASE

TEST_CASE( "GridIndex Simple Assignment" ){
    GridIndex index;
    index.column = 1;
    index.row = 2;

    CHECK( 1 == index.column );
    CHECK( 2 == index.row );
} // TEST_CASE

TEST_CASE( "RollingGridIndex calculates storage offset "){
    CHECK(  0 == GridIndex(0,0).offset(4) );
    CHECK(  1 == GridIndex(1,0).offset(4) );
    CHECK(  2 == GridIndex(2,0).offset(4) );
    CHECK(  3 == GridIndex(3,0).offset(4) );
    CHECK(  4 == GridIndex(0,1).offset(4) );
    CHECK(  5 == GridIndex(1,1).offset(4) );
    CHECK( 10 == GridIndex(2,2).offset(4) );
    CHECK( 15 == GridIndex(3,3).offset(4) );
} // TEST_CASE

TEST_CASE( "GridIndex can mod indices "){
    const auto rem = GridIndex(4,9) % 2;
    CHECK( 0 == rem.column );
    CHECK( 1 == rem.row );
} // TEST_CASE

TEST_CASE( "GridIndex can add indices "){
    const auto sum = GridIndex(1,1) + GridIndex(1,1);
    CHECK( 2 == sum.column );
    CHECK( 2 == sum.row );
} // TEST_CASE


TEST_CASE( "GridIndex can multiply indices "){
    const auto prod = GridIndex(2,3) * 2;
    CHECK( 4 == prod.column );
    CHECK( 6 == prod.row );
} // TEST_CASE

TEST_CASE( "GridIndex can subtract indices "){
    const auto diff = GridIndex(1,2) - GridIndex(1,1);

    CHECK( 0 == diff.column );
    CHECK( 1 == diff.row );
} // TEST_CASE

TEST_CASE( "GridIndex can divide indices "){
    const auto div = GridIndex(4,8) / 2;
    // WARNING -- this implementation is probably wrong!  
    // .... Or at the very last, not relevant to our use case
    CHECK( 2 == div.column );
    CHECK( 4 == div.row );
} // TEST_CASE
