// GPL v3 (c) 2020, Daniel Williams 

#include <iostream>
#include <memory>
#include <vector>

#include "gtest/gtest.h"

#include "geometry/path.hpp"


using chart::geometry::Path;

TEST(Path, CreateDefault) {
    const Path path;

    ASSERT_EQ( path.size(), 0 );
    ASSERT_TRUE( path.empty() );
}

TEST(Path, CreateWithInitializer){
    Path path = {{0,0}, {2,2}, {2,0}, {3,0}};

    ASSERT_EQ( path.size(), 4);
    ASSERT_NEAR( path.length(), 5.82842712, 1e-8);
}

TEST(Path, CreateAndEmplaceBack ) {
    Path path;

    ASSERT_TRUE( path.empty() );
    ASSERT_EQ( path.size(), 0 );

    path.emplace_back(0,0);
    ASSERT_FALSE( path.empty() );
    ASSERT_EQ( path.size(), 1 );
    ASSERT_NEAR( path.length(), 0, 1e-8);

    path.emplace_back(2,2);
    ASSERT_EQ( path.size(), 2 );
    ASSERT_NEAR( path.length(), 2.82842712, 1e-8);

    path.emplace_back(2,0);
    ASSERT_EQ( path.size(), 3 );
    ASSERT_NEAR( path.length(), 4.82842712, 1e-8);

    path.emplace_back(3,0);
    ASSERT_EQ( path.size(), 4 );
    ASSERT_NEAR( path.length(), 5.82842712, 1e-8);
}

TEST(Path, CreateAndPushBack ) {
    Path path;

    ASSERT_TRUE( path.empty() );
    ASSERT_EQ( path.size(), 0 );

    path.push_back({0,0});

    ASSERT_FALSE( path.empty() );
    ASSERT_EQ( path.size(), 1 );
    ASSERT_NEAR( path.length(), 0, 1e-8);
}

TEST(Path, CreateFromVector) {
    Path path({{0,0}, {1,0}, {1,1}, {0,1}});

    ASSERT_EQ( path.size(), 4);
    ASSERT_DOUBLE_EQ( path.length(), 3.);

    ASSERT_TRUE( path[0].isApprox( Vector2d(0,0) ));
    ASSERT_TRUE( path[1].isApprox( Vector2d(1,0) ));
    ASSERT_TRUE( path[2].isApprox( Vector2d(1,1) ));
    ASSERT_TRUE( path[3].isApprox( Vector2d(0,1) ));
}

TEST(Path, CreateFromInitializer){
    const Path path = {{0,0}, {1,0}, {1,1}, {0,1}};

    ASSERT_EQ( path.size(), 4);
    ASSERT_DOUBLE_EQ( path.length(), 3.);

    ASSERT_TRUE( path[0].isApprox( Vector2d(0,0) ));
    ASSERT_TRUE( path[1].isApprox( Vector2d(1,0) ));
    ASSERT_TRUE( path[2].isApprox( Vector2d(1,1) ));
    ASSERT_TRUE( path[3].isApprox( Vector2d(0,1) ));
}

TEST(Path, GetSegments){
    Path path = {{0,0}, {1,0}, {1,1}, {0,1}};

    ASSERT_EQ( path.size(), 4);

    const auto& seg0 = path.segment(0);
    ASSERT_DOUBLE_EQ( seg0.x(), 1.);
    ASSERT_DOUBLE_EQ( seg0.y(), 0.);

    const auto& seg1 = path.segment(1);
    ASSERT_DOUBLE_EQ( seg1.x(), 0.);
    ASSERT_DOUBLE_EQ( seg1.y(), 1.);

    const auto& seg2 = path.segment(2);
    ASSERT_DOUBLE_EQ( seg2.x(), -1.);
    ASSERT_DOUBLE_EQ( seg2.y(),  0.);
}

TEST(Path, IteratePoints){
    Path path = {{0,0}, {1,0}, {1,1}, {0,1}};

    auto iter = path.begin();
    ASSERT_TRUE( *iter == path[0] );

    ++iter;
    ASSERT_TRUE( *iter == path[1] );

    ++iter;
    ASSERT_TRUE( *iter == path[2] );

    ++iter;
    ASSERT_TRUE( *iter == path[3] );
}
