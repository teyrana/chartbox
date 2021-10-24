// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <initializer_list>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "bound-box.hpp"

namespace chartbox::geometry {

template<typename frame_vector_t>
class Path {
public:
    Path();
    Path( size_t initial_size );
    Path(std::vector<frame_vector_t>& init);
    Path(std::initializer_list<frame_vector_t> init);

    // clears the internal point vector
    void clear();

    // inline const BoundBox<frame_vector_t>& bounds() const { return bounds_; }

    void emplace_back( frame_vector_t v );

    void emplace_back( const double x, const double y );

    bool empty() const;

    frame_vector_t segment(const size_t index) const;

    double length() const;

    bool load(std::vector<frame_vector_t> source);

    frame_vector_t& operator[](const size_t index);

    const frame_vector_t& operator[](const size_t index) const;

    void push_back(const frame_vector_t p);

    void resize(size_t capacity);

    size_t size() const;

    std::string str( int indent = 0 ) const;

// ====== ====== ====== Class Attributes ====== ====== ====== 
private:
    /// Main data store for this class.
    std::vector<frame_vector_t> points;

    // double length_;

public:
    // these need to be declared after the 'points' property
    auto begin() -> decltype(points.begin());
    auto end() -> decltype(points.end());

};

} // namespace chart::geometry
