// GPL v3 (c) 2020, Daniel Williams 

// NOTE: This is the template-class implementation --
//       It is not compiled until referenced, even though it contains the
//       function implementations.

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>
#include <vector>


// third-party includes
#include <Eigen/Geometry>
#include <nlohmann/json.hpp>


#ifdef ENABLE_GDAL
#include <gdal/gdal.h>
#include <gdal/gdal_priv.h>
#endif

#ifdef ENABLE_PDAL
#include <pdal/pdal.h>
#endif


using Eigen::Vector2d;

using chart::geometry::Polygon;

template<typename layer_t>
ChartLoader<layer_t>::ChartLoader( layer_t& _dest )
    : destination_(_dest)
{}



template<typename layer_t>
bool ChartLoader<layer_t>::load_file( std::string& filename ){
    if( access( fname, R_OK ) == 0 ) {
        // file exists
        std::ifstream source_stream(filename);
        const auto doc = json::parse(source_stream);
        return load_json( doc );
    } else {
        // file missing
        std::cerr << "!! Could not find input file !!: " << filename << std::endl;
        return false;
    }
}

template<typename layer_t>
bool ChartLoader<layer_t>::load_text( std::string& source ){
    // TODO: there exists some other function call for this...
    const json doc = nlohmann::json::parse( text,    // source document
                                            nullptr, // callback argument
                                            false);  // allow exceptions?
    return load_json( doc );
}

template<typename layer_t>
bool ChartLoader<layer_t>::load_json( const nlohmann::json& doc ){
    if (doc.is_discarded()) {
        // cerr << "malformed json! ignore.\n" << endl;
        // cerr << text << endl;
        return false;
    } else if (!doc.is_object()) {
        // cerr << "input should be a json _object_!!\n" + doc.dump(4) << endl;
        return false;
    }

    if (doc.contains(bounds_key)) {
        const Bounds new_bounds = Bounds::make_from_json(doc[bounds_key]);
        if( new_bounds.is_valid()){
            destination_.set_bounds(new_bounds);
            // continue
        }else{
            return true;
        }
    }

    if ( doc.contains(grid_key) ){
        const auto& grid = doc[grid_key];
        if (!grid.is_array()){
            return false;
        }

        if( destination_.size() != grid.size() ){
            cerr << " expected a arrays!  aborting!\n";
            return false;
        }

        std::vector<cell_t> buffer;
        for (auto& cell : grid  ) {
            buffer.push_back(cell.get<cell_t>());
        }
        auto flipped = chart::geometry::vflip(buffer, destination_.dimension() );

        return destination_.fill(flipped);

    }else if( doc.contains(allow_key) && doc[allow_key].is_array() ){
        for( auto& list : doc[allow_key] ){
            if (!list.is_array()){
                cerr << "!? malformed json input... expected allow polygons to be lists" << endl;
                return false;
            }
            const Polygon& allow_shape = geometry::Polygon(list);
            destination_.fill(allow_shape, allow_value);
        }

        if( doc.contains(block_key) && doc[block_key].is_array() ){
            for( auto& list : doc[block_key] ){
                if (!list.is_array()){
                    cerr << "!? malformed json input... expected allow polygons to be lists" << endl;
                    return false;
                }
                const Polygon& block_shape = geometry::Polygon(list);
                destination_.fill(block_shape, block_value);
            }
        }
        return true;
    }

    return true;

}


// template <typename target_t, typename cell_t>
// void chart::io::fill_from_polygon(target_t& t, const Polygon& poly,
//                                       const cell_t fill_value) {
//     // adapted from:
//     //  Public-domain code by Darel Rex Finley, 2007:  "Efficient Polygon Fill
//     //  Algorithm With C Code Sample" Retrieved:
//     //  (https://alienryderflex.com/polygon_fill/); 2019-09-07

//     const double scale_2 = t.scale / 2;

//     const Bounds bounds = t.get_bounds();

//     // Loop through the rows of the image.
//     for (double y = bounds.min().y() + scale_2; y < bounds.max().y();
//          y += t.scale) {
//         // generate a list of line-segment crossings from the polygon
//         std::vector<double> crossings;
//         for (int i = 0; i < poly.size() - 1; ++i) {
//             const Vector2d& p1 = poly[i];
//             const Vector2d& p2 = poly[i + 1];

//             const double y_max = std::max(p1[1], p2[1]);
//             const double y_min = std::min(p1[1], p2[1]);
//             // if y is in range:
//             if ((y_min <= y) && (y < y_max)) {
//                 // construct x-coordinate that crosses this line:
//                 auto value =
//                     p1[0] + (y - p1[1]) * (p2[0] - p1[0]) / (p2[1] - p1[1]);
//                 crossings.emplace_back(value);
//             }
//         }

//         // early exit
//         if (0 == crossings.size()) {
//             continue;
//         }

//         // Sort the crossings:
//         std::sort(crossings.begin(), crossings.end());

//         //  Fill the pixels between node pairs.
//         for (int crossing_index = 0; crossing_index < crossings.size();
//              crossing_index += 2) {
//             const double start_x =
//                 bounds.constrain_x(crossings[crossing_index] + scale_2);
//             const double end_x =
//                 bounds.constrain_x(crossings[crossing_index + 1] + scale_2);
//             for (double x = start_x; x < end_x; x += t.scale) {
//                 t.store({x, y}, fill_value);
//             }
//         }
//     }
// }

// template <typename target_t, typename cell_t>
// bool chart::io::load_areas_from_json(target_t& target,
//                                          nlohmann::json allow_doc,
//                                          nlohmann::json block_doc) {

//     const cell_t allow_value = 0;
//     const cell_t block_value = 0x99;

//     target.fill(block_value);

