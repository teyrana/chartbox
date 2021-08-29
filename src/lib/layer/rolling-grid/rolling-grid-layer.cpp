// GPL v3 (c) 2020, Daniel Williams 

// #include <algorithm>
// #include <cmath>
// #include <cstring>
// #include <iomanip>
// #include <iostream>
// #include <sstream>
// #include <string>
// #include <memory>
// #include <vector>

#include <Eigen/Geometry>


#include "chart-box/geometry/bound-box.hpp"
#include "chart-box/geometry/local-location.hpp"
#include "chart-box/geometry/polygon.hpp"

#include "rolling-grid-layer.hpp"

using chartbox::geometry::BoundBox;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::Polygon;

namespace chartbox::layer {

RollingGridLayer::RollingGridLayer( double _precision, const BoundBox<UTMLocation>& _bounds )
    : ChartLayerInterface<RollingGridLayer>(_bounds)
    , precision_(_precision)
{
    // if( (dimension != static_cast<size_t>(_bounds.width()/precision())) || ( dimension != static_cast<size_t>(_bounds.width()/precision())) ){
        // yes, this is limiting.  This class is limiting -- it is only intended to be a fast / cheap / easy medium for testing other parts of the code
        // if you run up against this limitation, don't change it here: use a class which better fits your use case.
    // }

    fmt::print( "======== ======== RollingGridLayer CTOR:  ======== ======== \n" );
    // fmt::print( "Bounds:\n");
    // fmt::print( "      min:      {:12.6f}, {:12.6f} \n", _bounds.min.easting, _bounds.min.northing );
    // fmt::print( "      max:      {:12.6f}, {:12.6f} \n", _bounds.max.easting, _bounds.max.northing );
    // fmt::print( "      Width:    {:12.6f}, {:12.6f} \n", _bounds.width(), _bounds.width() );
    // fmt::print( "Grid:\n");
    // fmt::print( "      dimension:    {:12lu} x {:%12lu} \n", dimension, dimension );
}


bool RollingGridLayer::fill(const uint8_t value){
    // NYI
    return true;
}

bool RollingGridLayer::fill( const BoundBox<LocalLocation>& box, uint8_t value ){
    // NYI
    return false;
}

bool RollingGridLayer::fill( const Polygon<LocalLocation>& source, uint8_t value ){
    // NYI
    return false;
}

uint8_t RollingGridLayer::get(const LocalLocation& p) const {
    // NYI
    // if(bounds_.contains(p)){
    //     size_t i = index.lookup(as_index(p));
    //     // return grid[i];
    // }

    return default_cell_value;
}

// uint8_t& RollingGridLayer::get_cell(const size_t xi, const size_t yi) {
//     // Index2u location(xi,yi);
//     // return grid[index.lookup(location)];
// }

// uint8_t RollingGridLayer::get_cell(const size_t xi, const size_t yi) const {
//     Index2u location(xi,yi);
//     return grid[index.lookup(location)];
// }

bool RollingGridLayer::focus(const BoundBox<LocalLocation>& new_bounds ) {
    return false;
}

// uint8_t& RollingGridLayer::operator[](const index::Index2u& location) {
//     return grid[index.lookup(location)];
// }

// uint8_t RollingGridLayer::operator[](const index::Index2u& location) const {
//     return grid[index.lookup(location)];
// }

double RollingGridLayer::precision() const {
    return precision_;
}

// void RollingGridLayer::set_bounds(const Bounds& _bounds) {
//     bounds_ = Bounds::make_square(_bounds);
//     precision_ = static_cast<double>(width()) / static_cast<double>(dim);
// }

bool RollingGridLayer::store(const LocalLocation& p, uint8_t new_value) {
    // NYI
    // if(contains(p)){
    //     size_t i = index.lookup(as_index(p));
    //     grid[i] = new_value;
    //     return true;
    // }

    return false;
}

std::string RollingGridLayer::print_contents() const {
    std::ostringstream buf;
    static const std::string header("======== ======= ======= ======= ======= ======= ======= =======\n");
    buf << header;
    // for (size_t j = dim - 1; j < dim; --j) {
    //     for (size_t i = 0; i < dim; ++i) {
    //         const auto value = grid[index.lookup(i, j)];
    //         buf << ' ';
    //         if (value < 32) {
    //             buf << ' ';
    //         } else {
    //             buf << static_cast<char>(value);
    //         }
    //     }
        buf << std::endl;
    // }
    buf << header;

    return buf.str();
}


}  // namespace
