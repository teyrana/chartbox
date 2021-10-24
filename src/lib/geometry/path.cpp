// GPL v3 (c) 2021, Daniel Williams 

#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

#include "path.hpp"

using chartbox::geometry::Path;


//---------------------------------------------------------------


template<typename frame_vector_t>
Path<frame_vector_t>::Path()
{}

template<typename frame_vector_t>
Path<frame_vector_t>::Path( size_t initial_size ){
    points.reserve( initial_size );
}


template<typename frame_vector_t>
Path<frame_vector_t>::Path(std::vector<frame_vector_t>& init) 
    : points(init)
{}

template<typename frame_vector_t>
Path<frame_vector_t>::Path(std::initializer_list<frame_vector_t> init_list) 
    : points(init_list)
{}

template<typename frame_vector_t>
auto Path<frame_vector_t>::begin() -> decltype(points.begin()) {
    return points.begin();
}

template<typename frame_vector_t>
void Path<frame_vector_t>::clear() { 
    points.clear();
}

template<typename frame_vector_t>
void Path<frame_vector_t>::emplace_back( frame_vector_t v ){
    points.emplace_back( v );
}


template<typename frame_vector_t>
void Path<frame_vector_t>::emplace_back(const double x, const double y) {
    points.emplace_back( x, y );
}

template<typename frame_vector_t>
bool Path<frame_vector_t>::empty() const {
    return points.empty();
}

template<typename frame_vector_t>
auto Path<frame_vector_t>::end() -> decltype(points.end()) {
    return points.end();
}

template<typename frame_vector_t>
frame_vector_t Path<frame_vector_t>::segment(const size_t index) const {
    return points[index+1] - points[index];
}

template<typename frame_vector_t>
double Path<frame_vector_t>::length() const {

    if(points.size() < 2){
        return 0;
    }

    double length = 0;
    for( size_t i = 0; i < points.size()-1; ++i){
        length += segment(i).norm2();
    }
    return length;
}

template<typename frame_vector_t>
bool Path<frame_vector_t>::load(std::vector<frame_vector_t> source) {
    // if the new polygon contains insufficient points, abort and clear.
    if (2 > source.size()) {
        return false;
    }

    points = std::move(source);

    return true;
}

template<typename frame_vector_t>
frame_vector_t& Path<frame_vector_t>::operator[](const size_t index) { 
    return points[index];
}

template<typename frame_vector_t>
const frame_vector_t& Path<frame_vector_t>::operator[](const size_t index) const {
    return points[index];
}

template<typename frame_vector_t>
void Path<frame_vector_t>::push_back(const frame_vector_t p) {
    points.push_back(p);
}

template<typename frame_vector_t>
void Path<frame_vector_t>::resize(size_t capacity) {
    points.resize(capacity);
}

template<typename frame_vector_t>
size_t Path<frame_vector_t>::size() const {
    return points.size();
}

template<typename frame_vector_t>
std::string Path<frame_vector_t>::str( int indent ) const {
    std::ostringstream buf;
    std::string prefix = fmt::format("{:{}}", " ", indent );
    buf << fmt::format( "{} =======  Path Contents  ======= \n", prefix );
    buf << fmt::format( "{}    :: {} points\n", prefix, points.size() );
    for( size_t i=0; i < points.size(); ++i ){
        auto pt = points[i];
        buf << fmt::format( "{}    [{:3d}] {:8.2f} N,    {:8.2f} E\n", prefix, i, pt[0], pt[1] );
    }
    buf << fmt::format( "{} ======= ======= ======= ======= \n", prefix );
    return buf.str();
}

// #include "global-location.hpp"
#include "local-location.hpp"
// #include "utm-location.hpp"
#include "xy-location.hpp"

// template class chartbox::geometry::Path< chartbox::geometry::GlobalLocation >;
template class chartbox::geometry::Path< chartbox::geometry::LocalLocation >;
// template class chartbox::geometry::Path< chartbox::geometry::UTMLocation >;
template class chartbox::geometry::Path< chartbox::geometry::XYLocation >;
