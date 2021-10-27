// GPL v3 (c) 2021, Daniel Williams 

#include <cstddef>
#include <filesystem>
#include <string>

#include "gdal_priv.h"

#include "chart-box/chart-box.hpp"

#include "png.hpp"

using chartbox::ChartBox;


namespace chartbox::io::png {

bool save( const ChartBox& from_chart, double precision, const std::filesystem::path& to_path ) {
    fmt::print( stderr, "    >>> Write Composite Chart to: {}\n", to_path.string() );

    const auto bounds = from_chart.mapping().local_bounds();

    const int output_height = static_cast<int>(bounds.height())/precision;
    const int output_width = static_cast<int>(bounds.width())/precision;
    fmt::print( "         :: png output size: ( width: {} x {} : height )\n", output_width, output_height );

    const double easting_increment = precision;
    const double northing_increment = -precision; 

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

    // copy one line at a time, reading from south, and writting at buffer-begin
    std::vector<uint8_t> line_buffer(output_width);
    for( int write_line_index = 0; write_line_index < output_height; ++write_line_index ){
        const double at_northing = bounds.max.northing + northing_increment/2 + northing_increment * write_line_index;

        // fmt::print( "         @northing: {:6}  // [line: {:6}]\n", at_northing, write_line_index    );

        LocalLocation at_location ( NAN, at_northing );
        double at_easting = bounds.min.easting + easting_increment/2;
        for( size_t buffer_index = 0; buffer_index < line_buffer.size(); ++buffer_index ){
            at_location.easting = at_easting;

            // fmt::print( "             [{:4}]: @( {:6}, {:6} ) = {} \n", buffer_index, at_easting, at_northing, from_chart.classify(at_location) ); 

            line_buffer[buffer_index] = from_chart.classify(at_location);
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
