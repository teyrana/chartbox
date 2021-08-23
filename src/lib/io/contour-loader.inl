// GPL v3 (c) 2021, Daniel Williams 
//
// NOTE: This is not an independent compilation unit! 
//       It is a template-class implementation, and should only be included from its header.

#include <cmath>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>

#include <cpl_json.h>
#include <gdal.h>
#include <ogr_geometry.h>

#include "geometry/global-location.hpp"
#include "geometry/local-location.hpp"
#include "geometry/polygon.hpp"

using chartbox::io::geojson::ContourLoader;
using chartbox::geometry::GlobalLocation;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::Polygon;
using chartbox::geometry::UTMLocation;

template<typename layer_t>
const std::string ContourLoader<layer_t>::extension = ".geojson";

template<typename layer_t>
ContourLoader<layer_t>::ContourLoader( FrameMapping& _mapping, layer_t& _destination_layer )
    : layer_(_destination_layer)
    , mapping_(_mapping)
{}

template<typename layer_t>
bool ContourLoader<layer_t>::load( const std::filesystem::path& filepath ){
    if( not std::filesystem::exists(filepath) ) {
        fmt::print( stderr, "!! Could not find input path: '{}' !!\n", filepath.string() );
        return false;
    }

    CPLJSONDocument doc;
    if( doc.Load( filepath.string()) ){
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

        layer_.fill( layer_.unknown_value );

        for( int feature_index = 0; feature_index < features.Size(); ++feature_index ){
            const CPLJSONObject& each_properties = features[feature_index].GetObj("properties");
            // const int id = each_properties.GetInteger("OBJECTID");
            const bool inside = static_cast<bool>(each_properties.GetInteger("INSIDE"));

            // fmt::print( stderr, "            [{: >3d}]: {} \n", id, (outside?"outside":"inside") );

            const CPLJSONObject& each_geometry = features[feature_index].GetObj("geometry");
            const CPLJSONArray& polygon_list = each_geometry.GetArray("coordinates")[0].ToArray();
            
            if( inside ){
                // fmt::print( stderr, "            [{: >3d}]: inside >> fill blocked \n", id );
                if( load_feature( polygon_list, layer_.block_value, layer_.clear_value ) ){
                    // fmt::print( stderr, "                <<< Loaded.\n" );
                }
            }else{
                // fmt::print( stderr, "            [{: >3d}]: outside >> fill clear. \n", id );
                if( load_feature( polygon_list, layer_.clear_value, layer_.block_value ) ){
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

template<typename layer_t>
bool ContourLoader<layer_t>::load_feature( const CPLJSONArray& feat, uint8_t fill_value, uint8_t except_value ){
    // fmt::print( stderr, "            >>> Loading Feature: {} polygons.\n", feat.Size() );

    const auto& chart_local_bounds = mapping_.local_bounds();

    if( 0 < feat.Size() ){
        const Polygon<LocalLocation> outer_polygon = load_polygon( feat[0].ToArray() );
        if( 0 == outer_polygon.size() ){
            return false;
        }

        if( outer_polygon.bounds().overlaps( chart_local_bounds ) ){
            // fmt::print( stderr, "                >>> feature overlaps. Filling outer border.\n" );
            layer_.fill( outer_polygon, fill_value );


            // fill interior holes
            if( false ){
                for( int hole_index = 1; hole_index < feat.Size(); ++hole_index ){
                    // fmt::print( stderr, "                    >> Hole[{:>2d}]...\n", hole_index );
                    const Polygon<LocalLocation> hole_polygon = load_polygon( feat[0].ToArray() );
                    layer_.fill( hole_polygon, except_value );
                }
            }

            return true;
        }
    }

    return false;
}

template<typename layer_t>
Polygon<LocalLocation> ContourLoader<layer_t>::load_polygon( const CPLJSONArray& source ){
    if( 4 > source.Size() ){
        return {};
    }else{
        Polygon<LocalLocation> dest( source.Size() );

        for( int point_index = 0; point_index < source.Size(); ++point_index ){
            const CPLJSONArray& each = source[point_index].ToArray();
            if( 2 != each.Size()){
                return {};
            }

            const GlobalLocation each_global( each[1].ToDouble(), each[0].ToDouble() );

            const LocalLocation each_local = mapping_.map_to_local( each_global );

            dest.set( point_index, each_local );
        }

        dest.enclose();

        return dest;
    }
}

