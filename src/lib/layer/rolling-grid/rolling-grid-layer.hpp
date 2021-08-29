// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <string>

#include "chart-box/chart-layer-interface.hpp"
#include "chart-box/geometry/polygon.hpp"
#include "chart-box/geometry/bound-box.hpp"

namespace chartbox::layer {

//  chart => layer => sector => cell
//            ^^^ you are here
class RollingGridLayer : public ChartLayerInterface< RollingGridLayer > {
public:

    constexpr static double sector_precision = 1.0;
    constexpr static size_t sector_dimension = 4;

    constexpr static double layer_precision = sector_precision * sector_dimension;
    constexpr static size_t layer_dimension = 3;
    
    /// \brief name of this layer's type
    constexpr static char type_name_[] = "RollingGridLayer";

public:
    /// \brief Constructs a new 2d square grid
    RollingGridLayer() = delete;
    
    /// \brief Constructs a new 2d square grid
    // RollingGridLayer(const geometry::BoundBox<UTMLocation>& _bounds);

    RollingGridLayer(double precision, const geometry::BoundBox<UTMLocation>& _bounds);

    /**
     *  Releases all memory associated with this quad tree.
     */
    ~RollingGridLayer(){};

    /**
     * Get the overall bounds of this tree
     *
     * @return Bounds object describing the tree's overall bounds.
     */
    const BoundBox<LocalLocation>& active() const;

    const BoundBox<LocalLocation>& allowed() const;

    bool active(const LocalLocation& p) const;

    bool fill( const uint8_t value );

    bool fill( const BoundBox<LocalLocation>& box, uint8_t value );

    bool fill( const Polygon<LocalLocation>& source, uint8_t value );

    bool focus( const BoundBox<LocalLocation>& new_bounds );

    uint8_t get(const LocalLocation& p) const;

    double precision() const;

    /// \brief Access the value at an (x, y) Eigen::Vector2d
    ///
    /// \param p - the x,y coordinates to search at:
    /// \param new_value - the value to stored at the specified location
    /// \return true if successful
    bool store(const LocalLocation& p, uint8_t new_value);

    /// \brief Draws a simple debug representation of this grid to stderr
    std::string print_contents() const;

    // /// \brief the _total_ number of cells in this grid === (width * height)
    // size_t width() const;


public:
    /// \brief Contains a grid of cells -- may be "relocated" to represent a different patch of terrain
    ///
    ///  chart => layer => sector => cell
    ///                    ^^ you are here
    ///
    class Sector {
        public:
            // Sector() = default;
            // ~Sector() = default;

            inline void fill( uint8_t value ) { data_.fill( value ); }

            inline uint8_t get( uint32_t col, uint32_t row ){ 
                return data_[lookup(col,row)]; }
            
            inline uint32_t lookup( uint32_t col, uint32_t row ) const { 
                return (col - column_anchor_) + (row - row_anchor_)*sector_dimension; }

            inline void move( uint32_t new_row_anchor, uint32_t new_column_anchor ){  
                row_anchor_ = new_row_anchor; 
                column_anchor_ = new_column_anchor; }

            inline uint32_t size() const { 
                return data_.size(); }

        private:
            // coordinates of the southest corner, in the frame of the larger grid.
            uint32_t row_anchor_ = 0;
            uint32_t column_anchor_ = 0;
            
            //  chart => layer => sector => cell
            //                              ^^^ you are here
            std::array<uint8_t, sector_dimension * sector_dimension> data_;
    };

private:

    std::filesystem::path cache_path;

    geometry::BoundBox<LocalLocation> active_bounds_;
    geometry::BoundBox<LocalLocation> allow_bounds_;

    double precision_;

    /// \brief contains the sectors for this tile
    typedef Sector sector_grid_t;  ///< placeholder
    //  chart => layer => sector => cell
    //                 ^^ you are here
    std::array<sector_grid_t, layer_dimension * layer_dimension > sector_holder_grid;

    // optional (?)
    // typedef uint8_t sector_map_t;  ///< placeholder
    // std::array<sector_grid_t*, layer_dimension * layer_dimension> sector_map;

};


} // namespace
