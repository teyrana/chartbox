// GPL v3 (c) 2021, Daniel Williams 

// NOTE: This is the template-class implementation --
//       It is not compiled until referenced, even though it contains the
//       function implementations.

#include <cstddef>
#include <string>

#include "cell-cache-generated.h"

using chartbox::io::flatbuffer::FlatBufferLoader;

template< typename layer_t >
bool FlatBufferLoader<layer_t>::load( const std::filesystem::path& filepath ){
    using chartbox::flatbuffer::Cell;
    using chartbox::flatbuffer::CellBuilder;
    using chartbox::flatbuffer::Location;

    fmt::print( "    >>> Loading from file: {}\n", filepath.string() );

    switch( layer_.purpose() ){
        case LAYER_PURPOSE_BOUNDARY:
            fmt::print( "    <<< Boundary Layers do not (yet) support loading flatbuffers" );
            // not supported
            return false;
        case LAYER_PURPOSE_CONTOUR:
            // continue
            break;
        case LAYER_PURPOSE_LIDAR:
            fmt::print( "    <<< Lidar Layers do not (yet) support loading flatbuffers" );
            // not supported
            return false;
        default:
            return false;
    }

    // write bytes to file:
    std::ifstream source( filepath.string(), std::ios::binary );

    std::array<uint8_t, 1056784> buffer;

    source.read( reinterpret_cast<char*>(buffer.data()), buffer.size() );

    source.close();

    // Get a pointer to the root object inside the buffer.
    const auto cell = chartbox::flatbuffer::GetCell( buffer.data() );

    fmt::print( "    >>> Loaded file into flatbuffer struct.\n" );
    fmt::print( stderr,  "    :: dimensions: {} x {} \n", layer_.dimension, layer_.dimension );
    fmt::print( stderr,  "    :: origin:     {} x {} \n", layer_.bounds().min.easting, layer_.bounds().min.northing );

    fmt::print( "    >>> Loaded file into flatbuffer struct.\n" );
    fmt::print( stderr,  "    :: dimensions: {} x {} \n", cell->width(), cell->height() );
    fmt::print( stderr,  "    :: origin:     {} x {} \n", cell->origin()->easting(), cell->origin()->northing() );


    // not a guaranteed property of the layer; not all of them have this...
    assert( cell->height() == layer_.dimension );
    assert( cell->width() == layer_.dimension );

    const UTMLocation origin( cell->origin()->easting(), cell->origin()->northing() );

    // current iteration -- eventually, this should be dynamic
    constexpr double tolerance = 1.0;
    assert( tolerance > std::fabs(cell->origin()->easting() - layer_.bounds().min.easting) );
    assert( tolerance > std::fabs(cell->origin()->northing() - layer_.bounds().min.northing) );

    layer_.fill( cell->data()->data(), cell->data()->size() );

    fmt::print( "    <<< Successfuly Loaded.\n" );

    return true;
}
