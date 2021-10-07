// GPL v3 (c) 2021, Daniel Williams 

#include <array>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

#include <fmt/core.h>

#include "chart-box/geometry/local-location.hpp"

#include "leaf-node.hpp"

using chartbox::geometry::LocalLocation;
using chartbox::layer::LeafNode;


template <uint32_t n>
LeafNode<n>::LeafNode()
    : LeafNode( {0,0} )
{}

template <uint32_t n>
LeafNode<n>::LeafNode( LocalLocation _origin)
    : origin_(_origin)
    , width_(static_cast<double>(n))
    , scale_(1.0)

{
    fill(0);
}

template <uint32_t n>
bool LeafNode<n>::contains( const LocalLocation& p ) const {
    if( (p.easting < origin_.easting) 
        || (p.northing < origin_.northing) 
        || ((origin_.easting+width_) <= p.easting) 
        || ((origin_.northing+width_) <= p.northing ))
    {
        return false;
    }
    return true;
}

template <uint32_t n>
uint8_t LeafNode<n>::get(const LocalLocation& p, uint8_t or_default_value) const {
    if (contains(p)) {
        const uint32_t i = (p.easting - origin_.easting) * scale_;
        const uint32_t j = (p.northing - origin_.northing) * scale_;
        return get( i, j );
    }

    return or_default_value;
}

template <uint32_t n>
void LeafNode<n>::fill( uint8_t value) {
    memset( data_.data(), value, sizeof(uint8_t)*data_.size());
}

template <uint32_t n>
void LeafNode<n>::fill( const uint8_t * const source, uint32_t count ){
    if( count != data_.size() ){
        return;
    }
    memcpy( data_.data(), source, count );
}

template <uint32_t n>
uint8_t& LeafNode<n>::get( uint32_t xi, uint32_t yi ) {
    return data_[ xi + yi*n ];
}

template <uint32_t n>
uint8_t LeafNode<n>::get( uint32_t xi, uint32_t yi) const {
    return data_[ xi + yi*n ];
}

template <uint32_t n>
const LocalLocation& LeafNode<n>::origin() const {
    return origin_;
}

template <uint32_t n>
bool LeafNode<n>::set(const LocalLocation& p, uint8_t new_value) {
    if (contains(p)) {
        const LocalLocation scaled = (p - origin_) / scale_;
        const size_t xi = static_cast<size_t>(scaled.x());
        const size_t yi = static_cast<size_t>(scaled.y());
        get(xi, yi) = new_value;
        return true;
    }

    return false;
}

template <uint32_t n>
std::string LeafNode<n>::to_string() const {
    std::ostringstream buf;
    
    // // raw data:
    // buf << "======== ======= ======= ======= ======= ======= ======= =======";
    // for (size_t i = 0; i < data_.size(); ++i ){
    //     if( 0 == (i % n)){
    //         buf << fmt::format("\n{:04X}:    ", i);
    //     }
    //     buf << fmt::format(" {:02X}", data_[i]);
    // }
    // buf << '\n';

    // print with X -> left; Y -> Up
    buf << "======== ======= ======= ======= ======= ======= ======= =======\n";
    buf << "       ";
    for (size_t i = 0; i < dimension(); ++i) {
        buf << fmt::format(" {:2}", i );
    }
    buf << '\n';
    for (size_t i = 0; i < dimension(); ++i) {
        buf << " --";
    }
    buf << '\n';
            
    for (size_t j = dimension() - 1; 0 < j; --j) {
        buf << fmt::format("{:2}:    ", j);

        for (size_t i = 0; i < dimension(); ++i) {
            const size_t offset = i + (j-1)*dimension();


            const uint8_t value = data_[offset];
            
            if (value < 32) {
                buf << "   ";
            } else {
                buf << ' ' <<static_cast<int>(value);
            }
        }
        buf << '\n';
    }
    buf << std::endl;

    return buf.str();
}


// explicit class template instantiation
template class chartbox::layer::LeafNode<8u>;
template class chartbox::layer::LeafNode<32u>;
template class chartbox::layer::LeafNode<64u>;
