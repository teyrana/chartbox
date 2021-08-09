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

using chartbox::io::GeoJSONLoader;

template<typename layer_t>
const std::string GeoJSONLoader<layer_t>::extension = ".geojson";

template<typename layer_t>
GeoJSONLoader<layer_t>::GeoJSONLoader( FrameMapping& _mapping, layer_t& _destination_layer )
    : layer_(_destination_layer)
    , mapping_(_mapping)
{}

template<typename layer_t>
bool GeoJSONLoader<layer_t>::load( const std::filesystem::path& filepath ){
    fmt::print("        >>> With GeoJSONLoader:\n");

    if( not std::filesystem::exists(filepath) ) {
        std::cerr << "!! Could not find input path !!: " << filepath.string() << std::endl;
        return false;
    }

    CPLJSONDocument doc;
    if( doc.Load( filepath.string()) ){
        return load_json( doc.GetRoot() );
    }else{
        std::cerr << "?!?! Unknown failure while loading GeoJSON text into GDAL...\n"; 
        return false;
    }
}

template<typename layer_t>
bool GeoJSONLoader<layer_t>::load_json( const CPLJSONObject& root ){
    const bool has_bound_box = ( root["bbox"].IsValid() );
    const bool has_polygon = ( root["features"].IsValid() 
                            && root.GetArray("features")[0].IsValid() 
                            && root.GetArray("features")[0]["geometry"].IsValid()
                            && root.GetArray("features")[0]["geometry"]["type"].IsValid()
                            && root.GetArray("features")[0]["geometry"].GetString("type") == "Polygon" );

    if( has_bound_box && ( ! load_json_boundary_box(root, !has_polygon ))){
        std::cerr << "!! Could not load GeoJSON bounding box: !!!" << std::endl;
        std::cerr << root.Format(CPLJSONObject::PrettyFormat::Pretty) << std::endl;   
        return false;
    }

    if( has_polygon && ( ! load_json_boundary_polygon(root))){
        std::cerr << "!! Could not load GeoJSON bounding polygon ?!?!" << std::endl;
        std::cerr << root.Format(CPLJSONObject::PrettyFormat::Pretty) << std::endl;   
        return false;
    }

    return true;
}

template<typename layer_t>
bool GeoJSONLoader<layer_t>::load_json_boundary_box( const CPLJSONObject& root, bool fill ){
    using chartbox::frame::BoundBox;
    using chartbox::frame::Location2LL;
    using chartbox::frame::Location2xy;

    auto bound_box_elem = root.GetArray("bbox");
    if( ! bound_box_elem.IsValid()){
        std::cerr << "'bbox' element is not valid!?" << std::endl;
        return false;
    }else if(4 != bound_box_elem.Size()) {
        std::cerr << "bbox element did not have 4 elements!?" << std::endl;
        return false;
    }

    const double west_longitude = bound_box_elem[0].ToDouble(NAN);
    const double south_latitude = bound_box_elem[1].ToDouble(NAN);
    const double east_longitude = bound_box_elem[2].ToDouble(NAN);
    const double north_latitude = bound_box_elem[3].ToDouble(NAN);

    using std::isnan;
    if( isnan(west_longitude) || isnan(south_latitude) || isnan(east_longitude) || isnan(north_latitude) ){
        std::cerr << "found NAN for a bounding-box limit!?" << std::endl;
        return false;
    }

    const BoundBox<Location2LL> bounds_lat_lon( Location2LL( south_latitude, west_longitude ),
                                                Location2LL( north_latitude, east_longitude ) );

    const bool move_success = mapping_.move_to_corners( bounds_lat_lon );

    if( move_success && fill ){
        const Location2xy bounds_min_local = mapping_.to_local( bounds_lat_lon.min );
        const Location2xy bounds_max_local = mapping_.to_local( bounds_lat_lon.max );
        const BoundBox<Location2xy> bounds_local( bounds_min_local, bounds_max_local );
        return layer_.fill( bounds_local, layer_.clear_value );
    }
    return move_success;
}

template<typename layer_t>
bool GeoJSONLoader<layer_t>::load_json_boundary_polygon( const CPLJSONObject& root ){
    using chartbox::frame::Location2LL;
    using chartbox::frame::Location2xy;

    if( root["features"].IsValid() ){
        CPLJSONObject feature0 = root.GetArray("features")[0];
        if( feature0["geometry"].IsValid() ){
            CPLJSONObject geom_obj = feature0["geometry"];

            OGRGeometry *geom = OGRGeometryFactory::createFromGeoJson( geom_obj );
            if( nullptr == geom ){
                std::cerr << "!! Could not load GeoJSON !! :(" << std::endl;
                return false;
            }

            OGRPolygon* world_frame_polygon = geom->toPolygon();
            if( nullptr == world_frame_polygon ){
                std::cerr << "!!? Could not convert GeoJSON data to a polygon." << std::endl;
                return false;
            }

            // translate from WGS84 (lat,lon) -> Local Frame (probably UTM)
            const OGRLinearRing * world_frame_ring = world_frame_polygon->getExteriorRing();
            OGRLinearRing utm_frame_ring;

            for ( auto iter = world_frame_ring->begin(); iter != world_frame_ring->end(); ++iter ) {
                // not the slightly anti-intuitive axis order, here
                const double latitude = (*iter).getY();
                const double longitude = (*iter).getX();

                const Location2xy to = mapping_.to_local( Location2LL( latitude, longitude ) );

                utm_frame_ring.addPoint( new OGRPoint( to.x, to.y ) );
            }
            utm_frame_ring.closeRings();
           

            return layer_.fill( utm_frame_ring, layer_.clear_value );
        }
    }
    std::cerr << "    << no boundary polygon found -- defaulting to boundary box.\n" << std::endl;
    return false;
}

