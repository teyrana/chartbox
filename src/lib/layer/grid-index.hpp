// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <cmath>
#include <cstdint>
#include <ostream>
#include <sstream>

namespace chartbox::layer {

/// \brief convenvience class for indexing into an orthogonal, 2D grid
struct GridIndex {
     uint32_t column;
     uint32_t row;

    GridIndex()
        : column(0), row(0)
    {}

    GridIndex( uint32_t _col, uint32_t _row )
        : column(_col), row(_row)
    {}

    GridIndex add( const GridIndex& other ) const {
            return { column + other.column, row + other.row }; }

    GridIndex div( uint32_t divisor ) const {
            return { column / divisor, row / divisor }; }

    GridIndex mod( uint32_t divisor ) const {
            return { column % divisor, row % divisor }; }

    /// \warning dead-code
    GridIndex mult( uint32_t factor ) const {
            return { column * factor, row * factor }; }

    uint32_t norm1( const GridIndex& other ) const {
        return std::abs(static_cast<int32_t>(column + row) - static_cast<int32_t>(other.column + other.row));
    }

    uint32_t norm2( const GridIndex& other ) const {
        return std::sqrt( std::pow(static_cast<int32_t>(column) - static_cast<int32_t>(other.column),2)
                        + std::pow(static_cast<int32_t>(row)    - static_cast<int32_t>(other.row),2));
    }

    uint32_t offset( uint32_t length) const {
        return column + row*length;
    }

    inline bool operator==( const GridIndex& other) const {
        return ( (column == other.column) && (row == other.row));
    }

    /// \warning dead-code
    GridIndex sub( const GridIndex& other ) const {
            return { column - other.column, row - other.row }; }

    bool within( uint32_t cells_across ) const { 
        return ((column < cells_across) && ( row < cells_across))?true:false;
    }

    inline GridIndex wrap( uint32_t sectors_across_view ) const {
        return { static_cast<uint32_t>(column + sectors_across_view)%sectors_across_view,
                 static_cast<uint32_t>(row + sectors_across_view)%sectors_across_view };
    }
};

inline std::ostream& operator<< (std::ostream& os, const GridIndex& index) {
    os << "{ " << index.column << ", " << index.row << " }";
    return os;
}

} // namespace
