// GPL v3 (c) 2021, Daniel Williams 

// standard libraries
#include <filesystem>
#include <fstream> 

// third-party libraries: nlohmann/json
#include <nlohmann/json.hpp>

// this project & libraries
#include "geojson.hpp"

using chartbox::geometry::FrameMapping;
using chartbox::geometry::GlobalLocation;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::Polygon;
using chartbox::geometry::UTMLocation;

using nlohmann::json;

namespace chartbox::io::geojson {

// ====== Internal Function Declarations ====== 

template<typename layer_t>
bool load_boundary_layer( const std::filesystem::path& from_path, const FrameMapping& mapping, layer_t& to_layer ){
    if( not std::filesystem::exists(from_path) ) {
        fmt::print( stderr, "!! Could not find input path !!: {}\n", from_path.string() );
        return false;
    }

    std::ifstream source_stream( from_path );
    if( source_stream.fail() ){
        fmt::print( "!!! Could not open json file?!: {}\n", from_path.string() );
        return false;
    }

    const json doc = json::parse( source_stream,
                                    nullptr,  // callback
                                    false,    // allow exceptions? no.
                                    true );   // ignore comments? yes.
    if( doc.is_discarded() ){
        fmt::print( "!!! Could not parse input file:discard: {}\n", from_path.string() );
        return false;
    }

    const bool has_polygon = ( doc.contains("features")
                            && doc["features"].size() > 0
                            && doc["features"][0].contains("geometry")
                            && doc["features"][0]["geometry"].contains("type")
                            && doc["features"][0]["geometry"]["type"].get<std::string>() == "Polygon" );

    if( has_polygon ){
        json geom = doc["features"][0]["geometry"];
        if( !geom.is_object() || (0 == geom.size()) ){
            fmt::print( stderr, "!! Could not load geom from feature: !!!\n{}\n", geom.dump(4) );
            return false;
        }

        json points = geom["coordinates"][0];
        if( 0 == points.size() ){
            fmt::print( stderr, "!! Could not load ponits from geom: !!!\n{}\n", geom.dump(4) );
            return false;
        }

        const auto polygon = load_polygon( points, mapping );

        to_layer.fill( polygon, mapping.local_bounds(), chartbox::layer::clear_cell_value );

    }else{
        to_layer.fill( mapping.local_bounds(), chartbox::layer::clear_cell_value );
    }

    return true;
}

}   // namespace