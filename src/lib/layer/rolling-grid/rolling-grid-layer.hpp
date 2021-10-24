// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>

#include "geometry/polygon.hpp"
#include "geometry/bound-box.hpp"
#include "layer/layer-interface.hpp"
#include "layer/grid-index.hpp"

#include "rolling-grid-sector.hpp"

namespace chartbox::layer::rolling {

/// \brief represents an entire layer of scrolling data
/// 
/// This layer offers two important features:
///   1. the coordinates (and corresponding data) may be scrolled without undue slowdown 
///   2. Data may be cached in and out of relevance
/// 
///  chart => layer => sector => cell
///            ^^^ you are here
///
/// Sources / Inspiration / Further Reading
/// 1. Grid-Map (aka Rolling-Grid)
///     - http://wiki.ros.org/grid_map
///     - https://github.com/ANYbotics/grid_map
/// 2. Implement Cyclical Grid-Buffer (Occupancy Grid) 
///     - https://github.com/embeddedartistry/embedded-resources/blob/master/examples/cpp/circular_buffer.cpp
///
/// \param cells_across_sector_ cell count across a single dimension of each sector
template<uint32_t cells_across_sector_>
class RollingGridLayer : public LayerInterface< RollingGridLayer<cells_across_sector_> > {
// `*_across_*` variables count the numbe in each dimension of a square grid
// `*_in_*` variables count the total number in the entire grid
public:

    /// \brief name of this layer's type
    constexpr static char type_name_[] = "RollingGridLayer";

    typedef RollingGridSector<cells_across_sector_> sector_t;

private:
    constexpr static double meters_across_cell_ = 1.0;   //aka precision

    constexpr static double meters_across_sector_ = meters_across_cell_ * cells_across_sector_;

    constexpr static uint32_t sectors_across_view_ = 5;  // TODO: convert to dynamic parameter...
    constexpr static uint32_t cells_across_view_ = sectors_across_view_ * cells_across_sector_; // TODO: make dynamic
    constexpr static uint32_t sectors_in_view_ = sectors_across_view_ * sectors_across_view_;
    constexpr static double meters_across_view_ = meters_across_cell_ * cells_across_view_;

public:
    /// \brief Constructs a new 2d square grid
    RollingGridLayer();

    ~RollingGridLayer(){};

    constexpr static uint32_t cells_across_sector() { return cells_across_sector_; }
    constexpr static uint32_t sectors_across_view() { return sectors_across_view_; }
    constexpr static uint32_t cells_across_view() { return cells_across_view_; }

    // Center in the middle of the tracked bounds:
    // ( Assume tracked-bounds are already set )
    bool center();

    /// \brief track the given bounds -- this does not update the actual data in the sectors!
    bool track( const BoundBox<LocalLocation>& bounds );
    bool track( const BoundBox<UTMLocation>& bounds );

    bool enable_cache( std::filesystem::path cache_path );

    bool fill( const uint8_t value );

    bool fill( const BoundBox<LocalLocation>& box, const uint8_t value ){
        return super().fill( box, value ); }

    bool fill( const Polygon<LocalLocation>& poly, const BoundBox<LocalLocation>& bound, uint8_t value ){
        return super().fill( poly, bound, value ); }

    // \brief flush layer contents to the internal cache
    bool flush_to_cache() const;

    // \brief load sector-tiles from the internal cache (if avaiable)
    bool load_from_cache();

    uint8_t get(const LocalLocation& p) const;

    inline uint8_t get( double easting, double northing ) const {
            return get({easting, northing});  }


    inline double meters_across_cell() const { return meters_across_cell_; }
    inline double meters_across_sector() const { return meters_across_sector_; }
    inline double meters_across_view() const { return meters_across_view_; }

    /// \brief Draws a simple debug representation of this grid to stderr
    std::string print_contents_by_cell( uint32_t indent = 0) const;
    std::string print_contents_by_location( uint32_t indent = 0 ) const;

    std::string print_properties( uint32_t indent = 0) const;

    bool scroll_east();
    bool scroll_north();
    bool scroll_south();
    bool scroll_west();

    const std::vector<sector_t>& sectors() const { return sectors_; }

    /// \brief Access the value at an (x, y)
    ///
    /// \param p - the x,y coordinates to search at:
    /// \param new_value - the value to stored at the specified location
    /// \return true if successful
    bool store(const LocalLocation& p, uint8_t new_value);

    /// \brief track from the given location  (... + the native width)
    bool track( const LocalLocation& bounds );

    /// \brief Get the bounds of the tracked (overall) area
    inline const BoundBox<LocalLocation>& tracked() const { return track_bounds_; }
    inline bool tracked(const LocalLocation& p) const { return track_bounds_.contains(p); }

    /// \brief shift the viewable area to this origin (with the current widths)
    bool view(const LocalLocation& p);

    /// \brief Get the currently visible (active) bounds of this layer
    inline const BoundBox<LocalLocation>& visible() const { return view_bounds_; }
    inline bool visible(const LocalLocation& p) const { return view_bounds_.contains(p); }
    

private:

    // Just wrap the indexes around the grid, starting from the anchor:
    GridIndex anchor_;

    //  chart => layer => sector => cell
    //                 ^^ you are here -- this structure maps from the layer to the sectors 
    std::vector<sector_t> sectors_;
    // NOTE: currently this is only adjusted|allocated in the constructor

    // this tracks the outer bounds (that the whole chart is tracking)
    geometry::BoundBox<LocalLocation> track_bounds_;

    // this tracks the inner bounds (where we're actively tracking cells)
    geometry::BoundBox<LocalLocation> view_bounds_;

private:

    LayerInterface<RollingGridLayer<cells_across_sector_>>& super() {
        return *static_cast< LayerInterface<RollingGridLayer<cells_across_sector_>>* >(this);
    }

    const LayerInterface<RollingGridLayer<cells_across_sector_>>& super() const {
        return *static_cast< const LayerInterface<RollingGridLayer<cells_across_sector_>>* >(this);
    }
};


} // namespace
