// GPL v3 (c) 2021, Daniel Williams 

// standard library includes
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <vector>


// third-party includes
// #include <Eigen/Geometry>

// #include <nlohmann/json.hpp>

// // first-party includes
// #include "geometry/bounds.hpp"
// #include "geometry/polygon.hpp"


// using chart::base::ChartInterface;
// using chart::geometry::Bounds;
// using chart::geometry::Path;
// using chart::geometry::Polygon;

// use this cast in order to play nice with the CRTP
#define THIS static_cast<layer_t*>(this)

template <typename cell_t, typename layer_t>
bool ChartInterface<cell_t, layer_t>::fill(const geometry::Path& path, const cell_t fill_value){
    // const bool emphasize_waypoints = false;

    // fill start point
    THIS->store(path[0], fill_value);
    
    // Iterate through the path segments
    for( size_t segment_index = 0; segment_index < path.size()-1; ++segment_index ){
        const auto delta = path.segment(segment_index);
        const size_t x_tick_count = std::abs(delta.x() / THIS->precision());
        const size_t y_tick_count = std::abs(delta.y() / THIS->precision());
        // because we're drawing into a square grid, at the worst case (diagonal) we can just 
        // drop one tick per row, or column, and  we'll have a contiguous line
        const size_t tick_count = std::max(x_tick_count, y_tick_count);
        const Vector2d tick_increment = delta / tick_count;
    
        // draw minor-tick points in between waypoints
        const Vector2d segment_start = path[segment_index];
        Vector2d fill_location = segment_start + tick_increment;
        for( size_t tick_index = 1; tick_index <= tick_count; ++tick_index ){
            THIS->store( fill_location, fill_value);
            fill_location += tick_increment; //Vector2d(x_incr, y_incr);
        }

        // DEBUG: drop a major tick at each waypoint:
        // if(emphasize_waypoints)
        //     THIS->store(seg.end, 'O'); // point tick
    }

    // DEBUG: drop a major tick at the start waypoint
    // if(emphasize_waypoints)
    //     THIS->store( path[0], 'O');

    return true;
}

template <typename cell_t, typename layer_t>
bool ChartInterface<cell_t, layer_t>::fill(const geometry::Polygon& poly, const cell_t fill_value){
    // adapted from:
    //  Public-domain code by Darel Rex Finley, 2007:  "Efficient Polygon Fill Algorithm With C Code Sample"
    //  Retrieved: (https://alienryderflex.com/polygon_fill/); 2019-09-07

    const Bounds& bounds = THIS->bounds();
    const double prec = THIS->precision();

    // Loop through the rows of the image.
    for( double y = bounds.min().y() + prec/2; y < bounds.max().y(); y += prec ){
        // generate a list of line-segment crossings from the polygon
        std::vector<double> crossings;
        for (size_t i=0; i < poly.size()-1; ++i) {
            const Vector2d& p1 = poly[i];
            const Vector2d& p2 = poly[i+1];

            const double y_max = std::max(p1[1], p2[1]);
            const double y_min = std::min(p1[1], p2[1]);
            // if y is in range:
            if( (y_min <= y) && (y < y_max) ){
                // construct x-coordinate that crosses this line:
                auto value = p1[0] + (y - p1[1]) * (p2[0]-p1[0])/(p2[1] - p1[1]);
                crossings.emplace_back(value);
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
            const double start_x = bounds.constrain_x(crossings[crossing_index] + prec/2);
            const double end_x = bounds.constrain_x(crossings[crossing_index+1] + prec/2);
            for( double x = start_x; x < end_x; x += prec){
                THIS->store({x,y}, fill_value);
            }
        }
    }

    return true;
}

template<typename cell_t, typename layer_t>
bool ChartInterface<cell_t,layer_t>::load_json(const std::string& text){
    const auto doc = nlohmann::json::parse( text,    // source document
                                            nullptr, // callback argument
                                            false);  // allow exceptions?

    if (doc.is_discarded()) {
        // cerr << "malformed json! ignore.\n" << endl;
        // cerr << text << endl;
        return false;
    } else if (!doc.is_object()) {
        // cerr << "input should be a json _object_!!\n" + doc.dump(4) << endl;
        return false;
    }

    if (doc.contains(bounds_key)) {
        const Bounds new_bounds = Bounds::make_from_json(doc[bounds_key]);
        if( new_bounds.is_valid()){
            THIS->set_bounds(new_bounds);
            // continue
        }else{
            return true;
        }
    }

    if ( doc.contains(grid_key) ){
        const auto& grid = doc[grid_key];
        if (!grid.is_array()){
            return false;
        }

        if( THIS->size() != grid.size() ){
            cerr << "chart::io::load_grid expected a arrays!  aborting!\n";
            return false;
        }

        std::vector<cell_t> buffer;
        for (auto& cell : grid  ) {
            buffer.push_back(cell.get<cell_t>());
        }
        auto flipped = chart::geometry::vflip(buffer, THIS->dimension() );

        return THIS->fill(flipped);

    }else if( doc.contains(allow_key) && doc[allow_key].is_array() ){
        for( auto& list : doc[allow_key] ){
            if (!list.is_array()){
                cerr << "!? malformed json input... expected allow polygons to be lists" << endl;
                return false;
            }
            const Polygon& allow_shape = geometry::Polygon(list);
            THIS->fill(allow_shape, allow_value);
        }

        if( doc.contains(block_key) && doc[block_key].is_array() ){
            for( auto& list : doc[block_key] ){
                if (!list.is_array()){
                    cerr << "!? malformed json input... expected allow polygons to be lists" << endl;
                    return false;
                }
                const Polygon& block_shape = geometry::Polygon(list);
                THIS->fill(block_shape, block_value);
            }
        }
        return true;
    }

    return true;
}


// template<typename T>
// Terrain<T>::Terrain(T& _ref): 
//     impl(_ref) 
// {}


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
