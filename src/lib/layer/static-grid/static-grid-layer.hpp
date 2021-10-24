// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <cmath>
#include <memory>
#include <cstdlib>
#include <string>
#include <vector>

#include "layer/layer-interface.hpp"

namespace chartbox::layer {

// template<uint32_t dimension>
class StaticGridLayer : public LayerInterface<StaticGridLayer> {
public:
    /// \brief name of this layer's type
    constexpr static char type_name_[] = "StaticGridLayer";

public:

    StaticGridLayer() = default;
    
//    const std::array<uint8_t, dimension*dimension> & array() const;

    bool contains(const LocalLocation& p ) const;

    /// \brief number of cells along each dimension of this grid
    constexpr static size_t dimension() { return cells_across_layer_; }

    inline uint8_t* data() {
        return grid.data(); }

    inline const uint8_t* data() const {
        return grid.data(); }

    // override from LayerInterface
    bool fill( const uint8_t value );

    bool fill( const uint8_t* const buffer, size_t count );
    
    bool fill( const BoundBox<LocalLocation>& box, const uint8_t value ){
        return super().fill( box, value ); }

    bool fill( const Polygon<LocalLocation>& poly, const BoundBox<LocalLocation>& bound, uint8_t value ){
        return super().fill( poly, bound, value ); }

    /// \brief Fill the entire grid with values from the buffer
    /// 
    /// \param source - values to fill.  This must be the same byte-count as this layer
    /// \param fill_value - value to write inside the area
    bool fill( const std::vector<uint8_t>& source );

    uint8_t& get( const LocalLocation& p );

    uint8_t get(const LocalLocation& p) const;

    size_t lookup( const uint32_t i, const uint32_t j ) const;

    size_t lookup( const LocalLocation& p ) const;


    inline uint32_t cells_across_view() const { return cells_across_layer_; }

    inline double meters_across_cell() const { return meters_across_cell_; }
    inline double meters_across_view() const { return meters_across_layer_; }

    /// \brief Draws a simple debug representation of this grid to stderr
    void print_contents() const;

    inline void reset(){ 
        fill( default_cell_value );  }


    /// \brief Access the value at an (x, y)
    ///!
    /// \param p - the x,y coordinates to search at:
    /// \return reference to the cell value
    bool store(const LocalLocation& p, const uint8_t new_value);

    ~StaticGridLayer() = default;


protected:

    /// \brief number of cells along each dimension of this grid
    constexpr static size_t cells_across_layer_ = 1024;

    /// \brief width of each cell, in meters
    constexpr static double meters_across_cell_ = 16.0;

    /// \brief width of each cell, in meters
    constexpr static double meters_across_layer_ = meters_across_cell_ * cells_across_layer_;

    /// \brief contains the data for this tile
    // raw array:  2D addressing is performed through the index, below
    std::array<uint8_t, cells_across_layer_*cells_across_layer_> grid;

private:

    LayerInterface<StaticGridLayer>& super() {
        return *static_cast< LayerInterface<StaticGridLayer>* >(this);
    }

    const LayerInterface<StaticGridLayer>& super() const {
        return *static_cast< const LayerInterface<StaticGridLayer>* >(this);
    }
};


} // namespace