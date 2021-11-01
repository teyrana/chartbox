// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <string>

#include "layer/layer-interface.hpp"
#include "layer/grid-index.hpp"

namespace chartbox::layer::dynamic {

/// \brief represents an entire layer of scrolling data
/// 
/// This layer offers two important features:
///   1. the coordinates (and corresponding data) may be scrolled without undue slowdown 
///   2. Data may be cached in and out of relevance
/// 
///  chart => layer => sector => cell
///            ^^^ you are here
///
class DynamicGridLayer final : public LayerInterface<DynamicGridLayer> {
public:

    /// \brief name of this layer's type
    constexpr static char type_name_[] = "DynamicGridLayer";

    typedef std::vector<uint8_t> DynamicGridSector;

public:
    /// \brief Constructs a new 2d square grid
    DynamicGridLayer();

    ~DynamicGridLayer() = default;

    inline uint32_t cells_across_sector() const { return cells_across_sector_; }
    inline uint32_t cells_per_sector() const { return sectors_[0].size(); }
    uint32_t cells_across_sector( uint32_t new_cells_across );

    inline uint32_t cells_across_view() const { return cells_across_view_; }

    // Center in the middle of the tracked bounds:
    // ( Assume tracked-bounds are already set )
    bool center( const geometry::LocalLocation& center );
    bool origin( const geometry::LocalLocation& origin );

    bool fill( uint8_t value );

    // NYI -- not needed?
    // bool fill( const uint8_t* const buffer, size_t count );

    bool fill( const Path<LocalLocation>& path, const BoundBox<LocalLocation>& bounds, uint8_t value ){
        return super().fill( path, bounds, value); }

    bool fill( const BoundBox<LocalLocation>& box, const uint8_t value ){
        return super().fill( box, value ); }

    bool fill( const Polygon<LocalLocation>& poly, const BoundBox<LocalLocation>& bound, uint8_t value ){
        return super().fill( poly, bound, value ); }

    double meters_across_cell( double across );

    inline double meters_across_cell() const { return meters_across_cell_; }
    inline double meters_across_sector() const { return meters_across_sector_; }
    inline double meters_across_view() const { return meters_across_view_; }

    uint8_t get(const LocalLocation& p) const;

    const geometry::LocalLocation& origin() const { return view_bounds_.min; }

    double precision() const {  return meters_across_cell(); }

    /// \brief Draws a simple debug representation of this grid to stderr
    std::string to_cell_content_string( uint32_t indent = 0 ) const;
    std::string to_location_content_string( uint32_t indent = 0 ) const { return super().to_location_content_string(indent); }
    std::string to_sector_content_string( uint32_t indenrt = 0 ) const;
    std::string to_property_string( uint32_t indent = 0 ) const;

    bool save( const DynamicGridSector& sector, const LocalLocation& sector_origin ) const;

    inline uint32_t sectors_across_view() const { return sectors_across_view_; }

    // sets the numbers of sectors across a view
    uint32_t sectors_across_view( uint32_t new_sector_count );
    inline size_t sectors_per_view() const { return sectors_.size(); }

    /// \brief Access the value at an (x, y) Eigen::Vector2d
    ///
    /// \param p - the x,y coordinates to search at:
    /// \param new_value - the value to stored at the specified location
    /// \return true if successful
    bool store(const LocalLocation& p, uint8_t new_value);

    bool track( const geometry::BoundBox<geometry::LocalLocation>& bounds );

    /// \brief set the active-visible bounds for this layer
    bool view(const BoundBox<LocalLocation>& box);

    /// \brief Get the currently visible (active) bounds of this layer
    inline const BoundBox<LocalLocation>& visible() const { 
            return view_bounds_; }
    inline bool visible(const LocalLocation& p) const { 
            return view_bounds_.contains(p); }

    bool load( DynamicGridSector& sector, const LocalLocation& sector_origin );


private:

    // group 1: these variables are interdependent
    uint32_t cells_across_sector_;
    uint32_t sectors_across_view_;
    uint32_t cells_across_view_;

    // group 2: depends on group1
    double meters_across_cell_; // should this variable be pinned ?
    double meters_across_sector_;
    double meters_across_view_;

    // group 3: depends on group 1:
    //  chart => layer => sector => cell
    //                 ^^ you are here -- this structure maps from the layer to the sectors 
    std::vector<DynamicGridSector> sectors_;

    // group 4: depends on group 2
    // this tracks the outer bounds (that the whole chart is tracking)
    geometry::BoundBox<LocalLocation> view_bounds_;


private:

    LayerInterface<DynamicGridLayer>& super() {
        return *static_cast< LayerInterface<DynamicGridLayer>* >(this);
    }

    const LayerInterface<DynamicGridLayer>& super() const {
        return *static_cast< const LayerInterface<DynamicGridLayer>* >(this);
    }
};


} // namespace
