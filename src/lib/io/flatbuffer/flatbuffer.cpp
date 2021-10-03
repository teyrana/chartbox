// GPL v3 (c) 2021, Daniel Williams 

// NOTE: This is the template-class implementation --
//       It is not compiled until referenced, even though it contains the
//       function implementations.

#include <fstream>

#include "layer/static-grid/static-grid-layer.hpp"
#include "layer/rolling-grid/rolling-grid-layer.hpp"
#include "chart-box/geometry/local-location.hpp"

#include "flatbuffer.hpp"
#include "tile-cache-generated.hpp"

using chartbox::layer::StaticGridLayer;

namespace chartbox::io::flatbuffer {

// template<>
// bool load( const std::filesystem::path& from_path, StaticGridLayer& to_layer ){
//     using chartbox::geometry::LocalLocation;
//     using chartbox::io::flatbuffer::TileCache;
//     using chartbox::io::flatbuffer::TileCacheBuilder;
//     using chartbox::io::flatbuffer::Location;

//     if( from_path.empty() ){
//         fmt::print(stderr, "<<!! No path given !! : {}\n", from_path.string() );
//         return false;
//     }else if( not std::filesystem::is_regular_file(from_path) ){
//         fmt::print(stderr, "    !! Could contour input path is not a file?!: {}\n", from_path.string());
//         return false;
//     }

//     fmt::print( "    >>> Loading from file: {}\n", from_path.string() );

//     // open source file
//     std::ifstream source( from_path.string(), std::ios::binary );

//     std::array<uint8_t, 1056784> buffer;

//     source.read( reinterpret_cast<char*>(buffer.data()), buffer.size() );

//     source.close();

//     // Get a pointer to the root object inside the buffer.
//     const auto tile = chartbox::io::flatbuffer::GetTileCache( buffer.data() );

//     fmt::print( "    >>> Loaded file into flatbuffer struct.\n" );
//     fmt::print( stderr,  "    :: dimensions: {} x {} \n", to_layer.dimension, to_layer.dimension );

//     fmt::print( "    >>> Loaded file into flatbuffer struct.\n" );
//     fmt::print( stderr,  "    :: dimensions: {} x {} \n", tile->dimension(), tile->dimension() );
//     fmt::print( stderr,  "    :: dimensions: {} x {} \n", tile->dimension(), tile->dimension() );
//     fmt::print( stderr,  "    :: origin:     {} x {} @{}\n", tile->origin()->easting(), tile->origin()->northing(), tile->precision() );

//     assert( StaticGridLayer::dimension == tile->dimension() );
//     assert( to_layer.precision() == tile->precision() );

//     const LocalLocation origin( tile->origin()->easting(), tile->origin()->northing() );

//     to_layer.fill( tile->data()->data(), tile->data()->size() );

//     fmt::print( "    <<< Successfully Loaded.\n" );

//     return true;
// }

// template<>
// bool save( const StaticGridLayer& from_layer, const std::filesystem::path& to_path){
//     using chartbox::io::flatbuffer::TileCache;
//     using chartbox::io::flatbuffer::TileCacheBuilder;
//     using chartbox::io::flatbuffer::Location;

//     fmt::print( stderr, "    >>> Write Layer to: {}\n", to_path.string() );

//     // not a guaranteed property of the layer; not all of them have this...
//     const size_t dimension = from_layer.dimension;

//     flatbuffers::FlatBufferBuilder builder( sizeof(TileCache) + dimension*dimension );

//     // create internal objects before parent objects:
//     const auto& min = from_layer.bounds().min;
//     auto origin = Location( min.easting, min.northing );
//     auto datavec = builder.CreateVector( const_cast<uint8_t*>(from_layer.data()), dimension*dimension );

//     // build internal representation
//     auto tile = CreateTileCache( builder, &origin, dimension, dimension, datavec );
//     builder.Finish(tile);

//     // write bytes to file:
//     std::ofstream dest( to_path.string(), std::ios::binary | std::ios::trunc );
//     dest.write( reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize() );
//     dest.close();

//     fmt::print(  "    <<< Successfuly wrote {} bytes to: {} \n", builder.GetSize(), to_path.string() );

//     return true;
// }

}  // namespace