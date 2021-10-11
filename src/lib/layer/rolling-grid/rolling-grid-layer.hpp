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

#include "rolling-grid-sector.hpp"
#include "rolling-grid-index.hpp"

namespace chartbox::layer {

/// \brief represents an entire layer of scrolling data
/// 
/// This layer offers two important features:
///   1. the coordinates (and corresponding data) may be scrolled without undue slowdown 
///   2. Data may be cached in and out of relevance
/// 
///  chart => layer => sector => cell
///            ^^^ you are here
///
/// \param cells_across_sector cell count across a single dimension of each sector
/// \param sectors_across_view sector count across a single dimension of the view
template<uint32_t cells_across_sector, uint32_t sectors_across_view>
class RollingGridLayer : public ChartLayerInterface< RollingGridLayer<cells_across_sector,sectors_across_view> > {
public:

    // `*_across_*` variables count the numbe in each dimension of a square grid
    // `*_per_*` variables count the total number in the entire grid

    // constexpr static size_t cells_per_sector = cells_across_sector * cells_across_sector;  ///< not yet needed
    constexpr static double meters_across_cell = 1.0;   //aka precision
    // constexpr static double area_per_cell = meters_across_cell * meters_across_cell;

    // constexpr static size_t sectors_per_view = sectors_across_view * sectors_across_view;
    constexpr static double meters_across_sector = meters_across_cell * cells_across_sector; 
    // constexpr static double area_per_sector = area_per_cell * cells_per_sector;

    constexpr static size_t cells_across_view = cells_across_sector * sectors_across_view;
    constexpr static double meters_across_view = meters_across_cell * cells_across_view;

    constexpr static double precision = meters_across_cell;

    /// \brief name of this layer's type
    constexpr static char type_name_[] = "RollingGridLayer";

    // used to address a cell within a sector
    // Is explicitly a different type to prevent cross-talk
    typedef CellInSectorIndex<cells_across_sector> CellIndex;

    // used to address a sector within the active view
    // Is explicitly a different type to prevent cross-talk
    typedef SectorInViewIndex<sectors_across_view> SectorIndex;

    // used to address a cell within the active view
    // Is explicitly a different type to prevent cross-talk
    typedef CellInViewIndex<cells_across_sector,sectors_across_view> ViewIndex;

public:
    /// \brief Constructs a new 2d square grid
    RollingGridLayer();

    ~RollingGridLayer(){};

    // Center in the middle of the tracked bounds:
    // ( Assume tracked-bounds are already set )
    bool center();
    bool track( const BoundBox<LocalLocation>& bounds );
    bool track( const BoundBox<UTMLocation>& bounds );

    bool enable_cache( std::filesystem::path cache_path );

    bool fill( const uint8_t value );


    uint8_t get(const LocalLocation& p) const;

    inline uint8_t get( double easting, double northing ) const {
            return get({easting, northing});  }

    // size_t lookup( const LocalLocation& p ) const;

    /// \brief Draws a simple debug representation of this grid to stderr
    std::string print_contents() const;

    /// \brief Access the value at an (x, y)
    ///
    /// \param p - the x,y coordinates to search at:
    /// \param new_value - the value to stored at the specified location
    /// \return true if successful
    bool store(const LocalLocation& p, uint8_t new_value);

    /// \brief Get the bounds of the tracked (overall) area
    inline const BoundBox<LocalLocation>& tracked() const { return track_bounds_; }
    inline bool tracked(const LocalLocation& p) const { return track_bounds_.contains(p); }

    /// \brief Get the currently visible (active) bounds of this layer
    inline const BoundBox<LocalLocation>& visible() const { return view_bounds_; }
    inline bool visible(const LocalLocation& p) const { return view_bounds_.contains(p); }

    bool save( const SectorIndex& sector_index, const LocalLocation& sector_anchor );

    bool load( const SectorIndex& sector_index, const LocalLocation& sector_anchor );


    bool scroll_east();
    bool scroll_north();
    bool scroll_south();
    bool scroll_west();

private:
    std::filesystem::path generate_tilecache_filename( const std::filesystem::path& path, const LocalLocation& origin ) const;

    // this tracks the outer bounds (that the whole chart is tracking)
    geometry::BoundBox<LocalLocation> track_bounds_;
    geometry::BoundBox<LocalLocation> view_bounds_;

private:
    //  chart => layer => sector => cell
    //                 ^^ you are here -- this structure maps from the layer to the sectors 
    std::array<RollingGridSector<cells_across_sector>, sectors_across_view * sectors_across_view > sectors;

    // Just wrap the indexes around the grid, starting from the anchor:
    SectorIndex anchor;

    std::filesystem::path cache_path_;

private:

    ChartLayerInterface<RollingGridLayer<cells_across_sector,sectors_across_view>>& super() {
        return *static_cast< ChartLayerInterface<RollingGridLayer<cells_across_sector,sectors_across_view>>* >(this);
    }

    const ChartLayerInterface<RollingGridLayer<cells_across_sector,sectors_across_view>>& super() const {
        return *static_cast< const ChartLayerInterface<RollingGridLayer<cells_across_sector,sectors_across_view>>* >(this);
    }
};


} // namespace
