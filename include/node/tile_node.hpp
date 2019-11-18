// GPL v3 (c) 2020

#ifndef _TILE_NODE_HPP_
#define _TILE_NODE_HPP_

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <string>

#include <Eigen/Geometry>

#include <nlohmann/json.hpp>

#include "geometry/layout.hpp"
#include "geometry/polygon.hpp"

using std::cerr;
using std::endl;
using std::isnan;
using std::string;
using std::unique_ptr;

using Eigen::Vector2d;
using nlohmann::json;


using yggdrasil::geometry::Polygon;
using yggdrasil::geometry::Layout;

namespace yggdrasil::node {


/// \brief tile in the world tree, containing a grid of data
/// \param T type of each grid cell
/// \param n dimension of the occupancy grid
template<typename T, size_t n >
class TileNode {
public:
    TileNode();

     /**
      * @param layout which this grid will represent
      */
    TileNode(const Layout& _layout);
 
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
    T classify(const Eigen::Vector2d& p) const;

    ///! \brief check if this tile contains a given point
    ///!
    ///! \param p The x-y coordinates (in meters) to test at
    ///! @return Whether the tree contains the given point
    bool contains(const Eigen::Vector2d& p) const;

    ///! \brief get the dimension (in cell-count of the tile
    size_t dimension() const {return n;}

    ///! \brief sets tile to all zeros
    void fill();

    ///! \brief sets tile to all 'value'
    ///! \param fill_value - fill value for entire grid
    void fill(const T fill_value);

    ///! \brief Fills the _interior_ of the given polygon with the given value.
    ///! 
    ///! @param source - polygon defining the fill araea. Assumed to be closed, CCW, and non-intersecting
    ///! @param fill_value -fill value for area
    void fill(const Polygon& source, const T fill_value);

    ///! \brief Get the overall bounds of this tree
    ///!
    ///! \return Bounds object describing the tree's overall bounds.
    inline const Layout& get_layout() const { return layout; }

    constexpr size_t get_memory_usage() const;

    T& operator()(size_t xi, size_t yi);

    inline void reset(){}

    inline void reset(const Layout& _layout){ layout = _layout; }

    inline void set_layout( Layout& _layout){ layout = _layout; }

    // void set(const double x, const double y, const T new_value);

    ///! \brief the _total_ number of cells in this grid === (width * height)
    size_t size() const;

    bool store(const Vector2d& p, const T new_value) ;

    size_t width() const {return layout.get_width();}

private:
    bool cache_read(std::byte* buffer);
    std::byte* cache_write();

    T& get_cell(const size_t xi, const size_t yi);
    T get_cell(const size_t xi, const size_t yi) const;

    size_t grid_to_storage(size_t xi, size_t yi) const;
    size_t x_to_index(const double x) const;
    size_t y_to_index(const double y) const;
 
private:
    ///! the data layout this grid represents
    geometry::Layout layout;
 
    // raw array:  2D addressing is performed through the class methods
    std::array<std::array<T, n>, n> storage;

private:
    friend class TileTest_CacheRoundTrip_Test;
    // friend class TileTest_XYToIndex_Test;

}; // class Tile

}; // namespace yggdrasil::node

#include "geometry/cell_value.hpp"
using yggdrasil::geometry::cell_value_t;

// 1024-cell-tile <<==>> 32 x 32
typedef yggdrasil::node::TileNode<cell_value_t, 32> Tile1k;

// 1,048,576-cell-tile <<==>> 1024 x 1024 cells
typedef yggdrasil::node::TileNode<cell_value_t, 1024> Tile1M;

#endif  // #ifdef _TILE_NODE_HPP_
