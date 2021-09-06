// GPL v3 (c) 2021, Daniel Williams 

#include <cmath>
#include <iostream>
#include <sstream>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
using Catch::Approx;

#include "chart-box/geometry/bound-box.hpp"
#include "chart-box/geometry/polygon.hpp"
#include "rolling-grid-index.hpp"
#include "rolling-grid-layer.hpp"
#include "rolling-grid-sector.hpp"

using chartbox::geometry::BoundBox;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::Polygon;
using chartbox::geometry::UTMLocation;
 
using chartbox::layer::CellInSectorIndex;
using chartbox::layer::CellInViewIndex;
using chartbox::layer::Index2u32;
using chartbox::layer::RollingGridSector;
using chartbox::layer::RollingGridLayer;
using chartbox::layer::SectorInViewIndex;

// ============ ============  Rolling-Grid-Index-Tests  ============ ============
TEST_CASE( "RollingGridIndex Simple Initialization" ){
    const CellInViewIndex<2,2> viewIndex = {1,1};
    CHECK( viewIndex.offset() == 5 );
} // TEST_CASE

TEST_CASE( "RollingGridIndex can divide into sub-index "){
    const CellInViewIndex<2,2> viewIndex = {2,3};
    const auto sectorIndex = viewIndex.divide({0,0});
    CHECK( Index2u32(1,1) == sectorIndex.data() );
    CHECK( sectorIndex.offset() == 3 );
} // TEST_CASE

TEST_CASE( "RollingGridIndex can clamp into sub-index "){
    const CellInViewIndex<2,2> viewIndex = {2,3};
    const auto sectorIndex = viewIndex.zoom();
    CHECK( Index2u32(0,1) == sectorIndex.data() );
    CHECK( sectorIndex.offset() == 2);
} // TEST_CASE

TEST_CASE( "RollingGridIndex calculates storage offset "){
    CHECK(  0 == CellInSectorIndex<4>(0,0).offset() );
    CHECK(  1 == CellInSectorIndex<4>(1,0).offset() );
    CHECK(  2 == CellInSectorIndex<4>(2,0).offset() );
    CHECK(  3 == CellInSectorIndex<4>(3,0).offset() );
    CHECK(  4 == CellInSectorIndex<4>(0,1).offset() );
    CHECK(  5 == CellInSectorIndex<4>(1,1).offset() );
    CHECK( 10 == CellInSectorIndex<4>(2,2).offset() );
    CHECK( 15 == CellInSectorIndex<4>(3,3).offset() );
} // TEST_CASE

TEST_CASE( "RollingGridIndex wraps correctly in both directions "){
    const CellInViewIndex<2,3> viewIndex(2,3);

    //                           vvv Anchor vvv
    REQUIRE( Index2u32(1,1) == viewIndex.divide({0,0}).data() );
    REQUIRE( Index2u32(2,2) == viewIndex.divide({1,1}).data() );
    REQUIRE( Index2u32(0,0) == viewIndex.divide({2,2}).data() );

} // TEST_CASE


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
    CellInSectorIndex<4> index;
    for( uint32_t j=0; j<sector.dimension(); ++j){
        index.row = j;
        for( uint32_t i=0; i<sector.dimension(); ++i){
            index.column = i;
            sector.set( index, value );
            ++value; 
        }
    }

    CHECK( sector.contains({0,0}) );
    CHECK( 0 == sector.get({0,0}) );

    CHECK( sector.contains({1,1}) );
    CHECK( 5 == sector.get({1,1}) );

    CHECK( sector.contains({3,1}) );
    CHECK( 7 == sector.get({3,1}) );

    CHECK(  sector.contains({0,3}) );
    CHECK( 12 == sector.get({0,3}) );
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
    // sets a distinct debug value for each cell
    // iteration progress West => East, South => North;  Cell 0,0 starts in the Southwest Corner.
    for (double northing = layer.visible().min.northing + 0.5*layer.meters_across_cell; northing < layer.visible().max.northing; northing += layer.meters_across_cell ) {
        for (double easting = layer.visible().min.easting + 0.5*layer.meters_across_cell; easting < layer.visible().max.easting; easting += layer.meters_across_cell) {
            const uint8_t value = (static_cast<uint8_t>(easting-layer.visible().min.easting) << 4) + static_cast<uint8_t>(northing - layer.visible().min.northing);
            layer.store( {easting, northing}, value );
        }
    }
}

