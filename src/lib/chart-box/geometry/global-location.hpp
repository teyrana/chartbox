// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <Eigen/Core>

namespace chartbox::geometry {

/// \brief repesents a global latitude-longitude location.
struct GlobalLocation {
    double longitude;
    double latitude;

    GlobalLocation()
        : longitude(NAN), latitude(NAN)
    {}

    GlobalLocation( double _lat, double _lon )
        : longitude(_lon), latitude(_lat) 
    {}

    GlobalLocation( Eigen::Vector2d vec )
        : longitude(vec.x()), latitude(vec.y()) 
    {}

    bool nearby( const GlobalLocation& other, double tolerance = 0.001 ) const { 
        return ( tolerance > std::fabs( latitude - other.latitude)
               && tolerance > std::fabs( longitude - other.longitude));
    }

    double operator[](uint32_t index) const { 
        return (0==index?longitude:
                1==index?latitude:
                NAN);
    }

    bool operator==( const GlobalLocation& other ) const { 
        return this->nearby(other);
    }

    void set( uint32_t index, double new_value ){
        switch( index ){
            case 0: longitude = new_value; return;
            case 1: latitude = new_value; return;
            default: return;
        }
    }

    Eigen::Vector2d to_vector() const { return { longitude, latitude }; }

};

} // namespace chart::geometry