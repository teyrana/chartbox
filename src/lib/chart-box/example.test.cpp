// GPL v3 (c) 2020, Daniel Williams 

#include <cmath>
#include <cstddef>
#include <iostream>
#include <sstream>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
using Catch::Approx;

// #include "frame-mapping.hpp"
// using chartbox::frame::FrameMapping;


TEST_CASE( "ChartMapping", "[ConstructDefaultMapping:WGS84<=>UTM19F]" ) {

    // FrameMapping mpg;

    const double placeholder = 0.0f;
    CHECK( placeholder == Approx(0) );
    
    
    // CHECK( mpg.utm_bounds().min().x() == Approx(0).epsilon(__DBL_EPSILON__) );
    // CHECK( mpg.utm_bounds().min().y() == Approx(0).epsilon(__DBL_EPSILON__) );
    // CHECK( mpg.utm_bounds().max().x() == Approx(0).epsilon(__DBL_EPSILON__) );
    // CHECK( mpg.utm_bounds().max().y() == Approx(0).epsilon(__DBL_EPSILON__) );

}