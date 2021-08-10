// GPL v3 (c) 2021, Daniel Williams 
#pragma once

// standard library includes
#include <string>
#include <vector>

// GDAL
#include <cpl_json.h>

// fmt
#include <fmt/core.h>

#include "chart-box.hpp"
#include "base-loader.hpp"

namespace chartbox::io::geojson {

/// \brief Load a GeoJSON file into a layer
///
/// References:
///   - https://geojson.org/
///   - https://datatracker.ietf.org/doc/html/rfc7946
///   - https://gdal.org/drivers/vector/geojson.html
template< typename layer_t >
class ContourLoader : BaseLoader<layer_t, ContourLoader<layer_t> > {
public:
    const static std::string extension;

    ContourLoader( FrameMapping& _mapping, layer_t& _layer );

    bool load(const std::filesystem::path& filename);

private:

    bool load_polygon(const CPLJSONArray& points, uint8_t fill_value );

private:
    layer_t& layer_;
    FrameMapping& mapping_;

};

} // namespace chart::io

#include "contour-loader.inl"