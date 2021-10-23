// GPL v3 (c) 2021, Daniel Williams 

#include "geometry/bound-box.hpp"
#include "geometry/local-location.hpp"
#include "geometry/polygon.hpp"

#include "rolling-grid-layer.hpp"

#include "io/flatbuffer.hpp"

using chartbox::geometry::BoundBox;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::Polygon;

namespace chartbox::layer::rolling {

template<uint32_t cells_across_sector_>
RollingGridLayer<cells_across_sector_>::RollingGridLayer()
    : anchor_({0,0})
    , sectors_(sectors_in_view_)
    , track_bounds_( {0,0}, {meters_across_view_,meters_across_view_} )
    , view_bounds_( {0,0}, {meters_across_view_,meters_across_view_} )
{
    fill( chartbox::layer::default_cell_value );
}

template<uint32_t cells_across_sector_>
bool RollingGridLayer<cells_across_sector_>::center() {
    const auto center = track_bounds_.center();
    view_bounds_.min = { center.easting - meters_across_view_/2, center.northing - meters_across_view_/2 };
    view_bounds_.max = { center.easting + meters_across_view_/2, center.northing + meters_across_view_/2 };
    view_bounds_ = view_bounds_.snap( meters_across_sector_, meters_across_view_ );
    return true;
}

template<uint32_t cells_across_sector_>
bool RollingGridLayer<cells_across_sector_>::enable_cache( std::filesystem::path new_path ){
    if( new_path.empty() ){
        fmt::print(stderr, "<<!! No path given !! : {}\n", new_path.string() );
        return false;
    }else if( not std::filesystem::is_directory(new_path) ){
        fmt::print(stderr, "    !! Could contour input path is not a directory!!: {}\n", new_path.string());
        return false;
    }

    return chartbox::io::flatbuffer::enable( new_path );
}

template<uint32_t cells_across_sector_>
bool RollingGridLayer<cells_across_sector_>::fill(const uint8_t value){
    for( auto& each_sector : sectors_){
        each_sector.fill(value);
    }
    return true;
}

template<uint32_t cells_across_sector_>
bool RollingGridLayer<cells_across_sector_>::flush_to_cache() const {

    for( uint32_t at_row = 0; at_row < sectors_across_view_; ++at_row ){
        for( uint32_t at_column = 0; at_column < sectors_across_view_; ++at_column ){
            const GridIndex at_index( at_column, at_row );

            auto& sector = sectors_[at_index.offset(sectors_across_view_)];
            
            const LocalLocation relative_location = LocalLocation(at_index.column, at_index.row) * meters_across_sector_;
            const LocalLocation absolute_location = view_bounds_.min + relative_location;

            // fmt::print( stderr, "{:>16s}@[ {}, {}]:   >> store: ( {}, {} )\n", "", at_index.column, at_index.row, absolute_location.easting, absolute_location.northing );
            chartbox::io::flatbuffer::save( sector, absolute_location );
        }
    }

    return true;
}


template<uint32_t cells_across_sector_>
bool RollingGridLayer<cells_across_sector_>::load_from_cache() {
    if( chartbox::io::flatbuffer::active() ){
        for( uint32_t at_row = 0; at_row < sectors_across_view_; ++at_row ){
            for( uint32_t at_column = 0; at_column < sectors_across_view_; ++at_column ){

                const GridIndex index(at_column, at_row);
                const LocalLocation sector_offset = { at_column*meters_across_sector_, at_row*meters_across_sector_ };
                const LocalLocation sector_origin = view_bounds_.min + sector_offset ;

                auto& sector = sectors_[index.offset(sectors_across_view_)];

                chartbox::io::flatbuffer::load( sector_origin, sector);
            }
        }

        // fmt::print( "    <<< Successfully Loaded Cache @ {},{}\n", view_bounds_.min.easting, view_bounds_.min.northing );
    }else{
        fmt::print( "    !!! Error: Cache is not active!\n" );
    }

    return true;
}

template<uint32_t cells_across_sector_>
uint8_t RollingGridLayer<cells_across_sector_>::get(const LocalLocation& layer_location ) const {

    if( visible(layer_location) ){
        
        const LocalLocation view_location = layer_location - view_bounds_.min;

        const GridIndex view_index = GridIndex( static_cast<uint32_t>(view_location.easting), 
                                                static_cast<uint32_t>(view_location.northing) )
                                        / meters_across_cell_;

        /// index of the sector to lookup in
        const size_t sector_offset = ((view_index / cells_across_sector_) + anchor_)
                                        .wrap(sectors_across_view_)
                                        .offset(sectors_across_view_);

        /// location of cell within sector (indexed above)
        const size_t cell_offset = (view_index % cells_across_sector_).offset(cells_across_sector_);

        // current_index.get_sector()  // refactor idea ==> retreives raw-index to sector-within-layer
        // current_index.get_cell()    // refactor idea: retreives raw-index to cell-within-sector
        // const auto& current_sector = sectors[ sector_index ];
        uint8_t current_cell_value = sectors_[ sector_offset ][ cell_offset ];

        return current_cell_value;
    }

    return default_cell_value;
}

template<uint32_t cells_across_sector_>
std::string RollingGridLayer<cells_across_sector_>::print_contents_by_cell( uint32_t indent ) const {
    std::ostringstream buf;

    const std::string prefix = fmt::format("{:<{}}", "", indent );

    // print raw contents
    buf << prefix << "======== ======= ======= Printing By Cell: ======= ======= =======\n";
    buf << std::hex;
    for (size_t cell_row_index = cells_across_view_ - 1; cell_row_index < cells_across_view_; --cell_row_index) {
        for (size_t cell_column_index = 0; cell_column_index < cells_across_view_; ++cell_column_index ) {
            if( 0 == ((cell_column_index) % cells_across_sector_ ) ){
                buf << "    ";
            }

            const size_t sector_index = (cell_column_index/cells_across_sector_) + (cell_row_index/cells_across_sector_)*sectors_across_view_;
            const auto& current_sector = sectors_[ sector_index ];

            const GridIndex cell_lookup_index( cell_column_index%cells_across_sector_, cell_row_index%cells_across_sector_ );
            const uint8_t current_cell_value = current_sector.get( cell_lookup_index );

            buf << ' ' << std::setfill('0') << std::setw(2) << static_cast<int>(current_cell_value);
        }
        if( 0 == (cell_row_index % cells_across_sector_ ) ){
            buf << '\n' << prefix;
        }

        buf << '\n' << prefix;
    }
    buf << "======== ======= ======= ======= ======= ======= ======= =======\n";
    return buf.str();
}


template<uint32_t cells_across_sector_>
std::string RollingGridLayer<cells_across_sector_>::print_contents_by_location( uint32_t indent ) const {
    std::ostringstream buf;

    const std::string prefix = fmt::format("{:<{}}", "", indent );

    buf << prefix << "======== ======= ======= Printing By Location ======= ======= =======\n";
    buf << prefix << "     View:      [ [ " << view_bounds_.min[0] << ", " << view_bounds_.min[1] << " ] < [ "
                                 << view_bounds_.max[0] << ", " << view_bounds_.max[1] << " ] ]\n";
    buf << prefix << "     Anchor:    ( " << anchor_.column << ", " << anchor_.row << " )\n";
    
    // print location-aware contents
    for( double northing = (view_bounds_.max.northing - (meters_across_cell_/2)); northing > view_bounds_.min.northing; northing -= meters_across_cell_ ) {
        for( double easting = (view_bounds_.min.easting + (meters_across_cell_/2)); easting < view_bounds_.max.easting; easting += meters_across_cell_ ) {
            if( 0.1*meters_across_cell_ > std::fabs(std::fmod( easting - view_bounds_.min.easting, meters_across_sector_ )) ){
                buf << "    ";
            }

            const LocalLocation cell_lookup_index( easting, northing );
            uint8_t current_cell_value = get( cell_lookup_index );
            buf << ' ' << std::setw(2) << static_cast<int>(current_cell_value);

        }
        if( 0.1*meters_across_cell_ > std::fabs(std::fmod( northing - view_bounds_.min.northing, meters_across_sector_ ))){
            buf << '\n' << prefix;
        }

        buf << '\n' << prefix;
    }

    buf << "======== ======= ======= ======= ======= ======= ======= =======\n";
    return buf.str();
}

template<uint32_t cells_across_sector_>
std::string RollingGridLayer<cells_across_sector_>::print_properties( uint32_t indent ) const {
    std::ostringstream buf;

    const std::string prefix = fmt::format("{:<{}}", "", indent );

    buf << fmt::format( "{}====== ====== ====== Layer Properties: ====== ====== ======\n", prefix );
    buf << fmt::format( "{}    :: bounds::tracked::min:    {:6.0f},{:6.0f}\n", prefix, track_bounds_.min.easting, track_bounds_.min.northing );
    buf << fmt::format( "{}    :: bounds::visible::min:    {:6.0f},{:6.0f}\n", prefix, view_bounds_.min.easting, view_bounds_.min.northing );
    buf << fmt::format( "{}    :: bounds::visible::max:    {:6.0f},{:6.0f}\n", prefix, view_bounds_.max.easting, view_bounds_.max.northing );
    buf << fmt::format( "{}    :: bounds::tracked::max:    {:6.0f},{:6.0f}\n", prefix, track_bounds_.max.easting, track_bounds_.max.northing );
    buf << fmt::format( "{}    :: meters-across-cell:      {:6.0f}\n", prefix, meters_across_cell_ );
    buf << fmt::format( "{}    :: meters-across-sector:    {:6.0f}\n", prefix, meters_across_sector_ );
    buf << fmt::format( "{}    :: meters-across-window:    {:6.0f}\n", prefix, meters_across_view_ );
    buf << fmt::format( "{}    :: cells_across_sector:     {:6d}\n", prefix, cells_across_sector_ );
    buf << fmt::format( "{}    :: sectors-across-view:     {:6d}\n", prefix, sectors_across_view_ );

    return buf.str();
}

template<uint32_t cells_across_sector_>
bool RollingGridLayer<cells_across_sector_>::track( const LocalLocation& new_origin ) {
    track_bounds_.min = new_origin;
    track_bounds_.max = new_origin + LocalLocation( meters_across_view_, meters_across_view_ );

    view_bounds_.min = track_bounds_.min;
    view_bounds_.max = track_bounds_.max;
    
    return true;
}

template<uint32_t cells_across_sector_>
bool RollingGridLayer<cells_across_sector_>::track( const BoundBox<LocalLocation>& new_bounds ) {
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
    if( tolerance < std::fmod( width, meters_across_sector_ ) ){
        width = std::ceil( width/meters_across_sector_) * meters_across_sector_;
    }

    track_bounds_.min = {0,0};
    track_bounds_.max = {width,width};

    center();
    return true;
}

template<uint32_t cells_across_sector_>
bool RollingGridLayer<cells_across_sector_>::track( const BoundBox<UTMLocation>& utm_bounds ) {
    return track( utm_bounds.relative().as<LocalLocation>()  );
}

template<uint32_t cells_across_sector_>
bool RollingGridLayer<cells_across_sector_>::scroll_east() {
 const auto& last_bounds = view_bounds_;
    const LocalLocation delta( meters_across_sector_, 0.0 );
    const auto next_bounds = last_bounds.move( delta );
    const uint32_t at_column = static_cast<uint32_t>(anchor_.column + 1 + sectors_across_view_)%sectors_across_view_;
    const GridIndex next_anchor{ at_column, anchor_.row };

    for( uint32_t at_row = 0; at_row < sectors_across_view_; ++at_row ){
        const GridIndex at_index( anchor_.column, at_row );
        auto& sector = sectors_[at_index.offset(sectors_across_view_)];

        // (A) Store values to old location
        const LocalLocation from_location = (LocalLocation( at_index.column, at_index.row) * meters_across_sector_) + last_bounds.min;
        // fmt::print( stderr, "    @[ {}, {}]:   >> store: ( {}, {} )\n", at_index.column, at_index.row, from_location.easting, from_location.northing );
        chartbox::io::flatbuffer::save( const_cast<const sector_t&>(sector), from_location );

        // (B) Load values at new location 
        const LocalLocation to_location( (next_bounds.max.easting - meters_across_sector_), from_location.northing );
        // fmt::print( stderr, "                << load:  ( {}, {} )\n", to_location.easting, to_location.northing );
        chartbox::io::flatbuffer::load( to_location, sector);
    }

    anchor_ = next_anchor;
    view_bounds_ = next_bounds;
    return true;
}

template<uint32_t cells_across_sector_>
bool RollingGridLayer<cells_across_sector_>::scroll_north() {
    const auto& last_bounds = view_bounds_;
    const LocalLocation delta( 0.0, meters_across_sector_);
    const auto next_bounds = last_bounds.move( delta );
    const uint32_t at_row = static_cast<uint32_t>(anchor_.row + 1 + sectors_across_view_)%sectors_across_view_;
    const GridIndex next_anchor{ anchor_.column, at_row };

    for( uint32_t at_column = 0; at_column < sectors_across_view_; ++at_column ){
        const GridIndex at_index( at_column, anchor_.row );
        auto& sector = sectors_[at_index.offset(sectors_across_view_)];

        // (A) Store values to old location
        const LocalLocation from_location = (LocalLocation(at_index.column, at_index.row) * meters_across_sector_) + last_bounds.min;
        // fmt::print( stderr, "    @[ {}, {}]:   >> store: ( {}, {} )\n", at_index.column, at_index.row, from_location.easting, from_location.northing );
        chartbox::io::flatbuffer::save( const_cast<const sector_t&>(sector), from_location );

        // (B) Load values at new location 
        const LocalLocation to_location( from_location.easting, next_bounds.max.northing-meters_across_sector_ );
        // fmt::print( stderr, "                << load:  ( {}, {} )\n", to_location.easting, to_location.northing );
        chartbox::io::flatbuffer::load( to_location, sector);
    }

    anchor_ = next_anchor;
    view_bounds_ = next_bounds;
    return true;
}

template<uint32_t cells_across_sector_>
bool RollingGridLayer<cells_across_sector_>::scroll_south() {
    const auto& last_bounds = view_bounds_;
    const LocalLocation delta( 0.0, -meters_across_sector_);
    const auto next_bounds = last_bounds.move( delta );
    
    const uint32_t at_row = (anchor_.row + sectors_across_view_ - 1 ) % sectors_across_view_;
    const GridIndex next_anchor{ anchor_.column, at_row };

    for( uint32_t at_column = 0; at_column < sectors_across_view_; ++at_column ){
        const GridIndex at_index( at_column, at_row );
        auto& sector = sectors_[at_index.offset(sectors_across_view_)];

        // (A) Store values to old location
        const LocalLocation from_location = (LocalLocation(at_index.column, at_index.row) * meters_across_sector_) + last_bounds.min;
        // fmt::print( stderr, "    @[ {}, {}]:   >> store: ( {}, {} )\n", at_index.column, at_index.row, from_location.easting, from_location.northing );
        chartbox::io::flatbuffer::save( const_cast<const sector_t&>(sector), from_location );

        // (B) Load values at new location 
        const LocalLocation to_location( from_location.easting, next_bounds.min.northing );
        // fmt::print( stderr, "                << load:  ( {}, {} )\n", to_location.easting, to_location.northing );
        chartbox::io::flatbuffer::load( to_location, sector);
    }

    anchor_ = next_anchor;
    view_bounds_ = next_bounds;
    return true;
}

template<uint32_t cells_across_sector_>
bool RollingGridLayer<cells_across_sector_>::scroll_west() {
    const auto& last_bounds = view_bounds_;
    const LocalLocation delta( -meters_across_sector_, 0.0 );
    const auto next_bounds = last_bounds.move( delta );

    const uint32_t at_column = (anchor_.column + sectors_across_view_ - 1 )% sectors_across_view_;
    const GridIndex next_anchor{ at_column, anchor_.row };

    for( uint32_t at_row = 0; at_row < sectors_across_view_; ++at_row ){
        const GridIndex at_index( at_column, at_row );
        auto& sector = sectors_[at_index.offset(sectors_across_view_)];

        // (A) Store values to old location
        const LocalLocation from_location = (LocalLocation( at_index.column, at_index.row) * meters_across_sector_) + last_bounds.min;
        // fmt::print( stderr, "    @[ {}, {}]:   >> store: ( {}, {} )\n", at_index.column, at_index.row, from_location.easting, from_location.northing );
        chartbox::io::flatbuffer::save( const_cast<const sector_t&>(sector), from_location );

        // (B) Load values at new location 
        const LocalLocation to_location( next_bounds.min.easting, from_location.northing );
        // fmt::print( stderr, "                << load:  ( {}, {} )\n", to_location.easting, to_location.northing );
        chartbox::io::flatbuffer::load( to_location, sector);
    }

    anchor_ = next_anchor;
    view_bounds_ = next_bounds;
    return true;
}

template<uint32_t cells_across_sector_>
bool RollingGridLayer<cells_across_sector_>::store(const LocalLocation& layer_location, uint8_t new_value) {
    if( visible(layer_location) ){
        const LocalLocation view_location = layer_location - view_bounds_.min;

        const GridIndex view_index = GridIndex( static_cast<uint32_t>(view_location.easting), 
                                                static_cast<uint32_t>(view_location.northing) )
                                        / meters_across_cell_;

        /// index of the sector to lookup in
        const size_t sector_offset = ((view_index / cells_across_sector_) + anchor_)
                                        .wrap(sectors_across_view_)
                                        .offset(sectors_across_view_);

        /// location of cell within sector (indexed above)
        const size_t cell_offset = (view_index % cells_across_sector_).offset(cells_across_sector_);

        sectors_[ sector_offset ][ cell_offset ] = new_value;

        // fmt::print( stderr, ">> store   @location:     {:12.4f}, {:12.4f} <<== {:X} \n", layer_location.easting, layer_location.northing, new_value );
        // fmt::print( stderr, "      in-view:            {:12.4f}, {:12.4f} \n", view_location.easting, view_location.northing );
        // fmt::print( stderr, "      cell-in-layer-index:     {:2d}, {:2d} \n", view_cell_column , view_cell_row );
        // fmt::print( stderr, "      sector-in-layer-index:   {:2d} \n", sector_index );
        // fmt::print( stderr, "      cell-in-sector-index:    {:2d} \n", cell_index );

        return true;
    }

    return false;
}

template<uint32_t cells_across_sector_>
bool RollingGridLayer<cells_across_sector_>::view(const LocalLocation& p) {
    view_bounds_.min = p;
    view_bounds_.max = p + meters_across_view_;
    return true;
}

// used for tests
template class RollingGridLayer<4>;

// optionally used in production
template class RollingGridLayer<64>;  // tested
template class RollingGridLayer<128>;
template class RollingGridLayer<256>; // tested
template class RollingGridLayer<512>;
template class RollingGridLayer<1024>; // tested
// ... larger sizes should work, but are expected to be performance prohibitive [citation needed]

}  // namespace
