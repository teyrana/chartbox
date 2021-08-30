// GPL v3 (c) 2021, Daniel Williams 
#pragma once

// standard library includes
#include <filesystem>
#include <string>

#include "chart-box/chart-box.hpp"
#include "layer/static-grid/static-grid.hpp"

namespace chartbox::io::png {

const std::string extension = ".png";

template< typename source_t >
bool save( const source_t& from_layer, const std::filesystem::path& to_path );

template<>
bool save( const chartbox::layer::StaticGridLayer& from_layer, const std::filesystem::path& to_path );

template<>
bool save( const chartbox::ChartBox& from_chart, const std::filesystem::path& to_path );

} // namespace chartbox::io

