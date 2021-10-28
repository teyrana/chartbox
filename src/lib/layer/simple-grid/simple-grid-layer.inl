// using chartbox::layer::LayerInterface;
using chartbox::layer::simple::SimpleGridLayer;



template<typename cell_t, uint32_t dimension_, uint32_t precision_mm>
SimpleGridLayer<cell_t,dimension_,precision_mm>::SimpleGridLayer()
    : view_bounds_( {0,0}, {meters_across_view(),meters_across_view()})
{}

template<typename cell_t, uint32_t dimension_, uint32_t precision_mm>
bool SimpleGridLayer<cell_t,dimension_,precision_mm>::contains(const LocalLocation& p ) const {
    if( 0 > p.easting || 0 > p.northing ){
        return false;
    }else if( cells_across_layer_ < p.easting || cells_across_layer_ < p.northing ){
        return false;
    }
    return true;
}

template<typename cell_t, uint32_t dimension_, uint32_t precision_mm>
bool SimpleGridLayer<cell_t,dimension_,precision_mm>::fill( const cell_t  value) {
    grid_.fill( value);
    return true;
}

template<typename cell_t, uint32_t dimension_, uint32_t precision_mm>
bool SimpleGridLayer<cell_t,dimension_,precision_mm>::fill( const cell_t * const source, size_t count ){
    if ( count > grid_.size()) {
        return false;
    }
    memcpy( grid_.data(), source, sizeof(cell_t ) * std::min(count, grid_.size()) );
    return true;
}

template<typename cell_t, uint32_t dimension_, uint32_t precision_mm>
bool SimpleGridLayer<cell_t,dimension_,precision_mm>::fill(const std::vector<cell_t >& source) {
    if (source.size() != grid_.size()) {
        return false;
    }
    memcpy(grid_.data(), source.data(), sizeof(cell_t ) * source.size());
    return true;
}

template<typename cell_t, uint32_t dimension_, uint32_t precision_mm>
cell_t SimpleGridLayer<cell_t,dimension_,precision_mm>::get(const LocalLocation& p ) const {
    const size_t offset = lookup(static_cast<uint32_t>(p.easting/meters_across_cell_),
                                 static_cast<uint32_t>(p.northing/meters_across_cell_));
    return grid_[ offset ];
}

template<typename cell_t, uint32_t dimension_, uint32_t precision_mm>
size_t SimpleGridLayer<cell_t,dimension_,precision_mm>::lookup( const uint32_t i, const uint32_t j ) const {
    return i + (j * cells_across_layer_);
}

template<typename cell_t, uint32_t dimension_, uint32_t precision_mm>
void SimpleGridLayer<cell_t,dimension_,precision_mm>::print_contents() const {
    fmt::print( "============ ============ Fixed-Grid-Layer Contents ============ ============\n" );
    for (size_t j = cells_across_layer_ - 1; j < cells_across_layer_; --j) {
        for (size_t i = 0; i < cells_across_layer_; ++i) {
            const auto offset = lookup(i,j);
            const auto value = grid_[offset];
            if( 0 == (i%8) ){
                fmt::print(" ");
            }
            if( 0 < value ){
                fmt::print(" {:2X}", static_cast<int>(value) );
            }else{
                fmt::print(" --");
            }
        }
        if( 0 == (j%8) ){
            fmt::print("\n");
        }
        fmt::print("\n");
    }
    fmt::print( "============ ============ ============ ============ ============ ============\n" );
}

template<typename cell_t, uint32_t dimension_, uint32_t precision_mm>
bool SimpleGridLayer<cell_t,dimension_,precision_mm>::store( const LocalLocation& p, const cell_t  value) {
    const auto offset = lookup( static_cast<uint32_t>(p.easting/meters_across_cell_),
                                static_cast<uint32_t>(p.northing/meters_across_cell_) );
    grid_[offset] = value;
    return true;
}


template<typename cell_t, uint32_t dimension_, uint32_t precision_mm>
bool SimpleGridLayer<cell_t,dimension_,precision_mm>::view(const BoundBox<LocalLocation>& nb ) {
    if( nb.width() > meters_across_view() || nb.height() > meters_across_view() ){
       return false;
    }

    view_bounds_.min = nb.min;
    view_bounds_.max = nb.min + meters_across_view();
    return true; 
}
