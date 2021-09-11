// GPL v3 (c) 2020, Daniel Williams 

// #include <cmath>

#include <Eigen/Geometry>

#include "chart-box/geometry/bound-box.hpp"
#include "chart-box/geometry/local-location.hpp"
#include "chart-box/geometry/polygon.hpp"

#include "rolling-grid-layer.hpp"

using chartbox::geometry::BoundBox;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::Polygon;

namespace chartbox::layer {

RollingGridLayer::RollingGridLayer( const BoundBox<UTMLocation>& _bounds )
    : ChartLayerInterface<RollingGridLayer>(_bounds)
{

    // fmt::print( "Bounds:\n");
    // fmt::print( "      min:      {:12.6f}, {:12.6f} \n", _bounds.min.easting, _bounds.min.northing );
    // fmt::print( "      max:      {:12.6f}, {:12.6f} \n", _bounds.max.easting, _bounds.max.northing );
    // fmt::print( "      Width:    {:12.6f}, {:12.6f} \n", _bounds.width(), _bounds.width() );
    // fmt::print( "Grid:\n");
    // fmt::print( "      dimension:    {:12lu} x {:%12lu} \n", dimension, dimension );

    // temporary / placeholder version:
    track_bounds_.min = {0,0};
    track_bounds_.max = {_bounds.width(), _bounds.height()};

    const auto center = track_bounds_.center();
    view_bounds_.min = { center.easting - meters_across_view/2, center.northing - meters_across_view/2 };
    view_bounds_.max = { center.easting + meters_across_view/2, center.northing + meters_across_view/2 };
    view_bounds_ = view_bounds_.snap( meters_across_sector, meters_across_view );

    anchor = {0,0};

    // iteration progress West => East, South => North;  Cell 0,0 and Sector 0,0 both start in the Southwest Corner.
    for( uint32_t sector_row=0; sector_row < sectors_across_view; ++sector_row ){
        for( uint32_t sector_column=0; sector_column < sectors_across_view; ++sector_column ){
            const uint32_t sector_index = sector_column + (sector_row * sectors_across_view);
            sectors[sector_index].fill(default_cell_value);
        }
    }
}

bool RollingGridLayer::fill(const uint8_t value){
    for( auto& each_sector : sectors){
        each_sector.fill(value);
    }
    return true;
}

uint8_t RollingGridLayer::get( double easting, double northing ) const {
    return get( {easting, northing} );
}

uint8_t RollingGridLayer::get(const LocalLocation& layer_location ) const {
    if( visible(layer_location) ){
        
        const LocalLocation view_location = layer_location - view_bounds_.min;

        const ViewIndex view_index = {  static_cast<uint32_t>(view_location.easting / meters_across_cell),
                                        static_cast<uint32_t>(view_location.northing / meters_across_cell) };

        /// index of the sector to lookup in
        // const size_t sector_offset = view_index.divide<SectorIndex>(cells_across_sector).offset();
        const size_t sector_offset = view_index.divide<SectorIndex>(cells_across_sector).wrap().offset();

        /// location of cell within sector (indexed above)
        const size_t cell_offset = view_index.zoom<CellIndex>(cells_across_sector).offset();

        // current_index.get_sector()  // refactor idea ==> retreives raw-index to sector-within-layer
        // current_index.get_cell()    // refactor idea: retreives raw-index to cell-within-sector
        // const auto& current_sector = sectors[ sector_index ];
        uint8_t current_cell_value = sectors[ sector_offset ][ cell_offset ];

        // {
        // const SectorIndex sector_in_view_index = (anchor + view_index.divide<SectorIndex>(cells_across_sector));
        // const CellIndex cell_in_sector_index = view_index.zoom<CellIndex>(cells_across_sector);

        // fmt::print( stderr, ">> get: location:     {:12.4f}, {:12.4f} ):\n", layer_location.easting, layer_location.northing );
        // fmt::print( stderr, "      cell-in-view-index:      {:2d}, {:2d} ({} sectors)({} cells)\n", view_index.column , view_index.row, sectors_across_view, cells_across_sector );
        // fmt::print( stderr, "      sector-in-view-index:    {:2d}, {:2d} => {:2d} \n", sector_in_view_index.column, sector_in_view_index.row, sector_offset );
        // fmt::print( stderr, "      cell-in-sector-index:    {:2d}, {:2d} => {:2d} \n", cell_in_sector_index.column, cell_in_sector_index.row, cell_offset );
        // }

        return current_cell_value;
    }

    return default_cell_value;
}

bool RollingGridLayer::focus(const BoundBox<LocalLocation>& new_bounds ) {

    if( view_bounds_.overlaps(new_bounds) ){

        // iteration progress West => East, South => North;  Cell 0,0 and Sector 0,0 both start in the Southwest Corner.

        // for( uint32_t sector_column=0; sector_column < sectors.size(); ++sector_column ){
        //     const uint32_t each_cell_column = sector_column * cells_across_sector; // cell column, in the layer frame
        //     for( uint32_t sector_row=0; sector_row < sectors.size(); ++sector_row ){
        //         const uint32_t sector_index = sector_column + (sector_row * sectors_across_view);
        //         const uint32_t each_cell_row = sector_row * cells_across_sector; // cell row, in the layer frame
                
        //         auto& each_sector = sectors[sector_index];
        //         each_sector.anchor.column = each_cell_column;
        //         each_sector.anchor.row = each_cell_row;

        //         each_sector.fill(default_cell_value);

        //         // debug
        //         fmt::print( "      >> Moving cell: #{}  @ {}, {} \n", 0, sector_index, each_cell_column, each_cell_row );
        //         each_sector.fill(sector_index);
        //         // debug
        //     }
        // }

        view_bounds_ = new_bounds;

        return true;
    }

    return false;
}

std::string RollingGridLayer::print_contents() const {
    std::ostringstream buf;

    // print raw contents
    buf << "======== ======= ======= Printing By Cell: ======= ======= =======\n";
    buf << std::hex;
    for (size_t cell_row_index = cells_across_view - 1; cell_row_index < cells_across_view; --cell_row_index) {
        for (size_t cell_column_index = 0; cell_column_index < cells_across_view; ++cell_column_index ) {
            if( 0 == ((cell_column_index) % cells_across_sector ) ){
                buf << "    ";
            }

            const size_t sector_index = (cell_column_index/cells_across_sector) + (cell_row_index/cells_across_sector)*sectors_across_view;
            const auto& current_sector = sectors[ sector_index ];

            const CellIndex cell_lookup_index( static_cast<uint32_t>(cell_column_index), static_cast<uint32_t>(cell_row_index) );

            uint8_t current_cell_value = 0xFF;
            if( current_sector.contains( cell_lookup_index )){
                current_cell_value = current_sector.get( cell_lookup_index );
            }

            buf << ' ' << std::setfill('0') << std::setw(2) << static_cast<int>(current_cell_value);
        }
        if( 0 == (cell_row_index % cells_across_sector ) ){
            buf << '\n';
        }

        buf << '\n';
    }

    buf << "======== ======= ======= Printing By Location ======= ======= =======\n";
    // print location-aware contents
    for( double northing = (view_bounds_.max.northing - (meters_across_cell/2)); northing > view_bounds_.min.northing; northing -= meters_across_cell ) {
        for( double easting = (view_bounds_.min.easting + (meters_across_cell/2)); easting < view_bounds_.max.easting; easting += meters_across_cell ) {
            if( 0.6 > std::fabs(std::fmod( easting - view_bounds_.min.easting, meters_across_sector )) ){
                buf << "    ";
            }

            const LocalLocation cell_lookup_index( easting, northing );

            uint8_t current_cell_value = get( cell_lookup_index );

            buf << ' ' << std::setw(2) << static_cast<int>(current_cell_value);

        }
        if( 0.8 > std::fabs(std::fmod( northing - view_bounds_.min.northing, meters_across_sector ))){
            buf << '\n';
        }

        buf << '\n';
    }

    buf << "======== ======= ======= ======= ======= ======= ======= =======\n";
    return buf.str();
}

bool RollingGridLayer::store(const LocalLocation& layer_location, uint8_t new_value) {
    if( visible(layer_location) ){   
        
        const LocalLocation view_location = layer_location - view_bounds_.min;

        const SectorIndex view_index = { static_cast<uint32_t>(view_location.easting / meters_across_cell),
                                         static_cast<uint32_t>(view_location.northing / meters_across_cell) };

        /// index of the sector to lookup in
        // const size_t sector_offset = view_index.divide<SectorIndex>(cells_across_sector).offset();
        const size_t sector_offset = view_index.divide<SectorIndex>(cells_across_sector).wrap().offset();

        /// location of cell within sector (indexed above)
        const size_t cell_offset = view_index.zoom<CellIndex>(cells_across_sector).offset();

        sectors[ sector_offset ][ cell_offset ] = new_value;

        // fmt::print( stderr, ">> store   @location:     {:12.4f}, {:12.4f} <<== {:X} \n", layer_location.easting, layer_location.northing, new_value );
        // fmt::print( stderr, "      in-view:            {:12.4f}, {:12.4f} \n", view_location.easting, view_location.northing );
        // fmt::print( stderr, "      cell-in-layer-index:     {:2d}, {:2d} \n", view_cell_column , view_cell_row );
        // fmt::print( stderr, "      sector-in-layer-index:   {:2d} \n", sector_index );
        // fmt::print( stderr, "      cell-in-sector-index:    {:2d} \n", cell_index );

        return true;
    }

    return false;
}

const BoundBox<LocalLocation>& RollingGridLayer::tracked() const {
    return track_bounds_;
}


bool RollingGridLayer::tracked( const LocalLocation& p ) const {
    return track_bounds_.contains(p);
}

const BoundBox<LocalLocation>& RollingGridLayer::visible() const {
    return view_bounds_;
}

bool RollingGridLayer::visible( const LocalLocation& p ) const {
    return view_bounds_.contains(p);
}


}  // namespace
