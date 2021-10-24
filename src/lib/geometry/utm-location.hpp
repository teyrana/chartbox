// GPL v3 (c) 2021, Daniel Williams 

#pragma once

namespace chartbox::geometry {


/// \brief Represents a generic UTM-Frame location (in any UTM sector)
///
/// An ENU frame, representing a generic UTM frame (not tied to a specific UTM sector
/// It neither supplies no restricts the UTM sector.
struct UTMLocation {
    double easting;
    double northing;


    UTMLocation()
        : easting(NAN), northing(NAN)
    {}

    UTMLocation( double east, double north ) 
        : easting(east), northing(north) 
    {}

    bool nearby( const UTMLocation& other, double tolerance = 0.01 ) const { 
        return ( tolerance > std::fabs( easting - other.easting) 
               && tolerance > std::fabs( northing - other.northing) );
    }


    UTMLocation operator*( double factor ) const {
        return { easting * factor, northing * factor };
    }

    UTMLocation operator+( const UTMLocation& other ) const {
        return { easting + other.easting, northing + other.northing };
    }

    UTMLocation operator-( const UTMLocation& other ) const {
        return { easting - other.easting, northing - other.northing };
    }

    UTMLocation operator/( double divisor ) const {
        return { easting / divisor, northing / divisor };
    }

    bool operator==( const UTMLocation& other ) const {
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

};

} // namespace chart::geometry