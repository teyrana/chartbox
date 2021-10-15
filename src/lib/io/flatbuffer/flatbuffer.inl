// GPL v3 (c) 2021, Daniel Williams 

// NOTE: This is the template-class implementation --
//       It is not compiled until referenced, even though it contains the
//       function implementations.

#include <fstream>

#include "layer/rolling-grid/rolling-grid-layer.hpp"

#include "tile-cache-generated.hpp"

namespace chartbox::io::flatbuffer {

template<uint32_t n>
bool load( const chartbox::geometry::LocalLocation& at_origin, chartbox::layer::rolling::RollingGridSector<n>& to_sector ){
    using chartbox::io::flatbuffer::TileCache;
    using chartbox::io::flatbuffer::TileCacheBuilder;
    using chartbox::io::flatbuffer::Location;

    if( cache_directory_path.empty() ){
        // cache is not enabled
        to_sector.fill(chartbox::layer::unknown_cell_value);
        return false;
    }

    const auto& filepath = generate_tile_cache_filename( at_origin );
    if( not std::filesystem::is_regular_file(filepath)){
        fmt::print(stderr, "    !! Input path is not a file!!: {}\n", filepath.string());
        to_sector.fill(chartbox::layer::unknown_cell_value);
        return false;
    }

    // read bytes from file:
    std::ifstream source( filepath.string(), std::ios::binary );
    std::array<uint8_t, (sizeof(TileCache) + to_sector.size())> buffer;
    source.read( reinterpret_cast<char*>(buffer.data()), buffer.size() );
    source.close();

    // Load raw bytes into the structured flatbuffer class
    const auto tile = chartbox::io::flatbuffer::GetTileCache( buffer.data() );

    // .1. Check Dimensions Match:
    assert( tile->dimension() == to_sector.cells_across() );

    // .2. Check Real-World Bounds match:
    // sector doesn't track it's own origin ...
    constexpr double tolerance = 1.0;
    assert( tolerance > std::fabs(at_origin.easting - tile->origin()->easting()) );
    assert( tolerance > std::fabs(at_origin.northing - tile->origin()->northing()) );

    // .3. Check Stated Precision
    // TODO: implement a precision check;  the information is not currently available to this method :(
    // assert( tolerance > std::fabs(tile->precision() == with_precision );

    // .4. Checks Passed: load actual data
    to_sector.fill( tile->data()->data(), tile->data()->size() );

    // fmt::print( "{:8s}<<< Tile loaded: {}\n", "", filepath.string() );

    return true;
}


template<uint32_t n>
bool save( const chartbox::layer::rolling::RollingGridSector<n>& from_sector, const chartbox::geometry::LocalLocation& at_origin ){
    if( cache_directory_path.empty() ){
        // cache is not enabled
        return false;
    }

    const auto& filepath = generate_tile_cache_filename( at_origin );
    // fmt::print( stderr, "    >>> Write Sector to: {}\n", filepath.string() );

    flatbuffers::FlatBufferBuilder builder( sizeof(TileCache) + from_sector.size() );

    // create internal objects before parent objects:
    const float precision = from_sector.meters_across_cell;
    const uint32_t dimension = from_sector.cells_across();
    chartbox::io::flatbuffer::Location origin( static_cast<float>(at_origin.easting), static_cast<float>(at_origin.northing) );
    auto datavec = builder.CreateVector( const_cast<uint8_t*>(from_sector.data()), from_sector.size() );

    // build internal representation
    auto tile_cache = chartbox::io::flatbuffer::CreateTileCache( builder, &origin, precision, dimension, datavec );
    builder.Finish(tile_cache);

    // write bytes to file
    std::ofstream dest( filepath.string(), std::ios::binary | std::ios::trunc );
    dest.write( reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize() );
    dest.close();

    // fmt::print(  "{:>16s}<<< Successfuly wrote {} bytes to: {} \n", "", builder.GetSize(), filepath.string() );
    return true;
}


}  // namespace
