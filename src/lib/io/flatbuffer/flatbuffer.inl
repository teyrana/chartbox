// GPL v3 (c) 2021, Daniel Williams 

// NOTE: This is the template-class implementation --
//       It is not compiled until referenced, even though it contains the
//       function implementations.

#include <fstream>

#include "layer/rolling-grid/rolling-grid-layer.hpp"
#include "chart-box/geometry/local-location.hpp"

#include "tile-cache-generated.hpp"

namespace chartbox::io::flatbuffer {

// template<uint32_t n,uint32_t m>
// bool load( const std::filesystem::path& from_path, chartbox::layer::RollingGridLayer<n,m>& to_layer ){
//     using chartbox::geometry::LocalLocation;
//     using chartbox::io::flatbuffer::TileCache;
//     using chartbox::io::flatbuffer::TileCacheBuilder;
//     using chartbox::io::flatbuffer::Location;

//     if( from_path.empty() ){
//         fmt::print(stderr, "<<!! No path given !! : {}\n", from_path.string() );
//         return false;
//     }else if( not std::filesystem::is_directory(from_path) ){
//         fmt::print(stderr, "    !! Could contour input path is not a directory!!: {}\n", from_path.string());
//         return false;
//     }

//     fmt::print( "    >>> Loading from file: {}\n", from_path.string() );

    // for( uint32_t at_row = 0; at_row < index.size; ++at_row ){
    //     for( uint32_t at_column = 0; at_column < index.size; ++at_column ){

    //         const Index2u32 index(at_column, at_row);
    //         const auto& sector = to_layer.getSector( index );
    //         const LocalLocation sector_origin = to_layer.getOrigin( index );

    //         const std::string filename = fmt::format("tile_local_{}E_{}N.fb", sector.origin.easting, sector.origin.northing);
    //         const std::filesystem::path search_path = from_path / filename;  
    //         if( std::filesystem::is_regular_file(search_path)){
    //             // load file into tile

//             { // Option A:
//                 //     // write bytes to file:
//                 //     std::ifstream source( from_path.string(), std::ios::binary );
//                 //     std::array<uint8_t, layer.getSectorCacheSize()> buffer;
//                 //     source.read( reinterpret_cast<char*>(buffer.data()), buffer.size() );
//                 //     source.close();

//                 // raw bytes => structured struct
//                 // Get a pointer to the root object inside the buffer.
//                 //     const auto cell = chartbox::io::flatbuffer::GetCell( buffer.data() );

//                 //     fmt::print( "    >>> Loaded file into flatbuffer struct.\n" );
//                 //     fmt::print( stderr,  "    :: dimensions: {} x {} \n", to_layer.dimension, to_layer.dimension );
//                 //     fmt::print( stderr,  "    :: origin:     {} x {} \n", to_layer.bounds().min.easting, to_layer.bounds().min.northing );

//                 //     fmt::print( "    >>> Loaded file into flatbuffer struct.\n" );
//                 //     fmt::print( stderr,  "    :: dimensions: {} x {} \n", cell->width(), cell->height() );
//                 //     fmt::print( stderr,  "    :: origin:     {} x {} \n", cell->origin()->easting(), cell->origin()->northing() );


//                 //     // not a guaranteed property of the layer; not all of them have this...
//                 //     assert( cell->height() == sector.dimension() );
//                 //     assert( cell->width() == sector.dimension() );

//                 //     const LocalLocation origin( cell->origin()->easting(), cell->origin()->northing() );
//                 // assert( sector_origin.easting == cell->origin()->easting() );
//                 // assert( sector_origin.northing == cell->origin()->northing() );

// //     // current iteration -- eventually, this should be dynamic
// //     constexpr double tolerance = 1.0;
// //     assert( tolerance > std::fabs(cell->origin()->easting() - to_layer.bounds().min.easting) );
// //     assert( tolerance > std::fabs(cell->origin()->northing() - to_layer.bounds().min.northing) );

//                 //     sector.fill( cell->data()->data(), cell->data()->size() );

//             }

//             // Option B:
//             // sector.fill( path? );
    //         }else{
    //             // initialize as unknown
    //             sector.fill(unknown_cell_value);
    //         }
    //     }
    // }

//     fmt::print( "    <<< Successfully Loaded.\n" );
//     return true;

//     fmt::print( "    XXX Debug Return.\n" );
//     return false;
// }


// template<uint32_t n,uint32_t m>
// bool save( chartbox::layer::RollingGridLayer<n,m>& from_layer, const std::filesystem::path& to_path ){
//     using chartbox::io::flatbuffer::TileCache;
//     using chartbox::io::flatbuffer::TileCacheBuilder;
//     using chartbox::io::flatbuffer::Location;

//     fmt::print( stderr, "    >>> Write Layer to: {}\n", to_path.string() );

    // // not a guaranteed property of the layer; not all of them have this...
    // const size_t dimension = from_layer.dimension;

    // flatbuffers::FlatBufferBuilder builder( sizeof(Cell) + dimension*dimension );

    // // create internal objects before parent objects:
    // const auto& min = from_layer.bounds().min;
    // auto origin = Location( min.easting, min.northing );
    // auto datavec = builder.CreateVector( const_cast<uint8_t*>(from_layer.data()), dimension*dimension );

    // // build internal representation
    // auto grid_cell = CreateCell( builder, &origin, dimension, dimension, datavec );
    // builder.Finish(grid_cell);

    // // write bytes to file:
    // std::ofstream dest( to_path.string(), std::ios::binary | std::ios::trunc );
    // dest.write( reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize() );
    // dest.close();

    // fmt::print(  "    <<< Successfuly wrote {} bytes to: {} \n", builder.GetSize(), to_path.string() );

    // return true;
// }

}  // namespace