// GPL v3 (c) 2021, Daniel Williams 

#include "chart-box/geometry/bound-box.hpp"
#include "chart-box/geometry/local-location.hpp"
#include "chart-box/geometry/polygon.hpp"

#include "rolling-grid-layer.hpp"

using chartbox::geometry::BoundBox;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::Polygon;

namespace chartbox::layer {

template<uint32_t cells_across_sector, uint32_t sectors_across_view>
RollingGridLayer<cells_across_sector,sectors_across_view>::RollingGridLayer(){
    // simple initialization
    track( BoundBox<LocalLocation>( {0,0}, {meters_across_view,meters_across_view} ));

    for( auto& each_sector : sectors){
        each_sector.fill(chartbox::layer::default_cell_value);
    }
}

template<uint32_t cells_across_sector, uint32_t sectors_across_view>
bool RollingGridLayer<cells_across_sector,sectors_across_view>::center() {
    const auto center = track_bounds_.center();
    view_bounds_.min = { center.easting - meters_across_view/2, center.northing - meters_across_view/2 };
    view_bounds_.max = { center.easting + meters_across_view/2, center.northing + meters_across_view/2 };
    view_bounds_ = view_bounds_.snap( meters_across_sector, meters_across_view );
    return true;
}

template<uint32_t cells_across_sector, uint32_t sectors_across_view>
bool RollingGridLayer<cells_across_sector,sectors_across_view>::enable_cache( std::filesystem::path new_path ){
    if( new_path.empty() ){
        fmt::print(stderr, "<<!! No path given !! : {}\n", new_path.string() );
        return false;
    }else if( not std::filesystem::is_directory(new_path) ){
        fmt::print(stderr, "    !! Could contour input path is not a directory!!: {}\n", new_path.string());
        return false;
    }

    cache_path_ = new_path;

    fmt::print( "    >>> Loading sectors from cache directory: {}\n", cache_path_.string() );
    fmt::print( "        :: Cells-across-Sector:  {} x {}\n", cells_across_sector,cells_across_sector );
    fmt::print( "        :: Sectors-across-View:  {} x {}\n", sectors_across_view, sectors_across_view );
    fmt::print( "        :: Track Origin:  @  {},{}  <  {},{}\n", track_bounds_.min.easting, track_bounds_.min.northing, track_bounds_.max.easting, track_bounds_.max.northing );
    fmt::print( "        :: View Origin:   @  {},{}  <  {},{}\n", view_bounds_.min.easting, view_bounds_.min.northing, view_bounds_.max.easting, view_bounds_.max.northing );

    for( uint32_t at_row = 0; at_row < sectors_across_view; ++at_row ){
        for( uint32_t at_column = 0; at_column < sectors_across_view; ++at_column ){

            const SectorIndex index(at_column, at_row);
            const LocalLocation sector_offset { at_column*meters_across_sector, at_row*meters_across_sector };
            const LocalLocation sector_origin = view_bounds_.min + sector_offset ;

            const auto& each_path = generate_tilecache_filename( cache_path_, sector_origin );

            fmt::print( "        [{:2d},{:2d}][{:2d}]: Loading sectors from cache directory: {}\n", at_column, at_row, index.offset(), each_path.string() );

            if( std::filesystem::exists(each_path))
            {
                // load file into tile

                // auto& sector = sectors[index.offset()];

//             { // Option A:
//                 //     // write bytes to file:
//                 //     std::ifstream source( from_path.string(), std::ios::binary );
//                 //     std::array<uint8_t, getSectorCacheSize()> buffer;
//                 //     source.read( reinterpret_cast<char*>(buffer.data()), buffer.size() );
//                 //     source.close();

//                 // raw bytes => structured struct
//                 // Get a pointer to the root object inside the buffer.
//                 //     const auto cell = chartbox::io::flatbuffer::GetCell( buffer.data() );

//                 //     fmt::print( "    >>> Loaded file into flatbuffer struct.\n" );
//                 //     fmt::print( stderr,  "    :: dimensions: {} x {} \n", to_layer.dimension, to_layer.dimension );
//                 //     fmt::print( stderr,  "    :: origin:     {} x {} \n", to_layer.bounds().min.easting, to_layer.bounds().min.northing );

//                 //     fmt::print( "    >>> Loaded file into flatbuffer struct.\n" );
//                 //     fmt::print( stderr,  "    :: dimensions: {} x {} \n", cell->width(), cell->height() );
//                 //     fmt::print( stderr,  "    :: origin:     {} x {} \n", cell->origin()->easting(), cell->origin()->northing() );


//                 //     // not a guaranteed property of the layer; not all of them have this...
//                 //     assert( cell->height() == sector.dimension() );
//                 //     assert( cell->width() == sector.dimension() );

//                 //     const LocalLocation origin( cell->origin()->easting(), cell->origin()->northing() );
//                 // assert( sector_origin.easting == cell->origin()->easting() );
//                 // assert( sector_origin.northing == cell->origin()->northing() );

// //     // current iteration -- eventually, this should be dynamic
// //     constexpr double tolerance = 1.0;
// //     assert( tolerance > std::fabs(cell->origin()->easting() - to_layer.bounds().min.easting) );
// //     assert( tolerance > std::fabs(cell->origin()->northing() - to_layer.bounds().min.northing) );

//                 //     sector.fill( cell->data()->data(), cell->data()->size() );

//             }

                // Option B:
                // const bool success = load( each_tile_path, each_sector );
                // sector.fill( path? );

                fmt::print( "    <<< Dev-Return-False\n" );
                return false;
            }else{
                // initialize as unknown
                sectors[index.offset()].fill(unknown_cell_value);
            }
        }
    }

    fmt::print( "    <<< Successfully Loaded.\n" );
    return true;
}

template<uint32_t cells_across_sector, uint32_t sectors_across_view>
std::filesystem::path RollingGridLayer<cells_across_sector,sectors_across_view>::generate_tilecache_filename( const std::filesystem::path& dir, const LocalLocation& origin ) const {
    const std::string filename = fmt::format("tile_local_{:06}E_{:06}N.fb", origin.easting, origin.northing );
    return dir / filename;
}

template<uint32_t cells_across_sector, uint32_t sectors_across_view>
bool RollingGridLayer<cells_across_sector,sectors_across_view>::fill(const uint8_t value){
    for( auto& each_sector : sectors){
        each_sector.fill(value);
    }
    return true;
}

template<uint32_t cells_across_sector, uint32_t sectors_across_view>
uint8_t RollingGridLayer<cells_across_sector,sectors_across_view>::get(const LocalLocation& layer_location ) const {

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
        // fmt::print( stderr, ">> get: location:     {:12.4f}, {:12.4f} ):\n", layer_location.easting, layer_location.northing );
        // fmt::print( stderr, "      cell-in-view-index:      {:2d}, {:2d} ({} sectors)({} cells)\n", view_index.column , view_index.row, sectors_across_view, cells_across_sector );
        // fmt::print( stderr, "      sector-in-view-index:    {:2d}, {:2d} => {:2d} \n", sector_in_view_index.column, sector_in_view_index.row, sector_offset );
        // fmt::print( stderr, "      cell-in-sector-index:    {:2d}, {:2d} => {:2d} \n", cell_in_sector_index.column, cell_in_sector_index.row, cell_offset );
        // }

        return current_cell_value;
    }

