// GPL v3 (c) 2020

#ifndef _TILE_NODE_HPP_
#define _TILE_NODE_HPP_

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <memory>
#include <string>

#include <Eigen/Geometry>

#include <nlohmann/json.hpp>

#include "geometry/index.hpp"
#include "geometry/polygon.hpp"

using std::cerr;
using std::endl;
using std::isnan;
using std::string;
using std::unique_ptr;

using nlohmann::json;

using yggdrasil::geometry::Polygon;

namespace yggdrasil::node {

class TileNode {
public:
    typedef uint8_t cell_t;

    TileNode();

     /**
      * @param layout which this grid will represent
      */
    TileNode(const Eigen::Vector2d& anchor_minimum);
    
    /**
      *  Releases all memory associated with this quad tree.
      */
    ~TileNode(){};

    ///! \brief Draws a simple debug representation of this grid to stderr
    void debug() const;

    ///! \brief retrieve the value at given world coordinate (x, y)
    ///!
    ///! \param p The x-y coordinates (in meters) to search for
    ///! \return The value of the node, if available; or the default value.
    cell_t classify(const Eigen::Vector2d& p) const;

    ///! \brief check if this tile contains a given point
    ///!
    ///! \param p The x-y coordinates (in meters) to test at
    ///! @return Whether the tree contains the given point
    bool contains(const Eigen::Vector2d& p) const;

    ///! \brief sets tile to all zeros
    void fill();

    ///! \brief sets tile to all 'value'
    ///! \param fill_value - fill value for entire grid
    void fill(const cell_t fill_value);

    ///! \brief Fills the _interior_ of the given polygon with the given value.
    ///! 
    ///! @param source - polygon defining the fill araea. Assumed to be closed, CCW, and non-intersecting
    ///! @param fill_value -fill value for area
    void fill(const Polygon& source, const cell_t fill_value);

    static std::unique_ptr<TileNode> make(const std::byte * const buffer);

    ///! \brief loads a json document from the given input stream
    ///! 
    ///! Note: this json document may contain either polygons or a grid, and will be automatically loaded correctly
    static std::unique_ptr<TileNode> make(const std::string& buffer);

    inline void reset(){}

    // void set(const double x, const double y, const T new_value);

    cell_t& get_cell(const size_t xi, const size_t yi);
    cell_t get_cell(const size_t xi, const size_t yi) const;

    bool store(const Eigen::Vector2d& p, const cell_t new_value);

    ///! \brief writes a json document to given output stream
    std::string to_json() const;

    std::string to_string() const;
    
    std::vector<std::byte> to_raster() const;

private:
    bool cache_read( const std::byte* const buffer);
    const std::byte* const cache_write();

private:
    // used for reading and write json documents:
    const static std::string anchor_key;
    const static std::string grid_key;
    const static std::string x_key;
    const static std::string y_key;

public:
    constexpr static cell_t cell_default_value = '*';
    constexpr static size_t dimension = 32;
    constexpr static double width = dimension;
    constexpr static double scale = dimension/width;

    ///! \brief the _total_ number of cells in this grid === (width * height)
    constexpr static size_t size = dimension * dimension;

    /// \brief === lower-left / minimum-value point.
    const Eigen::Vector2d anchor;

private:
    /// \brief contains the data for this tile
    std::array< cell_t, size > data;

    /// \brief defines lookup method for data in this tile
    yggdrasil::geometry::RowMajorIndex<dimension, cell_t> index;

private:
    friend class TileTest_CacheRoundTrip_Test;
    // friend class TileTest_XYToIndex_Test;

}; // class Tile

}; // namespace yggdrasil::node


#endif  // #ifdef _TILE_NODE_HPP_
