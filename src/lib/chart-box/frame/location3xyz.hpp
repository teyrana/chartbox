// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <Eigen/Geometry>

namespace chartbox::frame {

/// \brief Represents a generic Egocentric-Frame vector
///
/// Axes:
///     x - through the bow of the boat
///     y - through the port side of the boat
///     z - upwards (altitude)
struct Location3xyz {
    double x;
    double y;    
    double z;

    Location2xy( double _x, double _y, double _z )
        : x(_x), y(_y), z(_z)
    {}

    Location2xy( Eigen::vector3d vec )
        : x(vec.x()), y(vec.y()), z(vec.z())
    {}

    Eigen::Vector3d toVector() const { return {x,y,z}; }

};

} // namespace chart::frame