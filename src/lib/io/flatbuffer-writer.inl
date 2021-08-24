// GPL v3 (c) 2021, Daniel Williams 

// NOTE: This is the template-class implementation --
//       It is not compiled until referenced, even though it contains the
//       function implementations.

#include <cstddef>
#include <string>

#include "cell-cache-generated.h"

using chartbox::io::flatbuffer::FlatBufferWriter;

template< typename layer_t >
bool FlatBufferWriter<layer_t>::write( const std::filesystem::path& filepath ){
    using chartbox::flatbuffer::Cell;
    using chartbox::flatbuffer::CellBuilder;
    using chartbox::flatbuffer::Location;

    fmt::print( stderr, "    >>> Write Layer to: {}\n", filepath.string() );

    // not a guaranteed property of the layer; not all of them have this...
    const size_t dimension = layer_.dimension;

    flatbuffers::FlatBufferBuilder builder( sizeof(Cell) + dimension*dimension );

    // create internal objects before parent objects:
    const auto& min = layer_.bounds().min;
    auto origin = Location( min.easting, min.northing );
    auto datavec = builder.CreateVector( layer_.data(), dimension*dimension );

    // build internal representation
    auto grid_cell = CreateCell( builder, &origin, dimension, dimension, datavec );
    builder.Finish(grid_cell);

    // write bytes to file:
    std::ofstream dest( filepath.string(), std::ios::binary | std::ios::trunc );
    dest.write( reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize() );
    dest.close();

    fmt::print(  "    <<< Successfuly wrote {} bytes to: {} \n", builder.GetSize(), filepath.string() );

    return true;
}
