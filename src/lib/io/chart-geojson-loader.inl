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
GeoJSONLoader<layer_t>::GeoJSONLoader( FrameMapping& _mapping, layer_t& _destination_layer )
    : layer_(_destination_layer)
    , mapping_(_mapping)
{}

template<typename layer_t>
bool GeoJSONLoader<layer_t>::load_file( const std::string& filename ){
    if( access( filename.c_str(), R_OK ) == 0 ) {
        // file exists
        std::ifstream source_stream(filename);

        CPLJSONDocument doc;
        if( doc.Load( filename )){
            return load_json( doc.GetRoot() );
        }

        std::cerr << "?!?! Unknown failure while loading GeoJSON text into GDAL...\n"; 
        return false;
    } else {
        // file missing
        std::cerr << "!! Could not find input file !!: " << filename << std::endl;
        return false;
    }
}

template<typename layer_t>
bool GeoJSONLoader<layer_t>::load_text( const std::string& source_text ){
    CPLJSONDocument doc;
    if( doc.LoadMemory( source_text )){
        return load_json( doc.GetRoot() );
    }
    std::cerr << "?!?! Unknown failure while loading GeoJSON text into GDAL...\n"; 
    return false;    
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
    auto bound_box_elem = root.GetArray("bbox");
    if( ! bound_box_elem.IsValid()){
        std::cerr << "'bbox' element is not valid!?" << std::endl;
        return false;
    }else if(4 != bound_box_elem.Size()) {
        std::cerr << "bbox element did not have 4 elements!?" << std::endl;
        return false;
    }

    const double lon_min = bound_box_elem[0].ToDouble(NAN);
    const double lat_min = bound_box_elem[1].ToDouble(NAN);
    const double lon_max = bound_box_elem[2].ToDouble(NAN);
    const double lat_max = bound_box_elem[3].ToDouble(NAN);

    using std::isnan;
    if( isnan(lon_min) || isnan(lat_min) || isnan(lon_max) || isnan(lat_max) ){
        std::cerr << "found NAN for a bounding-box limit!?" << std::endl;
        return false;
    }

    const Eigen::Vector2d bounds_min_lat_lon( lon_min, lat_min );
    const Eigen::Vector2d bounds_max_lat_lon( lon_max, lat_max );
    const bool move_success = mapping_.move_local_bounds( bounds_min_lat_lon, bounds_max_lat_lon );

    if( move_success && fill ){
        const Eigen::Vector2d bounds_min_local = mapping_.to_local( bounds_min_lat_lon );
        const Eigen::Vector2d bounds_max_local = mapping_.to_local( bounds_max_lat_lon );
        const Eigen::AlignedBox2d bounds_local( bounds_min_local, bounds_max_local );
        return layer_.fill( bounds_local, layer_.clear_value );
    }
    return false;
}

template<typename layer_t>
bool GeoJSONLoader<layer_t>::load_json_boundary_polygon( const CPLJSONObject& root ){
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
            OGRPolygon* local_frame_polygon = new OGRPolygon();
            const OGRLinearRing * from_ring = world_frame_polygon->getExteriorRing();
            OGRLinearRing * to_ring = new OGRLinearRing();

            for ( auto iter = from_ring->begin(); iter != from_ring->end(); ++iter ) {
                const double latitude = (*iter).getY();
                const double longitude = (*iter).getX();

                const OGRPoint * to_coord = mapping_.to_utm( longitude, latitude );

                to_ring->addPoint( to_coord );
            }

            to_ring->closeRings();
            local_frame_polygon->addRing( to_ring );

            return layer_.fill( std::move(std::unique_ptr<OGRPolygon>(local_frame_polygon)), layer_.clear_value );
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
