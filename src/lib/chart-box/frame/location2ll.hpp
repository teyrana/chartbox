// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <Eigen/Core>

namespace chartbox::frame {

/// \brief repesents a global latitude-longitude location.
struct Location2LL {
    double longitude;
    double latitude;

    Location2LL( double _lat, double _lon )
        : longitude(_lon), latitude(_lat) 
    {}

    Location2LL( Eigen::Vector2d vec )
        : longitude(vec.x()), latitude(vec.y()) 
    {}

    double operator[](uint32_t index) const { 
        return (0==index?longitude:
                1==index?latitude:
                NAN);
    }

    Eigen::Vector2d toVector() const { return { longitude, latitude }; }

};

} // namespace chart::frame