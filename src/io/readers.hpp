// GPL v3 (c) 2020

#ifndef _YGGDRASIL_READERS_HPP_
#define _YGGDRASIL_READERS_HPP_

#include <cstdio>
#include <string>
#include <vector>

#include <Eigen/Geometry>

#ifdef ENABLE_GDAL
#include "gdal.h"
#include "gdal_priv.h"
#include <gnm.h>
#endif

#include <nlohmann/json.hpp>

#include "geometry/polygon.hpp"

namespace yggdrasil::io {

///! \brief generates a sample json document, used for creating debug yggdrasil
template<typename target_t, typename cell_t>
nlohmann::json generate_diamond(target_t& yggdrasil,
                                const double width,
                                const double desired_precision);

///! \brief loads a generic / unspecified json document from the given input stream
///! 
///! Note: this json document may contain either polygons or a grid, and will be automatically loaded correctly
template<typename target_t, typename cell_t>
bool load_from_json(target_t& yggdrasil, std::string doc);

///! \brief loads content from a generic / unspecified json document
template<typename target_t, typename cell_t>
bool load_from_json(target_t& yggdrasil, nlohmann::json grid );

///! \brief loads a generic / unspecified json document from the given input stream
template<typename target_t, typename cell_t>
bool load_grid_from_json(target_t& yggdrasil, nlohmann::json grid );

///! \brief loads all the allowed and blocked areas
///! @param allow - a (json) list of allowed areas, as defined by polygons, as defined by a list of points.
///! @param block - a (json) list of blocked areas, as defined by polygons, as defined by a list of points.
template<typename target_t, typename cell_t>
bool load_areas_from_json(target_t& yggdrasil, nlohmann::json allow, nlohmann::json block);

///! \brief loads list of polygons from json, into a structure
///!
///! @param allow - a (json) list of allowed areas, as defined by polygons, as defined by a list of points.
///! @return vector of polygons
inline std::vector<geometry::Polygon> make_polygons_from_json(nlohmann::json list);

#ifdef ENABLE_GDAL
///! \brief load a .shp file into this yggdrasil.
inline geometry::Polygon _make_polygons_from_OGRLine( const OGRLinearRing& source );

///! \brief load a .shp file into this yggdrasil.
template<typename target_t, typename cell_t>
bool load_from_shape_file(target_t& yggdrasil, const std::string& filepath);
#endif

}; // namespace yggdrasil::io

#include "readers.inl"

#endif  // #ifdef _YGGDRASIL_READERS_HPP_
