// GPL v3 (c) 2020, Daniel Williams 

#ifndef _SAMPLE_HPP_
#define _SAMPLE_HPP_

#include <cmath>

#include "local-location.hpp"

namespace chart::geometry {

template <typename cell_value_t> struct Sample {
  public:
    const LocalLocation at;
    const cell_value_t is;
};

} // namespace chart::geometry

#endif // _SAMPLE_HPP_
