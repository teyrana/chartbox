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
    const double meters_across_window = window.meters_across_view(); 
    const double meters_across_layer = mapping.local_bounds().width();

    {
        fmt::print( "    ====== ====== Cache Parameters: ============ \n" );
        // this is how many sectors (aka tiles) across for the total-load-area
        const uint32_t sectors_across_layer = mapping.local_bounds().width() /  window.meters_across_sector();
        // this is how many sectors (aka tiles) across each load-area-increment
        const uint32_t sectors_across_window = window.sectors_across_view();
        // this is how many times we need to repeat the loading process to cover the entire loading area
        const uint32_t windows_across_layer = std::ceil( static_cast<double>(sectors_across_layer)/sectors_across_window );
        fmt::print( "        :: origin (layer):          {},{}\n", layer_origin.easting, layer_origin.northing );
        fmt::print( "        :: meters-across-cell:      {}\n", window.meters_across_cell() );
        fmt::print( "        :: meters-across-sector:    {}\n", window.meters_across_sector() );
        fmt::print( "        :: meters-across-window:    {}\n", meters_across_window );
        fmt::print( "        :: meters-across-layer:     {}\n", meters_across_layer );
        fmt::print( "        :: cells_across_sector:     {}\n", window.cells_across_sector() );
        fmt::print( "        :: sectors-across-window:   {}\n", sectors_across_window );
        fmt::print( "        :: sectors-across-layer:    {}\n", sectors_across_layer );
        fmt::print( "        :: windows-across-layer:    {}\n", windows_across_layer );

        fmt::print( "    ====== ====== Cache Windows: ============ \n" );
    }

    // - slide the window across the layer, incrementally;  
    //     a. at each position load the input file
    //     b. then save to cache tiles
    // - repeat until the entire desired area is covered.
    LocalLocation window_origin( layer_origin );
    for( window_origin.easting = layer_origin.easting; window_origin.northing < meters_across_layer; window_origin.northing += meters_across_window ){
        for( window_origin.easting = layer_origin.easting; window_origin.easting < meters_across_layer; window_origin.easting += meters_across_window ){

            // move to the next window
            fmt::print("{:<{}}@[ {:6.0f}East,  {:6.0f}North ]    =>", "", 8u, window_origin.easting, window_origin.northing );
            window.track( window_origin );

            fmt::print("  load-to-window    =>");
            // populates the load window with contents matching some of the input file
            load_contour_to_layer( from_document, mapping, window );

            fmt::print("  flush-to-cache.\n");
            // flushes load-window tiles to cache
            window.flush_to_cache();
        }
    }
    return true;
}

}   // namespace