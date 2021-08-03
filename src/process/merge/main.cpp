// GPL v3 (c) 2021, Daniel Williams

#include <chrono>
#include <filesystem>
#include <random>

// may not be standard
#include <sys/stat.h>

#include "chart-box.hpp"
#include "io/chart-debug-loader.hpp"
#include "io/chart-geojson-loader.hpp"

// #include "io/chart-debug-writer.hpp"
#include "io/chart-png-writer.hpp"

using chartbox::io::DebugLoader;
using chartbox::io::GeoJSONLoader;
// using chartbox::io::ShapefileLoader;
using chartbox::layer::FixedGridLayer;

constexpr double boundary_width = 4096.;
constexpr double desired_precision = 1.0;

constexpr size_t test_seed = 55;
static std::mt19937 generator;

// int main(int argc, char* argv[]){
int main( void ){
    // bool enable_input_lidar = false;
    // std::string input_path_terrain;

    //std::string boundary_input_path("data/block-island/boundary.simple.geojson");
    std::string boundary_input_path("data/block-island/boundary.polygon.geojson");


    // std::string boundary_output_path("stderr");
    std::string boundary_output_path("debug-height-map.png");

    // bool enable_output_height_map = false;
    // std::string output_path_height_map;

    // configure input:
    
    // std::string output_path("?????");
    // if( enable_output_height_map ){
    //     if( output_path_height_map.empty() ){
    //         output_path_height_map = "worldtree.output.png";
    //     }
    //     cerr << "  ## Save output to: " << output_path << '\n';
    // }

    if( ! std::filesystem::is_regular_file(boundary_input_path)){
        fmt::print(stderr, "!! Could not find boundary input path!! : {}\n", boundary_input_path);
        return EXIT_FAILURE;
    }

    // ^^^^ Configuration
    // vvvv Execution
    GDALAllRegister();

    chartbox::ChartBox box;

    const auto start_load = std::chrono::high_resolution_clock::now(); 
    {
        fmt::print( stderr, ">>> Starting Load: \n");

        if ( ! boundary_input_path.empty() ) {
            fmt::print("    >>> Loading boundary layer from path: {}\n", boundary_input_path );

            if( boundary_input_path.substr( boundary_input_path.find_last_of(".") + 1) == "shp") {
                fmt::print("        >>> With ShapefileLoader:\n");

                // .shp ==>> found a shapefile
                auto boundary_loader = DebugLoader<FixedGridLayer>( box.mapping(), box.get_boundary_layer());
                // auto boundary_loader = ShapefileLoader<FixedGridLayer>( box.mapping(), box.get_boundary_layer());
                if( ! boundary_loader.load_file(boundary_input_path) ){
                    fmt::print( stderr, "!!!! error while loading shapefile:!!!!\n" );
                    return EXIT_FAILURE;
                }
            }else if( boundary_input_path.substr( boundary_input_path.find_last_of(".") + 1) == "geojson") {
                fmt::print("        >>> With GeoJSONLoader:\n");

                auto boundary_loader = GeoJSONLoader<FixedGridLayer>( box.mapping(), box.get_boundary_layer());
                if( ! boundary_loader.load_file(boundary_input_path) ){
                    fmt::print( stderr, "!!!! error while loading data:!!!!\n" );
                    return EXIT_FAILURE;
                }
            }

            fmt::print("    <<< Successfuly loaded BoundaryLayer.\n");
        }

        // const bool load_success = chart.get_countour_layer()->load_from_json_stream( *boundary_document_stream);
        // if(!load_success){
        //     cerr << "!!!! error while loading data:!!!!\n";
        //     return -1;
        // }        
        const auto finish_load = std::chrono::high_resolution_clock::now(); 
        const auto load_duration = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(finish_load - start_load).count())/1000;
        fmt::print(  "<< Loaded in:   {:5.2f} s \n\n", load_duration );
    }


    {   // DEBUG
        //print the resultant bounds:
        box.mapping().print();

        // // print a summary of layers in the chartbox...
        // box.print_layers() ;

        // const auto boundary_layer = box.get_boundary_layer();
        // box.get_boundary_layer().print_contents();

        // // this layer is not yet relevant... or interesting.
        // const auto contour_layer = box.get_contour_layer();
        // box.get_contour_layer().print_contents();

    }   // DEBUG

    {
        const auto start_write = std::chrono::high_resolution_clock::now();
        fmt::print( stderr, ">>> Starting Write: \n");

        // Optionally load boundary path:
        if( ! boundary_output_path.empty() ){
            if( boundary_output_path == "stderr" ){
                fmt::print( stderr, "<<< File output disabled");

            }else if( boundary_output_path == "stdout" ){
                const auto boundary_layer = box.get_boundary_layer();
                box.get_boundary_layer().print_contents();

            }else{
                fmt::print( stderr, "    >>> Write Boundary Layer to: {}\n", boundary_output_path );
                chartbox::io::PNGWriter<chartbox::layer::FixedGridLayer> boundary_writer( box.get_boundary_layer() );
                if( ! boundary_writer.write_to_path(boundary_output_path) ){
                    fmt::print( stderr, "!!!! error while writing data:!!!!\n" );
                    return EXIT_FAILURE;
                }else{
                    fmt::print(  "    <<< Successfuly wrote BoundaryLayer.\n" );
                }
            }
        }
        
        const auto finish_write = std::chrono::high_resolution_clock::now(); 
        const auto write_duration = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(finish_write - start_write).count())/1000;
        fmt::print( stderr, "<<< Written in:   {:5.2f} s \n\n", write_duration );
    }

    // make sure this only happens once
    GDALDestroyDriverManager();

    return 0;
}
