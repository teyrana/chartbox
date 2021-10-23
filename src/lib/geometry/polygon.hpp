// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <initializer_list>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "geometry/bound-box.hpp"

namespace chartbox::geometry {

template<typename frame_vector_t>
class Polygon {
public:
    Polygon();
    Polygon(size_t initial_capacity);
    Polygon( std::vector<frame_vector_t>& init);
    Polygon(std::initializer_list<frame_vector_t> init);

    inline const auto begin() const { return points.cbegin(); }

    ///! \brief when manually constructing a polygon, call this to ensure it
    /// format-matches
    void complete();

    // clears the internal point vector
    void clear();

    inline const BoundBox<frame_vector_t>& bounds() const { return bounds_; }

    inline const auto end() const { return points.cend(); }

    bool load( std::vector<frame_vector_t> source );

    Polygon& move(const frame_vector_t& delta);

    /// \brief tests if the two polygons overlap in 2d space
    ///
    /// \param other -- another polygon of the same type
    /// \return true if the polygons overlap.  Otherwise false
    inline bool overlap(const Polygon<frame_vector_t>& other) const {
        return this->bounds_.overlaps(other.bounds());  }

    // void set( size_t index, double x, double y );
    void set( size_t index, frame_vector_t value );
    // frame_vector_t& operator[](const size_t index);

    const frame_vector_t& operator[](const size_t index) const;

    void resize(size_t capacity);

    size_t size() const;

    // \brief formats the contents to a given string
    std::string yaml(const std::string& indent = "") const;

    // if necessary, adds an extra point to the end of the polygon-point vector
    // to wrap it back to the first point. this is not strictly operationally
    // necessary, but it simplifies most of the algorithms that need to iterate
    // over the points.
    void enclose();

protected:
    // \brief isRightHanded()
    // calculates if the polygon is defined in a right-handed // CCW direction.
    // runs in O(n).
    //
    // See Also: https://en.wikipedia.org/wiki/Shoelace_formula
    //
    // \returns true: polgyon is right-handed.  false: left-handed
    bool is_right_handed() const;

    void recalculate_bounding_box();

protected: // Configuration parameters
    constexpr static size_t default_size = 12;

    /// \brief Main data store -- rontains polygon vertices
    std::vector<frame_vector_t> points;

    BoundBox<frame_vector_t> bounds_;

};

} // namespace chart::geometry

#include "polygon.inl"
