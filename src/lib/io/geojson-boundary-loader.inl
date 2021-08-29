// GPL v3 (c) 2021, Daniel Williams 
//
// NOTE: This is not an independent compilation unit! 
//       It is a template-class implementation, and should only be included from its header.

#include <filesystem>
#include <fstream>

// this project & libraries
#include "chart-box/geometry/frame-mapping.hpp"
#include "layer/static-grid/static-grid.hpp"

using chartbox::geometry::FrameMapping;
using chartbox::layer::BOUNDARY;
using chartbox::layer::StaticGridLayer;

namespace chartbox::io::geojson {

template<>
inline bool load_boundary( const std::filesystem::path& from_path, FrameMapping& mapping, StaticGridLayer& to_layer ){

    if( not std::filesystem::exists(from_path) ) {
        fmt::print( stderr, "!! Could not find input path !!: {}\n", from_path.string() );
        return false;
    }

    CPLJSONDocument doc;
    if( doc.Load( from_path.string()) ){
        const CPLJSONObject& root = doc.GetRoot();
        const bool has_bound_box = ( root["bbox"].IsValid() );
        const bool has_polygon = ( root["features"].IsValid() 
                                && root.GetArray("features")[0].IsValid() 
                                && root.GetArray("features")[0]["geometry"].IsValid()
                                && root.GetArray("features")[0]["geometry"]["type"].IsValid()
                                && root.GetArray("features")[0]["geometry"].GetString("type") == "Polygon" );

        if( has_bound_box && ( not load_bound_box(root, mapping) )){
            fmt::print( stderr, "!! Could not load GeoJSON bounding box: !!!\n");
            fmt::print( stderr, "{}\n", root.Format(CPLJSONObject::PrettyFormat::Pretty) );
            return false;
        }

        if( has_polygon ){
            CPLJSONObject geom = root.GetArray("features")[0]["geometry"];
            if( not geom.IsValid() ){
                fmt::print( stderr, "!! Could not load geom from feature: !!!\n{}\n", root.Format(CPLJSONObject::PrettyFormat::Pretty) );
                return false;
            }

            CPLJSONArray points = geom.GetArray("coordinates")[0].ToArray();
            if( 0 == points.Size() ){
                fmt::print( stderr, "!! Could not load ponits from geom: !!!\n{}\n", geom.Format(CPLJSONObject::PrettyFormat::Pretty) );
                return false;
            }

            const auto polygon = load_polygon( points, mapping );

            to_layer.fill( polygon, to_layer.clear_value );

        }else{
            to_layer.fill( mapping.local_bounds(), to_layer.clear_value );
        }

        return true;
    }else{
        fmt::print( stderr, "?!?! Unknown failure while loading GeoJSON text into GDAL...\n");
        return false;
    }
}

} // namespace