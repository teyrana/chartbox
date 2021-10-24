// GPL v3 (c) 2021, Daniel Williams 
#pragma once

// standard library includes
#include <filesystem>
#include <string>

#include "chart-box/chart-box.hpp"
#include "geometry/bound-box.hpp"
#include "geometry/local-location.hpp"

namespace chartbox::io::png {

const std::string extension = ".png";

template< typename source_t >
bool save( const source_t& from_layer, const geometry::BoundBox<geometry::LocalLocation>& bounds, double precision, const std::filesystem::path& to_path );

bool save( const chartbox::ChartBox& from_chart, double precision, const std::filesystem::path& to_path );

} // namespace chartbox::io

#include "png.inl"
