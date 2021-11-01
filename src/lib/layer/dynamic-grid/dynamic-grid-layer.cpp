// GPL v3 (c) 2021, Daniel Williams 

#include <fmt/core.h>

#include "dynamic-grid-layer.hpp"

using chartbox::geometry::BoundBox;
using chartbox::geometry::LocalLocation;

namespace chartbox::layer::dynamic {

DynamicGridLayer::DynamicGridLayer()
    : cells_across_sector_(4)
    , sectors_across_view_(3)
    , cells_across_view_(cells_across_sector_*sectors_across_view_)
    , meters_across_cell_(1.0)
    , meters_across_sector_(meters_across_cell_*cells_across_sector_)
    , meters_across_view_(meters_across_sector_*sectors_across_view_)
    , sectors_( sectors_across_view_ * sectors_across_view_ )    
    , view_bounds_( {0,0}, {meters_across_view_,meters_across_view_} )
{
    fill(chartbox::layer::default_cell_value);
}

uint32_t DynamicGridLayer::cells_across_sector( uint32_t new_cells_across ){
    for( auto& each_sector : sectors_){
        const size_t new_size = new_cells_across*new_cells_across;
        each_sector.resize( new_size );
        each_sector.assign( new_size, default_cell_value );
    }
    cells_across_sector_ = new_cells_across;
    return cells_across_sector_;
}

bool DynamicGridLayer::center( const LocalLocation& new_center ) {
    const double m_across_2 = meters_across_view_/2;
    
    const BoundBox<LocalLocation> candidate_bounds_ = { LocalLocation( new_center.easting - m_across_2, new_center.northing - m_across_2),
                                                        LocalLocation( new_center.easting + m_across_2, new_center.northing + m_across_2) };

    view_bounds_ = candidate_bounds_.snap( meters_across_sector_, meters_across_view_ );

    return true;
}


bool DynamicGridLayer::fill( uint8_t value){
    for( auto& each_sector : sectors_){
        each_sector.assign( cells_across_sector_*cells_across_sector_, value );
    }
    return true;
}

uint8_t DynamicGridLayer::get(const LocalLocation& layer_location ) const {
    using chartbox::layer::default_cell_value;

    if( visible(layer_location) ){
        const LocalLocation relative_location = layer_location - view_bounds_.min;

        // if the point is on the max-border (east OR north) clamp it inside bounds.
        const LocalLocation view_location( std::fmin(relative_location.easting, view_bounds_.width() - meters_across_cell_/10),
                                           std::fmin(relative_location.northing, view_bounds_.height() - meters_across_cell_/10) );

        const GridIndex view_index = GridIndex( view_location.easting, view_location.northing).div(meters_across_cell_);

        /// index of the sector to lookup in
        const size_t sector_offset = view_index.div(cells_across_sector_).offset(sectors_across_view_);

        /// location of cell within sector (indexed above)
        const size_t cell_offset = view_index.mod(cells_across_sector_)
                                            .offset(cells_across_sector_);
        uint8_t current_cell_value = sectors_[ sector_offset ][ cell_offset ];

        return current_cell_value;
    }

    return default_cell_value;
}


bool DynamicGridLayer::load( DynamicGridSector& sector, const LocalLocation& /*origin*/ ){
    using chartbox::layer::default_cell_value;

    // placeholder -- just reset sector
    sector.assign( cells_across_sector_*cells_across_sector_, default_cell_value );

    return true;
}

bool DynamicGridLayer::origin( const LocalLocation& new_origin ) {
    const double m_across = meters_across_view_;
    
    view_bounds_ = { LocalLocation( new_origin.easting, new_origin.northing ),
                     LocalLocation( new_origin.easting + m_across, new_origin.northing + m_across) };

    return true;
}

double DynamicGridLayer::meters_across_cell( double across ){
    if( 0 < across ){
        meters_across_cell_ = across;
        meters_across_sector_ = meters_across_cell_ * cells_across_sector_;
        meters_across_view_ = meters_across_sector_ * sectors_across_view_;
    }

    return meters_across_cell_;
}


std::string DynamicGridLayer::print_contents_by_cell( uint32_t indent ) const {
    std::ostringstream buf;
    const std::string prefix = fmt::format("{:<{}}", "", indent );

    buf << prefix << "======== ======= ======= Print Contents By Cell: ======= ======= =======\n";
    for (size_t cell_row_index = cells_across_view_ - 1; cell_row_index < cells_across_view_; --cell_row_index) {
        for (size_t cell_column_index = 0; cell_column_index < cells_across_view_; ++cell_column_index ) {
            if( 0 == ((cell_column_index) % cells_across_sector_ ) ){
                buf << prefix << "    ";
            }
            const GridIndex cell_view_index(cell_column_index, cell_row_index);
            const size_t sector_offset = cell_view_index.div(cells_across_sector_)
                                                        .offset(sectors_across_view_);
            const size_t cell_offset = cell_view_index.mod(cells_across_sector_)
                                                        .offset(cells_across_sector_);

            buf << fmt::format(" {:2X}", sectors_[sector_offset][cell_offset] );
        }
        if( 0 == (cell_row_index % cells_across_sector_ ) ){
            buf << '\n' << prefix;
        }

        buf << '\n' << prefix;
    }

    buf << "======== ======= ======= ======= ======= ======= ======= =======\n";
    return buf.str();
}

std::string DynamicGridLayer::print_contents_by_sector() const {
    std::ostringstream buf;
    buf << "======== ======= ======= Print Contents By Sector: ======= ======= =======\n";
    uint32_t sector_index = 0;
    for( auto& sector : sectors_ ){
        buf << fmt::format("{:2}:", sector_index );
        for( auto& current_cell_value : sector ){
            buf << fmt::format(" {:2X}", current_cell_value );
        }
        buf << '\n';
        ++sector_index;
    }
    buf << "======== ======= ======= =======  =======  ======= ======= ======= =======\n";
    return buf.str();
}

std::string DynamicGridLayer::print_properties() const {
    std::ostringstream buf;
    buf << "======== ======= Properties: ======= =======\n";
    buf << fmt::format( "    ::bounds-min:             {:8.1f}, {:8.1f}\n", view_bounds_.min.easting, view_bounds_.min.easting  );
    buf << fmt::format( "    ::bounds-max:             {:8.1f}, {:8.1f}\n", view_bounds_.max.easting, view_bounds_.max.easting  );
    buf << fmt::format( "    ::cells-across-sector:    {:6}\n", cells_across_sector_ );
    buf << fmt::format( "    ::sectors-across-view:    {:6}\n", sectors_across_view_ );
    buf << fmt::format( "    ::cells-across-view:      {:6}\n", cells_across_view_ );
    buf << fmt::format( "    ::meters-across-cell:     {:6}\n", meters_across_cell_ );
    buf << fmt::format( "    ::meters-across-sector:   {:6}\n", meters_across_sector_ );
    buf << fmt::format( "    ::meters-across-view:     {:6}\n", meters_across_view_ );
    return buf.str();
}

bool DynamicGridLayer::save( const DynamicGridSector& /*sector*/, const LocalLocation& /*origin*/ ) const { 
    // placeholder
    // NYI
    return false; 
}

uint32_t DynamicGridLayer::sectors_across_view( uint32_t new_sectors_across ){
    if( 0 == new_sectors_across ){
        return sectors_across_view_;
    }

    sectors_across_view_ = new_sectors_across;
    sectors_.reserve( sectors_across_view_*sectors_across_view_ );
    sectors_.resize( sectors_across_view_*sectors_across_view_ );
    fill( chartbox::layer::default_cell_value );

    cells_across_sector_ = cells_across_view_ / sectors_across_view_;

    if( 0 != ( cells_across_view_ % cells_across_sector_ )){
        cells_across_view_ = cells_across_sector_ * sectors_across_view_;
    }

    return sectors_across_view_;
}

bool DynamicGridLayer::store(const LocalLocation& layer_location, uint8_t new_value) {
    if( visible(layer_location) ){
        const LocalLocation view_location = layer_location - view_bounds_.min;
        const GridIndex view_index = GridIndex( view_location.easting, view_location.northing)
                                            .div(meters_across_cell_);
        
        /// index of the sector to lookup in
        const size_t sector_offset = view_index.div(cells_across_sector_)
                                                .offset(sectors_across_view_);

        /// location of cell within sector (indexed above)
        const size_t cell_offset = view_index.mod(cells_across_sector_)
                                                .offset(cells_across_sector_);

        sectors_[ sector_offset ][ cell_offset ] = new_value;

        return true;
    }

    return false;
}

bool DynamicGridLayer::track( const geometry::BoundBox<geometry::LocalLocation>& _new_bounds ) {
    const LocalLocation origin = _new_bounds.min;
    const double request_width = std::max(_new_bounds.width(), _new_bounds.height());
    const double divs = std::ceil( request_width / meters_across_cell_ );
    const double use_width = meters_across_cell_ * divs;

    view_bounds_.min = origin;
    view_bounds_.max = origin + LocalLocation(use_width);

    cells_across_view_ = use_width / meters_across_cell_;
    // sectors_across_view_ // no change needed
    cells_across_sector_ = cells_across_sector( cells_across_view_ / sectors_across_view_ );

    meters_across_view_ = use_width;
    meters_across_cell_ = meters_across_cell_;
    meters_across_sector_ = meters_across_cell_ * cells_across_sector_;

    return true;
}

bool DynamicGridLayer::view(const BoundBox<LocalLocation>& box) {
    return track( box );
}

}  // namespace
