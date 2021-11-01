// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <cmath>
#include <memory>
#include <cstdlib>
#include <string>
#include <vector>

#include "layer/layer-interface.hpp"

namespace chartbox::layer::simple {

template<typename cell_t=uint8_t, uint32_t dimension_=1024, uint32_t precision_mm_=1000>
class SimpleGridLayer final : public LayerInterface<SimpleGridLayer<cell_t,dimension_,precision_mm_>> {
public:
    /// \brief name of this layer's type
    constexpr static char type_name_[] = "SimpleGridLayer";

    /// \brief width of each cell, in meters
    constexpr static double meters_across_cell_ = static_cast<double>(precision_mm_)/1000;

    /// \brief number of cells along each dimension of this grid
    constexpr static size_t cells_across_layer_ = dimension_;

    /// \brief width of entire layer, in meters
    constexpr static double meters_across_layer_ = meters_across_cell_ * static_cast<double>(cells_across_layer_);


public:

    SimpleGridLayer();

    bool contains(const LocalLocation& p ) const;

    /// \brief number of cells along each dimension of this grid
    constexpr static size_t dimension() { return cells_across_layer_; }

    inline cell_t* data() {
        return grid_.data(); }

    // override from LayerInterface
    bool fill( cell_t value );

    bool fill( const cell_t* const buffer, size_t count );
    
    bool fill( const Path<LocalLocation>& path, const BoundBox<LocalLocation>& bounds, cell_t value ){
        return super().fill( path, bounds, value); }

    bool fill( const BoundBox<LocalLocation>& box, const cell_t value ){
        return super().fill( box, value ); }

    bool fill( const Polygon<LocalLocation>& poly, const BoundBox<LocalLocation>& bound, cell_t value ){
        return super().fill( poly, bound, value ); }

    /// \brief Fill the entire grid with values from the buffer
    /// 
    /// \param source - values to fill.  This must be the same byte-count as this layer
    /// \param fill_value - value to write inside the area
    bool fill( const std::vector<cell_t>& source );

    cell_t get(const LocalLocation& p) const;

    size_t lookup( const uint32_t i, const uint32_t j ) const;

    inline uint32_t cells_across_view() const { return cells_across_layer_; }

    inline double meters_across_cell() const { return meters_across_cell_; }
    inline double meters_across_view() const { return meters_across_cell_ * cells_across_layer_; }

    double precision() const {
        return meters_across_cell_; }

    /// \brief Draws a simple debug representation of this grid to stderr
    std::string to_cell_content_string( uint32_t indent = 0) const ;
    std::string to_location_content_string( uint32_t indent = 0 ) const { return super().to_location_content_string(indent); }
    std::string to_property_string( uint32_t indent = 0 ) const;

    inline void reset(){ 
        fill( default_cell_value );  }

    uint32_t size() const {
        return cells_across_layer_*cells_across_layer_; }

    /// \brief Access the value at an (x, y)
    ///!
    /// \param p - the x,y coordinates to search at:
    /// \return reference to the cell value
    bool store(const LocalLocation& p, const cell_t new_value);

    /// \brief track from the given location  (... + the native width)
    bool track( const BoundBox<LocalLocation>& bounds ){
        return false; }
    /// \brief Get the bounds of the tracked (overall) area
    const BoundBox<LocalLocation>& tracked() const {
        return visible(); }
    bool tracked(const LocalLocation& p) const {
        return visible(p); }

    bool view(const BoundBox<LocalLocation>& nb );
    const BoundBox<LocalLocation>& visible() const { 
        return view_bounds_; }
    bool visible(const LocalLocation& p) const {
        return view_bounds_.contains(p); }

    ~SimpleGridLayer() = default;

protected:

    /// \brief contains the data for this tile
    // raw array:  2D addressing is performed through the index, below
    std::array<cell_t, cells_across_layer_*cells_across_layer_> grid_;

    // this tracks the bounds of the visible grid
    geometry::BoundBox<LocalLocation> view_bounds_;

private:

    LayerInterface<SimpleGridLayer<cell_t,dimension_,precision_mm_>>& super() {
        return *static_cast< LayerInterface<SimpleGridLayer<cell_t,dimension_,precision_mm_>>* >(this);
    }

    const LayerInterface<SimpleGridLayer<cell_t,dimension_,precision_mm_>>& super() const {
        return *static_cast< const LayerInterface<SimpleGridLayer<cell_t,dimension_,precision_mm_>>* >(this);
    }
};

#include "simple-grid-layer.inl"

} // namespace
