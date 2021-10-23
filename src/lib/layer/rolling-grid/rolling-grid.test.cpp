// GPL v3 (c) 2021, Daniel Williams 

#include <cmath>
#include <iostream>
#include <sstream>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
using Catch::Approx;

#include "geometry/bound-box.hpp"
#include "geometry/polygon.hpp"
#include "layer/grid-index.hpp"

#include "rolling-grid-layer.hpp"
#include "rolling-grid-sector.hpp"

using chartbox::geometry::BoundBox;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::Polygon;
using chartbox::geometry::UTMLocation;
using chartbox::layer::GridIndex;
using chartbox::layer::rolling::RollingGridSector;
using chartbox::layer::rolling::RollingGridLayer;

// ============ ============  Rolling-Grid-Sector-Tests  ============ ============
TEST_CASE( "Default Initialize RollingGridSector"){
    RollingGridSector<4> sector;
    REQUIRE( sector.size() == 16 );
    // nothing else is guaranteed to be initialized
} // TEST_CASE

TEST_CASE( "Explicitly Initialize RollingGridSector"){
    RollingGridSector<4> sector( 7);
    REQUIRE( sector.size() == 16 );

    CHECK( sector.contains({0,0}) );
    CHECK( 7 ==  sector.get({0,0}) );
    CHECK( sector.contains({2,2}) );
    CHECK( 7 ==  sector.get({2,2}) );
    CHECK( sector.contains({2,3}) );
    CHECK( 7 ==  sector.get({2,3}) );
    CHECK( sector.contains({3,3}) );
    CHECK( 7 ==  sector.get({3,3}) );
    
    CHECK( not sector.contains({5,8}) );
} // TEST_CASE

TEST_CASE( "RollingGridSector populates and look-ups correctly "){
    RollingGridSector<4> sector;

    uint8_t value = 0;
    GridIndex index;
    for( uint32_t j=0; j<sector.cells_across(); ++j){
        index.row = j;
        for( uint32_t i=0; i<sector.cells_across(); ++i){
            index.column = i;
            sector.set( index, value );
            ++value; 
        }
    }

    CHECK( sector.contains({0,0}) );    CHECK( 0 == sector.get({0,0}) );
    CHECK( sector.contains({1,1}) );    CHECK( 5 == sector.get({1,1}) );
    CHECK( sector.contains({3,1}) );    CHECK( 7 == sector.get({3,1}) );
    CHECK( sector.contains({0,3}) );    CHECK( 12 == sector.get({0,3}) );
}

TEST_CASE( "RollingGridSector sets & gets correctly "){
    RollingGridSector<4> sector( 4 );

    CHECK( sector.contains({1,1}) );
    CHECK( 4 == sector.get({1,1}) );
    CHECK( sector.contains({1,2}) );
    CHECK( 4 == sector.get({1,2}) );
    CHECK( sector.contains({1,3}) );
    CHECK( 4 == sector.get({1,3}) );
    
    sector.set( {1,2}, 6 );

    CHECK( sector.contains({1,1}) );
    CHECK( 4 == sector.get({1,1}) );
    CHECK( sector.contains({1,2}) );
    CHECK( 6 == sector.get({1,2}) );
    CHECK( sector.contains({1,3}) );
    CHECK( 4 == sector.get({1,3}) );

} // TEST_CASE


