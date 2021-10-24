// GPL v3 (c) 2021, Daniel Williams 

#pragma once

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

    bool nearby( const GlobalLocation& other, double tolerance = 0.001 ) const { 
        return ( tolerance > std::fabs( latitude - other.latitude)
               && tolerance > std::fabs( longitude - other.longitude));
    }

    double operator[](uint32_t index) const { 
        return (0==index?longitude:
                1==index?latitude:
                NAN);
    }

    GlobalLocation operator*( double factor ) const {
        return { longitude * factor, latitude * factor };
    }

    GlobalLocation operator+( const GlobalLocation& other ) const {
        return { longitude + other.longitude, latitude + other.latitude };
    }

    GlobalLocation operator-( const GlobalLocation& other ) const {
        return { longitude - other.longitude, latitude - other.latitude };
    }

    GlobalLocation operator/( double divisor ) const {
        return { longitude / divisor, latitude / divisor };
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


};

} // namespace chart::geometry