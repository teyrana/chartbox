// GPL v3 (c) 2021, Daniel Williams

#include <chrono>
#include <filesystem>
#include <random>

// may not be standard
#include <sys/stat.h>

#include <fmt/core.h>

#include "chart-box.hpp"
// #include "io/chart-debug-loader.hpp"
#include "io/chart-geojson-loader.hpp"
#include "io/chart-shapefile-loader.hpp"

// #include "io/chart-debug-writer.hpp"
#include "io/chart-png-writer.hpp"

// using chartbox::io::DebugLoader;
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

    // const std::string boundary_input_path("data/block-island/boundary.simple.geojson");
    const std::string boundary_input_arg("data/block-island/boundary.polygon.geojson");
    // const std::string boundary_input_path("data/massachusetts/navigation_area_100k.shp");

    const std::string contour_input_arg("data/block-island/coastline.geojson");

    std::string boundary_output_arg("");
    // boundary_output_arg = "boundary.png";

    std::string contour_output_arg("");
    // boundary_output_arg = "countour.png";


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
    if( ! std::filesystem::is_regular_file(contour_input_arg)){
        fmt::print(stderr, "!! Could not find contour input path!! : {}\n", contour_input_arg);
        return EXIT_FAILURE;
    }
    std::filesystem::path contour_input_path( contour_input_arg );

    std::filesystem::path contour_output_path( contour_output_arg );


    // ^^^^ Configuration
    // vvvv Execution
    GDALAllRegister();

    chartbox::ChartBox box;

    const auto start_load = std::chrono::high_resolution_clock::now(); 
    {
        fmt::print( stderr, ">>> Starting Load: \n");

        if ( ! boundary_input_path.empty() ) {
            fmt::print("    >>> Loading boundary layer from path: {}\n", boundary_input_path.string() );

            if( boundary_input_path.extension() == ShapefileLoader<FixedGridLayer>::extension ){
                auto boundary_loader = ShapefileLoader<FixedGridLayer>( box.mapping(), box.get_boundary_layer());
                boundary_loader.load(boundary_input_path);

            }else if( boundary_input_path.extension() == GeoJSONLoader<FixedGridLayer>::extension ){
                auto boundary_loader = GeoJSONLoader<FixedGridLayer>( box.mapping(), box.get_boundary_layer());
                boundary_loader.load(boundary_input_path);

            }
        }

        if ( ! contour_input_path.empty() ) {
            fmt::print("    >>> Loading contour layer from path: {}\n", contour_input_path.string() );
            if( boundary_input_path.extension() == ShapefileLoader<FixedGridLayer>::extension ){
                auto boundary_loader = ShapefileLoader<FixedGridLayer>( box.mapping(), box.get_contour_layer());
                boundary_loader.load(boundary_input_path);

            }else if( boundary_input_path.extension() == GeoJSONLoader<FixedGridLayer>::extension ){
                auto boundary_loader = GeoJSONLoader<FixedGridLayer>( box.mapping(), box.get_contour_layer());
                boundary_loader.load(boundary_input_path);

            }
        }

        const auto finish_load = std::chrono::high_resolution_clock::now(); 
        const auto load_duration = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(finish_load - start_load).count())/1000;
        fmt::print(  "<< Loaded in:   {:5.2f} s \n\n", load_duration );
    }


    {   // DEBUG
        //print the resultant bounds:
        box.mapping().print();

        // box.get_boundary_layer().print_contents();

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

        // Optionally load boundary path:
        if( ! boundary_output_path.empty() ){
            fmt::print( stderr, ">>> Write Boundary Layer ...\n" );
            if( boundary_output_path.extension() == PNGWriter<FixedGridLayer>::extension ){
                chartbox::io::PNGWriter<chartbox::layer::FixedGridLayer> writer( box.get_boundary_layer() );
                writer.write_to_path(boundary_output_path);
            }
        }

        if( ! contour_output_path.empty() ){
            fmt::print( stderr, ">>> Write Contour Layer ...\n" );
            if( contour_output_path.extension() == PNGWriter<FixedGridLayer>::extension ){
                chartbox::io::PNGWriter<chartbox::layer::FixedGridLayer> writer( box.get_contour_layer() );
                writer.write_to_path( contour_output_path );
            }
        }
        
        const auto finish_write = std::chrono::high_resolution_clock::now(); 
        const auto write_duration = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(finish_write - start_write).count())/1000;
        if( 0.1 < write_duration ){
            fmt::print( stderr, "<<< Written in:   {:5.2f} s \n\n", write_duration );
        }
    }

    // make sure this only happens once
    GDALDestroyDriverManager();

    return 0;
}
