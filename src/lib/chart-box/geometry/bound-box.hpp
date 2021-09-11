// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <iostream>
#include <sstream>

#include <fmt/format.h>

namespace chartbox::geometry {


/// \brief Represents a geographic bounding box, in a given coordinate frame
///
/// note:  this class implicitly assumes that +x is eastward and +y is northward
template<typename frame_vector_t>
struct BoundBox {
    frame_vector_t min;
    frame_vector_t max;

    BoundBox()
        : min( std::numeric_limits<double>::max(), std::numeric_limits<double>::max() )
        , max( -std::numeric_limits<double>::max(), -std::numeric_limits<double>::max() )
    {}

    BoundBox( frame_vector_t _min, frame_vector_t _max )
        : min(_min), max(_max)
    {}

    inline void clear(){
        min = { std::numeric_limits<double>::max(), std::numeric_limits<double>::max() };
        max = { -std::numeric_limits<double>::max(), -std::numeric_limits<double>::max() };
    }

    inline frame_vector_t center() const {
        return {  (min[0] + max[0])/2, (min[1] + max[1])/2 };
    }

    inline bool contains( const frame_vector_t& p ) const {
        if(( min[0] > p[0] )||( min[1] > p[1] )||( max[0] < p[0] )||( max[1] < p[1] ) ){
            return false;
        }
        return true;
    }

    inline bool contains( const BoundBox<frame_vector_t> other) const {
        return contains(other.min) && contains(other.max);
    }

    inline double east() const { return max.x; }

    inline void grow( double zeroth, double first ){
        min.set(0, std::min(min[0], zeroth) );
        min.set(1, std::min(min[1], first) );
        max.set(0, std::max(max[0], zeroth) );
        max.set(1, std::max(max[1], first) );
    }

    inline void grow( const frame_vector_t& next ){
        min.set(0, std::min(min[0],next[0]) );
        min.set(1, std::min(min[1],next[1]) );
        max.set(0, std::max(max[0],next[0]) );
        max.set(1, std::max(max[1],next[1]) );
    }

    /// \brief returns the y-width of this box
    double height() const { return (max[1] - min[1]); }

    inline bool is_square() const {
        const double width = max[0] - min[0];
        const double height = max[1] - min[1];

        // these are invalid bounding boxes
        if( 0 > width) return false;
        if( 0 > height) return false;

        constexpr double tolerance = 0.001;
        if( tolerance > std::fabs(width - height) ){
            return true;
        }
        return false;
    }

    inline std::string json() const {
        std::ostringstream buf;
        buf << "[";
        buf << min[0] << ",";
        buf << min[1] << ",";
        buf << max[0] << ",";
        buf << max[1];
        buf << "]";
        return buf.str();
    }

    inline double north() const { return max.y; }
    
    inline const frame_vector_t& northeast() const { return max; }

    bool overlaps( const BoundBox<frame_vector_t>& other ) const {
        if( max[0] < other.min[0] ){
            return false;
        }else if( other.max[0] < min[0] ){
            return false;
        }else if( max[1] < other.min[1] ){
            return false;
        }else if( other.max[1] < min[1] ){
            return false;
        }
        return true;
    }

    inline BoundBox<frame_vector_t> snap( double interval, double new_size  ) const {
        const double snapped_easting = ::floor( min[0] / interval ) * interval;
        const double snapped_northing = ::floor( min[1] / interval ) * interval;
    
        return { { snapped_easting, snapped_northing },
                 { snapped_easting + new_size, snapped_northing + new_size } };
    }

    inline double south() const { return min.y; }
    
    inline const frame_vector_t& southwest() const { return min; }

    inline std::string dump( const std::string& indent="" ) const {
        std::ostringstream buf;
        buf << indent << "    - " << std::fixed << min[0] << " < " << std::fixed << max[0] << '\n';
        buf << indent << "    - " << std::fixed << min[1] << " < " << std::fixed << max[1] << '\n';
        return buf.str();
    }

    inline double west() const { return min.x; }

    /// \brief returns the x-width of this box
    double width() const { return (max[0] - min[0]); }

};

} // namespace chartbox::geometry