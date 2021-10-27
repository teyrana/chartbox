// GPL v3 (c) 2021, Daniel Williams 
#pragma once

// standard library includes
#include <filesystem>
#include <fstream>

// JSON Library
#include <nlohmann/json.hpp>

// fmt library
#include <fmt/core.h>

// this project & libraries
#include "geometry/bound-box.hpp"
#include "geometry/frame-mapping.hpp"
#include "geometry/local-location.hpp"
#include "geometry/polygon.hpp"
#include "layer/layer-interface.hpp"


/// \brief Load a GeoJSON file into a layer
///
/// References:
///   - https://geojson.org/
///   - https://datatracker.ietf.org/doc/html/rfc7946
///   - https://gdal.org/drivers/vector/geojson.html
/// Useful Tools:
///   - https://geojson.io
namespace chartbox::io::geojson {

const std::string extension = ".geojson";

// ====== ====== Load Boundary Methods ====== ====== 
bool load_boundary_box( const std::filesystem::path& from_path, chartbox::geometry::FrameMapping& mapping );

template<typename layer_t>
bool load_boundary_layer( const std::filesystem::path& from_path, const chartbox::geometry::FrameMapping& mapping, layer_t& to_layer );


// ====== ====== Load Contour Methods ====== ======
template<typename layer_t>
bool load_contour_layer( const std::filesystem::path& from_path, const chartbox::geometry::FrameMapping& mapping, layer_t& to_layer );

/// \brief generic version for loading data to the cache
///
/// \param transfer_window - data struct to load with input file, and then flush to cache -- this is a scratch buffer
template<typename buffer_t>
bool process_contour_to_cache( const std::filesystem::path& from_path, const chartbox::geometry::FrameMapping& mapping, const buffer_t& transfer_window );

// ====== Internal Functions ======

/// internal option
constexpr static bool fill_interior_holes = false;

/// \brief loads a polygon from the given geosjon document into the given mapping

///  The values of a "bbox" array are "[west, south, east, north]", not
///     "[minx, miny, maxx, maxy]" (see Section 5).  --rfc 7946
bool load_boundary_box( const nlohmann::json& root, chartbox::geometry::FrameMapping& mapping );

/// \brief loads a polygon from the given json object
chartbox::geometry::Polygon<chartbox::geometry::LocalLocation> load_polygon(const nlohmann::json& points, const chartbox::geometry::FrameMapping& mapping );

} // namespace

#include "boundary.inl"

#include "contour.inl"
