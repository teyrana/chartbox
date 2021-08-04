// GPL v3 (c) 2021, Daniel Williams 

// standard library includes
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// third-party includes
#include <Eigen/Geometry>

#include <fmt/core.h>

using chartbox::ChartLayerInterface;


template<typename cell_t, typename layer_t>
ChartLayerInterface<cell_t, layer_t>::ChartLayerInterface( const BoundBox<Location2EN>& _bounds )
    : bounds_(_bounds)
{}

template<typename cell_t, typename layer_t>
bool ChartLayerInterface<cell_t, layer_t>::fill(const BoundBox<Location2xy>& box, const cell_t value) {
    const double incr = layer().precision();
    const double x_max = box.max.x;
    const double x_min = box.min.x + incr/2;
    const double y_max = box.max.y;
    const double y_min = box.min.y + incr/2;

    // Loop through the rows of the image.
    for( double y = y_min; y < y_max; y += incr ){
        for( double x = x_min; x < x_max; x += incr ){
            layer().store({x,y}, value);
        }
    }
    return true;
}

template<typename cell_t, typename layer_t>
bool ChartLayerInterface<cell_t, layer_t>::fill( const OGRLinearRing& ring, cell_t value ){
    // adapted from:
    //  Public-domain code by Darel Rex Finley, 2007:  "Efficient Polygon Fill Algorithm With C Code Sample"
    //  Retrieved: (https://alienryderflex.com/polygon_fill/); 2019-09-07

    const size_t vertex_count = ring.getNumPoints();
    const double x_max = layer().bounds_.width();
    const double x_min = 0;
    const double x_incr = layer().precision();  // == y_incr.  This is a square grid.
    const double y_max = layer().bounds_.height();
    const double y_min = 0;
    const double y_incr = layer().precision();  // == x_incr.  This is a square grid.

    // Loop through the rows of the image.
    for( double y = y_min + y_incr/2; y < y_max; y += y_incr ){

        // generate a list of line-segment crossings from the polygon
        std::vector<double> crossings;
        auto iter = ring.begin();
        for (size_t i=0; i < (vertex_count-1); ++i) {
            const OGRPoint segment_start = *iter;
            ++iter;
            const OGRPoint segment_end = *iter;

            const double segment_y_min = std::min( segment_start.getY(), segment_end.getY());
            const double segment_y_max = std::max( segment_start.getY(), segment_end.getY());

            // if y is in range:
            if( (segment_y_min <= y) && (y < segment_y_max) ){
                // construct x-coordinate that crosses this line:
                const double x_crossing = segment_start.getX() + (y - segment_start.getY()) * (segment_end.getX()-segment_start.getX())/(segment_end.getY() - segment_start.getY());
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

// template <typename cell_t, typename layer_t>
// bool ChartInterface<cell_t, layer_t>::fill(const geometry::Path& path, const cell_t fill_value){
//     // const bool emphasize_waypoints = false;

//     // fill start point
//     THIS->store(path[0], fill_value);
    
//     // Iterate through the path segments
//     for( size_t segment_index = 0; segment_index < path.size()-1; ++segment_index ){
//         const auto incr = path.segment(segment_index);
//         const size_t x_tick_count = std::abs(incr.x() / THIS->precision());
//         const size_t y_tick_count = std::abs(incr.y() / THIS->precision());
//         // because we're drawing into a square grid, at the worst case (diagonal) we can just 
//         // drop one tick per row, or column, and  we'll have a contiguous line
//         const size_t tick_count = std::max(x_tick_count, y_tick_count);
//         const Vector2d tick_increment = incr / tick_count;
    
//         // draw minor-tick points in between waypoints
//         const Vector2d segment_start = path[segment_index];
//         Vector2d fill_location = segment_start + tick_increment;
//         for( size_t tick_index = 1; tick_index <= tick_count; ++tick_index ){
//             THIS->store( fill_location, fill_value);
//             fill_location += tick_increment; //Vector2d(x_incr, y_incr);
//         }

//         // DEBUG: drop a major tick at each waypoint:
//         // if(emphasize_waypoints)
//         //     THIS->store(seg.end, 'O'); // point tick
//     }

//     // DEBUG: drop a major tick at the start waypoint
//     // if(emphasize_waypoints)
//     //     THIS->store( path[0], 'O');

//     return true;
// }

// template<typename T>
// cell_value_t Terrain<T>::classify(const Vector2d& p) const {
//     return impl.classify(p);
// }

// template<typename T>
// void Terrain<T>::debug() const {
//     const Layout& layout = impl.get_layout();
//     const double precision = layout.get_precision();
//     const size_t dimension = layout.get_dimension();

//     cerr << "====== Structure: ======\n";
//     cerr << "##  layout:     " << layout.to_json().dump() << endl;
//     // cerr << "##  height:     " << get_height() << endl;
//     cerr << "##  precision:  " << precision << endl;
//     cerr << "##  dimension:  " << dimension << endl;

//     cerr << "           ======== ======== ======== ======== As Grid: ======== ======== ======== ========\n";
//     // print header (x-axis-labels: 
//     cerr << "             ";
//     for(double x = (layout.get_x_min() + precision/2); x < layout.get_x_max(); x += precision){
//         fprintf(stderr, "%5.2f ", x);
//     } cerr << endl;
//     // print top border
//     cerr << "           +";
//     for(double x = (layout.get_x_min() + precision/2); x < layout.get_x_max(); x += precision){
//         fprintf(stderr, "------");
//     } cerr << "---+\n";

//     for(double y = (layout.get_y_max() - precision/2); y > layout.get_y_min(); y -= precision ){
//         // print left header:
//         fprintf(stderr, "     %5.2f | ", y);
//         for(double x = (layout.get_x_min() + precision/2); x < layout.get_x_max(); x += precision){
//             auto value = impl.classify({x,y});
//             if( 0 < value ){
//                 fprintf(stderr, "   %2X,", static_cast<int>(value) );
//             }else{
//                 cerr << "     ,";
//             }
//         }
//         // print right header:
//         fprintf(stderr, "  | %5.2f\n", y);
//     }
//     // print bottom border
//     cerr << "           +";
//     for(double x = (layout.get_x_min() + precision/2); x < layout.get_x_max(); x += precision){
//         fprintf(stderr, "------");
//     } cerr << "---+\n";
//     // print footer: (x-axis-labels: 
//     cerr << "             ";
//     for(double x = (layout.get_x_min() + precision/2); x < layout.get_x_max(); x += precision){
//         fprintf(stderr, "%5.2f ", x);
//     } cerr << endl << endl;
// }

// template<typename T>
// std::string Terrain<T>::summary() const {
//     std::ostringstream buffer;
//     buffer.imbue(std::locale(""));
//     buffer << "====== Terrain Stats: ======\n";
//     buffer << "##  layout:       " << impl.get_layout().to_string() << '\n';
//     buffer << "##  dimension:    " << impl.get_layout().get_dimension() << endl;
//     buffer << "##  size:         " << impl.get_layout().get_size() <<  " nodes  ===  " << impl.get_memory_usage()/1000 << " kilobytes\n";
//     buffer << "##  compression:  " << impl.get_load_factor() << '\n';
//     buffer << '\n';
//     return buffer.str();
// }
