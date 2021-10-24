// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <cmath>

namespace chartbox::geometry {

/// \brief Represents a generic x-y orthogonal vector
struct XYLocation {
    double x;
    double y;

    XYLocation()
        : x(NAN), y(NAN)
    {}

    XYLocation( double _x, double _y )
        : x(_x), y(_y)
    {}

    bool nearby( const XYLocation& other, double tolerance = 0.01 ) const { 
        return ( tolerance > std::fabs( x - other.x) && tolerance > std::fabs( y - other.y) );
    }

    uint32_t norm1() const {
        return std::abs(x) + std::fabs(y );
    }

    uint32_t norm2() const {
        return std::sqrt( std::pow(x,2) + std::pow(y,2) );
    }

    XYLocation operator*( double factor ) const {
        return { x * factor, y * factor };
    }

    XYLocation operator+( const XYLocation& other ) const {
        return { x + other.x, y + other.y };
    }

    XYLocation operator-( const XYLocation& other ) const {
        return { x - other.x, y - other.y };
    }

    XYLocation operator/( double divisor ) const {
        return { x / divisor, y / divisor };
    }

    bool operator==( const XYLocation& other ) const { 
        return this->nearby(other);
    }

    inline double operator[](uint32_t index) const {
        switch( index ){
            case 0: return x;
            case 1: return y;
            default: return NAN;
        }
    }

    void set( uint32_t index, double new_value ){
        switch( index ){
            case 0: x = new_value; return;
            case 1: y = new_value; return;
            default: return;
        }
    }
};

inline std::ostream& operator<< (std::ostream& os, const XYLocation& ll) {
    os << "{ " << ll[0] << ", " << ll[1] << " }";
    return os;
}

} // namespace chart::geometry