// GPL v3 (c) 2020

#ifndef _INDEX_HPP_
#define _INDEX_HPP_


namespace yggdrasil::geometry {

/// \brief Defines the mapping function from 2d -> 1d (i.e. how i,j coordinates map into a single chunk of memory)
/// \param dimension - count of number of data cells in each direction of the square data
/// \param cell_t - data type stored at each grid cell.
/// \param storage_t - type of the backing data storage.  requires an 'operator[]'
template<size_t dimension, typename cell_t, typename storage_t = std::array<cell_t, dimension*dimension> >
class RowMajorIndex {
public:
    RowMajorIndex() = delete;
    RowMajorIndex(storage_t& values);

    constexpr cell_t& lookup( const uint32_t i, const uint32_t j);
    // constexpr const cell_t& lookup( const uint32_t i, const uint32_t j) const ;

    constexpr cell_t& operator()( const uint32_t i, const uint32_t j);
    constexpr const cell_t& operator()( const uint32_t i, const uint32_t j) const;

private:
    storage_t& data;
};

inline constexpr uint8_t _calculate_padding(const size_t dimension);
inline uint64_t _interleave( const uint32_t input);

/// \brief hashes x,y ... by a Z-Order Curve
/// [1] http://en.wikipedia.org/wiki/Z-Order_curve
/// \brief tile in the world tree, containing a grid of data
/// \param T type of each grid cell
/// \param n dimension of the occupancy grid

template<size_t dimension, typename cell_t, typename storage_t = std::array<cell_t, dimension*dimension> >
class ZOrderCurveIndex{
public:
    ZOrderCurveIndex() = delete;
    ZOrderCurveIndex( storage_t& _data);

    constexpr cell_t lookup( const uint32_t i, const uint32_t j);

    constexpr cell_t& operator()( const uint32_t i, const uint32_t j);
    constexpr const cell_t& operator()( const uint32_t i, const uint32_t j) const;

    constexpr static size_t index_bit_size = 64;
    constexpr static uint8_t padding = _calculate_padding(dimension);
private:
    storage_t& data;
};

} // namespace yggdrasil::geometry

#include "index.inl"

#endif // #ifdef _GEOMETRY_INDEX_HPP_
