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
#include "geometry/local-location.hpp"
#include "geometry/polygon.hpp"

namespace chartbox::io::geojson {

using chartbox::geometry::LocalLocation;
using chartbox::geometry::Polygon;
    
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
    bool load_feature(const CPLJSONArray& feat, uint8_t fill_value, uint8_t except_value );

    Polygon<LocalLocation> load_polygon(const CPLJSONArray& points );

private:
    layer_t& layer_;
    FrameMapping& mapping_;

};

} // namespace chart::io

#include "contour-loader.inl"