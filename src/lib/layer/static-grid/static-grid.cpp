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

#include <fmt/core.h>

#include "static-grid.hpp"

using chartbox::layer::ChartLayerInterface;
using chartbox::layer::StaticGridLayer;


StaticGridLayer::StaticGridLayer( const BoundBox<UTMLocation>& _bounds )
    : ChartLayerInterface<StaticGridLayer>(_bounds)
{
    if( (dimension != static_cast<size_t>(_bounds.width()/precision())) || ( dimension != static_cast<size_t>(_bounds.width()/precision())) ){
        // yes, this is limiting.  This class is limiting -- it is only intended to be a fast / cheap / easy medium for testing other parts of the code
        // if you run up against this limitation, don't change it here: use a class which better fits your use case.

        fmt::print( "======== ======== StaticGridLayer CTOR:  ======== ======== \n" );
        fmt::print( "Bounds:\n");
        fmt::print( "      min:      {:12.6f}, {:12.6f} \n", _bounds.min.easting, _bounds.min.northing );
        fmt::print( "      max:      {:12.6f}, {:12.6f} \n", _bounds.max.easting, _bounds.max.northing );
        fmt::print( "      Width:    {:12.6f}, {:12.6f} \n", _bounds.width(), _bounds.width() );
        fmt::print( "Grid:\n");
        fmt::print( "      dimension:    {:12lu} x {:%12lu} \n", dimension, dimension );
    }
}

bool StaticGridLayer::contains(const LocalLocation& p ) const {
    if( 0 > p.easting || 0 > p.northing ){
        return false;
    }else if( dimension < p.easting || dimension < p.northing ){
        return false;
    }
    return true;
}

bool StaticGridLayer::fill( const uint8_t  value) {
    grid.fill( value);
    return true;
}

bool StaticGridLayer::fill( const uint8_t * const source, size_t count ){
    if ( count > grid.size()) {
        return false;
    }
    memcpy( grid.data(), source, sizeof(uint8_t ) * std::min(count, grid.size()) );
    return true;
}

bool StaticGridLayer::fill(const std::vector<uint8_t >& source) {
    if (source.size() != grid.size()) {
        return false;
    }
    memcpy(grid.data(), source.data(), sizeof(uint8_t ) * source.size());
    return true;
}

uint8_t StaticGridLayer::get(const LocalLocation& p ) const {
    return grid[ lookup(p) ];
}

uint8_t & StaticGridLayer::get(const LocalLocation& p ) {
    return grid[ lookup(p) ];
}

size_t StaticGridLayer::lookup( const uint32_t i, const uint32_t j ) const {
    return i + (j * dimension);
}

size_t StaticGridLayer::lookup( const LocalLocation& p ) const {
    return lookup( static_cast<uint32_t>(p.easting/precision()),
                   static_cast<uint32_t>(p.northing/precision()) );
}

double StaticGridLayer::precision() const {
    return this->bounds_.width() / dimension;
}

void StaticGridLayer::print_contents() const {
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

bool StaticGridLayer::store( const LocalLocation& p, const uint8_t  value) {
    const auto offset = lookup( p );
    grid[offset] = value;
    return true;
}