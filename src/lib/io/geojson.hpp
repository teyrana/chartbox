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

namespace chartbox::io::geojson {

const std::string extension = ".geojson";

// ====== ====== entry point: template specialized in multiple files ====== ====== 
/// \brief Load a GeoJSON file into a layer
///
/// References:
///   - https://geojson.org/
///   - https://datatracker.ietf.org/doc/html/rfc7946
///   - https://gdal.org/drivers/vector/geojson.html
template< typename layer_t>
inline bool load( const std::filesystem::path& from_path, chartbox::geometry::FrameMapping& /*mapping*/, layer_t& to_layer ){
    // fallback implementation
    fmt::print( "    <<< 'geojson::load' not implemented for: {} in role: {}  (from path: {})\n", to_layer.name(), to_layer.role(), from_path.string() );
    return false;
}

// ====== ====== see: geojson.cpp ====== ====== 

// ====== Generic Functions ====== 

chartbox::geometry::Polygon<chartbox::geometry::LocalLocation> load_polygon(const CPLJSONArray& points, const chartbox::geometry::FrameMapping& mapping );

///  The values of a "bbox" array are "[west, south, east, north]", not
///     "[minx, miny, maxx, maxy]" (see Section 5).  --rfc 7946
bool load_bound_box( const CPLJSONObject& root, chartbox::geometry::FrameMapping& mapping );

bool load_boundary_polygon( const CPLJSONObject& root, const chartbox::geometry::FrameMapping& mapping, chartbox::layer::FixedGridLayer<chartbox::layer::BOUNDARY>& to_layer );

// ====== Contour-Specific Functions ====== 
/// \brief loads json data (of a specific shape) file into a layer
///
///     Partial Sample of expected json:
///         "geometry": { ...
///           "coordinates": [
///             [
///               [ // outer polygon
///                 [ // point #0 of polygon
///                   ...
///                 ]
///               ],[ // hole polygon #1
///               ],[ // hole polygon #2
///               ],[ // hole polygon #3
///                   ...
///               ]
///             ...
///
/// \param json array containing a list of point-lists (see above)
/// \param fill_value -- fill the outer polygon with this value
/// \param except_value -- fill the holes within the outer polygon with this value
bool load_contour_feature( const CPLJSONArray& from_feature, const chartbox::geometry::FrameMapping& mapping, uint8_t fill_value, uint8_t except_value, chartbox::layer::FixedGridLayer<chartbox::layer::CONTOUR>& to_layer );

// internal options:
constexpr static bool fill_interior_holes = false;

} // namespace

#include "geojson-boundary-loader.inl"
#include "geojson-contour-loader.inl"
