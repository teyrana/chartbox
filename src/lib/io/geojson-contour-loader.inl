// GPL v3 (c) 2021, Daniel Williams 
//
// NOTE: This is not an independent compilation unit! 
//       It is a template-class implementation, and should only be included from its header.

#include <filesystem>
#include <fstream> 

#include "chart-box/geometry/frame-mapping.hpp"
#include "chart-box/geometry/global-location.hpp"
#include "chart-box/geometry/local-location.hpp"
#include "chart-box/geometry/polygon.hpp"
#include "layer/fixed-grid/fixed-grid.hpp"

using chartbox::geometry::FrameMapping;
using chartbox::geometry::GlobalLocation;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::Polygon;
using chartbox::geometry::UTMLocation;
using chartbox::layer::CONTOUR;
using chartbox::layer::FixedGridLayer;

namespace chartbox::io::geojson {

template<>
inline bool load<FixedGridLayer<CONTOUR>>( const std::filesystem::path& from_path, FrameMapping& mapping, FixedGridLayer<CONTOUR>& to_layer ){

    if( not std::filesystem::exists(from_path) ) {
        fmt::print( stderr, "!! Could not find input path: '{}' !!\n", from_path.string() );
        return false;
    }

    CPLJSONDocument doc;
    if( doc.Load( from_path.string()) ){
        fmt::print( "        ::Name: {}\n", doc.GetRoot().GetString("name") );

        const CPLJSONObject& root = doc.GetRoot();
        const bool has_features = ( root.GetArray("features").IsValid() );
        if( not has_features ){
            fmt::print( stderr, "!! Could not find GeoJSON polygon features ?!?!" );
            fmt::print( stderr, "{}\n", root.Format(CPLJSONObject::PrettyFormat::Pretty) );
            return false;
        }

        const CPLJSONArray& features = root.GetArray("features");
        fmt::print( stderr, "        ::Contains {} features\n", features.Size() );

        to_layer.fill( to_layer.unknown_value );

        for( int feature_index = 0; feature_index < features.Size(); ++feature_index ){
            const CPLJSONObject& each_properties = features[feature_index].GetObj("properties");
            // const int id = each_properties.GetInteger("OBJECTID");
            const bool inside = static_cast<bool>(each_properties.GetInteger("INSIDE"));

            // fmt::print( stderr, "            [{: >3d}]: {} \n", id, (outside?"outside":"inside") );

            const CPLJSONObject& each_geometry = features[feature_index].GetObj("geometry");
            const CPLJSONArray& polygon_list = each_geometry.GetArray("coordinates")[0].ToArray();
            
            if( inside ){
                // fmt::print( stderr, "            [{: >3d}]: inside >> fill blocked \n", id );
                if( load_contour_feature( polygon_list, mapping, to_layer.block_value, to_layer.clear_value, to_layer ) ){
                    // fmt::print( stderr, "                <<< Loaded.\n" );
                }
            }else{
                // fmt::print( stderr, "            [{: >3d}]: outside >> fill clear. \n", id );
                if( load_contour_feature( polygon_list, mapping, to_layer.clear_value, to_layer.block_value, to_layer ) ){
                    // fmt::print( stderr, "                <<< Loaded.\n" );
                }
            }
        }

        fmt::print( stderr, "        << finished processing features.\n");
        return true;
    }else{
        fmt::print( stderr, "?!?! Unknown failure while loading GeoJSON text into GDAL...\n"); 
        return false;
    }
}

}   // namespace