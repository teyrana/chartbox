// GPL v3 (c) 2020

#ifndef _GRID_LAYOUT_HPP_
#define _GRID_LAYOUT_HPP_

#include <limits>
#include <memory>
#include <string>

#include <Eigen/Geometry>

#include <nlohmann/json.hpp>

using Eigen::Vector2d;

namespace yggdrasil::geometry {

typedef uint64_t index_t;

///! \brief Layout is used to encapsulate common logic about how to layout a square grid
class Layout {
public:

    constexpr Layout();
    // constexpr Layout()
    //     : precision(1), x(0), y(0), width(1) {}

    constexpr Layout(const double _precision, const double _x, const double _y, const double _width);

    bool contains(const Eigen::Vector2d& at) const;

    bool operator!=(const Layout& other) const;
    bool operator==(const Layout& other) const;

    // definitely _not_ constexpr ;)
    Layout(nlohmann::json& doc);

    void clear();
    
    const Vector2d get_anchor() const;
    inline Vector2d get_center() const { return { x_c, y_c}; }
    inline size_t get_dimension() const { return dimension; }
    inline double get_half_width() const { return half_width; }
    inline uint8_t get_padding() const { return padding; }
    inline double get_precision() const { return precision; }
    inline size_t get_size() const { return size; }
    double get_x() const { return x_c; }
    double get_x_max() const;
    double get_x_min() const;
    double get_y() const { return y_c; }
    double get_y_max() const;
    double get_y_min() const;
    inline size_t get_width() const { return width; }

    ///! \brief hashes x,y ... into a simple row-major indexing
    constexpr index_t hashr( const Eigen::Vector2d& p) const { return hashr( p[0], p[1]); }
    constexpr index_t hashr( const double x, const double y) const;
    constexpr index_t hashr( const uint32_t i, const uint32_t j) const;

    constexpr double limitx( const double x) const;

    /// \brief converts x into it's corresponding integer index
    /// \warning out-of-bounds calls are _undefined behavior_!
    constexpr uint32_t hashx( const double _x) const;

    constexpr double limity( const double y) const;

    /// \brief convertrs y into it's corresponding integer index
    /// \warning out-of-bounds calls are _undefined behavior_!
    constexpr uint32_t hashy( const double _y) const;

    ///! \brief hashes x,y ... by a Z-Order Curve
    ///! [1] http://en.wikipedia.org/wiki/Z-Order_curve
    constexpr index_t hashz( const Eigen::Vector2d& p) const { return hashz( p[0], p[1]); }
    constexpr index_t hashz( const double x, const double y) const;
    constexpr index_t hashz( const uint32_t i, const uint32_t j) const;

    ///! \brief factory method for creating from a json document
    static std::unique_ptr<Layout> make_from_json(nlohmann::json& doc);

    nlohmann::json to_json() const;

    std::string to_string() const;

// constants
public:
    // used for comparisons
    constexpr static double epsilon = 1e-6;
    constexpr static size_t index_bit_size = 64;
    constexpr static size_t maximum_supported_dimension = std::numeric_limits<uint32_t>::max();
    constexpr static double minimum_supported_precision = 1.;

private:
    constexpr uint64_t interleave( const uint32_t input) const;

    ///! \brief snaps this precision to match the next-power-of-2 dimension that covers the precision
    ///!
    ///! Note:  Precision <= 1.0
    constexpr double snap_precision(const double precision);
    constexpr double snap_width( const double _width);

    constexpr uint8_t calculate_padding( const double dimension);

private:  // primary variables

    double precision;

    double width;

    // Center Coordinates:
    // =====
    // these are stored as the raw doubles, (instead of Eigen::Vector2d) because ...
    //    (1) the layout can compress the layout,
    //    (2) allow the use of constexpr constructors
    double x_c;
    double y_c;

private: // secondary / cached variables
    size_t dimension;
    double half_width;
    uint8_t padding;  // left-pad the z-index with this many zeros.  Ranges from 0-64... which fits into a byte.
    size_t size;
};

#include "layout.inl"

} // namespace yggdrasil::geometry

#endif // #ifdef _GRID_LAYOUT_HPP_
