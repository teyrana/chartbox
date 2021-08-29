// GPL v3 (c) 2021, Daniel Williams 
//
// NOTE: This is not an independent compilation unit! 
//       It is a template-class implementation, and should only be included from its header.

#include <cpl_json.h>

#include "chart-box/geometry/frame-mapping.hpp"
#include "chart-box/geometry/global-location.hpp"
#include "chart-box/geometry/local-location.hpp"
#include "chart-box/geometry/polygon.hpp"
#include "layer/static-grid/static-grid.hpp"

#include "geojson.hpp"

using chartbox::layer::BOUNDARY;
using chartbox::layer::CONTOUR;
using chartbox::layer::StaticGridLayer;

using chartbox::geometry::FrameMapping;
using chartbox::geometry::GlobalLocation;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::Polygon;

namespace chartbox::io::geojson {

bool load_bound_box( const CPLJSONObject& root, FrameMapping& mapping ){

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

bool load_contour_feature( const CPLJSONArray& from_feature, const FrameMapping& mapping, uint8_t fill_value, uint8_t except_value, StaticGridLayer<CONTOUR>& to_layer ){
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

bool load_boundary_polygon( const CPLJSONObject& root, const FrameMapping& /*mapping*/, StaticGridLayer<BOUNDARY>& /*to_layer*/ ){
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

}   // namespace
