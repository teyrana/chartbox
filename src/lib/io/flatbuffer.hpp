// GPL v3 (c) 2021, Daniel Williams 

// standard library includes
#include <filesystem>

#pragma once

namespace chartbox::io::flatbuffer {

using chartbox::layer::role_t;

const std::string extension = ".fb";

template< typename layer_t >
inline bool load( const std::filesystem::path& source_path, layer_t& dest_layer ) {
    // fallback implementation
    fmt::print( "    <<< 'flatbuffer::load' not implemented for layer-role: {}  from: {} \n", dest_layer.role(), source_path.string() );
    return false;
}

template< typename layer_t >
inline bool save( const layer_t& from_layer, const std::filesystem::path& to_path ){
    // fallback implementation
    fmt::print( "    <<< 'flatbuffer::save' not implemented for layer-role: {}  to: {} \n", from_layer.role(), to_path.string() );
    return false;
}

} // namespace

#include "flatbuffer-saver.inl"
#include "flatbuffer-loader.inl"
