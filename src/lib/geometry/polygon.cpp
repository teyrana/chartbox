// GPL v3 (c) 2021, Daniel Williams 

// #include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <vector>

#include "polygon.hpp"

using chartbox::geometry::BoundBox;
using chartbox::geometry::Polygon;

//---------------------------------------------------------------
// Constructor
template<typename frame_vector_t>
Polygon<frame_vector_t>::Polygon() : Polygon(default_size) {}

template<typename frame_vector_t>
Polygon<frame_vector_t>::Polygon( size_t initial_capacity) : points(initial_capacity) {
    points.resize(initial_capacity);
}

template<typename frame_vector_t>
Polygon<frame_vector_t>::Polygon(std::vector<frame_vector_t>& init) { load(init); }

template<typename frame_vector_t>
Polygon<frame_vector_t>::Polygon(std::initializer_list<frame_vector_t> init_list) {
    std::vector<frame_vector_t> pts = init_list;
    load(pts);
}

template<typename frame_vector_t>
void Polygon<frame_vector_t>::clear() { points.clear(); }

template<typename frame_vector_t>
void Polygon<frame_vector_t>::complete() {
    // NOTE: Yes, this function performs multiple passes of the point-vector...
    //       where it could calculate all desired info in one pass.
    //       This is done for purely for clarity.
    //       This function is not expected to be hot-path code.

    // std::cerr << "====== ====== ====== " << std::endl;
    // std::cerr << write_json(, "    ") << std::endl;

    // we must enclose before the clockwise-check.
    this->enclose();

    recalculate_bounding_box();

    if (!is_right_handed()) {
        std::reverse(std::begin(points), std::end(points));
    }
}

template<typename frame_vector_t>
void Polygon<frame_vector_t>::enclose() {
    // ensure that polygon loops back
    const auto& first = points[0];
    const auto& last = points[points.size() - 1];

    if ( ! first.nearby(last) ) {
        points.emplace_back(first);
    }
}

template<typename frame_vector_t>
bool Polygon<frame_vector_t>::is_right_handed() const {
    double sum = 0;

    // NOTE THE RANGE!:
    //   this needs to iterate over all point-pairs!
    for (uint i = 0; i < (points.size() - 1); ++i) {
        auto& p1 = points[i];
        auto& p2 = points[i + 1];
        sum += (p1[0] * p2[1]) - (p1[1] * p2[0]);
    }

    // // The shoelace formula includes a divide-by-2 step; ... but we don't
    // // care about the magnitude, just the sign.  So skip this step.
    // sum /= 2;

    if (0 > sum) {
        // left-handed / clockwise points
        return false;
    } else {
        // right-handed / counter-clockwise points
        return true;
    }
}

template<typename frame_vector_t>
bool Polygon<frame_vector_t>::load(std::vector<frame_vector_t> source) {
    // if the new polygon contains insufficient points, abort and clear.
    if (4 > source.size()) {
        return false;
    }

    points = std::move(source);

    recalculate_bounding_box();

    return true;
}

template<typename frame_vector_t>
Polygon<frame_vector_t>& Polygon<frame_vector_t>::move(const frame_vector_t& delta) {
    std::for_each(points.begin(), points.end(), [&](frame_vector_t& p) { p = p + delta; });
    return *this;
}


template<typename frame_vector_t>
const frame_vector_t& Polygon<frame_vector_t>::operator[](const size_t index) const {
    return points[index];
}

template<typename frame_vector_t>
void Polygon<frame_vector_t>::recalculate_bounding_box() {
    bounds_.clear();
    for (auto& p : points) {
        bounds_.grow(p);
    }
}

template<typename frame_vector_t>
void Polygon<frame_vector_t>::resize(size_t capacity) { points.resize(capacity); }

// template<typename frame_vector_t>
// void Polygon<frame_vector_t>::set( size_t index, double x, double y) {
//     points[i] = frame_vector_t(x, y);
//     bounds_.grow( x, y );
// }

template<typename frame_vector_t>
void Polygon<frame_vector_t>::set(const size_t index, frame_vector_t value ){
    points[index] = value;
    bounds_.grow( value );
}

template<typename frame_vector_t>
size_t Polygon<frame_vector_t>::size() const { return points.size(); }


#include "global-location.hpp"
#include "local-location.hpp"
#include "utm-location.hpp"
#include "xy-location.hpp"

template class chartbox::geometry::Polygon< chartbox::geometry::GlobalLocation>;
template class chartbox::geometry::Polygon< chartbox::geometry::LocalLocation>;
template class chartbox::geometry::Polygon< chartbox::geometry::UTMLocation>;
template class chartbox::geometry::Polygon< chartbox::geometry::XYLocation>;
