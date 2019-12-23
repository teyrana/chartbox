// GPL v3 (c) 2020

#include <fstream>
#include <iostream>
#include <random>
#include <chrono>

#include <sys/stat.h> // may not be standard

using std::cerr;

#include <cxxopts/cxxopts.hpp>

#include <Eigen/Geometry>

#include <nlohmann/json.hpp>
using nlohmann::json;

// #include "node/quad_node.hpp"
// #include "node/tile_node.hpp"
#include "world_tree.hpp"
#include "io/readers.hpp"
#include "io/writers.hpp"

using namespace yggdrasil::io;

using yggdrasil::grid::Grid;
using yggdrasil::quadtree::Tree;
using yggdrasil::WorldTree;

constexpr double boundary_width = 4096.;
constexpr double desired_precision = 1.0;
// =====
const json source = yggdrasil::generate_diamond(  boundary_width, desired_precision);

constexpr size_t test_seed = 55;
static std::mt19937 generator;


cxxopts::Options command_line_options("Profiler", "Run metrics and speed tests of the Yggdrasil library");

void configure_parser_options(cxxopts::Options& opts){
    // define command line options
    opts.add_options()
        // ("d,debug", "Enable debugging")
        ("i,input", "input json path", cxxopts::value<std::string>())
        ("c,count", "Iteration Count", cxxopts::value<size_t>())
        ("o,image", "Output image filen name", cxxopts::value<std::string>())
    ;
}

void profile_tree(const WorldTree& worldtree, const size_t iteration_limit){
    const double max = worldtree.get_layout().get_x_max();
    const double min = worldtree.get_layout().get_x_min();

    generator.seed(test_seed);

    std::cout << ">> Starting testing:\n";
    cell_value_t dump = 0;
    
    auto start = std::chrono::high_resolution_clock::now(); 
    size_t iteration = 0;
    for( iteration = 0; iteration < iteration_limit; ++iteration ){
        const double x = static_cast<double>(generator()) / generator.max() * max + min;

        const double y = static_cast<double>(generator()) / generator.max() * max + min;
        //std::cout << "random value: x: " << x << ", " << y << '\n';

        dump = worldtree.classify({x,y});
        //std::cout << "             =>: " << static_cast<int>(value) << '\n';
    }
    auto stop = std::chrono::high_resolution_clock::now(); 

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count(); 
  
    std::cout.imbue(std::locale(""));
    std::cout << "<< Finished testing:\n";
    std::cout << "   Ran " << iteration << " iterations in " << duration << " \u03BCs \n\n";
}


int main(int argc, char* argv[]){
    std::string filename;
    std::unique_ptr<std::istream> document_stream;
    size_t trial_size = 10;
    
    configure_parser_options(command_line_options);
    auto result = command_line_options.parse(argc, argv);

    // configure input:
    if( 0 == result.count("input")){
        cerr << "  ## using default input.\n";
        // load default json source data
        filename = "<default>";
        document_stream.reset(new std::istringstream(source.dump()));
    }else if( 0 < result.count("input")){
        filename = result["opt"].as<std::string>();
        cerr << "  ## File input; with:  " << filename << endl;

        struct stat buf;
        if( stat(filename.c_str(), &buf) == -1){
            cerr << "!? could not find file!\n";
            return -1;
        }
        std::ifstream document_stream = std::ifstream(filename);
    }

    if( 0 < result.count("count")){
        trial_size = result["count"].as<size_t>();
        cerr << "    ## Selected Trial Size: " << trial_size << endl;
    }

    
    WorldTree tree;
    
    bool write_output = false;
    std::string output_path;
    if( 0 < result.count("image")){
        write_output = true;
        output_path = result["image"].as<std::string>();
        if( output_path.empty() ){
            output_path = "worldtree.output.png";
        }
        cerr << "  ## Save output to: " << output_path << '\n';
    }

    // ^^^^ Configuration
    // vvvv Execution

    cerr << ">> loading tree ... \n";
    const auto start_load = std::chrono::high_resolution_clock::now(); 
    
    bool load_success;
    load_success = load_from_json_stream(tree, *document_stream);
    if(!load_success){
        cerr << "!!!! error while loading into the tree!!!!\n";
        cerr << tree.get_error();
        return -1;
    }
    const auto finish_load = std::chrono::high_resolution_clock::now(); 
    const auto load_duration = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(finish_load - start_load).count())/1000;
    cerr << "<< Loaded in:   " << load_duration << " s \n";

    
    // tree.debug();
    cerr << tree.summary();
    profile_tree(tree, trial_size);

    if(write_output){
        cerr << "##>> writing output...\n";
        to_png(tree, output_path);
    }

    return 0;
}
