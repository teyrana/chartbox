// GPL v3 (c) 2021, Daniel Williams 
#pragma once

// standard library includes
#include <string>

namespace chartbox::io::png {

const std::string extension = ".png";

template< typename layer_t >
bool save( const layer_t& from_layer, const std::filesystem::path& to_path );

} // namespace chartbox::io

#include "png-saver.inl"
