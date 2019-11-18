// GPL v3 (c) 2020

#include <cmath>

#include <gtest/gtest.h>

#include "node/quad_node.hpp"

using std::isnan;

namespace yggdrasil::quadtree {

TEST(QuadNodeTest, ConstructDefault) {
    Node n;
    
    ASSERT_TRUE( n.is_leaf() );
    ASSERT_EQ( n.northeast.get(), nullptr);
    ASSERT_EQ( n.northwest.get(), nullptr);
    ASSERT_EQ( n.southwest.get(), nullptr);
    ASSERT_EQ( n.southeast.get(), nullptr);

    ASSERT_EQ( n.get_value(), 0);
}

TEST(QuadNodeTest, ConstructWithValue) {
    Node n(0);
    
    ASSERT_TRUE( n.is_leaf() );
    ASSERT_EQ( n.northeast.get(), nullptr);
    ASSERT_EQ( n.northwest.get(), nullptr);
    ASSERT_EQ( n.southwest.get(), nullptr);
    ASSERT_EQ( n.southeast.get(), nullptr);

    ASSERT_EQ( n.get_value(), 0);
}

TEST(QuadNodeTest, SetGet) {
    Node n(22);
    ASSERT_TRUE( n.is_leaf() );
    ASSERT_EQ(n.get_value(), 22);

    ASSERT_TRUE( n.is_leaf() );
    ASSERT_EQ( n.northeast.get(), nullptr);
    ASSERT_EQ( n.northwest.get(), nullptr);
    ASSERT_EQ( n.southwest.get(), nullptr);
    ASSERT_EQ( n.southeast.get(), nullptr);
    
    n.set_value(24);

    ASSERT_TRUE( n.is_leaf() );
    ASSERT_EQ(n.get_value(), 24);
}

TEST(QuadNodeTest, OperatorEquals){
    Node n1(NAN);
    Node n2(NAN);
    ASSERT_TRUE( n1 == n1 );
    ASSERT_FALSE( n1 == n2 );
}

TEST(QuadNodeTest, SplitNodeImperative){
    Node n;

    ASSERT_TRUE(n.is_leaf());
    n.split();
    ASSERT_FALSE(n.is_leaf());

    ASSERT_TRUE( n.get_northeast()->is_leaf() );
    ASSERT_TRUE( n.get_northwest()->is_leaf() );
    ASSERT_TRUE( n.get_southeast()->is_leaf() );
    ASSERT_TRUE( n.get_southwest()->is_leaf() );
}

TEST(QuadNodeTest, SplitNodeConditional){
    Node n(NAN);

    ASSERT_TRUE( n.is_leaf());
    n.split(3, 4);
    ASSERT_FALSE(n.is_leaf());
    ASSERT_TRUE(n.get_northeast()->is_leaf());

    ASSERT_TRUE( n.get_northeast()->is_leaf() );
    ASSERT_TRUE( n.get_northwest()->is_leaf() );
    ASSERT_TRUE( n.get_southeast()->is_leaf() );
    ASSERT_TRUE( n.get_southwest()->is_leaf() );
}

} // namespace quadtree
