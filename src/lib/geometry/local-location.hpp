// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <cmath>

namespace chartbox::geometry {


/// \brief Represents a generic UTM-Frame location (in any UTM sector)
///
/// An ENU frame, representing a generic UTM frame (not tied to a specific UTM sector
/// It neither supplies no restricts the UTM sector.
struct LocalLocation {
    double easting;
    double northing;

    LocalLocation()
        : easting(NAN), northing(NAN)
    {}

    LocalLocation( double value ) 
        : easting(value), northing(value) 
    {}

    LocalLocation( double east, double north ) 
        : easting(east), northing(north) 
    {}

    bool nearby( const LocalLocation& other, double tolerance = 0.01 ) const { 
        return ( tolerance > std::fabs( easting - other.easting) 
               && tolerance > std::fabs( northing - other.northing) );
    }

    LocalLocation operator*( double factor ) const {
        return { easting * factor, northing * factor };
    }

    LocalLocation operator+( const LocalLocation& other ) const {
        return { easting + other.easting, northing + other.northing };
    }

    LocalLocation operator+( double other ) const {
        return { easting + other, northing + other };
    }

    LocalLocation operator-( const LocalLocation& other ) const {
        return { easting - other.easting, northing - other.northing };
    }

    LocalLocation operator/( double divisor ) const {
        return { easting / divisor, northing / divisor };
    }

    bool operator==( const LocalLocation& other ) const {
        return this->nearby(other);
    }

    double operator[](uint32_t index) const { 
        return (0==index?easting:
                1==index?northing:
                NAN);
    }

    void set( uint32_t index, double new_value ){
        switch( index ){
            case 0: easting = new_value; return;
            case 1: northing = new_value; return;
            default: return;
        }
    }


    inline double x() const { return easting; }

    inline double y() const { return northing; }

};

inline std::ostream& operator<< (std::ostream& os, const LocalLocation& ll) {
    os << "{ " << ll[0] << ", " << ll[1] << " }";
    return os;
}

} // namespace chart::geometry