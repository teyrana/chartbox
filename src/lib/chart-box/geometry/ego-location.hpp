// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <Eigen/Geometry>

namespace chartbox::geometry {

/// \brief Represents an Egocentric-Frame vector
///
/// Axes:
///     x - through the bow of the boat
///     y - through the port side of the boat
///     z - upwards (altitude)
struct EgoLocation {
    double x;
    double y;
    double z;
    double roll;
    double pitch; 
    double yaw;

    EgoLocation( double _x, double _y, double _z )
        : x(_x), y(_y), z(_z)
    {}

    EgoLocation( Eigen::vector3d vec )
        : x(vec.x()), y(vec.y()), z(vec.z())
    {}

    Eigen::Vector3d to_vector() const { return {x,y,z}; }

};

} // namespace chart::geometry