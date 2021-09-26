// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <array>
#include <cmath>
#include <cstdint>

#include "rolling-grid-index.hpp"

namespace chartbox::layer {

/// \brief Contains a grid of cells -- may be "relocated" to represent a different patch of terrain
///
///  chart => layer => sector => cell
///                    ^^ you are here
///
template<uint32_t cells_across_sector>
class RollingGridSector {
public:
    // internal storage coordinates of the southest corner, in the frame of the larger owning grid.
    typedef CellInSectorIndex<cells_across_sector> CellIndex;

public:

    RollingGridSector() = default;
    RollingGridSector( uint8_t default_value ){ 
            data_.fill( default_value ); }

    ~RollingGridSector() = default;

    inline bool contains( CellIndex index ) const { 
        if( (index.column < cells_across_sector) && (index.row < cells_across_sector) ){
            return true;
        }
        return false;
    }

    inline uint32_t dimension() const {
            return cells_across_sector; }

    inline void fill( uint8_t value ) { 
            data_.fill(value); }

    inline uint8_t get( CellIndex index ) const { 
            return data_[index.offset()]; }

    inline uint8_t operator[](uint32_t index) const { 
            return data_[index]; }

    inline uint8_t& operator[](uint32_t index) { 
            return data_[index]; }

    inline uint8_t set( CellIndex index, uint8_t value ) {
            return data_[index.offset()] = value; }

    inline uint32_t size() const { 
            return data_.size(); }

private:
    
    //  chart => layer => sector => cell
    //                              ^^^ you are here
    std::array<uint8_t, cells_across_sector * cells_across_sector> data_;
};


} // namespace
