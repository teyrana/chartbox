// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <cmath>
#include <memory>
#include <cstdlib>
#include <string>
#include <vector>

#include "chart-box/chart-layer-interface.hpp"

namespace chartbox::layer {

// template<uint32_t dimension>
class StaticGridLayer : public ChartLayerInterface<StaticGridLayer> {
public:

    /// \brief number of cells along each dimension of this grid
    constexpr static size_t dimension = 1024;

    /// \brief name of this layer's type
    constexpr static char type_name_[] = "StaticGridLayer";

public:

    StaticGridLayer() = default;
    
    StaticGridLayer( const BoundBox<UTMLocation>& _bounds );

//    const std::array<uint8_t, dimension*dimension> & array() const;

    bool contains(const LocalLocation& p ) const;

    inline uint8_t* data() {
        return grid.data(); }

    inline const uint8_t* data() const {
        return grid.data(); }

    // override from ChartLayerInterface
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

    inline double precision() const {
        return precision_; }

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

    /// \brief width of each cell, in meters
    constexpr static double precision_ = 16.0;

    /// \brief contains the data for this tile
    // raw array:  2D addressing is performed through the index, below
    std::array<uint8_t, dimension*dimension> grid;

private:

    ChartLayerInterface<StaticGridLayer>& super() {
        return *static_cast< ChartLayerInterface<StaticGridLayer>* >(this);
    }

    const ChartLayerInterface<StaticGridLayer>& super() const {
        return *static_cast< const ChartLayerInterface<StaticGridLayer>* >(this);
    }
};


} // namespace
