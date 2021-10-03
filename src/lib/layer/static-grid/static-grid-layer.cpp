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

#include "static-grid-layer.hpp"

using chartbox::layer::ChartLayerInterface;
using chartbox::layer::StaticGridLayer;

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
    return lookup( static_cast<uint32_t>(p.easting/precision_),
                   static_cast<uint32_t>(p.northing/precision_) );
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