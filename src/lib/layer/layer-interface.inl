// GPL v3 (c) 2021, Daniel Williams 

// standard library includes
#include <cctype>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// third-party includes
#include <fmt/core.h>

using chartbox::layer::LayerInterface;


template< typename layer_t>
bool LayerInterface<layer_t>::fill(const BoundBox<LocalLocation>& box, const uint8_t value) {
    const double incr = layer().meters_across_cell();
    const double easting_max = box.max.easting;
    const double easting_min = box.min.easting + incr/2;
    const double northing_max = box.max.northing;
    const double northing_min = box.min.northing + incr/2;

    // Loop through the rows of the image.
    for( double northing = northing_min; northing < northing_max; northing += incr ){
        for( double easting = easting_min; easting < easting_max; easting += incr ){
            layer().store( { easting, northing }, value);
        }
    }
    return true;
}

template< typename layer_t>
bool LayerInterface<layer_t>::fill( const Path<LocalLocation>& path, const BoundBox<LocalLocation>& bounds, uint8_t value ){
    constexpr bool fill_segments = false;

    if( 2 > path.size() ){
        return false;
    }
    const size_t segment_count = path.size() - 1;
    const double fill_spacing = layer().precision();
    const uint8_t fill_segment_value = value;
    const uint8_t fill_vertex_value = value;

    for( size_t segment_index = 0; segment_index < segment_count; ++segment_index ){
        const auto segment_start_point = path[segment_index];
        const auto segment_end_point = path[segment_index+1];

        if( segment_start_point.isnan() || segment_end_point.isnan() ){
            return false;
        }

        // draw segment if at least one point is in-bounds
        if( bounds.contains(segment_start_point) || bounds.contains(segment_end_point) ){
            const auto fill_start_point = bounds.clamp(segment_start_point);
            const auto fill_end_point = bounds.clamp(segment_end_point);

            layer().store( fill_start_point, fill_vertex_value );

            if( fill_segments ){
                const LocalLocation fill_segment = (fill_end_point - fill_start_point);
                const size_t divs = static_cast<size_t>(std::abs(std::ceil(fill_segment.easting/fill_spacing)));
                const LocalLocation fill_increment = { fill_spacing, fill_segment.northing/fill_segment.easting*fill_spacing };
                LocalLocation fill_point = fill_start_point; // copy
                for( size_t i=1; i < divs; ++i ){
                    fill_point += fill_increment;
                    layer().store( fill_point, fill_segment_value );
                }
            }
        }
    }

    // print at the last vertex: (equality to 'segment_count' is coincidental
    const auto path_end_point = path[path.size()-1];
    if( bounds.contains(path_end_point) ){
        layer().store( path_end_point, fill_vertex_value );
    }

    return true;
}


template< typename layer_t>
bool LayerInterface<layer_t>::fill( const Polygon<LocalLocation>& poly, const BoundBox<LocalLocation>& bounds, uint8_t value ){
    // adapted from:
    //  Public-domain code by Darel Rex Finley, 2007:  "Efficient Polygon Fill Algorithm With C Code Sample"
    //  Retrieved: (https://alienryderflex.com/polygon_fill/); 2019-09-07

    const size_t vertex_count = poly.size();
    const double x_max = bounds.max[0];
    const double x_min = bounds.min[0];
    const double x_incr = layer().meters_across_cell();  // == y_incr.  This is a square grid.
    const double y_max = bounds.max[1];
    const double y_min = bounds.min[1];
    const double y_incr = layer().meters_across_cell();  // == x_incr.  This is a square grid.

    // Loop through the rows of the image.
    for( double y = y_min + y_incr/2; y < y_max; y += y_incr ){

        // generate a list of line-segment crossings from the polygon
        std::vector<double> crossings;
        auto iter = poly.begin();
        for (size_t i=0; i < (vertex_count-1); ++i) {
            const LocalLocation segment_start = *iter;
            ++iter;
            const LocalLocation segment_end = *iter;

            const double segment_y_min = std::min( segment_start.y(), segment_end.y() );
            const double segment_y_max = std::max( segment_start.y(), segment_end.y() );

            // if y is in range:
            if( (segment_y_min <= y) && (y < segment_y_max) ){
                // construct x-coordinate that crosses this line:
                const double x_crossing = segment_start.x() + (y - segment_start.y()) * (segment_end.x()-segment_start.x())/(segment_end.y() - segment_start.y());
                crossings.emplace_back(x_crossing);
            }
        }

        // early exit
        if( 0 == crossings.size()){
            continue;
        }

        // Sort the crossings:
        std::sort(crossings.begin(), crossings.end());

        //  Fill the pixels between node pairs.
        for( size_t crossing_index = 0; crossing_index < crossings.size(); crossing_index += 2){
            const double start_x = std::max( x_min, crossings[crossing_index]) + x_incr/2;
            const double end_x = std::min( x_max, crossings[crossing_index+1] + x_incr/2);
            for( double x = start_x; x < end_x; x += x_incr ){
                layer().store( {x,y}, value);
            }
        }
    }

    return true;
}

template< typename layer_t>
std::string LayerInterface<layer_t>::to_location_content_string( uint32_t indent ) const {
    const auto precision = layer().precision();
    const auto& visible = layer().visible();

    std::ostringstream buf;
    std::string prefix = fmt::format("{:{}}", "", indent );

    buf << prefix << "======== ======= ======= Print Contents By Location ======= ======= =======\n";
    // buf << prefix << "     View:      [ [ " << visible.min[0] << ", " << visible.min[1] << " ] < [ "
    //                                         << visible.max[0] << ", " << visible.max[1] << " ] ]\n";

    // print location-aware contents
    buf << prefix;
    for( double northing = (visible.max.northing - (precision/2)); northing > visible.min.northing; northing -= precision ) {
        for( double easting = (visible.min.easting + (precision/2)); easting < visible.max.easting; easting += precision ) {
            const LocalLocation at_point( easting, northing );
            uint8_t value = layer().get( at_point );

            if( 0 == value ){
                buf << "   ";
            }else if( std::isalpha(value) || std::ispunct(value) ){
                buf << fmt::format(" {:2c}", value );
            }else{
                buf << fmt::format(" {:2X}", static_cast<int>(value) );
            }
        
        }
        buf << '\n' << prefix;
    }

    buf << "======== ======= ======= =======  =======  ======= ======= ======= =======\n";
    return buf.str();
}