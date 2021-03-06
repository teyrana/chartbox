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

#include <nlohmann/json.hpp>

#include "fixed-grid.hpp"

using Eigen::Vector2d;

using chartbox::layer::FixedGridLayer;

FixedGridLayer::FixedGridLayer( const Eigen::AlignedBox2d& _bounds)
    : bounds_(_bounds)
    , name_("GenericGridLayer")
{}

FixedGridLayer::cell_t FixedGridLayer::get(const Eigen::Vector2d& p) const {
    auto offset = index.lookup(p); 
    return grid[offset];
}

FixedGridLayer::cell_t& FixedGridLayer::get(const Eigen::Vector2d& p) {
    auto offset = index.lookup(p); 
    return grid[offset];
}

void FixedGridLayer::reset() {
    fill( default_value );
}

void FixedGridLayer::fill( const cell_t value) {
    grid.fill( value);
}

std::string FixedGridLayer::name() const {
    return name_;
}

// template<typename cell_t, size_t dim>
// Index2u FixedGridLayer<cell_t,dim>::as_index(const Eigen::Vector2d& location) const {
//     auto local = bounds_.as_local(location);
//     return Index2u( static_cast<uint32_t>(local.x() / precision_)
//                   , static_cast<uint32_t>(local.y() / precision_) );
// }

// template<typename cell_t, size_t dim>
// Vector2d  FixedGridLayer<cell_t,dim>::as_location(const index::Index2u& index) const {
//     const Vector2d p_local = { (static_cast<float>(index.i) + 0.5) * precision_
//                              , (static_cast<float>(index.j) + 0.5) * precision_ };
//     return bounds_.as_global(p_local);
// }

// template<typename cell_t, size_t dim>
// bool FixedGridLayer<cell_t,dim>::blocked(const index::Index2u& at) const {
//     return (blocking_threshold <= operator[](at));
// }

// template<typename cell_t, size_t dim>
// FixedGridLayer<cell_t,dim>::FixedGridLayer()
//     : bounds_(Bounds::make_square({0,0},dim))
//     , precision_(1.0)
// {}

// template<typename cell_t, size_t dim>
// FixedGridLayer<cell_t,dim>::FixedGridLayer(const Bounds& _bounds)
//     : bounds_(Bounds::make_square(_bounds.center(), dim))
//     , precision_(width() / dim)
// {}

// template<typename cell_t, size_t dim>
// const Bounds& FixedGridLayer<cell_t,dim>::bounds() const {
//     return bounds_;
// }

// template<typename cell_t, size_t dim>
// bool FixedGridLayer<cell_t,dim>::contains(const Vector2d& p) const {
//     return bounds_.contains(p);
// }

// template<typename cell_t, size_t dim>
// bool FixedGridLayer<cell_t,dim>::contains(const Index2u& index) const {
//     if (index.i < dim && index.j < dim ){
//         return true;
//     }
//     return false;        
// }

// template<typename cell_t, size_t dim>
// cell_t FixedGridLayer<cell_t,dim>::classify(const Vector2d& p) const {
//     return classify(p, 0xFF);
// }

// template<typename cell_t, size_t dim>
// cell_t FixedGridLayer<cell_t,dim>::classify(const Vector2d& p, const cell_t default_value) const {
//     if(bounds_.contains(p)){
//         size_t i = index.lookup(as_index(p));
//         return grid[i];
//     }

//     return default_value;
// }

// template<typename cell_t, size_t dim>
// bool FixedGridLayer<cell_t,dim>::fill_uniform(const cell_t value){
//     memset(grid.data(), value, size());
//     return true;
// }

// template <typename cell_t, size_t dim>
// bool FixedGridLayer<cell_t, dim>::fill_from_bounds(const geometry::Bounds& area, const cell_t value) {
//     return false;
// }

// template <typename cell_t, size_t dim>
// int FixedGridLayer<cell_t, dim>::fill_from_buffer(const std::vector<cell_t>& source) {
//     if (source.size() != grid.size()) {
//         return -1;
//     }

//     memcpy(grid.data(), source.data(), sizeof(cell_t) * source.size());
//     return 0;
// }

// template<typename cell_t, size_t dim>
// cell_t& FixedGridLayer<cell_t,dim>::get_cell(const size_t xi, const size_t yi) {
//     Index2u location(xi,yi);
//     return grid[index.lookup(location)];
// }

// template<typename cell_t, size_t dim>
// cell_t FixedGridLayer<cell_t,dim>::get_cell(const size_t xi, const size_t yi) const {
//     Index2u location(xi,yi);
//     return grid[index.lookup(location)];
// }

// template<typename cell_t, size_t dim>
// cell_t& FixedGridLayer<cell_t,dim>::operator[](const index::Index2u& location) {
//     return grid[index.lookup(location)];
// }

// template<typename cell_t, size_t dim>
// cell_t FixedGridLayer<cell_t,dim>::operator[](const index::Index2u& location) const {
//     return grid[index.lookup(location)];
// }

// template<typename cell_t, size_t dim>
// double FixedGridLayer<cell_t,dim>::precision() const {
//     return precision_;
// }

// template<typename cell_t, size_t dim>
// size_t FixedGridLayer<cell_t,dim>::size() const {
//     return grid.size();
// }

// template<typename cell_t, size_t dim>
// void FixedGridLayer<cell_t,dim>::set_bounds(const Bounds& _bounds) {
//     bounds_ = Bounds::make_square(_bounds);
    
//     precision_ = static_cast<double>(width()) / static_cast<double>(dim);
// }

// template<typename cell_t, size_t dim>
// bool FixedGridLayer<cell_t,dim>::store(const Vector2d& p, const cell_t new_value) {
//     if(contains(p)){
//         size_t i = index.lookup(as_index(p));
//         grid[i] = new_value;
//         return true;
//     }

//     return false;
// }

std::string FixedGridLayer::to_string() const {
    std::ostringstream buf;
    static const std::string header("======== ======= ======= ======= ======= ======= ======= =======\n");
    buf << header;


    for (size_t j = dimension - 1; j < dimension; --j) {
        for (size_t i = 0; i < dimension; ++i) {
            const auto value = grid[index.lookup(i, j)];
            buf << ' ';
            if (value < 32) {
                buf << ' ';
            } else {
                buf << static_cast<cell_t>(value);
            }
        }
        buf << std::endl;
    }
    buf << header;

    return buf.str();
}

std::string FixedGridLayer::type() const { 
    return type_;
}

FixedGridLayer::~FixedGridLayer(){}
