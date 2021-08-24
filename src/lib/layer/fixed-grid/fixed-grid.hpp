// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <cmath>
#include <memory>
#include <cstdlib>
#include <string>
#include <vector>

#include "chart-box/chart-layer-interface.hpp"

namespace chartbox::layer {

class FixedGridLayer : public chartbox::ChartLayerInterface< uint8_t, FixedGridLayer> {
public:
    typedef uint8_t cell_t;
    typedef Eigen::Matrix<uint32_t,2,1> Vector2u;

    /// \brief number of cells along each dimension of this grid
    constexpr static size_t dimension = 1024;

    constexpr static cell_t default_value = 0xff;
    constexpr static cell_t blocking_threshold = 'A';

public:

    FixedGridLayer() = delete;
    
    FixedGridLayer( const BoundBox<UTMLocation>& _bounds, layer_purpose_t _purpose );

    const std::array<cell_t, dimension*dimension> & array() const;
    cell_t* data();

    // override from ChartLayerInterface
    bool fill( const cell_t value );

    bool fill( const cell_t* const buffer, size_t count );
    
    bool fill( const BoundBox<LocalLocation>& box, const cell_t value ){
        return super().fill( box, value ); }

    bool fill( const Polygon<LocalLocation>& poly, cell_t value ){
        return super().fill( poly, value ); }

    /// \brief Fill the entire grid with values from the buffer
    /// 
    /// \param source - values to fill.  This must be the same byte-count as this layer
    /// \param fill_value - value to write inside the area
    bool fill( const std::vector<cell_t>& source );

    // frame: local frame
    cell_t& get(const Eigen::Vector2d& p);
    // frame: local frame
    cell_t get(const Eigen::Vector2d& p) const;

    size_t lookup( const uint32_t x, const uint32_t y ) const;

    size_t lookup( const Vector2u i ) const;

    size_t lookup( const Eigen::Vector2d& p ) const;

    double precision() const;

    /// \brief Draws a simple debug representation of this grid to stderr
    void print_contents() const;

    void reset();

    /// \brief Access the value at an (x, y) Eigen::Vector2d
    ///!
    /// \param Eigen::Vector2d - the x,y coordinates to search at:
    /// \return reference to the cell value
    bool store(const Eigen::Vector2d& p, const cell_t new_value);

    std::string type() const;

    ~FixedGridLayer();

    // /// \brief Retrieve the value at an (x, y) Eigen::Vector2d
    // ///
    // /// \param Eigen::Vector2d - the x,y coordinates to search at
    // /// \param default_value to return on out-of-bounds
    // /// \return the cell value
    // cell_t classify(const Eigen::Vector2d& p, const cell_t default_value) const;

    // /// note: this version has a default value of '0xFF'
    // cell_t classify(const Eigen::Vector2d& p) const;

    // // Overide from ChartInterface
    // bool contains(const Eigen::Vector2d& p) const;

    // /// \brief convert a nav-space location to a storage-space index
    // /// \warning behavior is undefined if `location` is outside of chart bounds
    // /// \warning rounds up, on borders
    // index::Index2u as_index(const Eigen::Vector2d& location) const;

    // /// \brief convert a storage-space location to a navigation-space location
    // /// \warning behavior is undefined if `index` is outside of chart bounds 
    // Vector2d as_location(const index::Index2u& index) const;

    // /// \brief Load data into the grid, in memory-order
    // /// \param load n cells from the given pointer 
    // int fill_from_buffer(const std::vector<cell_t>& source);

    // /// \brief simply returns the value or reference to the internal data
    // /// \warning !! DOES NOT CHECK BOUNDS !!
    // cell_t& get_cell(const size_t xi, const size_t yi);
    // cell_t get_cell(const size_t xi, const size_t yi) const ;

    // cell_t& operator[](const index::Index2u& i);
    // cell_t operator[](const index::Index2u& i) const ;

    // bool to_png(const std::string filename) const;


protected:
    /// \brief name of this layer's type
    constexpr static char type_[] = "FixedGridLayer";

    /// \brief contains the data for this tile
    // raw array:  2D addressing is performed through the index, below
    std::array<cell_t, dimension*dimension> grid;

private:

    chartbox::ChartLayerInterface< uint8_t, FixedGridLayer>& super() {
        return *static_cast<chartbox::ChartLayerInterface< uint8_t, FixedGridLayer>*>(this);
    }

    const chartbox::ChartLayerInterface< uint8_t, FixedGridLayer>& super() const {
        return *static_cast<const chartbox::ChartLayerInterface< uint8_t, FixedGridLayer>*>(this);
    }
};


} // namespace chartbox::grid
