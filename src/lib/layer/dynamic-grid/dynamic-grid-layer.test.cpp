// GPL v3 (c) 2021, Daniel Williams 

#include <cmath>
#include <iostream>
#include <sstream>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
using Catch::Approx;

#include "dynamic-grid-layer.hpp"
#include "geometry/bound-box.hpp"

using chartbox::geometry::BoundBox;
using chartbox::geometry::LocalLocation;

using chartbox::layer::dynamic::DynamicGridLayer;


// ============ ============ ============ ============  Dynamic-Grid-Layer-Tests  ============ ============ ============ ============
template<typename T>
void populate_markers_per_cell( T& layer ) {
    const double meters_across_cell = layer.meters_across_cell();
    // sets a distinct debug value for each cell
    // iteration progress West => East, South => North;  Cell 0,0 starts in the Southwest Corner.
    for (double northing = layer.visible().min.northing + 0.5*meters_across_cell; northing < layer.visible().max.northing; northing += meters_across_cell ) {
        for (double easting = layer.visible().min.easting + 0.5*meters_across_cell; easting < layer.visible().max.easting; easting += meters_across_cell) {
            const uint8_t value = (static_cast<uint8_t>(easting-layer.visible().min.easting) << 4) + static_cast<uint8_t>(northing - layer.visible().min.northing);
            layer.store( {easting, northing}, value );
        }
    }
}

template<typename T>
void populate_markers_per_sector( T& layer ) {
    const auto& visible = layer.visible();
    const double meters_across_cell = layer.meters_across_cell();
    const double meters_across_sector = layer.meters_across_sector();
    // sets a distinct debug value for each sector
    // iteration progress West => East, South => North;  Cell 0,0 and Sector 0,0 both start in the Southwest Corner.
    for (double sector_northing = visible.min.northing; sector_northing < visible.max.northing; sector_northing += meters_across_sector ) {
        for (double sector_easting = visible.min.easting; sector_easting < visible.max.easting; sector_easting += meters_across_sector ) {
            const uint8_t sector_value = (static_cast<uint8_t>((sector_easting - visible.min.easting)/meters_across_sector) << 4) 
                                        + static_cast<uint8_t>((sector_northing - visible.min.northing)/meters_across_sector);
            for (double cell_northing = sector_northing + 0.5*meters_across_cell; cell_northing < (sector_northing + meters_across_sector); cell_northing += meters_across_cell ) {
                for (double cell_easting = sector_easting + 0.5*meters_across_cell; cell_easting < (sector_easting + meters_across_sector); cell_easting += meters_across_cell ) {
                    const LocalLocation store_at ( cell_easting, cell_northing );
                    layer.store( store_at, sector_value );
                }
            }
        }
    }
}

TEST_CASE( "DynamicGridLayer Default Initialization"){
    const DynamicGridLayer layer;

    REQUIRE(  4 == layer.cells_across_sector() );
    REQUIRE(  3 == layer.sectors_across_view() );
    REQUIRE(  9 == layer.sectors_per_view() );
    REQUIRE( 12 == layer.cells_across_view() );

    REQUIRE(  1.0 == Approx(layer.meters_across_cell()) );
    REQUIRE(  4.0 == Approx(layer.meters_across_sector()) );
    REQUIRE( 12.0 == Approx(layer.meters_across_view()) );
} // TEST_CASE


TEST_CASE( "DynamicGridLayer Can Track Bounds"){
    DynamicGridLayer layer;

    const BoundBox<LocalLocation> visible = {{0,0},{24,24}};
    layer.track( visible );

    REQUIRE(  8 == layer.cells_across_sector() );
    REQUIRE(  3 == layer.sectors_across_view() );
    REQUIRE(  9 == layer.sectors_per_view() );
    REQUIRE( 24 == layer.cells_across_view() );

    REQUIRE(  1.0 == Approx(layer.meters_across_cell()) );
    REQUIRE(  8.0 == Approx(layer.meters_across_sector()) );
    REQUIRE( 24.0 == Approx(layer.meters_across_view()) );
}

