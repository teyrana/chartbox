// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <cmath>

#include <fmt/core.h>

namespace chartbox::geometry {


/// \brief Represents a generic UTM-Frame location (in any UTM sector)
///
/// An ENU frame, representing a generic UTM frame (not tied to a specific UTM sector
/// It neither supplies no restricts the UTM sector.
struct LocalLocation {
    double easting;
    double northing;

    constexpr LocalLocation()
        : easting(NAN), northing(NAN)
    {}

    LocalLocation( double value )
        : easting(value), northing(value)
    {}

    constexpr LocalLocation( double east, double north )
        : easting(east), northing(north) 
    {}

    bool isnan() const {
        return std::isnan(easting) || std::isnan(northing);
    }

    double length() const {
        return norm2(); }

    bool nearby( const LocalLocation& other, double tolerance = 0.01 ) const {
        return ( tolerance > std::fabs( easting - other.easting)
               && tolerance > std::fabs( northing - other.northing) );
    }

    static LocalLocation nan() {
        return LocalLocation(NAN, NAN);
    }

    double norm1() const {
        return std::abs(easting) + std::fabs(northing );
    }

    double norm2() const {
        return std::sqrt( std::pow(easting,2) + std::pow(northing,2) );
    }

    LocalLocation operator*( double factor ) const {
        return { easting * factor, northing * factor };
    }

    LocalLocation operator+( const LocalLocation& other ) const {
        return { easting + other.easting, northing + other.northing };
    }

    LocalLocation* operator+=( const LocalLocation& other ){
        easting += other.easting;
        northing += other.northing;
        return this;
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

    std::string to_string() const {
        return fmt::format( "{{ {:9.4f}E, {:9.4f}N }}", easting, northing );
    }

    double  x() const { return easting; }
    double& x() { return easting; }

    double  y() const { return northing; }
    double& y() { return northing; }

};

inline std::ostream& operator<< (std::ostream& os, const LocalLocation& ll) {
    os << "{ " << ll[0] << ", " << ll[1] << " }";
    return os;
}

} // namespace chart::geometry