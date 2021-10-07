// GPL v3 (c) 2020, Daniel Williams 

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

#include <fmt/core.h>

#include "leaf-node.hpp"

// using chartbox::geometry::BoundBox;
using chartbox::geometry::LocalLocation;
// using chartbox::geometry::
 
using chartbox::layer::LeafNode;

namespace chartbox::layer {

// ============ ============ LeafNode Tests  ============ ============
TEST_CASE( "Leaf Node Simple Initialization" ){
    const LeafNode<8> leaf;
    
    // constexpr fields
    REQUIRE( leaf.dimension() == 8 );
    REQUIRE( leaf.size() == 64 );

    REQUIRE( leaf.scale() == Approx(1.0f) );
    REQUIRE( leaf.width() == Approx(8.0f) );
    REQUIRE( leaf.origin().x() == 0.0f );
    REQUIRE( leaf.origin().y() == 0.0f );
} // TEST_CASE

TEST_CASE( "Can construct 32-width LeafNode" ){
    const LeafNode<32> leaf;

    // constexpr fields
    REQUIRE( leaf.dimension() == 32 );
    REQUIRE( leaf.size() == 1024 );

    REQUIRE( leaf.scale() == Approx(1.0f) );
    REQUIRE( leaf.width() == Approx(32.0f) );
}

TEST_CASE( "Can construct 64-width LeafNode" ){
    const LeafNode<64> leaf;

    // constexpr fields
    REQUIRE( leaf.dimension() == 64 );
    REQUIRE( leaf.size() == 4096 );

    REQUIRE( leaf.scale() == Approx(1.0f) );
    REQUIRE( leaf.width() == Approx(64.0f) );
}

TEST_CASE( "Leaf Node initializes origin"){
    const LeafNode<8> leaf({ 2.3, 3.7 });

    REQUIRE( leaf.origin().x() == Approx(2.3) );
    REQUIRE( leaf.origin().y() == Approx(3.7) );
} // TEST_CASE

TEST_CASE( "Leaf Node can get values "){
    LeafNode<8> leaf({0,0});
    leaf.fill(5);
    const LocalLocation target{ 0.5, 0.5 };

    CHECK( leaf.get(target, 99) == 5 );
} // TEST_CASE

TEST_CASE( "Leaf Node can set values "){
    LeafNode<8> leaf({0,0});
    leaf.fill(5);
    const LocalLocation target{ 0.5, 0.5 };

    CHECK( leaf.get(target,99) == 5 );
    leaf.set( target, 8 );
    CHECK( leaf.get(target,99) == 8 );
    
} // TEST_CASE

}   // namespace chartbox::layer

static const std::array<uint8_t, 1024> leaf_test_data = {
        75, 74, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 73, 72,
        76, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 71,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        66, 67, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 68, 69,
        99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 
        99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0,  0,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 76, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 76, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
        65, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 70,
        66, 67, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 68, 69,
};

TEST_CASE( "Leaf Node Can initialize from data" ){
    LeafNode<32> leaf({ 4.4, 4.6 });
    leaf.fill( leaf_test_data.data(), leaf_test_data.size() );
    
    REQUIRE( leaf.scale() == Approx(1.0f) );
    REQUIRE( leaf.width() == Approx(32.0f) );
    REQUIRE( leaf.origin().x() == Approx(4.4) );
    REQUIRE( leaf.origin().y() == Approx(4.6) );

    // DEBUG
    // fmt::print( "{}\n", leaf.to_string() );

    REQUIRE( not leaf.contains({ 0.0, 0.0 }));      CHECK( 42 == leaf.get( {0.0, 0.0}, 42));
    REQUIRE( not leaf.contains({ 1.0, 1.0 }));      CHECK( 42 == leaf.get( {1.0, 1.0}, 42));
    REQUIRE( not leaf.contains({ 2.0, 2.0 }));      CHECK( 42 == leaf.get( {2.0, 2.0}, 42));
    REQUIRE( not leaf.contains({ 3.0, 3.0 }));      CHECK( 42 == leaf.get( {3.0, 3.0}, 42));
    REQUIRE( not leaf.contains({ 4.0, 4.0 }));      CHECK( 42 == leaf.get( {4.0, 4.0}, 42));
    REQUIRE( not leaf.contains({ 4.5, 4.5 }));      CHECK( 42 == leaf.get( {4.5, 4.5}, 42));
    // lower-left corner
    REQUIRE(     leaf.contains({  4.6,  4.6 }));    CHECK( 75 == static_cast<int>(leaf.get( { 4.6,  4.6}, 42)));
    REQUIRE(     leaf.contains({  5.0,  5.0 }));    CHECK( 75 == static_cast<int>(leaf.get( { 5.0,  5.0}, 42)));
    REQUIRE(     leaf.contains({ 10.0, 10.0 }));    CHECK( 99 == static_cast<int>(leaf.get( {10.0, 10.0}, 42)));
    REQUIRE(     leaf.contains({ 15.0, 15.0 }));    CHECK( 99 == static_cast<int>(leaf.get( {15.0, 15.0}, 42)));
    REQUIRE(     leaf.contains({ 18.0, 18.0 }));    CHECK(  0 == static_cast<int>(leaf.get( {18.0, 18.0}, 42)));
    REQUIRE(     leaf.contains({ 20.0, 20.0 }));    CHECK(  0 == static_cast<int>(leaf.get( {20.0, 20.0}, 42)));
    REQUIRE(     leaf.contains({ 22.0, 22.0 }));    CHECK(  0 == static_cast<int>(leaf.get( {22.0, 22.0}, 42)));
    REQUIRE(     leaf.contains({ 25.0, 25.0 }));    CHECK( 99 == static_cast<int>(leaf.get( {25.0, 25.0}, 42)));
    REQUIRE(     leaf.contains({ 30.0, 30.0 }));    CHECK( 99 == static_cast<int>(leaf.get( {30.0, 30.0}, 42)));
    REQUIRE(     leaf.contains({ 35.0, 35.0 }));    CHECK( 99 == static_cast<int>(leaf.get( {35.0, 35.0}, 42)));
    REQUIRE(     leaf.contains({ 36.0, 36.0 }));    CHECK( 69 == static_cast<int>(leaf.get( {36.0, 36.0}, 42)));
    REQUIRE(     leaf.contains({ 36.3, 36.3 }));    CHECK( 69 == static_cast<int>(leaf.get( {36.3, 36.3}, 42)));
    // upper-right corner
    REQUIRE( not leaf.contains({ 36.4, 36.4 }));    CHECK( 42 == static_cast<int>(leaf.get( {36.4, 36.4}, 42)));
    REQUIRE( not leaf.contains({ 36.5, 35.5 }));    CHECK( 42 == leaf.get( {36.5, 35.5}, 42));
    REQUIRE( not leaf.contains({ 37.6, 37.6 }));    CHECK( 42 == leaf.get( {37.6, 37.6}, 42));
    REQUIRE( not leaf.contains({ 37.0, 37.0 }));    CHECK( 42 == leaf.get( {37.0, 37.0}, 42));
    REQUIRE( not leaf.contains({ 40.0, 40.0 }));    CHECK( 42 == leaf.get( {40.0, 40.0}, 42));
}  // TEST_CASE
