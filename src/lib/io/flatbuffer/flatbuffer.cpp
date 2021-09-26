// GPL v3 (c) 2021, Daniel Williams 

// NOTE: This is the template-class implementation --
//       It is not compiled until referenced, even though it contains the
//       function implementations.

#include <fstream>

#include "layer/static-grid/static-grid.hpp"
#include "layer/rolling-grid/rolling-grid-layer.hpp"
#include "chart-box/geometry/local-location.hpp"

#include "flatbuffer.hpp"
#include "cell-cache-generated.h"

using chartbox::layer::StaticGridLayer;

namespace chartbox::io::flatbuffer {

template<>
bool load( const std::filesystem::path& from_path, StaticGridLayer& to_layer ){
    using chartbox::geometry::LocalLocation;
    using chartbox::io::flatbuffer::Cell;
    using chartbox::io::flatbuffer::CellBuilder;
    using chartbox::io::flatbuffer::Location;

    fmt::print( "    >>> Loading from file: {}\n", from_path.string() );

    // write bytes to file:
    std::ifstream source( from_path.string(), std::ios::binary );

    std::array<uint8_t, 1056784> buffer;

    source.read( reinterpret_cast<char*>(buffer.data()), buffer.size() );

    source.close();

    // Get a pointer to the root object inside the buffer.
    const auto cell = chartbox::io::flatbuffer::GetCell( buffer.data() );

    fmt::print( "    >>> Loaded file into flatbuffer struct.\n" );
    fmt::print( stderr,  "    :: dimensions: {} x {} \n", to_layer.dimension, to_layer.dimension );
    fmt::print( stderr,  "    :: origin:     {} x {} \n", to_layer.bounds().min.easting, to_layer.bounds().min.northing );

    fmt::print( "    >>> Loaded file into flatbuffer struct.\n" );
    fmt::print( stderr,  "    :: dimensions: {} x {} \n", cell->width(), cell->height() );
    fmt::print( stderr,  "    :: origin:     {} x {} \n", cell->origin()->easting(), cell->origin()->northing() );


    // not a guaranteed property of the layer; not all of them have this...
    assert( cell->height() == to_layer.dimension );
    assert( cell->width() == to_layer.dimension );

    const LocalLocation origin( cell->origin()->easting(), cell->origin()->northing() );

    // current iteration -- eventually, this should be dynamic
    constexpr double tolerance = 1.0;
    assert( tolerance > std::fabs(cell->origin()->easting() - to_layer.bounds().min.easting) );
    assert( tolerance > std::fabs(cell->origin()->northing() - to_layer.bounds().min.northing) );

    to_layer.fill( cell->data()->data(), cell->data()->size() );

    fmt::print( "    <<< Successfully Loaded.\n" );

    return true;
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