// GPL v3 (c) 2021, Daniel Williams 

// NOTE: This is the template-class implementation --
//       It is not compiled until referenced, even though it contains the
//       function implementations.

#include <fstream>

#include "layer/rolling-grid/rolling-grid-layer.hpp"
#include "chart-box/geometry/local-location.hpp"

#include "flatbuffer.hpp"
#include "cell-cache-generated.h"

namespace chartbox::io::flatbuffer {

template<uint32_t n,uint32_t m>
bool load( const std::filesystem::path& from_path, chartbox::layer::RollingGridLayer<n,m>& to_layer ){
    using chartbox::geometry::LocalLocation;
    using chartbox::io::flatbuffer::Cell;
    using chartbox::io::flatbuffer::CellBuilder;
    using chartbox::io::flatbuffer::Location;

    if( from_path.empty() ){
        fmt::print(stderr, "<<!! No path given !! : {}\n", from_path.string() );
        return false;
    }else if( not std::filesystem::is_directory(from_path) ){
        fmt::print(stderr, "    !! Could contour input path is not a directory!!: {}\n", from_path.string());
        return false;
    }

    fmt::print( "    >>> Loading from file: {}\n", from_path.string() );

    return false; // debug
}

template<>
bool save<StaticGridLayer>( const StaticGridLayer& from_layer, const std::filesystem::path& to_path){
    using chartbox::io::flatbuffer::Cell;
    using chartbox::io::flatbuffer::CellBuilder;
    using chartbox::io::flatbuffer::Location;

    fmt::print( stderr, "    >>> Write Layer to: {}\n", to_path.string() );

    // not a guaranteed property of the layer; not all of them have this...
    const size_t dimension = from_layer.dimension;

    flatbuffers::FlatBufferBuilder builder( sizeof(Cell) + dimension*dimension );

    // create internal objects before parent objects:
    const auto& min = from_layer.bounds().min;
    auto origin = Location( min.easting, min.northing );
    auto datavec = builder.CreateVector( const_cast<uint8_t*>(from_layer.data()), dimension*dimension );

    // build internal representation
    auto grid_cell = CreateCell( builder, &origin, dimension, dimension, datavec );
    builder.Finish(grid_cell);

    // write bytes to file:
    std::ofstream dest( to_path.string(), std::ios::binary | std::ios::trunc );
    dest.write( reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize() );
    dest.close();

    fmt::print(  "    <<< Successfuly wrote {} bytes to: {} \n", builder.GetSize(), to_path.string() );

    return true;
}

}  // namespace