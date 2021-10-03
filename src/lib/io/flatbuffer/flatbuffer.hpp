// GPL v3 (c) 2021, Daniel Williams 
#pragma once

// standard library includes
#include <filesystem>

#include "layer/static-grid/static-grid.hpp"

namespace chartbox::io::flatbuffer {

const std::string extension = ".fb";

template<typename T>
bool load( const std::filesystem::path& source_path, T& to );

// declare the specialization
template<>
bool load( const std::filesystem::path& from_path, chartbox::layer::StaticGridLayer& to_layer );

template<typename T>
bool save( const T& from, const std::filesystem::path& to_path );

// declare the specialization
template<>
bool save( const chartbox::layer::StaticGridLayer& from_layer, const std::filesystem::path& to_path);

} // namespace

