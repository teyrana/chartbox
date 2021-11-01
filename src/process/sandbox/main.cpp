// GPL v3 (c) 2021, Daniel Williams

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <random>

// may not be standard
#include <sys/stat.h>

#include <fmt/core.h>

#include "chart-box/chart-box.hpp"
#include "io/flatbuffer/flatbuffer.hpp"
#include "io/geojson/geojson.hpp"
#include "io/png/png.hpp"

using chartbox::layer::BOUNDARY;
using chartbox::layer::CONTOUR;
using namespace chartbox::io;

constexpr size_t test_seed = 55;
static std::mt19937 generator;


// int main(int argc, char* argv[]){
int main( void ){
    // bool enable_input_lidar = false;
    // std::string input_path_terrain;

    const std::filesystem::path cache_path = "cache/";

    // std::string boundary_input_arg("data/block-island/boundary.develop.geojson");
    // std::string boundary_input_arg("data/block-island/boundary.simple.geojson");
    std::string boundary_input_arg("data/block-island/boundary.polygon.geojson");
    // std::string boundary_input_arg("data/massachusetts/navigation_area_100k.shp");

    std::string contour_input_arg("(cache)");
                // contour_input_arg = "data/block-island/coastline.geojson";
    
    std::string boundary_output_arg("");
                // boundary_output_arg = "boundary.png";
                // boundary_output_arg = "boundary.cache.fb";

    std::string contour_output_arg("");
                // contour_output_arg = "auto";
                // contour_output_arg = "countour.png";

    bool clear_cache = false;//true;
    bool enable_cache = true;

    // std::string lidar_output_arg("");

    std::string chart_output_arg("");
                // chart_output_arg = "composite.view.png";

    // Boundary Layer
    // ==============
    if( ! std::filesystem::is_regular_file(boundary_input_arg)){
        fmt::print(stderr, "!! Could not find boundary input path!! : {}\n", boundary_input_arg);
        return EXIT_FAILURE;
    }
    std::filesystem::path boundary_input_path( boundary_input_arg );

    std::filesystem::path boundary_output_path( boundary_output_arg );


    // Contour Layer
    // ==============
    std::filesystem::path contour_input_path( contour_input_arg );

    std::filesystem::path contour_output_path( contour_output_arg );


    // Contour Layer
    // ==============
    std::filesystem::path view_output_path( chart_output_arg );


    // ^^^^ Configuration
    // vvvv Execution
    GDALAllRegister();

    chartbox::ChartBox box;

    {   // load inputs 
        auto& mapping = box.mapping();

        fmt::print( ">>> Starting Load: \n");
        const auto start_load_all = std::chrono::high_resolution_clock::now(); 

        if ( ! boundary_input_path.empty() ) {
            auto& layer = box.get_boundary_layer();
            fmt::print( ">>> Load Boundary from:     '{}'    to Layer: {}\n", boundary_input_path.string(), layer.name() );
            if( boundary_input_path.extension() == chartbox::io::geojson::extension ){
                geojson::load_boundary_box( boundary_input_path, mapping );

                layer.sectors_across_view(4);
                layer.meters_across_cell(8.0);
                layer.track( mapping.local_bounds() );

                geojson::load_boundary_layer( boundary_input_path, mapping, layer );
            }
        }

        const auto start_load_contour = std::chrono::high_resolution_clock::now(); 
        if ( enable_cache ) {
            auto& to_layer = box.get_contour_layer();

            if( clear_cache ) {
                const size_t files_deleted_count = std::filesystem::remove_all(cache_path);
                fmt::print(">>> Cleared Cache: {} files\n", files_deleted_count );
            }

            if ( ! contour_input_path.empty() ) {
                fmt::print(">>> Populating cache:        '{}'    <<== '{}'\n", cache_path.string(), contour_input_path.string() );

                // updates track-bounds and view-bounds
                to_layer.track( mapping.local_bounds() );

                if( contour_input_path.extension() == chartbox::io::geojson::extension ){
                    chartbox::layer::rolling::RollingGridLayer<to_layer.cells_across_sector()> load_window;
                    load_window.enable_cache( cache_path );
                    geojson::process_contour_to_cache( contour_input_path, mapping, load_window );
                }
            }

            fmt::print( ">>> Loading from cache:     '{}'    to layer: {}\n", cache_path.string(), to_layer.name() );
            to_layer.enable_cache( cache_path );
            to_layer.view( LocalLocation(0,0) );
            to_layer.load_from_cache();

            const auto finish_load_contour = std::chrono::high_resolution_clock::now(); 
            const auto load_duration = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(finish_load_contour - start_load_contour).count())/1000;
            if( 0.5 < load_duration ){
                fmt::print( "<< Loaded Contour Layer in:   {:5.2f} s \n\n", load_duration );
            }
        }

        const auto finish_load_all = std::chrono::high_resolution_clock::now(); 
        const auto load_duration = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(finish_load_all - start_load_all).count())/1000;
        if( 0.5 < load_duration ){
            fmt::print( "<< Loaded all layers in:   {:5.2f} s \n\n", load_duration );
        }
    }

    {   // DEBUG
        fmt::print(">>> Perform one-time modifications:\n");

        auto& layer = box.get_contour_layer();
        layer.view(LocalLocation( 1024, 5120));
        fmt::print("    >>> Move contour-layer-view:    {}, {}\n", layer.visible().min.easting, layer.visible().min.northing );
        //layer.view(LocalLocation( 5120, 5120));
        layer.load_from_cache();

    }   // DEBUG

    {   // DEBUG
        using namespace chartbox::io;

        //print the resultant bounds:
        // box.mapping().print();
        
        // box.get_boundary_layer().print_contents();

        // // print a summary of layers in the chartbox...
        // box.print_layers() ;

        // const auto boundary_layer = box.get_boundary_layer();
        // fmt::print( "====== ====== Layer:{} ====== ====== \n", boundary_layer.name() );
        // fmt::print( "{}\n", boundary_layer.print_properties() );

        // const auto contour_layer = box.get_contour_layer();
        // fmt::print( "====== ====== Layer:{} ====== ====== \n", contour_layer.name() );
        // fmt::print( "{}\n", contour_layer.print_properties() );

    }   // DEBUG

    {   // write outputs

        const auto start_write_all = std::chrono::high_resolution_clock::now();

        // Optionally load boundary path:
        if( ! boundary_output_path.empty() ){
            const auto& layer = box.get_boundary_layer();
            const auto& bounds = box.mapping().local_bounds();
            fmt::print( ">>> Write Layer: {}    to: {}\n", layer.name(), boundary_output_path.string() );
            if( boundary_output_path.extension() == chartbox::io::png::extension ){
                png::save( layer, bounds, boundary_output_path );
            }
        }

        if( ! contour_output_path.empty() ){
            const auto& layer = box.get_contour_layer();
            if( "auto" == contour_output_path ){
                const auto& visible = layer.visible();
                const std::string filepath = fmt::format("to-layer-visible-{:06.0f}E-{:06.0f}N.png", visible.min.easting, visible.min.northing );
                fmt::print( ">>> Write Layer: {}    to: {}\n", layer.name(), filepath );
                png::save( layer, visible, filepath );
            }else if( chartbox::io::png::extension == contour_output_path.extension() ){
                fmt::print( ">>> Write Layer: {}    to: {}\n", layer.name(), contour_output_path.string() );
                png::save( layer, box.mapping().local_bounds(), contour_output_path );
            // }else if( contour_output_path.extension() == chartbox::io::flatbuffer::extension ){
            //     flatbuffer::save( layer, contour_output_path);
            }
        }

        if( ! chart_output_arg.empty() ){
            fmt::print( ">>> Write chart to: {}\n", view_output_path.string() );
            if( view_output_path.extension() == chartbox::io::png::extension ){
                png::save( box, 8.0, view_output_path );
            }
        }

        const auto finish_write_all = std::chrono::high_resolution_clock::now(); 
        const auto write_duration = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(finish_write_all - start_write_all).count())/1000;
        if( 0.5 < write_duration ){
            fmt::print( "<<< Written in:   {:5.2f} s \n\n", write_duration );
        }
    }

    // make sure this only happens once
    GDALDestroyDriverManager();

    return 0;
}
