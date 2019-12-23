// GPL v3 (c) 2020

#ifndef _GRID_PATTERN_HPP_
#define _GRID_PATTERN_HPP_

#include <Eigen/Geometry>
using Eigen::Vector2d;

#include "geometry/index.hpp"

namespace yggdrasil::geometry {

constexpr size_t snap_power_2( const size_t target);

template<size_t dimension, size_t width>
class GridPattern {
public:
    constexpr GridPattern();

    constexpr inline size_t get_dimension() const { return dimension; }
    constexpr inline double get_precision() const { return width/dimension; }
    constexpr inline size_t get_size() const { return dimension*dimension; }
    constexpr inline double get_width() const { return width; }

private:
    // ========= ========= ========= Private Methods ========= ========= =========
    /// \brief converts x into it's corresponding integer index
    /// \warning out-of-bounds calls are _undefined behavior_!
    constexpr uint32_t index_x_axis( const double x) const  { return static_cast<uint32_t>( x / width * dimension); }

    // constexpr double limit_x( const double x) const;
    // constexpr double limit_y( const double y) const;

    /// \brief convertrs y into it's corresponding integer index
    /// \warning out-of-bounds calls are _undefined behavior_!
    constexpr uint32_t index_y_axis( const double y) const { return static_cast<uint32_t>( y / width * dimension); }

    /// \brief snaps this precision to match the next-power-of-2 dimension that covers the precision
    ///
    /// Note:  Precision <= 1.0
    // constexpr double snap_precision(const double precision);

    // constexpr double snap_width( const double width);

public:
    // used for comparisons
    constexpr static double epsilon = 1e-6;
    
    // no member variables
    // no static member variables
    // >> implicit template variables (see above)
};

#include "grid-pattern.inl"

// mostly for tests
typedef GridPattern <   4,   4>    GridPattern4;
typedef GridPattern <  16,  16>   GridPattern16;
typedef GridPattern <  32,  32>   GridPattern32;
typedef GridPattern <  64,  64>   GridPattern64;
typedef GridPattern < 128, 128>  GridPattern128;
typedef GridPattern < 256, 256>  GridPattern256;
typedef GridPattern < 512, 512>  GridPattern512;
typedef GridPattern <1024,1024> GridPattern1024;
typedef GridPattern <2048,2048> GridPattern2048;
typedef GridPattern <4096,4096> GridPattern4096;

} // namespace yggdrasil::geometry

#endif // #ifdef _GRID_PATTERN_HPP_
