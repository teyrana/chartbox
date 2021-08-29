// GPL v3 (c) 2021, Daniel Williams

#include <chrono>
#include <filesystem>
#include <random>

// may not be standard
#include <sys/stat.h>

#include <fmt/core.h>

#include "chart-box.hpp"

#include "io/flatbuffer/flatbuffer.hpp"
#include "io/geojson/geojson.hpp"
#include "io/png/png.hpp"

using chartbox::layer::BOUNDARY;
using chartbox::layer::CONTOUR;
using namespace chartbox::io;

constexpr double boundary_width = 4096.;
constexpr double desired_precision = 1.0;

constexpr size_t test_seed = 55;
static std::mt19937 generator;


// int main(int argc, char* argv[]){
int main( void ){
    // bool enable_input_lidar = false;
    // std::string input_path_terrain;

    // std::string boundary_input_arg("data/block-island/boundary.simple.geojson");
    std::string boundary_input_arg("data/block-island/boundary.polygon.geojson");
    // std::string boundary_input_arg("data/massachusetts/navigation_area_100k.shp");

    std::string contour_input_arg("");
                contour_input_arg = "data/block-island/coastline.geojson";
                // contour_input_arg = "contour.cache.fb";

    std::string boundary_output_arg("");
                // boundary_output_arg = "boundary.png";
                // boundary_output_arg = "boundary.cache.fb";

    std::string contour_output_arg("");
                // contour_output_arg = "countour.png";
                // contour_output_arg = "contour.cache.fb";

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
    if( (not contour_input_arg.empty()) && ( not std::filesystem::is_regular_file(contour_input_arg)) ){
        fmt::print(stderr, "!! Could not find contour input path!! : {}\n", contour_input_arg);
        return EXIT_FAILURE;
    }
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

        fmt::print( ">>> Starting Load: \n");
        const auto start_load = std::chrono::high_resolution_clock::now(); 

        if ( ! boundary_input_path.empty() ) {
            auto& layer = box.get_boundary_layer();
            auto& mapping = box.mapping();
            fmt::print( ">>> Load Layer: {}  to: {}\n", layer.name(), boundary_input_path.string() );
            if( boundary_input_path.extension() == chartbox::io::geojson::extension ){
                geojson::load_boundary_layer( boundary_input_path, mapping, layer );
            // NYI
            // }else if( boundary_input_path.extension() == chartbox::io::flatbuffer::extension ){
            //     flatbuffer::load( boundary_input_path, layer );
            }
        }

        if ( ! contour_input_path.empty() ) {
            auto& layer = box.get_contour_layer();
            auto& mapping = box.mapping();
            fmt::print( ">>> Loading layer: {} <{}>  from: {}\n", layer.name(), layer.type(), contour_input_path.string() );
            if( boundary_input_path.extension() == chartbox::io::geojson::extension ){
                geojson::load_contour_layer( contour_input_path, mapping, layer );

            // DEV: temporarily disable in this feature breanch
            // }else if( contour_input_path.extension() == chartbox::io::flatbuffer::extension ){
            //     flatbuffer::load( contour_input_path, layer );
            }
        }

        const auto finish_load = std::chrono::high_resolution_clock::now(); 
        const auto load_duration = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(finish_load - start_load).count())/1000;
        if( 0.5 < load_duration ){
            fmt::print( "<< Loaded in:   {:5.2f} s \n\n", load_duration );
        }
    }


    {   // DEBUG
        using namespace chartbox::io;

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
            const auto& layer = box.get_boundary_layer();
            fmt::print( ">>> Write Layer: {}  to: {}\n", layer.name(), boundary_output_path.string() );
            if( boundary_output_path.extension() == chartbox::io::png::extension ){
                png::save( layer, boundary_output_path );

            // NYI
            // }else if( boundary_output_path.extension() == chartbox::io::flatbuffer::extension ){
            //     flatbuffer::save( layer, boundary_output_path);
            }
        }

        // DEBUG
        // temporarily disable... renable later.
        // if( ! contour_output_path.empty() ){
        //     const auto& layer = box.get_contour_layer();
        //     fmt::print( ">>> Write Layer: {}  to: {}\n", layer.name(), contour_output_path.string() );
        //     if( contour_output_path.extension() == chartbox::io::png::extension ){
        //         png::save( layer, contour_output_path );
        //     }else if( contour_output_path.extension() == chartbox::io::flatbuffer::extension ){
        //         flatbuffer::save( layer, contour_output_path);
        //     }
        // }

        if( ! chart_output_arg.empty() ){
            // const auto& layer = box.get_view_layer();
            fmt::print( ">>> Write chart to: {}\n", view_output_path.string() );
            if( view_output_path.extension() == chartbox::io::png::extension ){
                png::save( box, view_output_path );
            }
        }

        const auto finish_write = std::chrono::high_resolution_clock::now(); 
        const auto write_duration = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(finish_write - start_write).count())/1000;
        if( 0.5 < write_duration ){
            fmt::print( "<<< Written in:   {:5.2f} s \n\n", write_duration );
        }
    }

    // make sure this only happens once
    GDALDestroyDriverManager();

    return 0;
}
