// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <array>
#include <cmath>
#include <cstdint>

#include "layer/grid-index.hpp"

namespace chartbox::layer::rolling {

/// \brief Contains a grid of cells -- may be "relocated" to represent a different patch of terrain
///
///  chart => layer => sector => cell
///                    ^^ you are here
///
template<uint32_t cells_across_sector>
class RollingGridSector {
public:
    constexpr static double meters_across_cell = 1.0;

public:

    RollingGridSector() = default;
    RollingGridSector( uint8_t default_value ){ 
            data_.fill( default_value ); }

    ~RollingGridSector() = default;

    inline bool contains( GridIndex index ) const { 
        if( (index.column < cells_across_sector) && (index.row < cells_across_sector) ){
            return true;
        }
        return false;
    }

    inline constexpr static uint32_t cells_across() { return cells_across_sector; }

    inline const uint8_t* data() const {
        return data_.data(); }

    inline void fill( uint8_t value ) { 
            data_.fill(value); }

    inline bool fill( const uint8_t * const source, size_t count ){
        if( count == data_.size() ){
            std::memcpy( data_.data(), source, count );
            return true;
        }
        return false;
    }

    inline uint8_t get( GridIndex index ) const { 
            return data_[index.offset(cells_across_sector)]; }

    inline uint8_t operator[](uint32_t index) const { 
            return data_[index]; }

    inline uint8_t& operator[](uint32_t index) { 
            return data_[index]; }

    inline std::string print_contents_by_cell() const {
        std::ostringstream buf;
        buf << "======== ======= ======= Print Sector By Cell: ======= ======= =======\n";
        buf << std::hex;
        for (size_t cell_row_index = cells_across_sector - 1; cell_row_index < cells_across_sector; --cell_row_index) {
            for (size_t cell_column_index = 0; cell_column_index < cells_across_sector; ++cell_column_index ) {
                if( 0 == ((cell_column_index) % cells_across_sector ) ){
                    buf << "    ";
                }

                const GridIndex cell_lookup_index( cell_column_index, cell_row_index );
                const uint8_t current_cell_value = get( cell_lookup_index );

                buf << ' ' << std::setfill('0') << std::setw(2) << static_cast<int>(current_cell_value);
            }
            if( 0 == (cell_row_index % cells_across_sector ) ){
                buf << '\n';
            }

            buf << '\n';
        }
        buf << "======== ======= ======= ======= ======= ======= ======= =======\n";
        return buf.str();
    }

    inline uint8_t set( GridIndex index, uint8_t value ) {
            return data_[index.offset(cells_across_sector)] = value; }

    constexpr inline uint32_t size() const { 
            return data_.size(); }

private:
    
    //  chart => layer => sector => cell
    //                              ^^^ you are here
    std::array<uint8_t, cells_across_sector * cells_across_sector> data_;
};


} // namespace
