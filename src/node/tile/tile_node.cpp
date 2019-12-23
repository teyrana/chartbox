// // GPL v3 (c) 2020

#include <cstddef>
#include <iostream>
#include <memory>
#include <sstream>

using std::byte;
using std::cerr;
using std::endl;
using std::string;
using std::unique_ptr;
using std::vector;

#include <nlohmann/json.hpp>
#include <flatbuffers/flatbuffers.h>
#include <Eigen/Geometry>

using Eigen::Vector2d;

#include "tile_node.hpp"

// auto-generated code for flatbuffers serialization.
#include "tile_cache_generated.h"


namespace yggdrasil::node {

const std::string TileNode::anchor_key = "anchor";
const std::string TileNode::grid_key = "grid";
const std::string TileNode::x_key = "x";
const std::string TileNode::y_key = "y";

TileNode::TileNode():
    anchor({0.,0.}),
    index(data)
{}

TileNode::TileNode(const Vector2d& _anchor):
    anchor(_anchor),
    index(data)
{}


bool TileNode::cache_read( const std::byte* const buffer){
    auto tile_cache = GetTileCache( buffer );

    const Eigen::Vector2d expected = { tile_cache->x(), tile_cache->y() };
    if( ! expected.isApprox( anchor ) ){
        return false;
    }
 
    auto gridv = tile_cache->grid();
    if( gridv->size() != size ){
        return false;
    }
    const uint8_t * read_buffer = gridv->Data();
    uint8_t* write_buffer = data.data();
    memcpy( write_buffer, read_buffer, sizeof(cell_t) * size );

    return true;
}

const std::byte* const TileNode::cache_write(){
    // for 1k tiles (32x32) => 1072 bytes
    // for 1M tiles (1024x1024) => 1048624 bytes
    flatbuffers::FlatBufferBuilder builder(1100);

    uint8_t* write_buffer;
    auto grid = builder.CreateUninitializedVector( size, 1, &write_buffer);
    
    const uint8_t* read_buffer = data.data();
    memcpy( write_buffer, read_buffer, sizeof(cell_t) * size ); 

    auto tile = CreateTileCache( builder, anchor.x(), anchor.y(), grid);

    builder.Finish(tile);
    // cerr << "::on_finish::buffer.size= " << builder.GetSize() << endl;

    uint8_t* buffer = builder.GetBufferPointer();
    // builder.Clear(); // useful if/when the builder is re-used.

    // because std::byte and uint8T _REALLY_ should be the same size.
    static_assert( sizeof(uint8_t) == sizeof(std::byte) );
    static_assert( sizeof(cell_t) == sizeof(std::byte) );
    
    return reinterpret_cast<std::byte*>(buffer);
}

TileNode::cell_t TileNode::classify(const Vector2d& p) const {
    if( contains(p) ){
        const Vector2d scaled = (p - anchor) * scale;
        const size_t xi = static_cast<size_t>(scaled.x());
        const size_t yi = static_cast<size_t>(scaled.y());
        return index(xi,yi);
    }

    return cell_default_value;
}

bool TileNode::contains(const Vector2d& p) const {
    const double x = p.x() - anchor.x();
    if( (x < 0) || (width <= x) ){
        return false;
    }
    
    const double y = p.y() - anchor.y();
    if( (y < 0) || (width <= y) ){
        return false;
    }

    return true;
}

void TileNode::fill(const cell_t value){
    memset( data.data(), value, sizeof(cell_t) * size );
}

TileNode::cell_t& TileNode::get_cell(const size_t xi, const size_t yi) {
    return index(xi,yi);
}

TileNode::cell_t TileNode::get_cell(const size_t xi, const size_t yi) const {
    return index(xi,yi);
}

std::unique_ptr<TileNode> TileNode::make(const std::byte* const cache_buffer){
    const auto cache_loader = GetTileCache( cache_buffer );
    const double x = cache_loader->x();
    const double y = cache_loader->y();

    std::unique_ptr<TileNode> result0 = std::make_unique<TileNode>();

    auto result = std::make_unique<TileNode>(Vector2d(x,y));

    result->cache_read( cache_buffer );

    return result;
} 

std::unique_ptr<TileNode> TileNode::make(const std::string& text){
    const nlohmann::json doc = nlohmann::json::parse( text,  // source document
                                                nullptr,   // callback argument
                                                false);    // allow exceptions?
    if(doc.is_discarded()){
        cerr << "malformed json! ignore.\n" << endl;
        // cerr << source.rdbuf() << endl;
        return {};
    }else if(!doc.is_object()){
        cerr << "input should be a json _document_!!\n" + doc.dump(4) << endl;
        return {}; 
    }else if( !doc.contains(anchor_key) ){
        cerr << "input document is missing an anchor object! \n";
        return {};
    }
    const auto anchor_object = doc[anchor_key];

    if(!anchor_object.contains(TileNode::x_key) || !anchor_object[TileNode::x_key].is_number()){
        cerr << "input document is missing the anchor.x field! \n";
        return {};
    }else if(!anchor_object.contains(TileNode::y_key) || !anchor_object[TileNode::y_key].is_number()){
        cerr << "input document is missing the anchor.y field! \n";
        return {};
    }

    const double x = anchor_object[x_key].get<double>();
    const double y = anchor_object[y_key].get<double>();
    auto result = std::make_unique<TileNode>(Vector2d(x,y));

    if( doc.contains(grid_key) ){
        nlohmann::json grid = doc[grid_key];

        if(!grid.is_array() || !grid[0].is_array()){
            cerr << "yggdrasil::io::load_grid expected a array-of-arrays! aborting!\n";
            result->fill(cell_default_value);
            return result;
        }

        if( grid.size() != dimension ||  grid[0].size() != dimension ){
            cerr << "yggdrasil::io::load_grid expected a array of the same dimension as configured!!\n";
            cerr << "    expected: " << dimension << endl;
            cerr << "    found:    " << grid.size() << " x " << grid[0].size() << endl;
            result->fill(cell_default_value);
            return result;
        }

        // populate the tree
        size_t row_index = dimension - 1;
        for(auto& row : grid){
            size_t column_index = 0;
            // i.e. a cell is the element at [column_index, row_index] <=> [x,y]
            for(auto& cell : row){
                result->index( column_index, row_index) = cell.get<TileNode::cell_t>();
                ++column_index;
            }
            --row_index;
        }
    }

    return result;
}

bool TileNode::store(const Vector2d& p, const cell_t new_value){
    if( contains(p) ){
        const Vector2d scaled = (p - anchor) * scale;
        const size_t xi = static_cast<size_t>(scaled.x());
        const size_t yi = static_cast<size_t>(scaled.y());
        index(xi,yi) = new_value;
        return true;
    }

    return false;
}

std::string TileNode::to_json() const {

    // explicitly create the json object
    nlohmann::json doc = nlohmann::json::object();

    doc[anchor_key] = {{x_key, anchor.x()}, {y_key, anchor.y()}};

    nlohmann::json grid = nlohmann::json::array();
    for(size_t j = dimension-1; j < dimension; --j ){
        grid[j] = nlohmann::json::array();
        for(size_t i = 0; i < dimension; ++i ){
            grid[j][i] = static_cast<int>(index(i,j));
        }
    }
    doc[grid_key] = grid;

    return doc.dump();
}

std::string TileNode::to_string() const {
    std::ostringstream buf;

    buf << " ======== ======== ======== Print Grid: ======== ======== ========\n";
    
    for(size_t j = dimension-1; j < dimension; --j ){
        for(size_t i = 0; i < dimension; ++i ){
            const auto value = index(i,j);
            buf << ' ';
            if( value < 32 ){
                buf << ' ';
            }else { 
                buf << static_cast<char>(value);
            }
        }
        buf << endl;
    }
    buf << " ======== ======== ======== ======== ======== ======== ========\n";
    
    return buf.str();
}

vector<byte> TileNode::to_raster() const {
    // allocate data buffers
    vector<byte> buffer( size );

    // bottom-row first:
    // std::memcpy( buffer.data(), data.data(), size);

    // top-row first:
    size_t write_index = 0;
    // // Loop through the rows of the image.
    for(size_t j = dimension-1; j < dimension; --j ){
        for(size_t i = 0; i < dimension; ++i ){
            buffer[write_index] = (std::byte)(index(i,j));
            ++write_index;
        }
    }

    return buffer;
}

// // Explicitly instantiate some common specializations:
// // ===========
// // this should match the Tile1k type:
// template class TileNode<cell_t, 32>;

// // this should match the Tile1M type:
// template class TileNode<cell_t, 1024>;


}; // namespace yggdrasil::node
