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

using chartbox::io::geojson::ContourLoader;

template<typename layer_t>
const std::string ContourLoader<layer_t>::extension = ".geojson";

template<typename layer_t>
ContourLoader<layer_t>::ContourLoader( FrameMapping& _mapping, layer_t& _destination_layer )
    : layer_(_destination_layer)
    , mapping_(_mapping)
{}

template<typename layer_t>
bool ContourLoader<layer_t>::load( const std::filesystem::path& filepath ){
    fmt::print("        >>> With ContourLoader:\n");

    if( not std::filesystem::exists(filepath) ) {
        fmt::print( stderr, "!! Could not find input path: '{}' !!\n", filepath.string() );
        return false;
    }

    CPLJSONDocument doc;
    if( doc.Load( filepath.string()) ){
        const CPLJSONObject& root = doc.GetRoot();
        const bool has_features = ( root.GetArray("features").IsValid() );

        fmt::print( "        >>> Loading GeoJSON. name: {}\n", root.GetString("name") );

        if( not has_features ){
            fmt::print( stderr, "!! Could not find GeoJSON polygon features ?!?!" );
            fmt::print( stderr, "{}\n", root.Format(CPLJSONObject::PrettyFormat::Pretty) );
            return false;
        }

        const CPLJSONArray& features = root.GetArray("features");
        fmt::print( stderr, "        >> Loading {} features\n", features.Size() );

        layer_.fill( layer_.clear_value );

        for( int feature_index = 0; feature_index < features.Size(); ++feature_index ){
            const CPLJSONObject& each_properties = features[feature_index].GetObj("properties");
            const CPLJSONObject& each_geometry = features[feature_index].GetObj("geometry");

            const int id = each_properties.GetInteger("OBJECTID");
            const bool is_inside = static_cast<bool>(each_properties.GetInteger("INSIDE"));

            fmt::print("            [{: >3d}]: (inside: {})\n", id,  is_inside );

        // this is how the Spec representions polygons + holes
        //
        //   "geometry": { ...
        //     "coordinates": [
        //       [
        //         {polygon},
        //         {hole},
        //         {hole},
        //         {hole}
        //       ]
        //     ]
            const CPLJSONArray& polygons = each_geometry.GetArray("coordinates")[0].ToArray();

            //auto success = 
            load_polygon( polygons[0].ToArray(), layer_.unknown_value );

            for( int hole_index = 1; hole_index < polygons.Size(); ++hole_index ){
                load_polygon( polygons[hole_index].ToArray(), layer_.clear_value );
            }

            // devel code.  Temporary only.
            break;
        }

        return false;

        // return true;
    }else{
        fmt::print( stderr, "?!?! Unknown failure while loading GeoJSON text into GDAL...\n"); 
        return false;
    }
}

template<typename layer_t>
bool ContourLoader<layer_t>::load_polygon( const CPLJSONArray& points, uint8_t /*fill_value*/ ){
    using chartbox::frame::Location2LL;
    using chartbox::frame::Location2xy;

    // just an alias
    // const CPLJSONArray& coords = poly;
    fmt::print("                ::exterior-polygon:  {} points\n", points.Size() );

    const CPLJSONArray& point0 = points[0].ToArray();
    fmt::print("                    ::start: {}, {}\n", point0[0].ToDouble(), point0[1].ToDouble() );

    // dev return
    return false;

//     if( root["features"].IsValid() ){
//         CPLJSONObject feature0 = root.GetArray("features")[0];
//         if( feature0["geometry"].IsValid() ){
//             CPLJSONObject geom_obj = feature0["geometry"];

//             OGRGeometry *geom = OGRGeometryFactory::createFromGeoJson( geom_obj );
//             if( nullptr == geom ){
//                 fmt::print( stderr, "!! Could not load GeoJSON !! :(" );
//                 return false;
//             }

//             OGRPolygon* world_frame_polygon = geom->toPolygon();
//             if( nullptr == world_frame_polygon ){
//                 fmt::print( stderr, "!!? Could not convert GeoJSON data to a polygon." );
//                 return false;
//             }

//             // translate from WGS84 (lat,lon) -> Local Frame (probably UTM)
//             const OGRLinearRing * world_frame_ring = world_frame_polygon->getExteriorRing();
//             OGRLinearRing utm_frame_ring;

//             for ( auto iter = world_frame_ring->begin(); iter != world_frame_ring->end(); ++iter ) {
//                 // not the slightly anti-intuitive axis order, here
//                 const double latitude = (*iter).getY();
//                 const double longitude = (*iter).getX();

//                 const Location2xy to = mapping_.to_local( Location2LL( latitude, longitude ) );

//                 utm_frame_ring.addPoint( new OGRPoint( to.x, to.y ) );
//             }
//             utm_frame_ring.closeRings();
           

//             return layer_.fill( utm_frame_ring, fill_value );
//         }
//     }

    fmt::print( stderr, "        << no polygon found??  Errorr!! \n" );
    return false;
}