// inline Polygon
// chart::io::_make_polygons_from_OGRLine(const OGRLinearRing& source) {
//     const size_t point_count = static_cast<size_t>(source.getNumPoints());

//     if (0 < point_count) {
//         Polygon result(point_count);

//         OGRPoint scratch;
//         for (size_t point_index = 0; point_index < point_count; ++point_index) {
//             source.getPoint(point_index, &scratch);
//             result[point_index] = {scratch.getX(), scratch.getY()};
//         }

//         result.complete();

//         return result;
//     }

//     return {};
// }

// #include <sys/stat.h>

// template <typename target_t, typename cell_t>
// bool chart::io::load_from_shape_file(target_t& target,
//                                          const string& filepath) {
//     const cell_t allow_value = 0;
//     const cell_t block_value = 0x99;

//     // might be a duplicate call, but duplicate calls don't seem to cause any
//     // problems.
//     GDALAllRegister();

//     auto* source_dataset = (GDALDataset*)GDALOpenEx(
//         filepath.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
//     if (source_dataset == NULL) {
//         cerr << "!> Open failed.  No source dataset available?\n";

//         struct stat buf;
//         if (stat(filepath.c_str(), &buf) != -1) {
//             cerr << "    >> Found file: '" << filepath << "' >>\n";
//         } else {
//             cerr << "    !! Missing data file: '" << filepath << "' !!\n";
//         }

//         return false;
//     }

//     // DEBUG
//     // cerr << "#> Loaded file: '" << filepath << "' into memory...\n";

//     // DEBUG
//     // for( auto * each_layer: source_dataset->GetLayers() ){
//     //     GNMGenericLayer * each_gnm_layer = (GNMGenericLayer*)each_layer;
//     //     cerr << "___?> Layer.Name: " << each_gnm_layer->GetName() << endl;
//     // }

//     OGRLayer* shape_layer = source_dataset->GetLayer(0);
//     // ... ->GetLayerByName( "navigation_area_100k" );
//     if (nullptr == shape_layer) {
//         fprintf(stderr, "    !! dataset doesn't contain any layer? ");
//         goto CLEANUP_LOAD_SHAPEFILE;
//     }

//     shape_layer->ResetReading();
//     OGRFeature* poly_feature;
//     while ((poly_feature = shape_layer->GetNextFeature()) != NULL) {

//         OGRGeometry* geom = poly_feature->GetGeometryRef();
//         if (geom == NULL) {
//             fprintf(stderr,
//                     "    !! layer -> feature: could not load geometry! \n");
//             goto CLEANUP_LOAD_SHAPEFILE;
//         }

//         if (wkbPolygon != wkbFlatten(geom->getGeometryType())) {
//             fprintf(stderr, "    <! geometry is not a polygon! aborting!\n");
//             goto CLEANUP_LOAD_SHAPEFILE;
//         }

//         // Reference: OGRPolygon
//         // https://gdal.org/api/ogrgeometry_cpp.html#ogrpolygon-class
//         OGRPolygon* poly = geom->toPolygon();
//         const Polygon& exterior =
//             _make_polygons_from_OGRLine(*poly->getExteriorRing());

//         target.fill(block_value);

//         // fprintf( stderr, "    .... loading exterior ring with %zu points.\n",
//         // exterior.size() );
//         target.fill(exterior, allow_value);

//         // if we have any interior rings, load them:
//         if (0 < poly->getNumInteriorRings()) {
//             if (wkbLineString != poly->getInteriorRing(0)->getGeometryType()) {
//                 fprintf(stderr, "    !! polygon does not contain "
//                                 "wkbLineString/LINEARRING geometries!\n");
//                 fprintf(stderr, "        (instead found : %s (%d) )\n",
//                         poly->getInteriorRing(0)->getGeometryName(),
//                         poly->getInteriorRing(0)->getGeometryType());
//                 goto CLEANUP_LOAD_SHAPEFILE;
//             }

//             // fprintf( stderr, "       .... loading %d interior rings: \n",
//             // poly->getNumInteriorRings() );
//             bool skip = true;
//             for (const OGRLinearRing* ring : poly) {
//                 if (skip) {
//                     // the zeroth entry is the exterior ring, which is treated
//                     // specially
//                     skip = false;
//                     continue;
//                 }
//                 const Polygon& block_poly = _make_polygons_from_OGRLine(*ring);
//                 target.fill(block_poly, block_value);

//                 // fputc('.', stderr);
//             }
//             // fputc( '\n', stderr);
//         }

//         OGRFeature::DestroyFeature(poly_feature);
//     }
//     // target.prune();

// CLEANUP_LOAD_SHAPEFILE:
//     GDALClose(source_dataset);
//     return true;
// }
