// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <cmath>
#include <memory>
#include <cstdlib>
#include <string>
#include <vector>

#include "chart-box/chart-layer-interface.hpp"

namespace chartbox::layer {

template< chartbox::layer::role_t layer_role >
class FixedGridLayer : public ChartLayerInterface<FixedGridLayer<layer_role>, layer_role> {
public:
    typedef Eigen::Matrix<uint32_t,2,1> Vector2u;

    /// \brief number of cells along each dimension of this grid
    constexpr static size_t dimension = 1024;

    /// \brief name of this layer's type
    constexpr static char type_name_[] = "FixedGridLayer";

public:

    FixedGridLayer() = delete;
    
    FixedGridLayer( const BoundBox<UTMLocation>& _bounds );

//    const std::array<uint8_t, dimension*dimension> & array() const;

    inline uint8_t* data() {
        return grid.data(); }

    inline const uint8_t* data() const {
        return grid.data(); }

    // override from ChartLayerInterface
    bool fill( const uint8_t value );

    bool fill( const uint8_t* const buffer, size_t count );
    
    bool fill( const BoundBox<LocalLocation>& box, const uint8_t value ){
        return super().fill( box, value ); }

    bool fill( const Polygon<LocalLocation>& poly, uint8_t value ){
        return super().fill( poly, value ); }

    /// \brief Fill the entire grid with values from the buffer
    /// 
    /// \param source - values to fill.  This must be the same byte-count as this layer
    /// \param fill_value - value to write inside the area
    bool fill( const std::vector<uint8_t>& source );

    uint8_t& get( const LocalLocation& p );

    uint8_t get(const LocalLocation& p) const;

    size_t lookup( const uint32_t x, const uint32_t y ) const;

    size_t lookup( const Vector2u i ) const;

    size_t lookup( const LocalLocation& p ) const;

    double precision() const;

    /// \brief Draws a simple debug representation of this grid to stderr
    void print_contents() const;

    inline void reset(){ 
        fill( super().default_value );  }


    /// \brief Access the value at an (x, y) Eigen::Vector2d
    ///!
    /// \param Eigen::Vector2d - the x,y coordinates to search at:
    /// \return reference to the cell value
    bool store(const LocalLocation& p, const uint8_t new_value);

    ~FixedGridLayer() = default;

    // /// \brief Retrieve the value at an (x, y) Eigen::Vector2d
    // ///
    // /// \param Eigen::Vector2d - the x,y coordinates to search at
    // /// \param default_value to return on out-of-bounds
    // /// \return the cell value
    // uint8_t classify(const Eigen::Vector2d& p, const uint8_t default_value) const;

    // /// note: this version has a default value of '0xFF'
    // uint8_t classify(const Eigen::Vector2d& p) const;

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
    // int fill_from_buffer(const std::vector<uint8_t>& source);

    // /// \brief simply returns the value or reference to the internal data
    // /// \warning !! DOES NOT CHECK BOUNDS !!
    // uint8_t& get_cell(const size_t xi, const size_t yi);
    // uint8_t get_cell(const size_t xi, const size_t yi) const ;

    // uint8_t& operator[](const index::Index2u& i);
    // uint8_t operator[](const index::Index2u& i) const ;

    // bool to_png(const std::string filename) const;


protected:

    /// \brief contains the data for this tile
    // raw array:  2D addressing is performed through the index, below
    std::array<uint8_t, dimension*dimension> grid;

private:

    ChartLayerInterface< FixedGridLayer<layer_role>, layer_role>& super() {
        return *static_cast< ChartLayerInterface< FixedGridLayer<layer_role>, layer_role>* >(this);
    }

    const ChartLayerInterface< FixedGridLayer<layer_role>, layer_role>& super() const {
        return *static_cast< const ChartLayerInterface< FixedGridLayer<layer_role>, layer_role>* >(this);
    }
};


} // namespace

#include "fixed-grid.inl"
