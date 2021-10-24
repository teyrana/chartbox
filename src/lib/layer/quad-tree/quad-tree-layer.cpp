// GPL v3 (c) 2020, Daniel Williams 

#include <cmath>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "geometry/local-location.hpp"


#include "quad-tree-layer.hpp"

using chartbox::geometry::LocalLocation;

using chartbox::layer::QuadTreeLayer;
using chartbox::layer::QuadNode;
using chartbox::layer::LeafNode;


// main method for descending through a tree and returning the appropriate
// location / node / value note: weakly optimized; intended to be a hot path.
QuadNode* descend(const LocalLocation& /*target*/, // double& x_c, double& y_c,
             const double /*start_width*/, QuadNode* current_node) {
//     double current_width = start_width;a
//     double next_width = start_width * 0.5;
    // LocalLocation out;

//     while (!current_node->is_leaf()) {
//         current_width = next_width;
//         next_width *= 0.5;

//         if (target[0] > x_c) {
//             if (target[1] > y_c) {
//                 x_c += next_width;
//                 y_c += next_width;
//                 current_node = current_node->get_northeast();
//             } else {
//                 x_c += next_width;
//                 y_c -= next_width;
//                 current_node = current_node->get_southeast();
//             }
//         } else {
//             if (target[1] > y_c) {
//                 x_c -= next_width;
//                 y_c += next_width;
//                 current_node = current_node->get_northwest();
//             } else {
//                 x_c -= next_width;
//                 y_c -= next_width;
//                 current_node = current_node->get_southwest();
//             }
//         }
//     }
    return current_node;
}

QuadTreeLayer::QuadTreeLayer()
    : center_( 0, 0 )
    , width_2_( static_cast<double>(dimension) / 2.0f )
    , precision_( 1.0 )
{}

QuadTreeLayer::QuadTreeLayer(const LocalLocation& _center, double _width )
    : center_( _center )
    , width_2_( _width / 2.0f )
    , precision_( _width/dimension )
{}

QuadTreeLayer::~QuadTreeLayer() {
    root->join();
}

bool QuadTreeLayer::contains(const LocalLocation& p) const {
    if( (p.easting  < (center_.easting - width_2_)) ||
        (p.northing < (center_.northing - width_2_)) ||
        ((center_.easting + width_2_)  < p.easting) ||
        ((center_.northing + width_2_) < p.northing))
    {
        return false;
    }

    return true;
}

constexpr size_t QuadTreeLayer::calculate_complete_tree(const size_t height) const {
    // see: https://en.wikipedia.org/wiki/M-ary_tree
    //      # properties of M-ary trees
    //
    //    m == branching_factor == 4  ///< for a quadtree, this is trivially 4
    //
    //       (h+1)
    //     m        - 1
    // N = ---------------
    //        m - 1
    //
    return (pow(4, height + 1) - 1) / 3;
}

double QuadTreeLayer::load_factor() const {
    const size_t height = root->height();
    const size_t actual_count = root->count();
    const size_t complete_count = calculate_complete_tree(height);
    return static_cast<double>(actual_count) / static_cast<double>(complete_count);
}

size_t QuadTreeLayer::memory_usage() const {
    return size() * sizeof(QuadNode);
}

void QuadTreeLayer::fill(const uint8_t fill_value) {
    root->fill(fill_value);
}

uint32_t QuadTreeLayer::height() const {
    return root->height() - 1;
}

void QuadTreeLayer::prune() {
    root->prune();
}

void QuadTreeLayer::reset() {
    if( root )
        root->join();
}

void QuadTreeLayer::split( double width ) {
    width_2_ = width/2;
    precision_ = width_2_/dimension;
    // TODO: implement!
    // auto current = root;
    // while( ??? ){
        // if (precision >= width) {
        //     return;
        // }

        // if (is_leaf()) {
        //     split();
        // }

        // const double half_width = width / 2;

        // this->northeast->split(precision, half_width);
        // this->northwest->split(precision, half_width);
        // this->southeast->split(precision, half_width);
        // this->southwest->split(precision, half_width);
    // }
}

uint8_t QuadTreeLayer::get( const LocalLocation& /*p*/, uint8_t or_default_value ) const {
    // TODO: implement tree-descend-and-retreive
    // return contains(p) ? root->get(p) : or_default_value;
    return or_default_value;
}

bool QuadTreeLayer::store( const LocalLocation& /*p*/, const uint8_t new_value ){
    if(contains(p)){
        return false;
        // TODO: implement tree-descend-and-store
        // root->set( p, new_value);
        // return true;
    }
    return false;
}

size_t QuadTreeLayer::size() const { 
    return root->count();
}

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


std::string QuadTreeLayer::summary() const {
    std::ostringstream buffer;
    buffer.imbue(std::locale(""));
    buffer << "====== QuadTree Stats: ======\n";
    // buffer << "##  layout:       " << impl.get_layout().to_string() << '\n';

    buffer << "##  width:        " << width() << '\n';
    buffer << "##  precision:    " << precision() << '\n';
    buffer << "##  dimension:    " << dimension << '\n';
    buffer << "##  size:         " << size() <<  " nodes  ===  " << memory_usage()/1000 << " kilobytes\n";
    buffer << "##  compression:  " << load_factor() << '\n';
    buffer << '\n';
    return buffer.str();
}

double QuadTreeLayer::width() const {
    return width_2_ * 2;
}
