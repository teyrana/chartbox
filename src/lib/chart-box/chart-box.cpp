// GPL v3 (c) 2021, Daniel Williams 

#include <cmath>

#include <fmt/core.h>

#include <gdal.h>
#include <ogr_spatialref.h>

#include "chart-box.hpp"

using chartbox::ChartBox;

ChartBox::ChartBox()
    : mapping_()
    , boundary_layer_()
    , contour_layer_()
{

    boundary_layer_.fill( chartbox::layer::default_cell_value );
    boundary_layer_.name("BoundaryLayerGrid");
    
    contour_layer_.fill( chartbox::layer::default_cell_value );
    contour_layer_.name("ContourLayerGrid");
}

uint8_t ChartBox::get( const LocalLocation& location ) const { 
    const uint8_t boundary_value = boundary_layer_.get( location );
    const uint8_t contour_value = contour_layer_.get( location );
    return std::max( boundary_value, contour_value );
}

void ChartBox::print_layers() const {
    
    fmt::print( "============ ============ Printing Layers: ============ ============ \n");

    uint32_t layer_index = 0;

    // Boundary Layer
    fmt::print( "    [{:2d}] <{}> :{} ({3} x {3})\n", layer_index, boundary_layer_.type(), boundary_layer_.name(), boundary_layer_.cells_across_view() );
    ++layer_index;

    // Contour Layer
    fmt::print( "    [{:2d}] <{}> :{} ({3} x {3})\n", layer_index, contour_layer_.type(), contour_layer_.name(), boundary_layer_.cells_across_view() );
    ++layer_index;

    // next layer 
    // ...


    fmt::print( "============ ============ {} layers total ============ ============ \n", layer_index );
}

