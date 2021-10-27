// GPL v3 (c) 2021, Daniel Williams 

// NOTE: This is the template-class implementation --
//       It is not compiled until referenced, even though it contains the
//       function implementations.

#include <fstream>

#include "geometry/local-location.hpp"

#include "flatbuffer.hpp"
#include "tile-cache-generated.hpp"

namespace chartbox::io::flatbuffer {

std::filesystem::path cache_directory_path;

bool active() { return not cache_directory_path.empty(); }

bool enable( std::filesystem::path _path ) {
    cache_directory_path = _path;

    if( not std::filesystem::exists(cache_directory_path) ){
        std::filesystem::create_directory(cache_directory_path);
    }

    return active();
}

std::filesystem::path generate_tile_cache_filename( const chartbox::geometry::LocalLocation& origin ) {
    const std::string filename = fmt::format("tile_local_{:06}E_{:06}N.fb", origin.easting, origin.northing );
    return cache_directory_path / filename;
}

}  // namespace