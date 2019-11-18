// // GPL v3 (c) 2020

#include <iostream>

#include "flatbuffers/flatbuffers.h"

#include "geometry/cell_value.hpp"

#include "node/tile_node.hpp"

// auto-generated code for flatbuffers serialization.
#include "node/tile_cache_generated.h"

using std::cerr;
using std::endl;
using std::string;

using yggdrasil::geometry::cell_default_value;

namespace yggdrasil::node {

template<typename T, size_t n>
TileNode<T,n>::TileNode():
    layout( 1., 0., 0., 32)
{}

template<typename T, size_t n>
TileNode<T,n>::TileNode(const Layout& _layout):
    layout(_layout)
{}

template<typename T, size_t n>
bool TileNode<T,n>::cache_read(std::byte* buffer){

    auto tile_cache = GetTileCache( buffer );

    if( n != tile_cache->dimension()){
        return false;
    }
    const float precision = tile_cache->width()/n;
    const float w = tile_cache->width();
    const float x = tile_cache->x();
    const float y = tile_cache->y();
    Layout new_layout( precision, x, y, w);
    reset(new_layout);

    auto gridv = tile_cache->grid();
    assert( gridv->size() == n*n );

    const uint8_t * grid_buffer = gridv->Data();
    const size_t row_width = sizeof(uint8_t) * n;
    for( size_t j = 0; j < n ; ++j ){
        const uint8_t* src = grid_buffer + j*row_width;
        uint8_t* dest = storage[j].data();
        memcpy( dest, src, row_width); 
    }

    return true;
}

template<typename T, size_t n>
std::byte* TileNode<T,n>::cache_write(){
    // for 1k tiles (32x32) => 1072 bytes
    // for 1M tiles (1024x1024) => 1048624 bytes
    flatbuffers::FlatBufferBuilder builder(1100);

    uint8_t* write_buffer;
    auto grid = builder.CreateUninitializedVector(n*n, 1, &write_buffer);
    const size_t row_width = sizeof(uint8_t) * n;
    for( size_t j = 0; j < n ; ++j ){
        uint8_t* dest = write_buffer + j*row_width;
        const uint8_t* src = storage[j].data();
        memcpy( dest, src, row_width); 
    }

    auto tile = CreateTileCache( builder,
                                layout.get_x(),
                                layout.get_y(),
                                layout.get_width(),
                                n,
                                grid);

    builder.Finish(tile);
    // cerr << "::on_finish::buffer.size= " << builder.GetSize() << endl;

    uint8_t* buffer = builder.GetBufferPointer();
    // builder.Clear(); // useful if/when the builder is re-used.

    // because std::byte and uint8T _REALLY_ should be the same size.
    assert( sizeof(uint8_t) == sizeof(std::byte) );
    return reinterpret_cast<std::byte*>(buffer);
}

template<typename T, size_t n>
T TileNode<T,n>::classify(const Vector2d& p) const {
    if(contains(p)){
        const size_t xi = layout.hashx(p.x());
        const size_t yi = layout.hashy(p.y());
        return get_cell(xi,yi);
    }

    return geometry::cell_default_value;
}

template<typename T, size_t n>
bool TileNode<T,n>::contains(const Vector2d& p) const {
    return layout.contains(p);
}

template<typename T, size_t n>
void TileNode<T,n>::fill(const T value){
    // simple version
    // memset(storage.data(), value, size());
    
    const size_t row_width = sizeof(T) * n;
    for( size_t j = 0; j < n ; ++j ){
        uint8_t* dest = storage[j].data();
        memset( dest, value, row_width);
    }
}

template<typename T, size_t n>
T& TileNode<T,n>::get_cell(const size_t xi, const size_t yi) {
    return storage[xi][yi];
}

template<typename T, size_t n>
T TileNode<T,n>::get_cell(const size_t xi, const size_t yi) const {
    return storage[xi][yi];
}

template<typename T, size_t n>
constexpr size_t TileNode<T,n>::get_memory_usage() const { 
    return layout.get_size() * sizeof(T);
}

template<typename T, size_t n>
size_t TileNode<T,n>::size() const {
    return storage.size() * storage.size();
}

template<typename T, size_t n>
bool TileNode<T,n>::store(const Vector2d& p, const T new_value){
    if(contains(p)){
        const size_t xi = layout.hashx(p.x());
        const size_t yi = layout.hashy(p.y());
        get_cell(xi,yi) = new_value;

        return true;
    }

    return false;
}

// Explicitly instantiate some common specializations:
// ===========
// this should match the Tile1k type:
template class TileNode<cell_value_t, 32>;

// this should match the Tile1M type:
template class TileNode<cell_value_t, 1024>;


}; // namespace yggdrasil::node