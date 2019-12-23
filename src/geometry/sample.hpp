// GPL v3 (c) 2020

#ifndef _SAMPLE_HPP_
#define _SAMPLE_HPP_

#include <cmath>

#include <Eigen/Geometry>

namespace yggdrasil::geometry {

template <typename cell_value_t>
struct Sample {
public:
    const Eigen::Vector2d at;
    const cell_value_t is;
};

} // namespace yggdrasil

#endif // _SAMPLE_HPP_
