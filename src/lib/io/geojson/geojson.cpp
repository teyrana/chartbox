// GPL v3 (c) 2021, Daniel Williams 

// third-party libraries:
#include <nlohmann/json.hpp>

using nlohmann::json;

// this project & libraries
#include "geojson.hpp"

using chartbox::geometry::FrameMapping;
using chartbox::geometry::GlobalLocation;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::Polygon;

namespace chartbox::io::geojson {

bool load_boundary_box( const std::filesystem::path& from_path, FrameMapping& mapping ){
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

    if( not doc.contains("bbox")){
        fmt::print( stderr, "!! JSON Document does not contain a 'bbox' object?!\n");
        fmt::print( stderr, "{}\n", doc.dump(4) );
        return false;
    }
    const auto bound_box_elem = doc["bbox"];
    if( not bound_box_elem.is_array() ){
        fmt::print( stderr, "'bbox' element is not an array!?\n");
        return false;
    }else if(4 != bound_box_elem.size()) {
        fmt::print( stderr, "bbox element did not have 4 elements!?\n" );
        return false;
    }
    
    const double west_longitude = bound_box_elem[0].get<double>();
    const double south_latitude = bound_box_elem[1].get<double>();
    const double east_longitude = bound_box_elem[2].get<double>();
    const double north_latitude = bound_box_elem[3].get<double>();

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

Polygon<LocalLocation> load_polygon( const nlohmann::json& source, const FrameMapping& mapping ){
    using nlohmann::json;

    if( 4 > source.size() ){
        return {};
    }else{
        Polygon<LocalLocation> dest( source.size() );

        for( size_t point_index = 0; point_index < source.size(); ++point_index ){
            const json& point = source[point_index];
            if( ! point.is_array() ){
                return {};
            }else if( 2 != point.size()){
                return {};
            }

            const GlobalLocation each_global( point[1].get<double>(), point[0].get<double>() );

            const LocalLocation each_local = mapping.map_to_local( each_global );

            dest.set( point_index, each_local );
        }

        dest.enclose();

        return dest;
    }
}

}   // namespace 