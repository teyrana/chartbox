// GPL v3 (c) 2021, Daniel Williams 
//
// NOTE: This is not an independent compilation unit! 
//       It is a template-class implementation, and should only be included from its header.

// standard libraries
#include <filesystem>
#include <fstream> 

// third-party libraries: GDAL:
#include <cpl_json.h>

// this project & libraries
#include "chart-box/geometry/frame-mapping.hpp"
#include "chart-box/geometry/global-location.hpp"
#include "chart-box/geometry/local-location.hpp"
#include "chart-box/geometry/polygon.hpp"
#include "layer/static-grid/static-grid.hpp"

#include "geojson.hpp"

using chartbox::geometry::FrameMapping;
using chartbox::geometry::GlobalLocation;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::Polygon;
using chartbox::geometry::UTMLocation;
using chartbox::layer::BOUNDARY;
using chartbox::layer::CONTOUR;
using chartbox::layer::StaticGridLayer;

namespace chartbox::io::geojson {

// ====== Internal Function Declarations ====== 

chartbox::geometry::Polygon<chartbox::geometry::LocalLocation> load_polygon(const CPLJSONArray& points, const chartbox::geometry::FrameMapping& mapping );

///  The values of a "bbox" array are "[west, south, east, north]", not
///     "[minx, miny, maxx, maxy]" (see Section 5).  --rfc 7946
bool load_boundary_box( const CPLJSONObject& root, chartbox::geometry::FrameMapping& mapping );

bool load_boundary_polygon( const CPLJSONObject& root, const chartbox::geometry::FrameMapping& mapping, chartbox::layer::StaticGridLayer& to_layer );

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
bool load_contour_feature( const CPLJSONArray& from_feature, const chartbox::geometry::FrameMapping& mapping, uint8_t fill_value, uint8_t except_value, chartbox::layer::StaticGridLayer& to_layer );

// ====== Function Definitions ====== 
bool load_boundary_box( const CPLJSONObject& root, FrameMapping& mapping ){

    auto bound_box_elem = root.GetArray("bbox");
    if( ! bound_box_elem.IsValid()){
        fmt::print( stderr, "'bbox' element is not valid!?\n" );
        return false;
    }else if(4 != bound_box_elem.Size()) {
        fmt::print( stderr, "bbox element did not have 4 elements!?\n" );
        return false;
    }

    const double west_longitude = bound_box_elem[0].ToDouble(NAN);
    const double south_latitude = bound_box_elem[1].ToDouble(NAN);
    const double east_longitude = bound_box_elem[2].ToDouble(NAN);
    const double north_latitude = bound_box_elem[3].ToDouble(NAN);

    using std::isnan;
    if( isnan(west_longitude) || isnan(south_latitude) || isnan(east_longitude) || isnan(north_latitude) ){
        fmt::print( stderr, "found NAN for a bounding-box limit!?\n" );
        return false;
    }

    const BoundBox<GlobalLocation> bounds_lat_lon( GlobalLocation( south_latitude, west_longitude ),
                                                   GlobalLocation( north_latitude, east_longitude ) );

    const bool move_success = mapping.move_to_corners( bounds_lat_lon );

    return move_success;
}

bool load_boundary_layer( const std::filesystem::path& from_path, FrameMapping& mapping, StaticGridLayer& to_layer ){

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

        if( has_bound_box && ( not load_boundary_box(root, mapping) )){
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

bool load_boundary_polygon( const CPLJSONObject& root, const FrameMapping& /*mapping*/, StaticGridLayer& /*to_layer*/ ){
    if( root["features"].IsValid() ){
        CPLJSONObject feature0 = root.GetArray("features")[0];
        if( feature0["geometry"].IsValid() ){
            CPLJSONObject geom_obj = feature0["geometry"];

            fmt::print( stderr, "    << load boundary polygon ... NYI\n");

            // Polygon<LocalLocation> local_frame_polygon = load_polygon( world, mapping );

            // return to_layer.fill( local_frame_polygon, layer_.clear_value );
        }
    }
    fmt::print( stderr, "    << no boundary polygon found -- defaulting to boundary box.\n" );
    return false;
}

Polygon<LocalLocation> load_polygon( const CPLJSONArray& source, const FrameMapping& mapping ){
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

            const LocalLocation each_local = mapping.map_to_local( each_global );

            dest.set( point_index, each_local );
        }

        dest.enclose();

        return dest;
    }
}


bool load_contour_feature( const CPLJSONArray& from_feature, const FrameMapping& mapping, uint8_t fill_value, uint8_t except_value, StaticGridLayer& to_layer ){
    // fmt::print( stderr, "            >>> Loading Feature: {} polygons.\n", feat.Size() );

    const auto& chart_local_bounds = mapping.local_bounds();

    if( 0 < from_feature.Size() ){
        const Polygon<LocalLocation> outer_polygon = load_polygon( from_feature[0].ToArray(), mapping );
        if( 0 == outer_polygon.size() ){
            return false;
        }

        if( outer_polygon.bounds().overlaps( chart_local_bounds ) ){
            // fmt::print( stderr, "                >>> feature overlaps. Filling outer border.\n" );
            to_layer.fill( outer_polygon, fill_value );

            if( chartbox::io::geojson::fill_interior_holes ){
                for( int hole_index = 1; hole_index < from_feature.Size(); ++hole_index ){
                    // fmt::print( stderr, "                    >> Hole[{:>2d}]...\n", hole_index );
                    const Polygon<LocalLocation> hole_polygon = load_polygon( from_feature[0].ToArray(), mapping );
                    to_layer.fill( hole_polygon, except_value );
                }
            }

            return true;
        }
    }

    return false;
}

bool load_contour_layer( const std::filesystem::path& from_path, FrameMapping& mapping, StaticGridLayer& to_layer ){

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