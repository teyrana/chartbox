// GPL v3 (c) 2021, Daniel Williams 

// NOTE: This is the template-class implementation --
//       It is not compiled until referenced, even though it contains the
//       function implementations.

#include "gdal_priv.h"

namespace chartbox::io::png {

using chartbox::geometry::BoundBox;
using chartbox::geometry::LocalLocation;

template< typename layer_t >
bool save( const layer_t& from_layer, const BoundBox<LocalLocation>& bounds, const std::filesystem::path& to_path ) {
    const double precision = from_layer.meters_across_cell();
    const int output_height = static_cast<int>(bounds.height())/precision;
    const int output_width = static_cast<int>(bounds.width())/precision;
    //fmt::print( "         :: png output size: ( width: {} x {} : height )\n", output_width, output_height );

    // might be a duplicate call, but duplicate calls don't seem to cause any problems.
    GDALAllRegister();

    // create the source dataset (load into this source dataset)
    GDALDriver* p_memory_driver = GetGDALDriverManager()->GetDriverByName("MEM");
    if (nullptr == p_memory_driver) {
        fmt::print( stderr, "!! error allocating memory driver !! (did you initialize GDAL?)" );
        return false;
    }
    GDALDataset* p_grid_dataset = p_memory_driver->Create( "", output_height, output_width, 1, GDT_Byte, nullptr);
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


    // copy one line at a time, reading from the bottom-up, but writing top-down (i.e. Raster-Order)
    std::vector<uint8_t> line_buffer(output_width);
    const double easting_increment = precision;
    const double northing_increment = -precision;
    for( int write_line_index = 0; write_line_index < output_height; ++write_line_index ){
        const double at_northing = bounds.max.northing + northing_increment/2 + northing_increment * write_line_index;

        // fmt::print( "         @northing: {:4}  // [line: {:4}]\n", at_northing, write_line_index    );

        LocalLocation at_location ( NAN, at_northing );
        double at_easting = bounds.min.easting + easting_increment/2;
        for( size_t buffer_index = 0; buffer_index < line_buffer.size(); ++buffer_index ){
            at_location.easting = at_easting;

            // fmt::print( "             [{:4}]: @( {:4}, ____ ) = {} \n", buffer_index, at_easting, from_layer.classify(at_location) ); 

            line_buffer[buffer_index] = from_layer.get(at_location);
            at_easting += easting_increment;
        }
        
        if (CE_Failure == p_gray_band->RasterIO(GF_Write, 0, write_line_index, output_width, 1, line_buffer.data(), output_width, 1, GDT_Byte, 1, 0)) {
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