template<typename T>
void populate_markers_per_sector( T& layer ) {
    // sets a distinct debug value for each sector
    // iteration progress West => East, South => North;  Cell 0,0 and Sector 0,0 both start in the Southwest Corner.
    for (double sector_northing = layer.visible().min.northing; sector_northing < layer.visible().max.northing; sector_northing += layer.meters_across_sector ) {
        for (double sector_easting = layer.visible().min.easting; sector_easting < layer.visible().max.easting; sector_easting += layer.meters_across_sector ) {
            const uint8_t sector_value = (static_cast<uint8_t>(sector_easting-layer.visible().min.easting) << 4) + static_cast<uint8_t>(sector_northing - layer.visible().min.northing);            
            for (double cell_northing = sector_northing + 0.5*layer.meters_across_cell; cell_northing < (sector_northing + layer.meters_across_sector); cell_northing += layer.meters_across_cell ) {
                for (double cell_easting = sector_easting + 0.5*layer.meters_across_cell; cell_easting < (sector_easting + layer.meters_across_sector); cell_easting += layer.meters_across_cell ) {
                    const LocalLocation store_at ( cell_easting, cell_northing );
                    layer.store( store_at, sector_value );
                }
            }
        }
    }
}
 
TEST_CASE( "Verify Layer Initialization Bounds"){
    // tracks the interval [ 0.0, 12.0 ]
    const BoundBox<UTMLocation> utm_bounds( {0,0}, {12,12} );
    RollingGridLayer layer( utm_bounds );

    // it's a constexpr variable.  So it _BETTER_ be.)
    REQUIRE( 1.0 == Approx(layer.precision) );

    // the UTM bounds are incidentally set, not in a realistic way
    // (i.e. not like they'd be set in a actual production use)

    REQUIRE( LocalLocation(  0.0,  0.0) == layer.tracked().min );
    REQUIRE( LocalLocation( 12.0, 12.0) == layer.tracked().max );
    REQUIRE( LocalLocation(  0.0,  0.0) == layer.visible().min );
    REQUIRE( LocalLocation( 12.0, 12.0) == layer.visible().max );

    {   const LocalLocation loc(  0.0,  0.0 );  CHECK( layer.tracked(loc) );  CHECK( layer.visible(loc) );
    }{  const LocalLocation loc(  0.1,  0.1 );  CHECK( layer.tracked(loc) );  CHECK( layer.visible(loc) );
    }{  const LocalLocation loc( 11.9, 11.9 );  CHECK( layer.tracked(loc) );  CHECK( layer.visible(loc) );
    }{  const LocalLocation loc( 12.0, 12.0 );  CHECK( layer.tracked(loc) );  CHECK( layer.visible(loc) );
    }{  const LocalLocation loc(  6.0,  0.1 );  CHECK( layer.tracked(loc) );  CHECK( layer.visible(loc) );
    }{  const LocalLocation loc(  6.0, 12.0 );  CHECK( layer.tracked(loc) );  CHECK( layer.visible(loc) );
    }{  const LocalLocation loc(  0.0,  6.0 );  CHECK( layer.tracked(loc) );  CHECK( layer.visible(loc) );
    }{  const LocalLocation loc( 12.0,  6.0 );  CHECK( layer.tracked(loc) );  CHECK( layer.visible(loc) );
    }{  const LocalLocation loc(  1.0,  8.0 );  CHECK( layer.tracked(loc) );  CHECK( layer.visible(loc) );
    }{  const LocalLocation loc(  0.1,  3.0 );  CHECK( layer.tracked(loc) );  CHECK( layer.visible(loc) );
    }{  const LocalLocation loc(  1.0, 11.9 );  CHECK( layer.tracked(loc) );  CHECK( layer.visible(loc) );
    }
} // TEST_CASE