//     auto allowed_polygons = make_polygons_from_json(allow_doc);
//     for (auto& poly : allowed_polygons) {
//         target.fill(poly, allow_value);
//     }

//     auto blocked_polygons = make_polygons_from_json(block_doc);
//     for (auto& poly : blocked_polygons) {
//         target.fill(poly, block_value);
//     }

//     target.prune();

//     return true;
// }

// inline std::vector<Polygon>
// chart::io::make_polygons_from_json(nlohmann::json doc) {
//     std::vector<Polygon> result(static_cast<size_t>(doc.size()));
//     if (0 < result.size()) {
//         for (size_t polygon_index = 0; polygon_index < doc.size();
//              ++polygon_index) {
//             auto& poly_doc = doc[polygon_index];
//             result[polygon_index] = Polygon(poly_doc);
//         }
//     }
//     return result;
// }

#ifdef ENABLE_GDAL
inline Polygon
chart::io::_make_polygons_from_OGRLine(const OGRLinearRing& source) {
    const size_t point_count = static_cast<size_t>(source.getNumPoints());

    if (0 < point_count) {
        Polygon result(point_count);

        OGRPoint scratch;
        for (size_t point_index = 0; point_index < point_count; ++point_index) {
            source.getPoint(point_index, &scratch);
            result[point_index] = {scratch.getX(), scratch.getY()};
        }

        result.complete();

        return result;
    }

    return {};
}

#include <sys/stat.h>

template <typename target_t, typename cell_t>
bool chart::io::load_from_shape_file(target_t& target,
                                         const string& filepath) {
    const cell_t allow_value = 0;
    const cell_t block_value = 0x99;

    // might be a duplicate call, but duplicate calls don't seem to cause any
    // problems.
    GDALAllRegister();

    auto* source_dataset = (GDALDataset*)GDALOpenEx(
        filepath.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
    if (source_dataset == NULL) {
        cerr << "!> Open failed.  No source dataset available?\n";

        struct stat buf;
        if (stat(filepath.c_str(), &buf) != -1) {
            cerr << "    >> Found file: '" << filepath << "' >>\n";
        } else {
            cerr << "    !! Missing data file: '" << filepath << "' !!\n";
        }

        return false;
    }

    // DEBUG
    // cerr << "#> Loaded file: '" << filepath << "' into memory...\n";

    // DEBUG
    // for( auto * each_layer: source_dataset->GetLayers() ){
    //     GNMGenericLayer * each_gnm_layer = (GNMGenericLayer*)each_layer;
    //     cerr << "___?> Layer.Name: " << each_gnm_layer->GetName() << endl;
    // }

    OGRLayer* shape_layer = source_dataset->GetLayer(0);
    // ... ->GetLayerByName( "navigation_area_100k" );
    if (nullptr == shape_layer) {
        fprintf(stderr, "    !! dataset doesn't contain any layer? ");
        goto CLEANUP_LOAD_SHAPEFILE;
    }

    shape_layer->ResetReading();
    OGRFeature* poly_feature;
    while ((poly_feature = shape_layer->GetNextFeature()) != NULL) {

        OGRGeometry* geom = poly_feature->GetGeometryRef();
        if (geom == NULL) {
            fprintf(stderr,
                    "    !! layer -> feature: could not load geometry! \n");
            goto CLEANUP_LOAD_SHAPEFILE;
        }

        if (wkbPolygon != wkbFlatten(geom->getGeometryType())) {
            fprintf(stderr, "    <! geometry is not a polygon! aborting!\n");
            goto CLEANUP_LOAD_SHAPEFILE;
        }

        // Reference: OGRPolygon
        // https://gdal.org/api/ogrgeometry_cpp.html#ogrpolygon-class
        OGRPolygon* poly = geom->toPolygon();
        const Polygon& exterior =
            _make_polygons_from_OGRLine(*poly->getExteriorRing());

        target.fill(block_value);

        // fprintf( stderr, "    .... loading exterior ring with %zu points.\n",
        // exterior.size() );
        target.fill(exterior, allow_value);

        // if we have any interior rings, load them:
        if (0 < poly->getNumInteriorRings()) {
            if (wkbLineString != poly->getInteriorRing(0)->getGeometryType()) {
                fprintf(stderr, "    !! polygon does not contain "
                                "wkbLineString/LINEARRING geometries!\n");
                fprintf(stderr, "        (instead found : %s (%d) )\n",
                        poly->getInteriorRing(0)->getGeometryName(),
                        poly->getInteriorRing(0)->getGeometryType());
                goto CLEANUP_LOAD_SHAPEFILE;
            }

            // fprintf( stderr, "       .... loading %d interior rings: \n",
            // poly->getNumInteriorRings() );
            bool skip = true;
            for (const OGRLinearRing* ring : poly) {
                if (skip) {
                    // the zeroth entry is the exterior ring, which is treated
                    // specially
                    skip = false;
                    continue;
                }
                const Polygon& block_poly = _make_polygons_from_OGRLine(*ring);
                target.fill(block_poly, block_value);

                // fputc('.', stderr);
            }
            // fputc( '\n', stderr);
        }

        OGRFeature::DestroyFeature(poly_feature);
    }
    // target.prune();

CLEANUP_LOAD_SHAPEFILE:
    GDALClose(source_dataset);
    return true;
}
#endif //#ifdef ENABLE_GDAL


template<typename T>
std::vector<T> vflip(const std::vector<T>& source, const size_t row_width) {
    const size_t row_count = source.size() / row_width;

    std::vector<T> sink(source.size());

    for (size_t read_row = 0; read_row < row_count; ++read_row) {
        size_t write_row = row_count - read_row-1;

        for (size_t column = 0; column < row_width; ++column) {
            sink[write_row*row_width + column] = source[read_row*row_width + column];
        }
    }

    return sink;
}
