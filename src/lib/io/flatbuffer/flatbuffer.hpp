// GPL v3 (c) 2021, Daniel Williams 
#pragma once

// standard library includes
#include <filesystem>

#include "geometry/local-location.hpp"
#include "layer/rolling-grid/rolling-grid-sector.hpp"

namespace chartbox::io::flatbuffer {

const std::string extension = ".fb";

extern std::filesystem::path cache_directory_path;

bool active();

// enable read/write of flatbuffer files by configuring a cache location for them
// - module defaults-off; and fails-off 
// - calling with an empty path explicitly disables read/write
// - scope: global:  all code in a particular program is affecter by this.
bool enable( std::filesystem::path _path );

std::filesystem::path generate_tile_cache_filename( const chartbox::geometry::LocalLocation& origin );

// general declaration
// template<typename T>
// bool load( const std::filesystem::path& source_path, T& to );

// used for cache tile loading
template<uint32_t n>
bool load( const chartbox::geometry::LocalLocation& at_origin, chartbox::layer::rolling::RollingGridSector<n>& to_sector );

// ============ ============ Save Methods ============ ============ 

// general declaration
// template<typename T>
// bool save( const T& from, const std::filesystem::path& to_path );

// specialization 2: Rolling-Grid-Sector: used for cache tile saving
template<uint32_t n>
bool save( const chartbox::layer::rolling::RollingGridSector<n>& from_sector, const chartbox::geometry::LocalLocation& at_origin );


} // namespace

#include "io/flatbuffer/flatbuffer.inl"