TEST_CASE( "Verify Layer Stores Values"){
    // tracks the interval [ 0.0, 12.0 ]
    const BoundBox<UTMLocation> utm_bounds( {0,0}, {12,12} );
    RollingGridLayer layer( utm_bounds );

    // documentation / expected steps:
    REQUIRE( 1.0 == Approx(layer.precision) );

    REQUIRE( LocalLocation(  0.0,  0.0) == layer.tracked().min );
    REQUIRE( LocalLocation( 12.0, 12.0) == layer.tracked().max );
    REQUIRE( LocalLocation(  0.0,  0.0) == layer.visible().min );
    REQUIRE( LocalLocation( 12.0, 12.0) == layer.visible().max );

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
        const BoundBox<UTMLocation> utm_bounds( {0,0}, {64,64} );
        RollingGridLayer layer( utm_bounds );

        REQUIRE( LocalLocation(  0.0,  0.0) == layer.tracked().min );
        REQUIRE( LocalLocation( 64.0, 64.0) == layer.tracked().max );

        REQUIRE( LocalLocation( 24.0, 24.0) == layer.visible().min );
        REQUIRE( LocalLocation( 36.0, 36.0) == layer.visible().max );
        
        // sets a distinct debug value for each cell
        populate_markers_per_sector( layer );

        {   // Region A: to-be-cached
            { const LocalLocation loc( 25.5, 26.5 );  CHECK(layer.visible(loc)); CHECK( 0x00 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 25.5, 30.5 );  CHECK(layer.visible(loc)); CHECK( 0x04 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 25.5, 34.5 );  CHECK(layer.visible(loc)); CHECK( 0x08 == static_cast<int>(layer.get(loc)) ); }

            // Region B: to-be-loaded
            { const LocalLocation loc( 38.5, 26.5 );  CHECK( not layer.visible(loc) ); }
            { const LocalLocation loc( 38.5, 30.5 );  CHECK( not layer.visible(loc) ); }
            { const LocalLocation loc( 38.5, 34.5 );  CHECK( not layer.visible(loc) ); }

            // test target:
            layer.scroll_east();

            REQUIRE( LocalLocation( 28.0, 24.0) == layer.visible().min );
            REQUIRE( LocalLocation( 40.0, 36.0) == layer.visible().max );
            
            // Region A: just-cached
            { const LocalLocation loc( 25.5, 26.5 );  CHECK( not layer.visible(loc)); }
            { const LocalLocation loc( 25.5, 30.5 );  CHECK( not layer.visible(loc)); }
            { const LocalLocation loc( 25.5, 34.5 );  CHECK( not layer.visible(loc)); }

            // Region B: just-loaded
            { const LocalLocation loc( 38.5, 26.5 );  CHECK( layer.visible(loc) ); CHECK( 0x80 == layer.get(loc) ); }
            { const LocalLocation loc( 38.5, 30.5 );  CHECK( layer.visible(loc) ); CHECK( 0x80 == layer.get(loc) ); }
            { const LocalLocation loc( 38.5, 34.5 );  CHECK( layer.visible(loc) ); CHECK( 0x80 == layer.get(loc) ); }
        }
    }

    SECTION( "Verify RollingGrid Can Scroll North" ){
        const BoundBox<UTMLocation> utm_bounds( {0,0}, {64,64} );
        RollingGridLayer layer( utm_bounds );

        REQUIRE( LocalLocation(  0.0,  0.0) == layer.tracked().min );
        REQUIRE( LocalLocation( 64.0, 64.0) == layer.tracked().max );

        REQUIRE( LocalLocation( 24.0, 24.0) == layer.visible().min );
        REQUIRE( LocalLocation( 36.0, 36.0) == layer.visible().max );
        

        // sets a distinct debug value for each cell
        populate_markers_per_sector( layer );

        {   // Region A: to-be-cached
            { const LocalLocation loc( 26.5, 25.5 );  CHECK(layer.visible(loc)); CHECK( 0x00 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 30.5, 25.5 );  CHECK(layer.visible(loc)); CHECK( 0x40 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 34.5, 25.5 );  CHECK(layer.visible(loc)); CHECK( 0x80 == static_cast<int>(layer.get(loc)) ); }

            // Region B: to-be-loaded
            { const LocalLocation loc( 26.5, 38.5 );  CHECK( not layer.visible(loc) ); }
            { const LocalLocation loc( 30.5, 38.5 );  CHECK( not layer.visible(loc) ); }
            { const LocalLocation loc( 34.5, 38.5 );  CHECK( not layer.visible(loc) ); }

            layer.scroll_north();

            REQUIRE( LocalLocation( 24.0, 28.0) == layer.visible().min );
            REQUIRE( LocalLocation( 36.0, 40.0) == layer.visible().max );

            // Region A: just-cached
            { const LocalLocation loc( 26.5, 25.5 );  CHECK( not layer.visible(loc) ); }
            { const LocalLocation loc( 30.5, 25.5 );  CHECK( not layer.visible(loc) ); }
            { const LocalLocation loc( 34.5, 25.5 );  CHECK( not layer.visible(loc) ); }

            // Region B: just-loaded
            { const LocalLocation loc( 26.5, 38.5 );  CHECK( layer.visible(loc) ); CHECK( 0x80 == layer.get(loc) ); }
            { const LocalLocation loc( 30.5, 38.5 );  CHECK( layer.visible(loc) ); CHECK( 0x80 == layer.get(loc) ); }
            { const LocalLocation loc( 34.5, 38.5 );  CHECK( layer.visible(loc) ); CHECK( 0x80 == layer.get(loc) ); }
        }
    }

    SECTION( "Verify RollingGrid Can Scroll South" ){
        const BoundBox<UTMLocation> utm_bounds( {0,0}, {64,64} );
        RollingGridLayer layer( utm_bounds );

        REQUIRE( LocalLocation(  0.0,  0.0) == layer.tracked().min );
        REQUIRE( LocalLocation( 64.0, 64.0) == layer.tracked().max );

        REQUIRE( LocalLocation( 24.0, 24.0) == layer.visible().min );
        REQUIRE( LocalLocation( 36.0, 36.0) == layer.visible().max );
        
        // sets a distinct debug value for each cell
        populate_markers_per_sector( layer );

        {   // Region A: to-be-cached
            { const LocalLocation loc( 26.5, 34.5 );  CHECK(layer.visible(loc)); CHECK( 0x08 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 30.5, 34.5 );  CHECK(layer.visible(loc)); CHECK( 0x48 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 34.5, 34.5 );  CHECK(layer.visible(loc)); CHECK( 0x88 == static_cast<int>(layer.get(loc)) ); }

            // Region B: to-be-loaded
            { const LocalLocation loc( 26.5, 21.5 );  CHECK( not layer.visible(loc) ); }
            { const LocalLocation loc( 30.5, 21.5 );  CHECK( not layer.visible(loc) ); }
            { const LocalLocation loc( 34.5, 21.5 );  CHECK( not layer.visible(loc) ); }

            // test target:
            layer.scroll_south();

            REQUIRE( LocalLocation( 24.0, 20.0) == layer.visible().min );
            REQUIRE( LocalLocation( 36.0, 32.0) == layer.visible().max );

            // Region A: just-cached
            { const LocalLocation loc( 26.5, 34.5 );  CHECK( not layer.visible(loc)); }
            { const LocalLocation loc( 30.5, 34.5 );  CHECK( not layer.visible(loc)); }
            { const LocalLocation loc( 34.5, 34.5 );  CHECK( not layer.visible(loc)); }
            
            // Region B: just-loaded
            { const LocalLocation loc( 26.5, 21.5 );  CHECK( layer.visible(loc) ); CHECK( 0x80 == layer.get(loc) ); }
            { const LocalLocation loc( 30.5, 21.5 );  CHECK( layer.visible(loc) ); CHECK( 0x80 == layer.get(loc) ); }
            { const LocalLocation loc( 34.5, 21.5 );  CHECK( layer.visible(loc) ); CHECK( 0x80 == layer.get(loc) ); }
        }
    }

    SECTION( "Verify RollingGrid Can Scroll West" ){
        const BoundBox<UTMLocation> utm_bounds( {0,0}, {64,64} );
        RollingGridLayer layer( utm_bounds );

        REQUIRE( LocalLocation(  0.0,  0.0) == layer.tracked().min );
        REQUIRE( LocalLocation( 64.0, 64.0) == layer.tracked().max );

        REQUIRE( LocalLocation( 24.0, 24.0) == layer.visible().min );
        REQUIRE( LocalLocation( 36.0, 36.0) == layer.visible().max );

        // sets a distinct debug value for each cell
        populate_markers_per_sector( layer );

        {   // Region A: to-be-cached
            { const LocalLocation loc( 34.5, 34.5 );  CHECK(layer.visible(loc)); CHECK( 0x88 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 34.5, 30.5 );  CHECK(layer.visible(loc)); CHECK( 0x84 == static_cast<int>(layer.get(loc)) ); }
            { const LocalLocation loc( 34.5, 26.5 );  CHECK(layer.visible(loc)); CHECK( 0x80 == static_cast<int>(layer.get(loc)) ); }

            // Region B: to-be-loaded
            { const LocalLocation loc( 22.5, 34.5 );  CHECK( not layer.visible(loc) ); }
            { const LocalLocation loc( 22.5, 30.5 );  CHECK( not layer.visible(loc) ); }
            { const LocalLocation loc( 22.5, 26.5 );  CHECK( not layer.visible(loc) ); }

           // move view east 
            layer.scroll_west();

            REQUIRE( LocalLocation( 20.0, 24.0) == layer.visible().min );
            REQUIRE( LocalLocation( 32.0, 36.0) == layer.visible().max );

            // Region A: just-cached
            { const LocalLocation loc( 34.5, 34.5 );  CHECK( not layer.visible(loc) ); }
            { const LocalLocation loc( 34.5, 30.5 );  CHECK( not layer.visible(loc) ); }
            { const LocalLocation loc( 34.5, 26.5 );  CHECK( not layer.visible(loc) ); }
            
            // Region B: just-loaded
            { const LocalLocation loc( 22.5, 34.5 );  CHECK( layer.visible(loc) ); CHECK( 0x80 == layer.get(loc) ); }
            { const LocalLocation loc( 22.5, 30.5 );  CHECK( layer.visible(loc) ); CHECK( 0x80 == layer.get(loc) ); }
            { const LocalLocation loc( 22.5, 26.5 );  CHECK( layer.visible(loc) ); CHECK( 0x80 == layer.get(loc) ); }
        }
    }
}
