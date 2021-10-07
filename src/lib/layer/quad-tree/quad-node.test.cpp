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

#include "quad-node.hpp"

using chartbox::geometry::LocalLocation;
 
using chartbox::layer::QuadNode;

// ============ ============ LeafNode Tests  ============ ============
TEST_CASE( "QuadNode Default Initialization" ){
    QuadNode n;

    CHECK( n.is_leaf() );
    CHECK( nullptr == n.northeast );
    CHECK( nullptr == n.northwest );
    CHECK( nullptr == n.southwest );
    CHECK( nullptr == n.southeast );
 
    CHECK( n.get() == 0);  // check default value
} // TEST_CASE

TEST_CASE( "QuadNode Simple Initialization" ){
    QuadNode n(17);

    CHECK( n.is_leaf() );
    CHECK( nullptr == n.northeast );
    CHECK( nullptr == n.northwest );
    CHECK( nullptr == n.southwest );
    CHECK( nullptr == n.southeast );
 
    CHECK( 17 == n.get() );
}

TEST_CASE( "QuadNode Equality tests value equality" ){
    QuadNode n1(17);
    QuadNode n2(17);
    CHECK(n1 == n2);
}

TEST_CASE( "QuadNode will explicity splits and join" ){
    QuadNode n;

    REQUIRE( n.is_leaf() );
    n.split();
    REQUIRE( not n.is_leaf() );

    CHECK( n.northeast->is_leaf() );
    CHECK( n.northwest->is_leaf() );
    CHECK( n.southeast->is_leaf() );
    CHECK( n.southwest->is_leaf() );

    n.join();
    REQUIRE( n.is_leaf() );
    CHECK( nullptr == n.northeast );
    CHECK( nullptr == n.northwest );
    CHECK( nullptr == n.southwest );
    CHECK( nullptr == n.southeast );
}