// GPL v3 (c) 2021, Daniel Williams 
#pragma once

// standard library includes
#include <filesystem>

// GDAL library
#include <cpl_json.h>

// fmt library
#include <fmt/core.h>

// this project & libraries
#include "chart-box/chart-layer-interface.hpp"
#include "chart-box/geometry/frame-mapping.hpp"
#include "chart-box/geometry/local-location.hpp"
#include "chart-box/geometry/polygon.hpp"
#include "layer/static-grid/static-grid.hpp"
#include "layer/rolling-grid/rolling-grid-layer.hpp"

namespace chartbox::io::geojson {

const std::string extension = ".geojson";

// ====== ====== entry point: template specialized in multiple files ====== ====== 
/// \brief Load a GeoJSON file into a layer
///
/// References:
///   - https://geojson.org/
///   - https://datatracker.ietf.org/doc/html/rfc7946
///   - https://gdal.org/drivers/vector/geojson.html

template<typename layer_t>
bool load_boundary_layer( const std::filesystem::path& from_path, chartbox::geometry::FrameMapping& mapping, layer_t& to_layer );

template<>
bool load_boundary_layer( const std::filesystem::path& from_path, chartbox::geometry::FrameMapping& mapping, chartbox::layer::StaticGridLayer& to_layer );


template<typename layer_t>
bool load_contour_layer( const std::filesystem::path& from_path, chartbox::geometry::FrameMapping& mapping, layer_t& to_layer );

template<>
bool load_contour_layer( const std::filesystem::path& from_path, chartbox::geometry::FrameMapping& mapping, chartbox::layer::RollingGridLayer& to_layer );

template<>
bool load_contour_layer( const std::filesystem::path& from_path, chartbox::geometry::FrameMapping& mapping, chartbox::layer::StaticGridLayer& to_layer );

// internal options:
constexpr static bool fill_interior_holes = false;

} // namespace
