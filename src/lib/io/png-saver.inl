// GPL v3 (c) 2021, Daniel Williams 

// NOTE: This is the template-class implementation --
//       It is not compiled until referenced, even though it contains the
//       function implementations.

#include <cstddef>
#include <string>

#include "gdal_priv.h"

namespace chartbox::io::png {

template< typename layer_t >
bool save( const layer_t& from_layer, const std::filesystem::path& to_path ) {
    fmt::print( stderr, "    >>> Write Layer to: {}\n", to_path.string() );

    // not a guaranteed property of the layer; not all of them have this...
    const size_t dimension = from_layer.dimension;

    // might be a duplicate call, but duplicate calls don't seem to cause any problems.
    GDALAllRegister();

    // create the source dataset (load into this source dataset)
    GDALDriver* p_memory_driver = GetGDALDriverManager()->GetDriverByName("MEM");
    if (nullptr == p_memory_driver) {
        fmt::print( stderr, "!! error allocating memory driver !! (did you initialize GDAL?)" );
        return false;
    }
    GDALDataset* p_grid_dataset = p_memory_driver->Create( "", dimension, dimension, 1, GDT_Byte, nullptr);
    if (nullptr == p_grid_dataset) {
        fmt::print( stderr, "!! error allocating grid dataset ?!" );
        return false;
    }
    auto p_gray_band = p_grid_dataset->GetRasterBand(1);
    // unfortunately, this is not automatic
    if (CE_Failure == p_gray_band->SetColorInterpretation(GCI_GrayIndex)) {
        fmt::print( stderr, "?? Unsupported color format? --- probably a program error." );
        GDALClose(p_grid_dataset);
        return false;
    }

    // optional property of layers... also: yes, it's a hack
    std::byte* layer_start_p = const_cast<std::byte*>(reinterpret_cast<const std::byte*>( from_layer.data() ));

    // copy one line at a time, reading from the bottom-up, but writing top-down (i.e. Raster-Order)
    for( size_t write_line_index = 0; write_line_index < dimension; ++write_line_index ){
        const size_t read_line_index = dimension - 1 - write_line_index;
        void* read_ptr = layer_start_p + (read_line_index * dimension);

        if (CE_Failure == p_gray_band->RasterIO(GF_Write, 0, write_line_index, dimension, 1, read_ptr, dimension, dimension, GDT_Byte, 1, 0)) {
            fmt::print( stderr, "?? Could not copy into the RasterIO buffer.\n" );
            GDALClose(p_grid_dataset);
            return false;
        }
    }

    // Use the png driver to copy the source dataset
    GDALDriver* p_png_driver = GetGDALDriverManager()->GetDriverByName("PNG");
    GDALDataset* p_png_dataset = p_png_driver->CreateCopy( to_path.string().c_str(), p_grid_dataset, false, nullptr, nullptr, nullptr);

    if (p_png_dataset) {
        GDALClose(p_grid_dataset);
        GDALClose(p_png_dataset);
    }

    fmt::print(  "    <<< Successfuly wrote Layer.\n" );

    return true;
}

}  // namespace
