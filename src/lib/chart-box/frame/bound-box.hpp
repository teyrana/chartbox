// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <Eigen/Core>

#include <fmt/format.h>

namespace chartbox::frame {


/// \brief Represents a generic bounding box, in a given coordinate frame

template<typename frame_vector_t>
struct BoundBox {
    frame_vector_t min;
    frame_vector_t max;

    BoundBox( frame_vector_t _min, frame_vector_t _max )
        : min(_min), max(_max)
    {}

    /// \brief returns the x-width of this box
    double width() const { return (max[0] - min[0]); }

    /// \brief returns the y-width of this box
    double height() const { return (max[1] - min[1]); }

    inline void dump() const {
        fmt::print( "::Bounds[0]: {} -> {} \n", min[0], max[0] );
        fmt::print( "::Bounds[1]: {} -> {} \n", min[1], max[1] );
    }

};

} // namespace chartbox::frame