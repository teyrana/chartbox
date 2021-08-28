// GPL v3 (c) 2021, Daniel Williams 

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <vector>

#include <Eigen/Geometry>
#include <fmt/core.h>

using Eigen::Vector2d;

using chartbox::layer::role_t;
using chartbox::layer::ChartLayerInterface;
using chartbox::layer::FixedGridLayer;


template<role_t role>
FixedGridLayer<role>::FixedGridLayer( const BoundBox<UTMLocation>& _bounds )
    : ChartLayerInterface<FixedGridLayer<role>, role>(_bounds)
{

    if( (dimension != static_cast<size_t>(_bounds.width()/precision())) || ( dimension != static_cast<size_t>(_bounds.width()/precision())) ){
        // yes, this is limiting.  This class is limiting -- it is only intended to be a fast / cheap / easy medium for testing other parts of the code
        // if you run up against this limitation, don't change it here: use a class which better fits your use case.

        fmt::print( "======== ======== FixedGridLayer CTOR:  ======== ======== \n" );
        fmt::print( "Bounds:\n");
        fmt::print( "      min:      {:12.6f}, {:12.6f} \n", _bounds.min.easting, _bounds.min.northing );
        fmt::print( "      max:      {:12.6f}, {:12.6f} \n", _bounds.max.easting, _bounds.max.northing );
        fmt::print( "      Width:    {:12.6f}, {:12.6f} \n", _bounds.width(), _bounds.width() );
        fmt::print( "Grid:\n");
        fmt::print( "      dimension:    {:12lu} x {:%12lu} \n", dimension, dimension );
    }

}

template<role_t role>
bool FixedGridLayer<role>::fill( const uint8_t  value) {
    grid.fill( value);
    return true;
}

template<role_t role>
bool FixedGridLayer<role>::fill( const uint8_t * const source, size_t count ){
    if ( count > grid.size()) {
        return false;
    }
    memcpy( grid.data(), source, sizeof(uint8_t ) * std::min(count, grid.size()) );
    return true;
}

template<role_t role>
bool FixedGridLayer<role>::fill(const std::vector<uint8_t >& source) {
    if (source.size() != grid.size()) {
        return false;
    }
    memcpy(grid.data(), source.data(), sizeof(uint8_t ) * source.size());
    return true;
}

template<role_t role>
uint8_t FixedGridLayer<role>::get(const Eigen::Vector2d& p) const {
    return grid[ lookup(p) ];
}

template<role_t role>
uint8_t & FixedGridLayer<role>::get(const Eigen::Vector2d& p) {
    return grid[ lookup(p) ];
}

template<role_t role>
size_t FixedGridLayer<role>::lookup( const uint32_t i, const uint32_t j ) const {
    return i + (j * dimension);
}

template<role_t role>
size_t FixedGridLayer<role>::lookup( const Eigen::Vector2d& p ) const {
    return lookup( static_cast<uint32_t>(p.x()/precision()),
                   static_cast<uint32_t>(p.y()/precision()) );
}

template<role_t role>
size_t FixedGridLayer<role>::lookup( const Vector2u i ) const {
    return i[0] + (i[1] * dimension);
}

template<role_t role>
double FixedGridLayer<role>::precision() const {
    return this->bounds_.width() / dimension;
}

template<role_t role>
void FixedGridLayer<role>::print_contents() const {
    fmt::print( "============ ============ Fixed-Grid-Layer Contents ============ ============\n" );
    for (size_t j = dimension - 1; j < dimension; --j) {
        for (size_t i = 0; i < dimension; ++i) {
            const auto offset = lookup(i,j);
            const auto value = grid[offset];
            if( 0 == (i%8) ){
                fmt::print(" ");
            }
            if( 0 < value ){
                fmt::print(" {:2X}", static_cast<int>(value) );
            }else{
                fmt::print(" --");
            }
        }
        if( 0 == (j%8) ){
            fmt::print("\n");
        }
        fmt::print("\n");
    }
    fmt::print( "============ ============ ============ ============ ============ ============\n" );
}

template<role_t role>
bool FixedGridLayer<role>::store( const Eigen::Vector2d& p, const uint8_t  value) {
    const auto offset = lookup( p );
    grid[offset] = value;
    return true;
}


// template<typename uint8_t , size_t dim>
// Index2u FixedGridLayer<uint8_t ,dim>::as_index(const Eigen::Vector2d& location) const {
//     auto local = bounds_.as_local(location);
//     return Index2u( static_cast<uint32_t>(local.x() / precision )
//                   , static_cast<uint32_t>(local.y() / precision ) );
// }

// template<typename uint8_t , size_t dim>
// Vector2d  FixedGridLayer<uint8_t ,dim>::as_location(const index::Index2u& index) const {
//     const Vector2d p_local = { (static_cast<float>(index.i) + 0.5) * precision_
//                              , (static_cast<float>(index.j) + 0.5) * precision_ };
//     return bounds_.as_global(p_local);
// }

// template<typename uint8_t , size_t dim>
// bool FixedGridLayer<uint8_t ,dim>::blocked(const index::Index2u& at) const {
//     return (blocking_threshold <= operator[](at));
// }

// template<typename uint8_t , size_t dim>
// bool FixedGridLayer<uint8_t ,dim>::contains(const Vector2d& p) const {
//     return bounds_.contains(p);
// }

// template<typename uint8_t , size_t dim>
// bool FixedGridLayer<uint8_t ,dim>::contains(const Index2u& index) const {
//     if (index.i < dim && index.j < dim ){
//         return true;
//     }
//     return false;        
// }

// template<typename uint8_t , size_t dim>
// uint8_t  FixedGridLayer<uint8_t ,dim>::classify(const Vector2d& p) const {
//     return classify(p, 0xFF);
// }

// template<typename uint8_t , size_t dim>
// uint8_t  FixedGridLayer<uint8_t ,dim>::classify(const Vector2d& p, const uint8_t  default_value) const {
//     if(bounds_.contains(p)){
//         size_t i = index.lookup(as_index(p));
//         return grid[i];
//     }

//     return default_value;
// }

// template<typename uint8_t , size_t dim>
// uint8_t & FixedGridLayer<uint8_t ,dim>::get_cell(const size_t xi, const size_t yi) {
//     Index2u location(xi,yi);
//     return grid[index.lookup(location)];
// }

// template<typename uint8_t , size_t dim>
// uint8_t  FixedGridLayer<uint8_t ,dim>::get_cell(const size_t xi, const size_t yi) const {
//     Index2u location(xi,yi);
//     return grid[index.lookup(location)];
// }

// template<typename uint8_t , size_t dim>
// uint8_t & FixedGridLayer<uint8_t ,dim>::operator[](const index::Index2u& location) {
//     return grid[index.lookup(location)];
// }

// template<typename uint8_t , size_t dim>
// uint8_t  FixedGridLayer<uint8_t ,dim>::operator[](const index::Index2u& location) const {
//     return grid[index.lookup(location)];
// }

// template<typename uint8_t , size_t dim>
// bool FixedGridLayer<uint8_t ,dim>::store(const Vector2d& p, const uint8_t  new_value) {
//     if(contains(p)){
//         size_t i = index.lookup(as_index(p));
//         grid[i] = new_value;
//         return true;
//     }

//     return false;
// }
