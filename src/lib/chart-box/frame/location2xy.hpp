// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <Eigen/Core>

namespace chartbox::frame {

/// \brief Represents a generic Egocentric-Frame vector
///
/// Axes:
///     x - through the bow of the boat
///     y - through the port side of the boat
struct Location2xy {
    double x;
    double y;    

    Location2xy( double _x, double _y )
        : x(_x), y(_y)
    {}

    Location2xy( Eigen::Vector2d vec )
        : x(vec.x()), y(vec.y()) 
    {}


    double operator[](uint32_t index) const { 
        return (0==index?x:
                1==index?y:
                NAN);
    }

    Eigen::Vector2d toVector() const { return { x, y};}

};

} // namespace chart::frame