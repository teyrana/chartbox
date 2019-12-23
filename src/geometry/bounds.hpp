// GPL v3 (c) 2020

#ifndef _BOUNDS_HPP_
#define _BOUNDS_HPP_

#include <Eigen/Geometry>
using Eigen::Vector2d;

namespace yggdrasil::geometry {

constexpr size_t snap_power_2( const size_t target);

class CornerBounds {
private:
    const Vector2d min;
    const Vector2d max;

public:
    Bounds(const vector2d _min, const vector2d _max ): 
        min(_min), max(_max) 
    {}

    const vector2d dimensions() const { return {max.x() - min.x(), max.y() - min.y()}; }
    const double width() const { return {max.x() - min.x(), max.y() - min.y()}.maxCoef(); }

};

class WidthBounds {
public:
    const Vector2d anchor;
    const double width;
    
public:
    Bounds(const vector2d _min, const vector2d _max ): 
        min(_min), width(max.getmax()) 
    {}

    const vector2d dimensions() const { return {max.x() - min.x(), max.y() - min.y()}; }
    
};

};
#endif // #ifndef _BOUNDS_HPP_