    return default_cell_value;
}

template<uint32_t cells_across_sector, uint32_t sectors_across_view>
bool RollingGridLayer<cells_across_sector,sectors_across_view>::load( const SectorIndex& index, const LocalLocation& /*location*/ ){
    // placeholder -- just reset sector
    sectors[ index.offset() ].fill(default_cell_value);

    return true;
}

template<uint32_t cells_across_sector, uint32_t sectors_across_view>
std::string RollingGridLayer<cells_across_sector,sectors_across_view>::print_contents() const {
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

template<uint32_t cells_across_sector, uint32_t sectors_across_view>
bool RollingGridLayer<cells_across_sector,sectors_across_view>::track( const BoundBox<LocalLocation>& new_bounds ) {
    constexpr double tolerance = 0.5; 

    // .1. Enforce zero-origin:
    if( (0 != new_bounds.min[0]) || (0 != new_bounds.min[1]) ){
        return false;
    }
    double width = new_bounds.max[0];

    // .2. Enforce square bounds:
    if( new_bounds.max[0] != new_bounds.max[1] ){
        width = std::max( new_bounds.max[0], new_bounds.max[1] );
    }

    // .3. Enforce width is a multiple of the sector width
    if( tolerance < std::fmod( width, meters_across_sector ) ){
        width = std::ceil( width/meters_across_sector) * meters_across_sector;
    }

    track_bounds_.min = {0,0};
    track_bounds_.max = {width,width};

    center();
    return true;
}

template<uint32_t cells_across_sector, uint32_t sectors_across_view>
bool RollingGridLayer<cells_across_sector,sectors_across_view>::track( const BoundBox<UTMLocation>& utm_bounds ) {
    return track( utm_bounds.relative().as<LocalLocation>()  );
}

template<uint32_t cells_across_sector, uint32_t sectors_across_view>
bool RollingGridLayer<cells_across_sector,sectors_across_view>::save( const SectorIndex& /*index*/, const LocalLocation& /*location*/ ){ 
    // placeholder
    // NYI
    return false; 
}

template<uint32_t cells_across_sector, uint32_t sectors_across_view>
bool RollingGridLayer<cells_across_sector,sectors_across_view>::scroll_east() {
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

template<uint32_t cells_across_sector, uint32_t sectors_across_view>
bool RollingGridLayer<cells_across_sector,sectors_across_view>::scroll_north() {
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

template<uint32_t cells_across_sector, uint32_t sectors_across_view>
bool RollingGridLayer<cells_across_sector,sectors_across_view>::scroll_south() {
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

template<uint32_t cells_across_sector, uint32_t sectors_across_view>
bool RollingGridLayer<cells_across_sector,sectors_across_view>::scroll_west() {
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

template<uint32_t cells_across_sector, uint32_t sectors_across_view>
bool RollingGridLayer<cells_across_sector,sectors_across_view>::store(const LocalLocation& layer_location, uint8_t new_value) {
    if( visible(layer_location) ){
        const LocalLocation view_location = layer_location - view_bounds_.min;

        const ViewIndex view_index = {  static_cast<uint32_t>(view_location.easting / meters_across_cell),
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

// used for tests
template class RollingGridLayer<4,3>;

// currently used for development:
template class RollingGridLayer<64,5>;

// used for production? in future?
template class RollingGridLayer<1024,7>;

}  // namespace