// ============ ============ ============ ============  Rolling-Grid-Layer-Tests  ============ ============ ============ ============
template<typename T>
void populate_markers_per_cell( T& layer ) {
    const auto& visible = layer.visible();
    const double meters_across_cell = layer.meters_across_cell();
    // sets a distinct debug value for each cell
    // iteration progress West => East, South => North;  Cell 0,0 starts in the Southwest Corner.
    for (double northing = visible.min.northing + 0.5*meters_across_cell; northing < visible.max.northing; northing += meters_across_cell ) {
        for (double easting = visible.min.easting + 0.5*meters_across_cell; easting < visible.max.easting; easting += meters_across_cell) {
            const uint8_t value = (static_cast<uint8_t>(easting - visible.min.easting) << 4) + static_cast<uint8_t>(northing - visible.min.northing);
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

TEST_CASE( "Verify RollingGridLayer Default Initialization"){
    RollingGridLayer<4> layer;

    REQUIRE(  1.0 == Approx(layer.meters_across_cell()) );
    REQUIRE(  4.0 == Approx(layer.meters_across_sector()) );
    REQUIRE( 20.0 == Approx(layer.meters_across_view()) );
    REQUIRE(    4 == layer.cells_across_sector() );
    REQUIRE(    5 == layer.sectors_across_view() );
    REQUIRE(   20 == layer.cells_across_view() );

    CHECK( LocalLocation(  0.0,  0.0) == layer.tracked().min );
    CHECK( LocalLocation( 20.0, 20.0) == layer.tracked().max );
    CHECK( LocalLocation(  0.0,  0.0) == layer.visible().min );
    CHECK( LocalLocation( 20.0, 20.0) == layer.visible().max );
}

TEST_CASE( "Verify Larger RollingGridLayers allocates 64x64 tiles correctly"){
    RollingGridLayer<64> layer;
    REQUIRE(    1.0 == Approx(layer.meters_across_cell()) );
    REQUIRE(   64.0 == Approx(layer.meters_across_sector()) );
    REQUIRE(  320.0 == Approx(layer.meters_across_view()) );
    REQUIRE(   64 == layer.cells_across_sector() );
    REQUIRE(    5 == layer.sectors_across_view() );
    REQUIRE(  320 == layer.cells_across_view() );
}

TEST_CASE( "Verify Larger RollingGridLayers allocates 256x256 tiles correctly"){
    RollingGridLayer<256> layer;
    REQUIRE(    1.0 == Approx(layer.meters_across_cell()) );
    REQUIRE(  256.0 == Approx(layer.meters_across_sector()) );
    REQUIRE( 1280.0 == Approx(layer.meters_across_view()) );
    REQUIRE(  256 == layer.cells_across_sector() );
    REQUIRE(    5 == layer.sectors_across_view() );
    REQUIRE( 1280 == layer.cells_across_view() );
}

TEST_CASE( "Verify Larger RollingGridLayers allocates 1024x1024 tiles correctly"){
    RollingGridLayer<1024> layer;
    REQUIRE(    1.0 == Approx(layer.meters_across_cell()) );
    REQUIRE( 1024.0 == Approx(layer.meters_across_sector()) );
    REQUIRE( 5120.0 == Approx(layer.meters_across_view()) );
    REQUIRE( 1024 == layer.cells_across_sector() );
    REQUIRE(    5 == layer.sectors_across_view() );
    REQUIRE( 5120 == layer.cells_across_view() );
}

TEST_CASE( "Verify Layer Set Bounds"){
    RollingGridLayer<4> layer;
    layer.track( BoundBox<LocalLocation>( {0,0}, {48,48} ));

    REQUIRE(  1.0 == Approx(layer.meters_across_cell()) );
    REQUIRE(  4.0 == Approx(layer.meters_across_sector()) );
    REQUIRE( 20.0 == Approx(layer.meters_across_view()) );
    REQUIRE(    4 == layer.cells_across_sector() );
    REQUIRE(    5 == layer.sectors_across_view() );
    REQUIRE(   20 == layer.cells_across_view() );

    CHECK( LocalLocation(  0.0,  0.0) == layer.tracked().min );
    CHECK( LocalLocation( 48.0, 48.0) == layer.tracked().max );
    CHECK( LocalLocation( 12.0, 12.0) == layer.visible().min );
    CHECK( LocalLocation( 32.0, 32.0) == layer.visible().max );
}

TEST_CASE( "Verify RollingGridLayer Can Test Bounds"){
    RollingGridLayer<4> layer;
    layer.track( BoundBox<LocalLocation>( {0,0}, {48,48} ));

    CHECK( LocalLocation(  0.0,  0.0) == layer.tracked().min );
    CHECK( LocalLocation( 48.0, 48.0) == layer.tracked().max );
    CHECK( LocalLocation( 12.0, 12.0) == layer.visible().min );
    CHECK( LocalLocation( 32.0, 32.0) == layer.visible().max );

    { const LocalLocation loc( -0.1, -0.1 );  CHECK( not layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc(  0.0,  0.0 );  CHECK(     layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc(  0.1,  0.1 );  CHECK(     layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc( 11.0, 11.0 );  CHECK(     layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc( 11.9, 11.9 );  CHECK(     layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc( 12.0, 12.0 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 12.1, 12.1 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 12.5, 12.5 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 15.0, 15.0 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 20.0, 20.0 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 25.0, 25.0 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 30.0, 30.0 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 31.5, 31.5 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 31.9, 31.9 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 32.0, 32.0 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 32.1, 32.1 );  CHECK(     layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc( 32.5, 32.5 );  CHECK(     layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc( 33.0, 33.0 );  CHECK(     layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc( 47.9, 47.9 );  CHECK(     layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc( 48.0, 48.0 );  CHECK(     layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc( 48.1, 48.1 );  CHECK( not layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
} // TEST_CASE

TEST_CASE( "Verify Layer Set Offset UTM Bounds"){
    RollingGridLayer<4> layer;
    layer.track( BoundBox<UTMLocation>({ 276550.31, 4553825.85},
                                       { 276612.0, 4553888.0}));

    CHECK( LocalLocation(  0.0,  0.0) == layer.tracked().min );
    CHECK( LocalLocation( 64.0, 64.0) == layer.tracked().max );
    CHECK( LocalLocation( 20.0, 20.0) == layer.visible().min );
    CHECK( LocalLocation( 40.0, 40.0) == layer.visible().max );

    { const LocalLocation loc( -0.1, -0.1 );  CHECK( not layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc(  0.0,  0.0 );  CHECK(     layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc(  0.1,  0.1 );  CHECK(     layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc( 10.0, 10.0 );  CHECK(     layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc( 19.9, 19.9 );  CHECK(     layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc( 20.0, 20.0 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 30.0, 30.0 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 39.9, 39.9 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 40.0, 40.0 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 40.1, 40.1 );  CHECK(     layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc( 50.0, 50.0 );  CHECK(     layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc( 63.9, 63.9 );  CHECK(     layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc( 64.0, 64.0 );  CHECK(     layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc( 64.1, 64.1 );  CHECK( not layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
} // TEST_CASE

TEST_CASE( "Verify RollingGridLayer can move view within tracked area" ){
    RollingGridLayer<4> layer;
    layer.track( BoundBox<LocalLocation>( {0,0}, {100,100} ));
    REQUIRE( LocalLocation(   0.0,   0.0) == layer.tracked().min );
    REQUIRE( LocalLocation(  40.0,  40.0) == layer.visible().min );
    REQUIRE( LocalLocation(  60.0,  60.0) == layer.visible().max );
    REQUIRE( LocalLocation( 100.0, 100.0) == layer.tracked().max );
    
    layer.view( {20,20} );
    CHECK( LocalLocation(   0.0,   0.0) == layer.tracked().min );
    CHECK( LocalLocation(  20.0,  20.0) == layer.visible().min );
    CHECK( LocalLocation(  40.0,  40.0) == layer.visible().max );
    CHECK( LocalLocation( 100.0, 100.0) == layer.tracked().max );
} // TEST_CASE

TEST_CASE( "Verify RollingGridLayer can track bounds matching its viewport" ){
    RollingGridLayer<4> layer;

    layer.track( BoundBox<LocalLocation>( {0,0}, {20,20} ));

    CHECK( LocalLocation(  0.0,  0.0) == layer.tracked().min );
    CHECK( LocalLocation( 20.0, 20.0) == layer.tracked().max );
    CHECK( LocalLocation(  0.0,  0.0) == layer.visible().min );
    CHECK( LocalLocation( 20.0, 20.0) == layer.visible().max );
    
    { const LocalLocation loc( -0.1, -0.1 );  CHECK( not layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc(  0.0,  0.0 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc(  0.1,  0.1 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc(  3.0,  3.0 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 12.0, 12.0 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 15.0, 15.0 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 19.9, 19.9 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 20.0, 20.0 );  CHECK(     layer.tracked(loc) );  CHECK(     layer.visible(loc) ); }
    { const LocalLocation loc( 20.1, 20.1 );  CHECK( not layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
    { const LocalLocation loc( 21.0, 21.0 );  CHECK( not layer.tracked(loc) );  CHECK( not layer.visible(loc) ); }
} // TEST_CASE

TEST_CASE( "Verify RollingGridLayer Stores Values"){
    RollingGridLayer<4> layer;

    layer.track( BoundBox<LocalLocation>( {0,0}, {20,20} ));
    CHECK( LocalLocation(  0.0,  0.0) == layer.tracked().min );
    CHECK( LocalLocation( 20.0, 20.0) == layer.tracked().max );
    CHECK( LocalLocation(  0.0,  0.0) == layer.visible().min );
    CHECK( LocalLocation( 20.0, 20.0) == layer.visible().max );

    populate_markers_per_cell( layer );

    // sector 0,0
    REQUIRE( layer.tracked({0.5, 0.5}) );
    CHECK( 0 == layer.get( 0.5, 0.5) );
    REQUIRE( layer.tracked({1.5, 1.5}));
    CHECK( 0x11 == layer.get( 1.5, 1.5 ) );
    REQUIRE(  layer.tracked({ 2.5, 2.5 }) );
    CHECK( 0x22 == layer.get( 2.5, 2.5 ) );
    REQUIRE(  layer.tracked( { 3.5, 3.5 }) );
    CHECK( 0x33 == layer.get({ 3.5, 3.5 }) );

    // sector 1,1
    REQUIRE(  layer.tracked( { 4.5, 4.5 }) );
    CHECK( 0x44 == layer.get({ 4.5, 4.5 }) );
    REQUIRE(  layer.tracked( { 5.5, 5.5 }) );
    CHECK( 0x55 == layer.get({ 5.5, 5.5 }) );
    REQUIRE( layer.tracked({  6.5, 6.5 }));
    CHECK( 0x66 == layer.get( 6.5, 6.5 ));
    REQUIRE( layer.tracked({ 7.5, 7.5 }));
    CHECK( 0x77 == layer.get( 7.5, 7.5 ));

    // sector 2.2
    REQUIRE(  layer.tracked( {  8.5,  8.5 }) );
    CHECK( 0x88 == layer.get({  8.5,  8.5 }) );
    REQUIRE(  layer.tracked( {  9.5,  9.5 }) );
    CHECK( 0x99 == layer.get({  9.5,  9.5 }) );
    REQUIRE(  layer.tracked( { 10.5, 10.5 }) );
    CHECK( 0xAA == layer.get({ 10.5, 10.5 }) );
    REQUIRE(  layer.tracked( { 11.5, 11.5 }) );
    CHECK( 0xBB == layer.get({ 11.5, 11.5 }) );

} // TEST_CASE


TEST_CASE( "Verify Layer Can Scroll"){

    SECTION( "Verify RollingGrid Can Scroll East" ){
        RollingGridLayer<4> layer;
        REQUIRE(    4 == layer.cells_across_sector() );
        REQUIRE(    5 == layer.sectors_across_view() );
        REQUIRE(   20 == layer.cells_across_view() );
        
        layer.track( BoundBox<LocalLocation>( {0,0}, {48,48} ));
        CHECK( LocalLocation(  0.0,  0.0) == layer.tracked().min );
        CHECK( LocalLocation( 48.0, 48.0) == layer.tracked().max );
        CHECK( LocalLocation( 12.0, 12.0) == layer.visible().min );
        CHECK( LocalLocation( 32.0, 32.0) == layer.visible().max );
        
        // sets a distinct debug value for each cell
        populate_markers_per_sector( layer );

        // 12,16,20,24,28,  ... 32
        {   // Region A: to-be-cached
            { const LocalLocation loc( 15.5, 30.5 );  CHECK(layer.visible(loc)); CHECK( 0x04 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 15.5, 25.5 );  CHECK(layer.visible(loc)); CHECK( 0x03 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 15.5, 22.5 );  CHECK(layer.visible(loc)); CHECK( 0x02 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 15.5, 18.5 );  CHECK(layer.visible(loc)); CHECK( 0x01 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 15.5, 15.5 );  CHECK(layer.visible(loc)); CHECK( 0x00 == static_cast<int>(layer.get(loc)) ); }

            // Region B: to-be-loaded
            CHECK( not layer.visible(LocalLocation( 32.5, 30.5 ) ));
            CHECK( not layer.visible(LocalLocation( 32.5, 25.5 ) ));
            CHECK( not layer.visible(LocalLocation( 32.5, 22.5 ) ));
            CHECK( not layer.visible(LocalLocation( 32.5, 18.5 ) ));
            CHECK( not layer.visible(LocalLocation( 32.5, 15.5 ) ));

            // test target:
            layer.scroll_east();

            REQUIRE( LocalLocation( 16.0, 12.0) == layer.visible().min );
            REQUIRE( LocalLocation( 36.0, 32.0) == layer.visible().max );
            
            // Region A: just-cached
            CHECK( not layer.visible(LocalLocation( 15.5, 30.5 ) ));
            CHECK( not layer.visible(LocalLocation( 15.5, 25.5 ) ));
            CHECK( not layer.visible(LocalLocation( 15.5, 22.5 ) ));
            CHECK( not layer.visible(LocalLocation( 15.5, 18.5 ) ));
            CHECK( not layer.visible(LocalLocation( 15.5, 15.5 ) ));

            // Region B: just-loaded
            { const LocalLocation loc( 32.5, 30.5 );  CHECK(layer.visible(loc)); CHECK( 0x80 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 32.5, 25.5 );  CHECK(layer.visible(loc)); CHECK( 0x80 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 32.5, 22.5 );  CHECK(layer.visible(loc)); CHECK( 0x80 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 32.5, 18.5 );  CHECK(layer.visible(loc)); CHECK( 0x80 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 32.5, 15.5 );  CHECK(layer.visible(loc)); CHECK( 0x80 == static_cast<int>(layer.get(loc)) ); }
        }
    }

    SECTION( "Verify RollingGrid Can Scroll North" ){
        RollingGridLayer<4> layer;


        layer.track( BoundBox<LocalLocation>( {0,0}, {48,48} ));
        CHECK( LocalLocation(  0.0,  0.0) == layer.tracked().min );
        CHECK( LocalLocation( 48.0, 48.0) == layer.tracked().max );
        CHECK( LocalLocation( 12.0, 12.0) == layer.visible().min );
        CHECK( LocalLocation( 32.0, 32.0) == layer.visible().max );

        // sets a distinct debug value for each cell
        populate_markers_per_sector( layer );

        {   // Region A: to-be-cached
            { const LocalLocation loc( 15.5, 14.5 );  CHECK(layer.visible(loc)); CHECK( 0x00 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 18.5, 14.5 );  CHECK(layer.visible(loc)); CHECK( 0x10 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 22.5, 14.5 );  CHECK(layer.visible(loc)); CHECK( 0x20 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 25.5, 14.5 );  CHECK(layer.visible(loc)); CHECK( 0x30 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 30.5, 14.5 );  CHECK(layer.visible(loc)); CHECK( 0x40 == static_cast<int>(layer.get(loc)) ); }

            // Region B: to-be-loaded
            CHECK( not layer.visible(LocalLocation( 30.5, 34.5 ) ));
            CHECK( not layer.visible(LocalLocation( 25.5, 34.5 ) ));
            CHECK( not layer.visible(LocalLocation( 22.5, 34.5 ) ));
            CHECK( not layer.visible(LocalLocation( 18.5, 34.5 ) ));
            CHECK( not layer.visible(LocalLocation( 15.5, 34.5 ) ));

            layer.scroll_north();

            REQUIRE( LocalLocation( 12.0, 16.0) == layer.visible().min );
            REQUIRE( LocalLocation( 32.0, 36.0) == layer.visible().max );

            // Region A: just-cached
            CHECK( not layer.visible(LocalLocation( 30.5, 10.5 ) ));
            CHECK( not layer.visible(LocalLocation( 25.5, 10.5 ) ));
            CHECK( not layer.visible(LocalLocation( 22.5, 10.5 ) ));
            CHECK( not layer.visible(LocalLocation( 18.5, 10.5 ) ));
            CHECK( not layer.visible(LocalLocation( 15.5, 10.5 ) ));

            // Region B: just-loaded
            { const LocalLocation loc( 15.5, 34.5 );  CHECK( layer.visible(loc) ); CHECK( 0x80 == layer.get(loc) ); }
            { const LocalLocation loc( 18.5, 34.5 );  CHECK( layer.visible(loc) ); CHECK( 0x80 == layer.get(loc) ); }
            { const LocalLocation loc( 22.5, 34.5 );  CHECK( layer.visible(loc) ); CHECK( 0x80 == layer.get(loc) ); }
            { const LocalLocation loc( 25.5, 34.5 );  CHECK( layer.visible(loc) ); CHECK( 0x80 == layer.get(loc) ); }
            { const LocalLocation loc( 30.5, 34.5 );  CHECK( layer.visible(loc) ); CHECK( 0x80 == layer.get(loc) ); }
        }
    }

    SECTION( "Verify RollingGrid Can Scroll South" ){
        RollingGridLayer<4> layer;

        layer.track( BoundBox<LocalLocation>( {0,0}, {48,48} ));
        CHECK( LocalLocation(  0.0,  0.0) == layer.tracked().min );
        CHECK( LocalLocation( 48.0, 48.0) == layer.tracked().max );
        CHECK( LocalLocation( 12.0, 12.0) == layer.visible().min );
        CHECK( LocalLocation( 32.0, 32.0) == layer.visible().max );

        // sets a distinct debug value for each cell
        populate_markers_per_sector( layer );

        {   // Region A: to-be-cached
            { const LocalLocation loc( 15.5, 30.5 );  CHECK(layer.visible(loc)); CHECK( 0x04 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 18.5, 30.5 );  CHECK(layer.visible(loc)); CHECK( 0x14 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 22.5, 30.5 );  CHECK(layer.visible(loc)); CHECK( 0x24 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 25.5, 30.5 );  CHECK(layer.visible(loc)); CHECK( 0x34 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 30.5, 30.5 );  CHECK(layer.visible(loc)); CHECK( 0x44 == static_cast<int>(layer.get(loc)) ); }

            // Region B: to-be-loaded
            CHECK( not layer.visible(LocalLocation( 30.5, 10.5 ) ));
            CHECK( not layer.visible(LocalLocation( 25.5, 10.5 ) ));
            CHECK( not layer.visible(LocalLocation( 22.5, 10.5 ) ));
            CHECK( not layer.visible(LocalLocation( 18.5, 10.5 ) ));
            CHECK( not layer.visible(LocalLocation( 15.5, 10.5 ) ));

            // test target:
            layer.scroll_south();

            REQUIRE( LocalLocation( 12.0,  8.0) == layer.visible().min );
            REQUIRE( LocalLocation( 32.0, 28.0) == layer.visible().max );
            
            // Region A: just-cached
            CHECK( not layer.visible(LocalLocation( 30.5, 30.5 ) ));
            CHECK( not layer.visible(LocalLocation( 25.5, 30.5 ) ));
            CHECK( not layer.visible(LocalLocation( 22.5, 30.5 ) ));
            CHECK( not layer.visible(LocalLocation( 18.5, 30.5 ) ));
            CHECK( not layer.visible(LocalLocation( 15.5, 30.5 ) ));

            // Region B: just-loaded
            { const LocalLocation loc( 15.5, 10.5 );  CHECK(layer.visible(loc)); CHECK( 0x80 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 18.5, 10.5 );  CHECK(layer.visible(loc)); CHECK( 0x80 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 22.5, 10.5 );  CHECK(layer.visible(loc)); CHECK( 0x80 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 25.5, 10.5 );  CHECK(layer.visible(loc)); CHECK( 0x80 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 30.5, 10.5 );  CHECK(layer.visible(loc)); CHECK( 0x80 == static_cast<int>(layer.get(loc)) ); }
        }
    }

    SECTION( "Verify RollingGrid Can Scroll West" ){
        RollingGridLayer<4> layer;

        layer.track( BoundBox<LocalLocation>( {0,0}, {48,48} ));
        CHECK( LocalLocation(  0.0,  0.0) == layer.tracked().min );
        CHECK( LocalLocation( 48.0, 48.0) == layer.tracked().max );
        CHECK( LocalLocation( 12.0, 12.0) == layer.visible().min );
        CHECK( LocalLocation( 32.0, 32.0) == layer.visible().max );

        // sets a distinct debug value for each cell
        populate_markers_per_sector( layer );

        {   // Region A: to-be-cached
            { const LocalLocation loc( 30.5, 30.5 );  CHECK(layer.visible(loc)); CHECK( 0x44 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 30.5, 25.5 );  CHECK(layer.visible(loc)); CHECK( 0x43 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 30.5, 22.5 );  CHECK(layer.visible(loc)); CHECK( 0x42 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 30.5, 18.5 );  CHECK(layer.visible(loc)); CHECK( 0x41 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 30.5, 15.5 );  CHECK(layer.visible(loc)); CHECK( 0x40 == static_cast<int>(layer.get(loc)) ); }

            // Region B: to-be-loaded
            CHECK( not layer.visible(LocalLocation( 10.5, 30.5 ) ));
            CHECK( not layer.visible(LocalLocation( 10.5, 25.5 ) ));
            CHECK( not layer.visible(LocalLocation( 10.5, 22.5 ) ));
            CHECK( not layer.visible(LocalLocation( 10.5, 18.5 ) ));
            CHECK( not layer.visible(LocalLocation( 10.5, 15.5 ) ));

            layer.scroll_west();

            REQUIRE( LocalLocation(  8.0, 12.0) == layer.visible().min );
            REQUIRE( LocalLocation( 28.0, 32.0) == layer.visible().max );
            
            // Region A: just-cached
            CHECK( not layer.visible(LocalLocation( 30.5, 30.5 ) ));
            CHECK( not layer.visible(LocalLocation( 30.5, 25.5 ) ));
            CHECK( not layer.visible(LocalLocation( 30.5, 22.5 ) ));
            CHECK( not layer.visible(LocalLocation( 30.5, 18.5 ) ));
            CHECK( not layer.visible(LocalLocation( 30.5, 15.5 ) ));

            // Region B: just-loaded
            { const LocalLocation loc( 10.5, 30.5 );  CHECK(layer.visible(loc)); CHECK( 0x80 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 10.5, 25.5 );  CHECK(layer.visible(loc)); CHECK( 0x80 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 10.5, 22.5 );  CHECK(layer.visible(loc)); CHECK( 0x80 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 10.5, 18.5 );  CHECK(layer.visible(loc)); CHECK( 0x80 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 10.5, 15.5 );  CHECK(layer.visible(loc)); CHECK( 0x80 == static_cast<int>(layer.get(loc)) ); }
        }
    }
}
