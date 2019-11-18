// GPL v3 (c) 2020

#ifndef _SAMPLE_HPP_
#define _SAMPLE_HPP_

#include <cmath>

#include <Eigen/Geometry>

#include "cell_value.hpp"


namespace yggdrasil::geometry {

struct Sample {
public:
    const Eigen::Vector2d at;
    const yggdrasil::geometry::cell_value_t is;
};

} // namespace yggdrasil

#endif // _SAMPLE_HPP_
