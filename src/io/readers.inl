// GPL v3 (c) 2020

// NOTE: This is the template-class implementation -- 
//       It is not compiled until referenced, even though it contains the function implementations.

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using std::cerr;
using std::endl;
using std::string;

#include <Eigen/Geometry>

#include <nlohmann/json.hpp>

#include "geometry/polygon.hpp"

#include "json.hpp"

using Eigen::Vector2d;

using yggdrasil::geometry::Polygon;
using yggdrasil::node::TileNode;

template<typename target_t, typename cell_t>
bool yggdrasil::io::load_grid_from_json(target_t& target, nlohmann::json grid ){
    // const Layout& layout = target.get_layout();

    if(!grid.is_array() && !grid[0].is_array()){
        cerr << "yggdrasil::io::load_grid expected a array-of-arrays! aborting!\n";
        return false;
    }

    if( grid.size() != target.dimension ){
        cerr << "yggdrasil::io::load_grid expected a array of the same dimension as configured!!\n";
        cerr << "    expected: " << target.dimension << endl;
        cerr << "    found:    " << grid.size() << " x " << grid[0].size() << endl;
        return false;
    }

    // populate the tree
    size_t row_index = target.dimension - 1;
    for(auto& row : grid){
        size_t column_index = 0;

        // i.e. a cell is the element at [column_index, row_index] <=> [x,y]
        for(auto& cell : row){
            target.get_cell( row_index, column_index) = cell.get<cell_t>();
            ++column_index;
        }
        --row_index;
    }

    // target.prune();

    return true;
}

template<typename target_t, typename cell_t>
bool yggdrasil::io::load_areas_from_json(target_t& target, nlohmann::json allow_doc, nlohmann::json block_doc){

    const cell_t allow_value = 0;
    const cell_t block_value = 0x99;

    target.fill(block_value);

    auto allowed_polygons = make_polygons_from_json(allow_doc);
    for( auto& poly : allowed_polygons ){
        target.fill(poly, allow_value);
    }

    auto blocked_polygons = make_polygons_from_json(block_doc);
    for( auto& poly : blocked_polygons ){
        target.fill(poly, block_value);
    }

    target.prune();

    return true;
}

inline std::vector<Polygon> yggdrasil::io::make_polygons_from_json( nlohmann::json doc){
    std::vector<Polygon> result(static_cast<size_t>(doc.size()));
    if(0 < result.size()){
        for( size_t polygon_index = 0; polygon_index < doc.size(); ++polygon_index ){
            auto& poly_doc = doc[polygon_index];
            result[polygon_index] = Polygon(poly_doc);
        }
    }
    return result;
}

#ifdef ENABLE_GDAL
inline Polygon yggdrasil::io::_make_polygons_from_OGRLine( const OGRLinearRing& source ){
    const size_t point_count = static_cast<size_t>(source.getNumPoints());
    
    if(0 < point_count ){
        Polygon result(point_count);
        
        OGRPoint scratch;
        for( size_t point_index=0; point_index < point_count; ++point_index ){
            source.getPoint(point_index, &scratch);
            result[point_index] = {scratch.getX(), scratch.getY()};
        }

        result.complete();

        return result;
    }

    return {};
}

template<typename target_t, typename cell_t>
bool yggdrasil::io::load_from_shape_file(target_t& target, const string& filepath){
    const cell_t allow_value = 0;
    const cell_t block_value = 0x99;

    auto * source_dataset = (GDALDataset*) GDALOpenEx( filepath.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL );
    if( source_dataset == NULL ){
        cerr << "!> Open failed. " << filepath << " into memory...\n";
        return false;
    }
    // // DEBUG
    // cerr << "#> Loaded file: " << filepath << " into memory...\n";
    // // DEBUG
    // for( auto * each_layer: source_dataset->GetLayers() ){
    //     GNMGenericLayer * each_gnm_layer = (GNMGenericLayer*)each_layer;
    //     cerr << "___?> Layer.Name: " << each_gnm_layer->GetName() << endl;
    // }

    OGRLayer* shape_layer = source_dataset->GetLayer( 0 );
    // ... ->GetLayerByName( "navigation_area_100k" );
    if( nullptr == shape_layer ){
        fprintf( stderr, "    !! dataset doesn't contain any layer? ");
        goto CLEANUP_LOAD_SHAPEFILE;
    }

    shape_layer->ResetReading();
    OGRFeature *poly_feature;
    while( (poly_feature = shape_layer->GetNextFeature()) != NULL ){
        
        OGRGeometry * geom = poly_feature->GetGeometryRef();
        if( geom == NULL ){
            fprintf( stderr, "    !! layer -> feature: could not load geometry! \n");
            goto CLEANUP_LOAD_SHAPEFILE;
        }

        if( wkbPolygon != wkbFlatten(geom->getGeometryType()) ){
            fprintf( stderr, "    <! geometry is not a polygon! aborting!\n" );
            goto CLEANUP_LOAD_SHAPEFILE;
        }

        // Reference: OGRPolygon
        // https://gdal.org/api/ogrgeometry_cpp.html#ogrpolygon-class
        OGRPolygon* poly = geom->toPolygon();
        const Polygon& exterior = _make_polygons_from_OGRLine( * poly->getExteriorRing());

        // target.reset( exterior.make_layout(16.) );
        // cerr << "    .... loaded layout"  << t.get_layout().to_string() << endl;

        target.fill( block_value );

        //fprintf( stderr, "    .... loading exterior ring with %zu points.\n", exterior.size() );
        target.fill( exterior, allow_value );

        // if we have any interior rings, load them:
        if( 0 < poly->getNumInteriorRings()){
            if( wkbLineString != poly->getInteriorRing(0)->getGeometryType() ){
                fprintf( stderr, "    !! polygon does not contain wkbLineString/LINEARRING geometries!\n");
                fprintf( stderr, "        (instead found : %s (%d) )\n", poly->getInteriorRing(0)->getGeometryName(), poly->getInteriorRing(0)->getGeometryType() );
                goto CLEANUP_LOAD_SHAPEFILE;
            }

            // fprintf( stderr, "       .... loading %d interior rings: \n", poly->getNumInteriorRings() );
            bool skip = true;
            for( const OGRLinearRing* ring : poly ){
                if(skip){
                    // the zeroth entry is the exterior ring, which is treated specially
                    skip = false;
                    continue;
                }
                const Polygon& block_poly = _make_polygons_from_OGRLine( *ring );
                target.fill(block_poly, block_value);

                // fputc('.', stderr);
            }
            // fputc( '\n', stderr);
        }

        OGRFeature::DestroyFeature(poly_feature);
    }
    target.prune();

    CLEANUP_LOAD_SHAPEFILE:
        GDALClose( source_dataset );
        return true;
}
#endif //#ifdef ENABLE_GDAL

