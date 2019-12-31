// GPL v3 (c) 2020

#include <array>
#include <cmath>
#include <iostream>
#include <vector>

#include "gtest/gtest.h"

#include "index.hpp"
using yggdrasil::geometry::RowMajorIndex;
using yggdrasil::geometry::ZOrderCurveIndex;

using std::cerr;
using std::endl;
using std::isnan;

typedef uint8_t cell_t;
typedef std::array<cell_t, 16> array_t;
typedef std::vector<cell_t> vector_t;

TEST(IndexTest, IndexByRowMajor) {
    array_t initial_grid = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    
    const RowMajorIndex<4, cell_t> index( initial_grid );

    EXPECT_EQ( index( 0, 0),  0);
    EXPECT_EQ( index( 1, 0),  1);
    EXPECT_EQ( index( 2, 0),  2);
    EXPECT_EQ( index( 3, 0),  3);

    ASSERT_EQ( index( 0, 1),  4);
    ASSERT_EQ( index( 1, 1),  5);
    ASSERT_EQ( index( 2, 1),  6);
    ASSERT_EQ( index( 3, 1),  7);

    ASSERT_EQ( index( 0, 2),  8);
    ASSERT_EQ( index( 1, 2),  9);
    ASSERT_EQ( index( 2, 2), 10);
    ASSERT_EQ( index( 3, 2), 11);

    ASSERT_EQ( index( 0, 3), 12);
    ASSERT_EQ( index( 1, 3), 13);
    ASSERT_EQ( index( 2, 3), 14);
    ASSERT_EQ( index( 3, 3), 15);
}

TEST(IndexTest, IndexByRowMajorModifyArray) {
    array_t initial_grid = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    RowMajorIndex<4, cell_t> index( initial_grid );

    EXPECT_EQ( index( 0, 0),  0);
    EXPECT_EQ( index( 1, 0),  1);
    EXPECT_EQ( index( 2, 0),  2);
    EXPECT_EQ( index( 3, 0),  3);

    // modify grid, and ensure that it still retrieves the value
    index(0, 0) = 100;
    index(1, 0) = 101;
    index(2, 0) = 102;
    index(3, 0) = 103;

    EXPECT_EQ( index( 0, 0),  100);
    EXPECT_EQ( index( 1, 0),  101);
    EXPECT_EQ( index( 2, 0),  102);
    EXPECT_EQ( index( 3, 0),  103);
}

TEST(IndexTest, IndexByRowMajorVectorBacking) {
    vector_t buffer = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    RowMajorIndex<4, cell_t, vector_t> index( buffer );

    EXPECT_EQ( index( 0, 0),  0);
    EXPECT_EQ( index( 1, 0),  1);
    EXPECT_EQ( index( 2, 0),  2);
    EXPECT_EQ( index( 3, 0),  3);
}

TEST(IndexTest, IndexByRowMajor_Iterator){
    array_t grid = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    RowMajorIndex<4, cell_t> index(grid);

    {
        // const cell_t* start_address = &(*index.begin());
        // const cell_t* end_address = &(**index.end());
        // ASSERT_EQ( (end_address - start_address) * sizeof(cell_t), 16);
    }{
        yggdrasil::geometry::RowIterator<4, cell_t> row = index.begin();
        // EXPECT_EQ( **row,  0);
        EXPECT_EQ( *row,  0);
        ++row;
        // EXPECT_EQ( **row,  4);
        EXPECT_EQ( *row,  0);
        
        // yggdrasil::geometry::CellIterator<cell_t> cell = row.begin();
        // EXPECT_EQ( *cell,  4);        
        // ++cell;
        // EXPECT_EQ( *cell,  5);

    }{
        // for (yggdrasil::geometry::RowIterator<4, cell_t> iter = begin(index);  iter != end(index);  ++iter){
        //     // cerr << "    @: " << (void*)(iter.data) << " = " << (int)(**iter);
        //     yggdrasil::geometry::CellIterator<cell_t> cell = *iter;
        //     cerr << (uint8_t)(*cell) << " | " << endl;
        // }
    
        for( yggdrasil::geometry::RowIterator<4UL, cell_t> row : index ){
            // for( yggdrasil::geometry::CellIterator<cell_t>& cell : row ){
            //     cerr << (void*)(&(*cell)) << " = " << (int)(*cell) << '|';
            // }
            // cerr << endl;
        }
        cerr << endl;
    }

}

TEST(IndexTest, IndexZOrderCurveConst ){
    ASSERT_EQ( sizeof(uint64_t), 8);  // should be guaranteed, by spec
    ASSERT_EQ( sizeof(size_t), 8);    // not strictly necessary

    array_t grid = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    const yggdrasil::geometry::ZOrderCurveIndex<4, cell_t> index(grid);

    ASSERT_EQ( index.padding, 60);

    EXPECT_EQ( index( 0, 0),  0);
    EXPECT_EQ( index( 1, 0),  1);
    EXPECT_EQ( index( 0, 1),  2);
    EXPECT_EQ( index( 1, 1),  3);

    EXPECT_EQ( index( 2, 0), 4);
    EXPECT_EQ( index( 3, 0), 5);
    ASSERT_EQ( index( 2, 1), 6);
    ASSERT_EQ( index( 3, 1), 7);

    EXPECT_EQ( index( 0, 2), 8);
    EXPECT_EQ( index( 1, 2), 9);
    ASSERT_EQ( index( 0, 3), 10);
    ASSERT_EQ( index( 1, 3), 11);

    EXPECT_EQ( index( 2, 2), 12);
    EXPECT_EQ( index( 3, 2), 13);
    ASSERT_EQ( index( 2, 3), 14);
    ASSERT_EQ( index( 3, 3), 15);
}

TEST(IndexTest, IndexZOrderCurveMutable ){
    ASSERT_EQ( sizeof(uint64_t), 8);  // should be guaranteed, by spec
    ASSERT_EQ( sizeof(size_t), 8);    // not strictly necessary

    array_t grid = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    yggdrasil::geometry::ZOrderCurveIndex<4, cell_t> index(grid);

    ASSERT_EQ( index.padding, 60);

    EXPECT_EQ( index( 0, 0),  0);
    EXPECT_EQ( index( 1, 0),  1);
    EXPECT_EQ( index( 0, 1),  2);
    EXPECT_EQ( index( 1, 1),  3);

    // modify grid, and ensure that it still retrieves the value
    index(0, 0) = 100;
    index(1, 0) = 101;
    index(0, 1) = 102;
    index(1, 1) = 103;

    EXPECT_EQ( index( 0, 0),  100);
    EXPECT_EQ( index( 1, 0),  101);
    EXPECT_EQ( index( 0, 1),  102);
    EXPECT_EQ( index( 1, 1),  103);

}
