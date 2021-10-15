// GPL v3 (c) 2021, Daniel Williams 
#pragma once

// standard library includes
#include <filesystem>
#include <string>

#include "chart-box/chart-box.hpp"

namespace chartbox::io::png {

const std::string extension = ".png";

template< typename source_t >
bool save( const source_t& from_layer, const geometry::FrameMapping& mapping, const std::filesystem::path& to_path );

template< typename source_t >
bool save( const source_t& from_source, const std::filesystem::path& to_path );

bool save( const chartbox::ChartBox& from_chart, const std::filesystem::path& to_path );

} // namespace chartbox::io

#include "png.inl"
