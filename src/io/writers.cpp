// GPL v3 (c) 2020

// NOTE: This is the template-class implementation -- 
//       It is not compiled until referenced, even though it contains the function implementations.

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

using std::byte;
using std::cerr;
using std::endl;

#include <Eigen/Geometry>

#ifdef ENABLE_GDAL
#include "gdal.h"
#include "gdal_priv.h"
#endif

#include <nlohmann/json.hpp>

#include "geometry/polygon.hpp"

#include "writers.hpp"

using Eigen::Vector2d;

#ifdef ENABLE_GDAL
bool yggdrasil::io::to_grayscale_png(const std::vector<byte>& source, const std::string& filepath){
    const double dimension = std::sqrt(source.size());

    // might be a duplicate call, but duplicate calls don't seem to cause any problems.
    GDALAllRegister();

    // create the source dataset (load into this source dataset)
    GDALDriver* p_memory_driver = GetGDALDriverManager()->GetDriverByName("MEM");
    if( nullptr == p_memory_driver ){
        cerr << "!! error allocating memory driver !! (did you initialize GDAL?)" << endl;
        return false;
    }
    GDALDataset *p_grid_dataset = p_memory_driver->Create("", dimension, dimension, 1, GDT_Byte, nullptr);
    if( nullptr == p_grid_dataset ){
        cerr << "!! error allocating grid dataset ?!" << endl;
        return false;
    }

    auto p_gray_band = p_grid_dataset->GetRasterBand(1);
    // unfortunately, this is not automatic
    if( CE_Failure == p_gray_band->SetColorInterpretation(GCI_GrayIndex)){
        cerr << "?? Unsupported color format? --- probably a program error." << endl;
        GDALClose( p_grid_dataset );
        return false;
    }

    // allocate data buffers
    void* read_p = reinterpret_cast<void*>(const_cast<byte*>(source.data()));
    if( CE_Failure == p_gray_band->RasterIO(GF_Write, 0, 0, dimension, dimension, read_p, dimension, dimension, GDT_Byte, 0, 0)){
        GDALClose( p_grid_dataset );
        return false;
    }

    // Use the png driver to copy the source dataset
    GDALDriver* p_png_driver = GetGDALDriverManager()->GetDriverByName("PNG");
    GDALDataset* p_png_dataset = nullptr;
    p_png_dataset = p_png_driver->CreateCopy(filepath.c_str(), p_grid_dataset, FALSE, nullptr, nullptr, nullptr);

    if( p_png_dataset ){
        GDALClose( p_grid_dataset );
        GDALClose( p_png_dataset );
    }

    return true;
}
#endif //#ifdef ENABLE_LIBPNG

