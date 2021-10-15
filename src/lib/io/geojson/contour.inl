// GPL v3 (c) 2021, Daniel Williams 
//
// NOTE: This is the template-class implementation --
//       It is not compiled until referenced, even though it contains the
//       function implementations.

#include "layer/rolling-grid/rolling-grid-layer.hpp"

namespace chartbox::io::geojson {

// ====== Internal Function Declarations -- Contour-Specific ====== 
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
template<typename sink_t>
bool load_contour_feature_to_layer( const nlohmann::json& from_feature, const geometry::FrameMapping& mapping, uint8_t fill_value, uint8_t except_value, sink_t& to_layer ){
    using ::nlohmann::json;
    using ::chartbox::geometry::LocalLocation;
    using ::chartbox::geometry::Polygon;
    
    // fmt::print( stderr, "                >>> Loading Feature: {} polygons\n", from_feature.size() );

    const auto& chart_local_bounds = mapping.local_bounds();

    if( 0 < from_feature.size() ){
        const Polygon<LocalLocation> outer_polygon = load_polygon( from_feature[0], mapping );
        if( 0 == outer_polygon.size() ){
            return false;
        }

        if( outer_polygon.bounds().overlaps( chart_local_bounds ) ){
            // fmt::print( stderr, "                >>> feature overlaps. Filling outer border.\n" );
            to_layer.fill( outer_polygon, mapping.local_bounds(), fill_value );

            if( fill_interior_holes ){   /// note: namespace-scope variable
                for( int hole_index = 1; hole_index < from_feature.size(); ++hole_index ){
                    // fmt::print( stderr, "                    >> Hole[{:>2d}]...\n", hole_index );
                    const Polygon<LocalLocation> hole_polygon = load_polygon( from_feature[0], mapping );
                    to_layer.fill( hole_polygon, mapping.local_bounds(), except_value );
                }
            }

            return true;
        }
    }

    return false;
}

template<typename sink_t>
bool load_contour_to_layer( const nlohmann::json& from_document, const geometry::FrameMapping& mapping, sink_t& to_layer ){
    using ::nlohmann::json;
    using ::chartbox::layer::block_cell_value;
    using ::chartbox::layer::clear_cell_value;
    using ::chartbox::layer::unknown_cell_value;

    const bool has_features = from_document.contains("features");
    if( not has_features ){
        fmt::print( stderr, "!! Could not find GeoJSON polygon features ?!?!" );
        fmt::print( stderr, "{}\n", from_document.dump(4) );
        return false;
    }

    const json& features = from_document["features"];

    to_layer.fill( unknown_cell_value );

    for( size_t feature_index = 0; feature_index < features.size(); ++feature_index ){
        const json& each_properties = features[feature_index]["properties"];
        // const int id = each_properties["OBJECTID"].get<int>();
        const bool is_inside = (1==each_properties["INSIDE"].get<int>());
        
        const json& each_geometry = features[feature_index]["geometry"];
        const json& polygon_list = each_geometry["coordinates"][0];
            
        if( is_inside ){
            load_contour_feature_to_layer( polygon_list, mapping, chartbox::layer::block_cell_value, chartbox::layer::clear_cell_value, to_layer );
        }else{
            // fmt::print( stderr, "            [{: >3d}]: outside >> fill clear. \n", id );
            load_contour_feature_to_layer( polygon_list, mapping, chartbox::layer::clear_cell_value, chartbox::layer::block_cell_value, to_layer );
        }
    }

    fmt::print( stderr, "                << finished processing features.\n");
    return true;
}

template<typename buffer_t>
bool process_contour_to_cache( const std::filesystem::path& from_path, const geometry::FrameMapping& mapping, buffer_t& window ) {
    using ::nlohmann::json;
    using ::chartbox::geometry::BoundBox;
    using ::chartbox::geometry::LocalLocation;

    if( not std::filesystem::exists(from_path) ) {
        fmt::print( stderr, "!! Could not find input path: '{}' !!\n", from_path.string() );
        return false;
    }

    std::ifstream source_stream( from_path );
    if( source_stream.fail() ){
        fmt::print( "!!! Could not open json file?!: {}\n", from_path.string() );
        return false;
    }

    const json from_document = json::parse( source_stream,
                                            nullptr,  // callback
                                            false,    // allow exceptions? no.
                                            true );   // ignore comments? yes.
    if( from_document.is_discarded() ){
        fmt::print( "!!! Could not parse input file:discard: {}\n", from_path.string() );
        return false;
    }

    const LocalLocation layer_origin = mapping.local_bounds().min;
    const double width = mapping.local_bounds().width();
    const double meters_across_sector = window.meters_across_sector;
    // this is how many sectors to use for the loading window
    const uint32_t sectors_across_window = window.sectors_across();
    // this is how many sectors (aka tiles) across for the area we plan to load...
    const uint32_t sectors_across_layer = width / meters_across_sector;
    // this is how many times we need to repeat the loading process to cover the entire loading area
    const uint32_t windows_across_layer = std::ceil( static_cast<double>(sectors_across_layer)/sectors_across_window );

    fmt::print( "        ============ cache parameters: ============ \n" );
    //fmt::print( "        :: cells_across_sector:     {}\n", cells_across_sector );
    fmt::print( "        :: origin (layer):          {},{}\n", layer_origin.easting, layer_origin.northing );
    fmt::print( "        :: width:                   {}\n", width );
    fmt::print( "        :: meters_across_sector:    {}\n", meters_across_sector );
    fmt::print( "        :: sectors_across_window:   {}\n", sectors_across_window );
    fmt::print( "        :: sectors_across_layer:    {}\n", sectors_across_layer );
    fmt::print( "        :: windows_across_layer:    {}\n", windows_across_layer );

    fmt::print( "        ============ Load @ Window: [row,col] ============ \n" );
    // - slide the window across the layer, incrementally;  
    //     a. at each position load the input file
    //     b. then save to cache tiles
    // - repeat until the entire desired area is covered.
    for (size_t window_row_index = 0; window_row_index < windows_across_layer; ++window_row_index) {
        for (size_t window_column_index = 0; window_column_index < windows_across_layer; ++window_column_index ) {
            window.track( layer_origin + LocalLocation( window_column_index*meters_across_sector, window_row_index*meters_across_sector ) );
            fmt::print("            >>> {{ row: {}, column: {} }}\n", window_column_index, window_row_index );

            // populates the load window with contents matching some of the input file
            load_contour_to_layer( from_document, mapping, window );

            // flushes load-window tiles to cache
            window.flush();
        }
        fmt::print("\n");
    }

    return false; // debug

    // return true;
}

}   // namespace