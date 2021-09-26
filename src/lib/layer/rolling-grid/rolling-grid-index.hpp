// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <cstdint>
#include <ostream>
#include <sstream>

namespace chartbox::layer {

struct Index2u32 {
     uint32_t column;
     uint32_t row;

    Index2u32() = default;

    Index2u32( uint32_t _col, uint32_t _row )
        : column(_col), row(_row)
    {}

    inline bool operator==( const Index2u32& other) const {
        return ( (column == other.column) && (row == other.row));
    }
};

inline std::ostream& operator<< (std::ostream& os, const Index2u32& index) {
    os << "{ " << index.column << ", " << index.row << " }";
    return os;
}

template<uint32_t cells_across_sector>
struct CellInSectorIndex {
    uint32_t column;
    uint32_t row;

    CellInSectorIndex() = default;

    CellInSectorIndex( uint32_t _col, uint32_t _row )
        : column(_col), row(_row)
    {}

    Index2u32 data() const { return {column,row}; }

    uint32_t offset() const {
        return column + row*cells_across_sector;
    }

};

template<uint32_t sectors_across_view>
struct SectorInViewIndex {
    uint32_t column;
    uint32_t row;

    SectorInViewIndex() = default;

    SectorInViewIndex( uint32_t _col, uint32_t _row )
        : column(_col), row(_row)
    {}

    Index2u32 data() const { return {column,row}; }

    uint32_t offset() const {
        return column + row*sectors_across_view;
    }

    inline SectorInViewIndex<sectors_across_view> operator+( const SectorInViewIndex<sectors_across_view>& other ) const {
        return { column + other.column, row + other.row };
    }
};

template<uint32_t cells_across_sector, uint32_t sectors_across_view>
struct CellInViewIndex {
    uint32_t column;
    uint32_t row;

    CellInViewIndex() = default;

    CellInViewIndex( uint32_t _col, uint32_t _row )
        : column(_col), row(_row)
    {}

    Index2u32 data() const { return {column,row}; }

    uint32_t offset() const {
        return column + row*(cells_across_sector*sectors_across_view);
    }

    inline SectorInViewIndex<sectors_across_view> divide() const { 
        return divide({0u,0u});
    }

    inline SectorInViewIndex<sectors_across_view> divide( SectorInViewIndex<sectors_across_view> anchor ) const {
        return { (static_cast<uint32_t>(column/cells_across_sector) + anchor.column + sectors_across_view)%sectors_across_view,
                 (static_cast<uint32_t>(row/cells_across_sector) + anchor.row + sectors_across_view)%sectors_across_view };
    }

    inline CellInSectorIndex<cells_across_sector> zoom() const {
        return { column%cells_across_sector, row%cells_across_sector };
    }

};

} // namespace
