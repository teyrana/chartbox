// GPL v3 (c) 2021, Daniel Williams 
//
// NOTE: This is not an independent compilation unit! 
//       It is a template-class implementation, and should only be included from its header.

// #include <cmath>
// #include <cstddef>
#include <limits>
// #include <memory>
// #include <unistd.h>

#include <fmt/core.h>

#include <gdal.h>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#include <ogr_geometry.h>

using chartbox::io::ShapefileLoader;


// // could not get this variant to work, even though it would be much more efficient...
// template<typename layer_t>
// bool ShapefileLoader<layer_t>::load_boundary_box( const OGRSpatialReference * const spatial ){
//
//     fmt::print( stderr, "        ## Using reference: {} ...\n", spatial->GetName() );
//     fmt::print( stderr, "                     empty: {} ...\n", spatial->IsEmpty() );
//     fmt::print( stderr, "                     empty: {} ...\n", spatial->IsEmpty() );
//
//     const_cast<OGRSpatialReference*>(spatial)->dumpReadable();
//
//     fmt::print( stderr, "        ## Fetching Shapefile 'AreaOfUse'...\n");
//     spatial->GetAreaOfUse( &west_longitude, &south_latitude, &east_longitude, &north_latitude, nullptr );
//
//     ...
//
// }


template<typename layer_t>
bool ShapefileLoader<layer_t>::load_boundary_box( const OGRLinearRing& ring ){

    fmt::print( stderr, "        ## Using reference: {} ...\n", ring.getGeometryName() );
    fmt::print( stderr, "               point-count: {} ...\n", ring.getNumPoints() );

    double min_x = std::numeric_limits<double>::max();
    double min_y = std::numeric_limits<double>::max();
    double max_x = -std::numeric_limits<double>::max();
    double max_y = -std::numeric_limits<double>::max();

    fmt::print( stderr, "        >> Finding ring bounds...\n");

    // for( auto&& p : ring.getPoints() ){
    for( auto iter = ring.begin(); iter != ring.end(); ++iter){
        min_x = std::min( max_x, (*iter).getX() );
        min_y = std::min( max_y, (*iter).getY() );
        max_x = std::max( min_x, (*iter).getX() );
        max_y = std::max( min_y, (*iter).getY() );
    }

    fmt::print( stderr, "        << Shapefile Bounds:\n");
    fmt::print( stderr, "           x: {:+#12.4f} - {:+#12.4f} \n", min_x, max_x );
    fmt::print( stderr, "           y: {:+#12.4f} - {:+#12.4f} \n", min_y, max_y );

    fmt::print( stderr, "-----------------------------------------------------\n");
    fmt::print( stderr, "        ??? Are UTM values!! (19F)\n");
    fmt::print( stderr, "        ??? Are these some local frame? \n");
    
    fmt::print( stderr, "XX Shapefile loading is currently broken. \n");

    // return mapping_.move_utm_bounds(  Eigen::Vector2d( west_longitude, south_latitude),
    //                                     Eigen::Vector2d( east_longitude, north_latitude) );

    // devel / debug
    return false;
}


template <typename layer_t>
bool ShapefileLoader<layer_t>::load( const std::string& filename ) {


    const GDALDatasetH file_dataset = GDALOpenEx( filename.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
    if (file_dataset == NULL) {
        fmt::print( stderr, "    <<XX Open failed.  No source dataset available!?\n");

        struct stat buf;
        if (stat(filename.c_str(), &buf) != -1) {
            fmt::print( stderr, "    >> Found file: '{}' >>\n", filename);
        } else {
            fmt::print( stderr, "    !! Missing data file: '{}' !!\n", filename);
        }
        return false;

    }
    GDALDataset* source_dataset = reinterpret_cast<GDALDataset*>(file_dataset);
 
    // DEBUG
    fmt::print( stderr, "    >>> Loaded file:      {}    into memory.\n", filename );


    fmt::print( stderr, "        ::Using reference: {} ...\n", reinterpret_cast<void*>(const_cast<OGRSpatialReference*>(source_dataset->GetSpatialRef())) );

    layer_.fill(layer_.block_value);
    bool success = false;
    for( auto * each_layer: source_dataset->GetLayers() ){
        // GNMGenericLayer * each_gnm_layer = (GNMGenericLayer*)each_layer;
        fmt::print( stderr, "        >>> Layer Name:  {};  Geometry:    {}\n", each_layer->GetName(), each_layer->GetGeomType() );

        OGRFeature* each_feature = nullptr;
        while( nullptr != (each_feature = each_layer->GetNextFeature()) ){

            OGRGeometry* geom = each_feature->GetGeometryRef();
            if ( geom and (wkbPolygon == wkbFlatten(geom->getGeometryType())) ){
            
                // Reference: OGRPolygon
                // https://gdal.org/api/ogrgeometry_cpp.html#ogrpolygon-class
                OGRPolygon* poly = geom->toPolygon();
                if( poly ){
                    const OGRLinearRing * const exterior = poly->getExteriorRing();
                    if( exterior && load_boundary_box(*exterior) ){
                        fmt::print( stderr, "        >>> loading exterior ring: {}  with: {} points.\n", exterior->getGeometryName(), exterior->getNumPoints() );


                        // DEBUG
                        // target.fill(exterior, layer_.clear_value););

                        // if we have any interior rings, load them:
                        if (0 < poly->getNumInteriorRings()) {
                            fmt::print( stderr,  "        >>> polygon contains {} interior rings.\n", poly->getNumInteriorRings() );
                        //         fmt::print( stderr,  "        (instead found : %s (%d) )\n",
                        //                 poly->getInteriorRing(0)->getGeometryName(),
                        //                 poly->getInteriorRing(0)->getGeometryType());
                        
                        //     if (wkbLineString != poly->getInteriorRing(0)->getGeometryType()) {
                        //         fmt:print( stderr,  "    !! polygon does not contain wkbLineString/LINEARRING geometries!\n");
                        //         fmt:print( stderr,  "        (instead found : %s (%d) )\n",
                        //                 poly->getInteriorRing(0)->getGeometryName(),
                        //                 poly->getInteriorRing(0)->getGeometryType());
                        //         GDALClose(source_dataset);
                        //         return false;

                        }

                        //     fprintf( stderr, "       .... loading %d interior rings: \n",
                        //     // poly->getNumInteriorRings() );
                        //     bool skip = true;
                        //     for (const OGRLinearRing* ring : poly) {
                        //         if (skip) {
                        //             // the zeroth entry is the exterior ring, which is treated
                        //             // specially
                        //             skip = false;
                        //             continue;
                        //         }
                        //         const Polygon& block_poly = _make_polygons_from_OGRLine(*ring);
                        //         target.fill(block_poly, layer_.block_value);

                        //         // fputc('.', stderr);
                        //     }
                        //     // fputc( '\n', stderr);
                        // }
                        
                        success = true;
                    }else{
                        fmt::print( stderr, "        <<XX Could not convert to OGRLinearRing! \n" );
                    }
                }else{
                    fmt::print( stderr, "        <<XX Could not convert to polygon! \n" );
                }  
            }else{
                fmt::print( stderr, "        <<XX geometry is not a 'wkbPolygon':?: {} / {}\n", geom->getGeometryName(), geom->getGeometryType() );
            }

            if( each_feature){
                OGRFeature::DestroyFeature(each_feature);
            }
        }
    }

    GDALClose(source_dataset);
    return success;
}


