// GPL v3 (c) 2020

#include "layout.hpp"
using yggdrasil::geometry::Layout;

// these three functions need to be defined "in" the header, because
// they are constexpr / inline, and need to be _defined_ in every
// translation unit they #include into.  (see the #include of this
// file, at the bottom of the grid_layout file.

constexpr Layout::Layout() 
    : precision(1.), width(1.), x_c(0.), y_c(0.),  // primary fields
      dimension(1), half_width(0.5), padding(64), size(1)   // derived fields
{}

constexpr Layout::Layout(const double _precision, const double x_in, const double y_in, const double _width)
    : precision(snap_precision(_precision)), width(snap_width(_width)), x_c(x_in), y_c(y_in),     // primary fields
      dimension(width/precision), half_width(width/2), padding(calculate_padding(dimension)), size(dimension*dimension)     // derived fields
{
    assert( dimension <= maximum_supported_dimension );
}

constexpr uint8_t Layout::calculate_padding( const double dimension ){
    size_t power = 1;
    size_t padding = 64;

    // this loop basically calculates log2(dimension), but in constexpr 
    while( dimension > power){
        power <<= 1;
        padding -= 2;
    }

    return padding;
}

constexpr double Layout::snap_precision( double _precision){
    double next_precision = 1.;
    while( 1 < _precision ){
        if( 2 > _precision ){
            return next_precision;
        }
        _precision *= 0.5;
        next_precision *= 2;
    }

    return next_precision;
}

// constexpr double Layout::snap_precision( const double precision, const double width){
//     const double dimension_estimate = width / precision;
//     // calculate the next-higher power-of-2, in constexpr form
//     size_t power = 1;
//     while( dimension_estimate > power){
//         power = power << 1;
//     }
//     return width / power;
// }

constexpr double Layout::snap_width( const double _width){
    double next_width = precision;

    while( next_width < _width){
        next_width *= 2;
    }

    return next_width;
}


constexpr uint64_t Layout::interleave( const uint32_t input) const {
    uint64_t word = input;
    word = (word ^ (word<<16)) & 0x0000ffff0000ffff;
    word = (word ^ (word<<8))  & 0x00ff00ff00ff00ff;
    word = (word ^ (word<<4))  & 0x0f0f0f0f0f0f0f0f;
    word = (word ^ (word<<2))  & 0x3333333333333333; 
    word = (word ^ (word<<1))  & 0x5555555555555555; 
    return word;
}

constexpr index_t Layout::hashr( const double x_p, const double y_p) const {
    const uint32_t i = hashx(x_p);
    const uint32_t j = hashy(y_p);
    return hashr(i,j);
}

constexpr index_t Layout::hashr( const uint32_t i, const uint32_t j) const {
    return i + j*dimension;
}
constexpr double Layout::limitx( const double x_in) const {
    if( x_in < get_x_min()){
        return get_x_min();
    }else if( x_in > get_x_max()){
        return get_x_max();
    }else{
        return x_in;
    }
}

constexpr uint32_t Layout::hashx( const double x_in) const {
    if( x_in < get_x_min()){
        return 0;
    }else if( x_in >= get_x_max()){
        return dimension-1;
    }else{
        return ( x_in - x_c + half_width )/precision;
    }
}

constexpr double Layout::limity( const double y_in) const {
    if( y_in < get_y_min()){
        return get_y_min();
    }else if( y_in > get_y_max()){
        return get_y_max();
    }else{
        return y_in;
    }
}

constexpr uint32_t Layout::hashy( const double y_in) const {
    if( y_in < get_y_min()){
        return 0;
    }else if( y_in >= get_y_max()){
        return dimension-1;
    }else{
        return ( y_in - y_c + half_width)/precision;
    }
}

constexpr index_t Layout::hashz( const double x_p, const double y_p) const {
    const uint32_t i = hashx(x_p);
    const uint32_t j = hashy(y_p);
    return hashz(i,j);
}

constexpr index_t Layout::hashz( const uint32_t i, const uint32_t j) const {
    return (interleave(i) + (interleave(j) << 1)) << padding;
}
