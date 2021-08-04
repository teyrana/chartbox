// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <Eigen/Core>

namespace chartbox::frame {


/// \brief Represents a generic UTM-Frame location (in any UTM sector)
///
/// An ENU frame, representing a generic UTM frame (not tied to a specific UTM sector
/// It neither supplies no restricts the UTM sector.
struct Location2EN {
    double easting;
    double northing;

    Location2EN( double east, double north ) 
        : easting(east), northing(north) 
    {}

    Location2EN( const Eigen::Vector2d& vec )
        : easting(vec.x()), northing(vec.y()) 
    {}

    double operator[](uint32_t index) const { 
        return (0==index?easting:
                1==index?northing:
                NAN);
    }

    Eigen::Vector2d toVector() const { return {easting, northing}; }

};

} // namespace chart::frame