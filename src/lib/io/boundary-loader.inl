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

using chartbox::io::geojson::BoundaryLoader;


template<typename layer_t>
BoundaryLoader<layer_t>::BoundaryLoader( FrameMapping& _mapping, layer_t& _destination_layer )
    : layer_(_destination_layer)
    , mapping_(_mapping)
{}

template<typename layer_t>
bool BoundaryLoader<layer_t>::load( const std::filesystem::path& filepath ){
    fmt::print("        >>> With BoundaryLoader:\n");

    if( not std::filesystem::exists(filepath) ) {
        fmt::print( stderr, "!! Could not find input path !!: {}\n", filepath.string() );
        return false;
    }

    CPLJSONDocument doc;
    if( doc.Load( filepath.string()) ){
        const CPLJSONObject& root = doc.GetRoot();
        const bool has_bound_box = ( root["bbox"].IsValid() );
        const bool has_polygon = ( root["features"].IsValid() 
                                && root.GetArray("features")[0].IsValid() 
                                && root.GetArray("features")[0]["geometry"].IsValid()
                                && root.GetArray("features")[0]["geometry"]["type"].IsValid()
                                && root.GetArray("features")[0]["geometry"].GetString("type") == "Polygon" );

        if( has_bound_box && ( not load_boundary_box(root) )){
            fmt::print( stderr, "!! Could not load GeoJSON bounding box: !!!\n");
            fmt::print( stderr, "{}\n", root.Format(CPLJSONObject::PrettyFormat::Pretty) );
            return false;
        }

        if( has_polygon && ( ! load_boundary_polygon(root))){
            fmt::print( stderr, "!! Could not load GeoJSON bounding polygon ?!?!");
            fmt::print( stderr, "{}\n", root.Format(CPLJSONObject::PrettyFormat::Pretty) );
            return false;
        }

        if( not has_polygon ){
            layer_.fill( mapping_.utm_bounds(), layer_.clear_value );
        }
        
        return true;
    }else{
        fmt::print( stderr, "?!?! Unknown failure while loading GeoJSON text into GDAL...\n");
        return false;
    }
}


template<typename layer_t>
bool BoundaryLoader<layer_t>::load_boundary_box( const CPLJSONObject& root ){
    using chartbox::geometry::BoundBox;
    using chartbox::geometry::GlobalLocation;
    using chartbox::geometry::UTMLocation;

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

    const bool move_success = mapping_.move_to_corners( bounds_lat_lon );

    return move_success;
}

template<typename layer_t>
bool BoundaryLoader<layer_t>::load_boundary_polygon( const CPLJSONObject& root ){
using chartbox::geometry::BoundBox;
    using chartbox::geometry::GlobalLocation;
    using chartbox::geometry::LocalLocation;

    if( root["features"].IsValid() ){
        CPLJSONObject feature0 = root.GetArray("features")[0];
        if( feature0["geometry"].IsValid() ){
            CPLJSONObject geom_obj = feature0["geometry"];

            OGRGeometry *geom = OGRGeometryFactory::createFromGeoJson( geom_obj );
            if( nullptr == geom ){
                fmt::print( stderr, "!! Could not load GeoJSON !! \n" );
                return false;
            }

            OGRPolygon* world_frame_polygon = geom->toPolygon();
            if( nullptr == world_frame_polygon ){
                fmt::print( stderr, "!!? Could not convert GeoJSON data to a polygon." );
                return false;
            }

            // translate from WGS84 (lat,lon) -> Local Frame (UTM, but offset relative to the local origin)
            const OGRLinearRing * world_frame_ring = world_frame_polygon->getExteriorRing();
            OGRLinearRing utm_frame_ring;

            for ( auto iter = world_frame_ring->begin(); iter != world_frame_ring->end(); ++iter ) {
                // not the slightly anti-intuitive axis order, here
                const double latitude = (*iter).getY();
                const double longitude = (*iter).getX();

                const LocalLocation& to = mapping_.map_to_local( GlobalLocation( latitude, longitude ) );
                utm_frame_ring.addPoint( new OGRPoint( to.easting, to.northing ) );
            }
            utm_frame_ring.closeRings();
           

            return layer_.fill( utm_frame_ring, layer_.clear_value );
        }
    }
    fmt::print( stderr, "    << no boundary polygon found -- defaulting to boundary box.\n" );
    return false;
}