TEST_CASE( "DynamicGridLayer Can Set Visible Bounds"){
    DynamicGridLayer layer;

    const BoundBox<LocalLocation> visible = {{0,0},{24,24}};
    layer.view( visible );

    REQUIRE(  8 == layer.cells_across_sector() );
    REQUIRE(  3 == layer.sectors_across_view() );
    REQUIRE(  9 == layer.sectors_per_view() );
    REQUIRE( 24 == layer.cells_across_view() );

    REQUIRE(  1.0 == Approx(layer.meters_across_cell()) );
    REQUIRE(  8.0 == Approx(layer.meters_across_sector()) );
    REQUIRE( 24.0 == Approx(layer.meters_across_view()) );
}

TEST_CASE( "DynamicGridLayer Check Default Bounds"){
    const DynamicGridLayer layer;

    REQUIRE(  4 == layer.cells_across_sector() );
    REQUIRE(  3 == layer.sectors_across_view() );
    REQUIRE( 12 == layer.cells_across_view() );

    REQUIRE(  1.0 == Approx(layer.meters_across_cell()) );
    REQUIRE(  4.0 == Approx(layer.meters_across_sector()) );
    REQUIRE( 12.0 == Approx(layer.meters_across_view()) );

    REQUIRE( LocalLocation(  0.0,  0.0) == layer.visible().min );
    REQUIRE( LocalLocation( 12.0, 12.0) == layer.visible().max );

    { const LocalLocation loc(  0.0,  0.0 );  CHECK( layer.visible(loc) );}
    { const LocalLocation loc(  0.1,  0.1 );  CHECK( layer.visible(loc) );}
    { const LocalLocation loc( 11.9, 11.9 );  CHECK( layer.visible(loc) );}
    { const LocalLocation loc( 12.0, 12.0 );  CHECK( layer.visible(loc) );}
    { const LocalLocation loc(  6.0,  0.1 );  CHECK( layer.visible(loc) );}
    { const LocalLocation loc(  6.0, 12.0 );  CHECK( layer.visible(loc) );}
    { const LocalLocation loc(  0.0,  6.0 );  CHECK( layer.visible(loc) );}
    { const LocalLocation loc( 12.0,  6.0 );  CHECK( layer.visible(loc) );}
    { const LocalLocation loc(  1.0,  8.0 );  CHECK( layer.visible(loc) );}
    { const LocalLocation loc(  0.1,  3.0 );  CHECK( layer.visible(loc) );}
    { const LocalLocation loc(  1.0, 11.9 );  CHECK( layer.visible(loc) );}
    
} // TEST_CASE

TEST_CASE( "DynamicGridLayer Move Origin"){
    DynamicGridLayer layer;
    layer.origin({12,12});

    REQUIRE( LocalLocation( 12.0, 12.0) == layer.visible().min );
    REQUIRE( LocalLocation( 24.0, 24.0) == layer.visible().max );

    { const LocalLocation loc( 11.0, 11.0 );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc( 11.9, 11.9 );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc( 12.0, 12.0 );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 12.1, 12.1 );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 16.0, 16.0 );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 20.0, 20.0 );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 23.5, 23.5 );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 23.9, 23.9 );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 24.0, 24.0 );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 24.1, 24.1 );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc( 25.0, 25.0 );  CHECK( not layer.visible(loc) ); }
    
} // TEST_CASE

TEST_CASE( "DynamicGridLayer Resize Cell"){
    DynamicGridLayer layer;
    REQUIRE(  4 == layer.cells_across_sector() );
    REQUIRE(  3 == layer.sectors_across_view() );
    REQUIRE( 12 == layer.cells_across_view() );
    REQUIRE(  1.0 == Approx(layer.meters_across_cell()) );
    REQUIRE(  4.0 == Approx(layer.meters_across_sector()) );
    REQUIRE( 12.0 == Approx(layer.meters_across_view()) );

    layer.meters_across_cell(3.0);

    REQUIRE(  4 == layer.cells_across_sector() );
    REQUIRE(  3 == layer.sectors_across_view() );
    REQUIRE(  9 == layer.sectors_per_view() );
    REQUIRE( 12 == layer.cells_across_view() );

    CHECK(  3.0 == Approx(layer.meters_across_cell()) );
    CHECK( 12.0 == Approx(layer.meters_across_sector()) );
    CHECK( 36.0 == Approx(layer.meters_across_view()) );
} // TEST_CASE

