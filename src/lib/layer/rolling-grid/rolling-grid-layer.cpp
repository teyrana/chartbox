// GPL v3 (c) 2020, Daniel Williams 

#include <cmath>

#include <Eigen/Geometry>

#include "chart-box/geometry/bound-box.hpp"
#include "chart-box/geometry/local-location.hpp"
#include "chart-box/geometry/polygon.hpp"

#include "rolling-grid-layer.hpp"

using chartbox::geometry::BoundBox;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::Polygon;

namespace chartbox::layer {

// /// \brief calculate index within a 2d grid, with rows of length `across` 
// template<uint32_t across>
// size_t lookup( uint32_t column, uint32_t row ) {
//     return column + row*across;
// }

RollingGridLayer::RollingGridLayer( const BoundBox<UTMLocation>& _bounds )
    : ChartLayerInterface<RollingGridLayer>(_bounds)
{

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
        const size_t sector_offset = view_index.divide(anchor).offset();

        /// location of cell within sector (indexed above)
        const size_t cell_offset = view_index.zoom().offset();

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

bool RollingGridLayer::focus( const BoundBox<LocalLocation>& request_bounds ) {
    // if the requested bounds don't even ovelap the 
    if( not track_bounds_.overlaps(request_bounds) ){
        return false;
    }

    // 1. enforce that views must align with the sectors
    // 2. enforce that views must be EXACTLY the width x height that our code assumes
    const BoundBox<LocalLocation> snapped_bounds = request_bounds.snap( meters_across_sector,  meters_across_view );
    // fmt::print( "    :: snap bounds:\n{}", snapped_bounds.dump("    "));

    fmt::print( "===========================================================\n" );
    BoundBox<LocalLocation> next_bounds;
    if( track_bounds_.contains(snapped_bounds) ){
        // case A: (easy) new bounds are _entirely_withn tracked_bounds
        next_bounds = snapped_bounds;
    }else{
        // case B: (complicated) new bounds must be adjusted:
        if( snapped_bounds.min.easting < track_bounds_.min.easting ){
            next_bounds.min.easting = track_bounds_.min.easting;
            next_bounds.max.easting = track_bounds_.min.easting + meters_across_view;
        }else if( snapped_bounds.max.easting > track_bounds_.max.easting ){
            next_bounds.min.easting = track_bounds_.max.easting - meters_across_view;
            next_bounds.max.easting = track_bounds_.max.easting;
        }else{
            next_bounds.min.easting = request_bounds.min.easting;
            next_bounds.max.easting = request_bounds.max.easting;
        }

        if( snapped_bounds.min.northing < track_bounds_.min.northing ){
            next_bounds.min.northing = track_bounds_.min.northing;
            next_bounds.max.northing = track_bounds_.min.northing + meters_across_view;
        }else if( snapped_bounds.max.northing > track_bounds_.max.northing ){
            next_bounds.min.northing = track_bounds_.max.northing - meters_across_view;
            next_bounds.max.northing = track_bounds_.max.northing;
        }else{
            next_bounds.min.northing = snapped_bounds.min.northing;
            next_bounds.max.northing = snapped_bounds.max.northing;
        }
    }
    // iteration direction: West => East, South => North;  Cell 0,0 and Sector 0,0 both start in the Southwest Corner.
    // for( uint32_t sector_column=0; sector_column < sectors_across_view; ++sector_column ){
    //     for( uint32_t sector_row=0; sector_row < sectors_across_view; ++sector_row ){
    //         const uint32_t old_sector_index = sector_column + (sector_row * sectors_across_view);
    //         const uint32_t new_sector_index = sector_column + (sector_row * sectors_across_view);

    //         fmt::print( "  {}??{} \n", old_sector_index, new_sector_index );

    // //         auto& each_sector = sectors[sector_index];
    // //         each_sector.anchor.column = each_cell_column;
    // //         each_sector.anchor.row = each_cell_row;

    // //         each_sector.fill(default_cell_value);

    // //         // debug
    //         // fmt::print( "      >> Moving cell: #{}  @ {}, {} \n", 0, sector_index, each_cell_column, each_cell_row );
    // //         each_sector.fill(sector_index);
    // //         // debug
    //     }
    // }

    {   // remap sectors:
        fmt::print( "======== ======= ======= Remapping Sectors ======= ======= =======\n");
        fmt::print( "    :: tracked:\n{}", track_bounds_.dump("    ") );

        const LocalLocation delta_bounds = next_bounds.min - view_bounds_.min;
        // const SectorIndex delta_index = delta_bounds.to_sector_index( meters_across_sector );
        const int32_t column_delta = delta_bounds.easting * meters_across_sector;
        const int32_t row_delta = delta_bounds.northing * meters_across_sector;

        fmt::print( "    :: Δ:        {},{}\n", delta_bounds.easting, delta_bounds.northing );
        fmt::print( "    :: move-by:  {},{}\n", column_delta, row_delta );

        // scroll east/west: only one of these will be executed in practice, but it's simpler to just leave them like this.
        for( int32_t i = column_delta; i > 0; i-- ){
            scroll_east();
        }
        for( int32_t i = column_delta; i < 0; i-- ){
            scroll_west();
        }

        // scroll north/south: only one of these will be executed in practice, but it's simpler to just leave them like this.
        for( int32_t i = row_delta; i > 0; i-- ){
            scroll_north();
        }
        for( int32_t i = row_delta; i < 0; i-- ){
            scroll_south();
        }
    }

    fmt::print( "    :: old view:\n{}", view_bounds_.dump("    ") );
    fmt::print( "    :: next view:\n{}", next_bounds.dump("    "));

    // ought to be redundant
    // view_bounds_ = next_bounds;

    // DEBUG
    fmt::print( "{}\n", print_contents());

    return true;
}

bool RollingGridLayer::load( const SectorIndex& index, const LocalLocation& /*location*/ ){
    // placeholder -- just reset sector
    sectors[ index.offset() ].fill(default_cell_value);

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

            const CellIndex cell_lookup_index( cell_column_index, cell_row_index );

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
    buf << "     View:      [ [ " << view_bounds_.min[0] << ", " << view_bounds_.min[1] << " ] < [ "
                                 << view_bounds_.max[0] << ", " << view_bounds_.max[1] << " ] ]\n";
    buf << "     Anchor:    ( " << anchor.column << ", " << anchor.row << " )\n";
    
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

bool RollingGridLayer::save( const SectorIndex& /*index*/, const LocalLocation& /*location*/ ){ 
    // placeholder
    // NYI
    return false; 
}

bool RollingGridLayer::scroll_east() {
 const auto& last_bounds = view_bounds_;
    const LocalLocation delta( meters_across_sector, 0.0 );
    const auto next_bounds = last_bounds.move( delta );
    const uint32_t at_column = static_cast<uint32_t>(anchor.column + 1 + sectors_across_view)%sectors_across_view;
    const SectorIndex next_anchor{ at_column, anchor.row };

    // // debug
    // fmt::print( stderr, ">>> scroll:east/+1:    anchor:    {}, {}:\n", anchor.column, anchor.row );
    // fmt::print( "    :: old view:\n{}", last_bounds.dump("    "));
    // fmt::print( "    :: next view:\n{}", next_bounds.dump("    "));
    // fmt::print( "    :: Δ(m):           {},{}\n", delta.easting, delta.northing );
    // fmt::print( "    :: Last Anchor:    {},{}\n", anchor.column, anchor.row );
    // fmt::print( "    :: Next Anchor:    {},{}\n", next_anchor.column, next_anchor.row );

    for( uint32_t at_row = 0; at_row < sectors_across_view; ++at_row ){
        const SectorIndex at_index( anchor.column, at_row );

        // (A) Store values to old location
        const LocalLocation from_location = (LocalLocation( at_index.column, at_index.row) * meters_across_sector) + last_bounds.min;
        // fmt::print( stderr, "    @[ {}, {}]:   >> store: ( {}, {} )\n", at_index.column, at_index.row, from_location.easting, from_location.northing );
        save( at_index, from_location );

        // (B) Load values at new location 
        const LocalLocation to_location( (next_bounds.max.easting - meters_across_sector), from_location.northing );
        // fmt::print( stderr, "                << load:  ( {}, {} )\n", to_location.easting, to_location.northing );
        load( at_index, to_location );
    }

    anchor = next_anchor;
    view_bounds_ = next_bounds;
    return true;
}

bool RollingGridLayer::scroll_north() {
    const auto& last_bounds = view_bounds_;
    const LocalLocation delta( 0.0, meters_across_sector);
    const auto next_bounds = last_bounds.move( delta );
    const uint32_t at_row = static_cast<uint32_t>(anchor.row + 1 + sectors_across_view)%sectors_across_view;
    const SectorIndex next_anchor{ anchor.column, at_row };

    for( uint32_t at_column = 0; at_column < sectors_across_view; ++at_column ){
        const SectorIndex at_index( at_column, anchor.row );

        // (A) Store values to old location
        const LocalLocation from_location = (LocalLocation(at_index.column, at_index.row) * meters_across_sector) + last_bounds.min;
        // fmt::print( stderr, "    @[ {}, {}]:   >> store: ( {}, {} )\n", at_index.column, at_index.row, from_location.easting, from_location.northing );
        save( at_index, from_location );

        // (B) Load values at new location 
        const LocalLocation to_location( from_location.easting, next_bounds.max.northing-meters_across_sector );
        // fmt::print( stderr, "                << load:  ( {}, {} )\n", to_location.easting, to_location.northing );
        load( at_index, to_location );
    }

    anchor = next_anchor;
    view_bounds_ = next_bounds;
    return true;
}

bool RollingGridLayer::scroll_south() {
    const auto& last_bounds = view_bounds_;
    const LocalLocation delta( 0.0, -meters_across_sector);
    const auto next_bounds = last_bounds.move( delta );
    
    const uint32_t at_row = (anchor.row + sectors_across_view - 1 ) % sectors_across_view;
    const SectorIndex next_anchor{ anchor.column, at_row };

    // debug
    // fmt::print( stderr, ">>> scroll:south/-1:    anchor:    {}, {}:\n", anchor.column, anchor.row );
    // fmt::print( "    :: old view:\n{}", last_bounds.dump("    "));
    // fmt::print( "    :: next view:\n{}", next_bounds.dump("    "));
    // fmt::print( "    :: Δ(m):           {},{}\n", delta.easting, delta.northing );
    // fmt::print( "    :: Last Anchor:    {},{}\n", anchor.column, anchor.row );
    // fmt::print( "    :: Next Anchor:    {},{}\n", next_anchor.column, next_anchor.row );


    for( uint32_t at_column = 0; at_column < sectors_across_view; ++at_column ){
        const SectorIndex at_index( at_column, at_row );

        // (A) Store values to old location
        const LocalLocation from_location = (LocalLocation(at_index.column, at_index.row) * meters_across_sector) + last_bounds.min;
        // fmt::print( stderr, "    @[ {}, {}]:   >> store: ( {}, {} )\n", at_index.column, at_index.row, from_location.easting, from_location.northing );
        save( at_index, from_location );

        // (B) Load values at new location 
        const LocalLocation to_location( from_location.easting, next_bounds.min.northing );
        // fmt::print( stderr, "                << load:  ( {}, {} )\n", to_location.easting, to_location.northing );
        load( at_index, to_location );
    }

    anchor = next_anchor;
    view_bounds_ = next_bounds;
    return true;
}

bool RollingGridLayer::scroll_west() {
    const auto& last_bounds = view_bounds_;
    const LocalLocation delta( -meters_across_sector, 0.0 );
    const auto next_bounds = last_bounds.move( delta );

    const uint32_t at_column = (anchor.column + sectors_across_view - 1 )% sectors_across_view;
    const SectorIndex next_anchor{ at_column, anchor.row };

    // fmt::print( ">>> scroll:west/-1:    anchor:    {}, {}:\n", anchor.column, anchor.row );
    // fmt::print( "    :: old view:\n{}", last_bounds.dump("    ") );
    // fmt::print( "    :: next view:\n{}", next_bounds.dump("    "));
    // fmt::print( "    :: Δ(m):        {},{}\n", delta.easting, delta.northing );
    // fmt::print( "    :: Last Anchor:    {},{}\n", anchor.column, anchor.row );
    // fmt::print( "    :: Next Anchor:    {},{}\n", next_anchor.column, next_anchor.row );


    for( uint32_t at_row = 0; at_row < sectors_across_view; ++at_row ){
        const SectorIndex at_index( at_column, at_row );

        // (A) Store values to old location
        const LocalLocation from_location = (LocalLocation( at_index.column, at_index.row) * meters_across_sector) + last_bounds.min;
        // fmt::print( stderr, "    @[ {}, {}]:   >> store: ( {}, {} )\n", at_index.column, at_index.row, from_location.easting, from_location.northing );
        save( at_index, from_location );

        // (B) Load values at new location 
        const LocalLocation to_location( next_bounds.min.easting, from_location.northing );
        // fmt::print( stderr, "                << load:  ( {}, {} )\n", to_location.easting, to_location.northing );
        load( at_index, to_location );
    }

    anchor = next_anchor;
    view_bounds_ = next_bounds;
    return true;
}

bool RollingGridLayer::store(const LocalLocation& layer_location, uint8_t new_value) {
    if( visible(layer_location) ){   
        
        const LocalLocation view_location = layer_location - view_bounds_.min;

        const ViewIndex view_index = { static_cast<uint32_t>(view_location.easting / meters_across_cell),
                                         static_cast<uint32_t>(view_location.northing / meters_across_cell) };

        /// index of the sector to lookup in
        const size_t sector_offset = view_index.divide(anchor).offset();

        /// location of cell within sector (indexed above)
        const size_t cell_offset = view_index.zoom().offset();

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
