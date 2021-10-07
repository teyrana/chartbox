// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <array>
#include <cstddef>
#include <string>

#include "chart-box/geometry/local-location.hpp"

namespace chartbox::layer {

template <uint32_t n>
class LeafNode {
public:
    LeafNode();

    /**
     * @param layout which this grid will represent
     */
    LeafNode( geometry::LocalLocation anchor_minimum);

    /**
     *  Releases all memory associated with this quad tree.
     */
    ~LeafNode() = default;

    bool contains( const geometry::LocalLocation& p ) const;

    inline uint8_t* data() { return data_.data(); }

    inline uint32_t dimension() const { return n; }

    ///! \brief sets tile to all 'value'
    ///! \param fill_value - fill value for entire grid
    void fill( uint8_t fill_value);

    ///! \brief sets tile contents
    ///! \param fill_value - fill value to _move_ into this tile.
    void fill( const uint8_t * const fill_data, uint32_t count );

    ///! \brief retrieve the value at given world coordinate (x, y)
    ///!
    ///! \param p The x-y coordinates (in meters) to search for
    ///! \return The value of the node, if available; or the default value.
    uint8_t get(const geometry::LocalLocation& p, uint8_t or_default_value ) const;

    const geometry::LocalLocation& origin() const;

    inline double scale() const { return scale_; }

    bool set(const geometry::LocalLocation& p, const uint8_t new_value);

    ///! \brief the _total_ number of cells in this grid === (width * height)
    inline size_t size() const { return data_.size(); }

    std::string to_string() const;

    inline double width() const { return width_; }

private:

    ///! \brief sets tile to all zeros
    void reset();

    uint8_t& get(const uint32_t xi, const uint32_t yi);
    uint8_t get(const uint32_t xi, const uint32_t yi) const;

private:
    /// \brief === lower-left / minimum-value point.
    /// not strictly necessary, but a useful cache
    const geometry::LocalLocation origin_;
    double width_;
    double scale_;

    /// \brief contains the data for this tile
    std::array<uint8_t, n*n> data_;

}; // class LeafNode

} // namespace chartbox::layer
