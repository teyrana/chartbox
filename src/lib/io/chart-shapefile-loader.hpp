// GPL v3 (c) 2021, Daniel Williams

#pragma once

#include <string>

#include "chart-base-loader.hpp"


namespace chartbox::io {

/// Shapefile Reference
/// http://shapelib.maptools.org/
template< typename layer_t >
class ShapefileLoader : ChartBaseLoader<layer_t, ShapefileLoader<layer_t> > {
public:

    ShapefileLoader( FrameMapping& _mapping, layer_t& _layer )
        : layer_(_layer)
        , mapping_(_mapping)
        {}

    bool load(const std::string& filename);

private:
    /// could not get this variant to work, even though it would be much more efficient...
    // bool load_boundary_box( const OGRSpatialReference& const spat_ref );

    bool load_boundary_box( const OGRLinearRing& exterior );

private:
    layer_t& layer_;
    FrameMapping& mapping_;

}; // class ShapefileLoader

} // namespace chartbox::io

#include "chart-shapefile-loader.inl"