TEST_CASE( "DynamicGridLayer Re-Sector the layer"){
    DynamicGridLayer layer;
    REQUIRE(  4 == layer.cells_across_sector() );
    REQUIRE(  3 == layer.sectors_across_view() );
    REQUIRE( 12 == layer.cells_across_view() );
    REQUIRE(  1.0 == Approx(layer.meters_across_cell()) );
    REQUIRE(  4.0 == Approx(layer.meters_across_sector()) );
    REQUIRE( 12.0 == Approx(layer.meters_across_view()) );

    layer.sectors_across_view(4);

    CHECK(  3 == layer.cells_across_sector() );
    CHECK(  4 == layer.sectors_across_view() );
    CHECK( 16 == layer.sectors_per_view() );
    CHECK( 12 == layer.cells_across_view() );

    REQUIRE(  1.0 == Approx(layer.meters_across_cell()) );
    REQUIRE(  4.0 == Approx(layer.meters_across_sector()) );
    REQUIRE( 12.0 == Approx(layer.meters_across_view()) );
} // TEST_CASE


TEST_CASE( "DynamicGridLayer Can Store and Retrieve Values"){
    DynamicGridLayer layer;
    REQUIRE(  4 == layer.cells_across_sector() );
    REQUIRE( 16 == layer.cells_per_sector() );
    REQUIRE(  3 == layer.sectors_across_view() );
    REQUIRE(  9 == layer.sectors_per_view() );
    REQUIRE( 12 == layer.cells_across_view() );
    REQUIRE( LocalLocation(  0.0,  0.0) == layer.visible().min );
    REQUIRE( LocalLocation( 12.0, 12.0) == layer.visible().max );

    populate_markers_per_cell( layer );

    // fmt::print("\n{}\n", layer.print_contents_by_location());

    { REQUIRE( not layer.visible({ -1.0, -1.0 }));}

    { REQUIRE( layer.visible({  0.0,  0.0 }));  CHECK(    0 == static_cast<int>(layer.get({  0.0,  0.0 }))); }
    { REQUIRE( layer.visible({  1.0,  0.0 }));  CHECK( 0x10 == static_cast<int>(layer.get({  1.0,  0.0 }))); }
    { REQUIRE( layer.visible({  2.0,  0.0 }));  CHECK( 0x20 == static_cast<int>(layer.get({  2.0,  0.0 }))); }
    { REQUIRE( layer.visible({  3.0,  0.0 }));  CHECK( 0x30 == static_cast<int>(layer.get({  3.0,  0.0 }))); }
    { REQUIRE( layer.visible({  2.0,  3.0 }));  CHECK( 0x23 == static_cast<int>(layer.get({  2.0,  3.0 }))); }
    { REQUIRE( layer.visible({  2.0,  4.0 }));  CHECK( 0x24 == static_cast<int>(layer.get({  2.0,  4.0 }))); }
    { REQUIRE( layer.visible({  2.0,  5.0 }));  CHECK( 0x25 == static_cast<int>(layer.get({  2.0,  5.0 }))); }
    { REQUIRE( layer.visible({  6.0,  6.0 }));  CHECK( 0x66 == static_cast<int>(layer.get({  6.0,  6.0 }))); }
    { REQUIRE( layer.visible({  7.0,  7.0 }));  CHECK( 0x77 == static_cast<int>(layer.get({  7.0,  7.0 }))); }
    { REQUIRE( layer.visible({  8.0,  8.0 }));  CHECK( 0x88 == static_cast<int>(layer.get({  8.0,  8.0 }))); }
    { REQUIRE( layer.visible({  9.0,  9.0 }));  CHECK( 0x99 == static_cast<int>(layer.get({  9.0,  9.0 }))); }
    { REQUIRE( layer.visible({ 10.0, 10.0 }));  CHECK( 0xAA == static_cast<int>(layer.get({ 10.0, 10.0 }))); }
    { REQUIRE( layer.visible({ 11.0, 11.0 }));  CHECK( 0xBB == static_cast<int>(layer.get({ 11.0, 11.0 }))); }
    { REQUIRE( layer.visible({ 11.5, 11.5 }));  CHECK( 0xBB == static_cast<int>(layer.get({ 11.5, 11.5 }))); }
    { REQUIRE( layer.visible({ 12.0, 12.0 }));  CHECK( 0xBB == static_cast<int>(layer.get({ 12.0, 12.0 }))); }

    { REQUIRE( not layer.visible({ 12.1, 12.1 }));}
    { REQUIRE( not layer.visible({ 13.0, 13.0 }));}

} // TEST_CASE

