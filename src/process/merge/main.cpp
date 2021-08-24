// GPL v3 (c) 2021, Daniel Williams

#include <chrono>
#include <filesystem>
#include <random>

// may not be standard
#include <sys/stat.h>

#include <fmt/core.h>

#include "chart-box.hpp"
// #include "io/debug-loader.hpp"
#include "io/boundary-loader.hpp"
#include "io/contour-loader.hpp"

// #include "io/debug-writer.hpp"
#include "io/png-writer.hpp"

// using chartbox::io::DebugLoader;
// using chartbox::io::GeoJSONLoader;

using chartbox::layer::FixedGridLayer;

constexpr double boundary_width = 4096.;
constexpr double desired_precision = 1.0;

constexpr size_t test_seed = 55;
static std::mt19937 generator;


// int main(int argc, char* argv[]){
int main( void ){
    // bool enable_input_lidar = false;
    // std::string input_path_terrain;

    std::string boundary_input_arg("data/block-island/boundary.simple.geojson");
            // boundary_input_arg("data/block-island/boundary.polygon.geojson");
            // boundary_input_arg("data/massachusetts/navigation_area_100k.shp");

    std::string contour_input_arg("");
                // contour_input_arg = "data/block-island/coastline.geojson";
                // contour_input_arg = "contour.cache.fb";

    std::string boundary_output_arg("");
                // boundary_output_arg = "boundary.png";
                // boundary_output_arg = "boundary.cache.fb";

    std::string contour_output_arg("");
                // contour_output_arg = "countour.png";
                // contour_output_arg = "contour.cache.fb";

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
    if( (not contour_input_arg.empty()) && ( not std::filesystem::is_regular_file(contour_input_arg)) ){
        fmt::print(stderr, "!! Could not find contour input path!! : {}\n", contour_input_arg);
        return EXIT_FAILURE;
    }
    std::filesystem::path contour_input_path( contour_input_arg );

    std::filesystem::path contour_output_path( contour_output_arg );


    // ^^^^ Configuration
    // vvvv Execution
    GDALAllRegister();

    chartbox::ChartBox box;

    {   // load inputs 
        fmt::print( stderr, ">>> Starting Load: \n");
        const auto start_load = std::chrono::high_resolution_clock::now(); 

        if ( ! boundary_input_path.empty() ) {
            auto& boundary_layer = box.get_boundary_layer();
            auto& mapping = box.mapping();
            fmt::print( stderr, ">>> Load Layer: {}  to: {}\n", boundary_layer.name(), boundary_input_path.string() );
            if( boundary_input_path.extension() == chartbox::io::geojson::extension ){
                auto loader = BoundaryLoader<FixedGridLayer>( mapping, boundary_layer );
                loader.load(boundary_input_path);
            }
        }

        if ( ! contour_input_path.empty() ) {
            auto& contour_layer = box.get_contour_layer();
            auto& mapping = box.mapping();
            fmt::print("    >>> Loading layer: {}  from: {}\n", contour_layer.name(), contour_input_path.string() );
            if( contour_input_path.extension() == chartbox::io::geojson::extension ){
                auto loader = ContourLoader<FixedGridLayer>( mapping, contour_layer );
                loader.load(contour_input_path);
            }
        }

        const auto finish_load = std::chrono::high_resolution_clock::now(); 
        const auto load_duration = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(finish_load - start_load).count())/1000;
        if( 0.5 < load_duration ){
            fmt::print(  "<< Loaded in:   {:5.2f} s \n\n", load_duration );
        }
    }


    {   // DEBUG
        //print the resultant bounds:
        box.mapping().print();

        // box.get_boundary_layer().print_contents();

        // // print a summary of layers in the chartbox...
        // box.print_layers() ;

        // const auto boundary_layer = box.get_boundary_layer();
        // box.get_boundary_layer().print_contents();

        // const auto contour_layer = box.get_contour_layer();
        // box.get_contour_layer().print_contents();

    }   // DEBUG

    {   // write outputs

        const auto start_write = std::chrono::high_resolution_clock::now();

        // Optionally load boundary path:
        if( ! boundary_output_path.empty() ){
            auto& boundary_layer = box.get_boundary_layer();
            fmt::print( stderr, ">>> Write Layer: {}  to: {}\n", boundary_layer.name(), boundary_output_path.string() );
            if( boundary_output_path.extension() == chartbox::io::png::extension ){
                auto writer = PNGWriter<FixedGridLayer>( boundary_layer );
                writer.write( boundary_output_path );
            }
        }

        if( ! contour_output_path.empty() ){
            auto& contour_layer = box.get_contour_layer();
            fmt::print( stderr, ">>> Write Layer: {}  to: {}\n", contour_layer.name(), contour_output_path.string() );
            if( contour_output_path.extension() == chartbox::io::png::extension ){
                auto writer = PNGWriter<FixedGridLayer>( contour_layer );
                writer.write( contour_output_path );
            }
        }
        
        const auto finish_write = std::chrono::high_resolution_clock::now(); 
        const auto write_duration = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(finish_write - start_write).count())/1000;
        if( 0.5 < write_duration ){
            fmt::print( stderr, "<<< Written in:   {:5.2f} s \n\n", write_duration );
        }
    }

    // make sure this only happens once
    GDALDestroyDriverManager();

    return 0;
}