TEST_CASE( "DynamicGridLayer Can Store and Retrieve Values at novel precision"){
    DynamicGridLayer layer;
    layer.meters_across_cell( 10.0 );
    layer.track( {{0,0}, {144,144}} );

    CHECK(  5 == layer.cells_across_sector() );
    CHECK( 25 == layer.cells_per_sector() );
    CHECK(  3 == layer.sectors_across_view() );
    CHECK(  9 == layer.sectors_per_view() );
    CHECK( 15 == layer.cells_across_view() );

    CHECK(  10.0 == Approx(layer.meters_across_cell()) );
    CHECK(  50.0 == Approx(layer.meters_across_sector()) );
    CHECK( 150.0 == Approx(layer.meters_across_view()) );

    REQUIRE( LocalLocation(   0.0,   0.0) == layer.visible().min );
    REQUIRE( LocalLocation( 150.0, 150.0) == layer.visible().max );

    { REQUIRE( layer.visible({  1.0,  1.0 }));  CHECK( 0x80 == static_cast<int>(layer.get({  1.0,  1.0 }))); }
    { REQUIRE( layer.visible({  5.0,  5.0 }));  CHECK( 0x80 == static_cast<int>(layer.get({  5.0,  5.0 }))); }
    { REQUIRE( layer.visible({  7.0,  7.0 }));  CHECK( 0x80 == static_cast<int>(layer.get({  7.0,  7.0 }))); }
    { REQUIRE( layer.visible({ 12.0, 12.0 }));  CHECK( 0x80 == static_cast<int>(layer.get({ 12.0, 12.0 }))); }
    { REQUIRE( layer.visible({ 51.0, 51.0 }));  CHECK( 0x80 == static_cast<int>(layer.get({ 51.0, 51.0 }))); }
    { REQUIRE( layer.visible({ 88.0, 88.0 }));  CHECK( 0x80 == static_cast<int>(layer.get({ 88.0, 88.0 }))); }
    { REQUIRE( layer.visible({120.0, 20.0 }));  CHECK( 0x80 == static_cast<int>(layer.get({120.0, 20.0 }))); }

    populate_markers_per_sector( layer );

    // fmt::print("\n{}\n", layer.print_contents_by_cell());
    // fmt::print("\n{}\n", layer.print_contents_by_location());

    { REQUIRE( layer.visible({  1.0,  1.0 }));  CHECK( 0x00 == static_cast<int>(layer.get({  1.0,  1.0 }))); }
    { REQUIRE( layer.visible({  5.0,  5.0 }));  CHECK( 0x00 == static_cast<int>(layer.get({  5.0,  5.0 }))); }
    { REQUIRE( layer.visible({  7.0,  7.0 }));  CHECK( 0x00 == static_cast<int>(layer.get({  7.0,  7.0 }))); }
    { REQUIRE( layer.visible({ 12.0, 12.0 }));  CHECK( 0x00 == static_cast<int>(layer.get({ 12.0, 12.0 }))); }
    { REQUIRE( layer.visible({ 51.0, 51.0 }));  CHECK( 0x11 == static_cast<int>(layer.get({ 51.0, 51.0 }))); }
    { REQUIRE( layer.visible({ 88.0, 88.0 }));  CHECK( 0x11 == static_cast<int>(layer.get({ 88.0, 88.0 }))); }
    { REQUIRE( layer.visible({120.0, 20.0 }));  CHECK( 0x20 == static_cast<int>(layer.get({120.0, 20.0 }))); }

} // TEST_CASE
