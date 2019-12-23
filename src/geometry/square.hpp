// GPL v3 (c) 2020

#ifndef _SQUARE_HPP_
#define _SQUARE_HPP_

#include <limits>
#include <memory>
#include <string>

#include <Eigen/Geometry>

using Eigen::Vector2d;

namespace yggdrasil::geometry {


template<size_t d, size_t w>
class Square {
public:
    constexpr Square(): precision(1), x(0), y(0), width(1) {}

    constexpr Square(const double _precision, const double _x, const double _y, const double _width);

    // definitely _not_ constexpr ;)
    Square(nlohmann::json& doc);

    bool contains(const Eigen::Vector2d& at) const;

    bool operator!=(const Square& other) const;
    bool operator==(const Square& other) const;

    void clear();
    
    const Vector2d get_anchor() const;
    inline Vector2d get_center() const { return { x_c, y_c}; }
    inline size_t get_dimension() const { return dimension; }
    inline uint8_t get_padding() const { return padding; }
    inline double get_precision() const { return precision; }
    inline size_t get_size() const { return size; }
    inline double get_width() const { return width; }
    double get_x() const { return x_c; }
    double get_x_max() const;
    double get_x_min() const;
    double get_y() const { return y_c; }
    double get_y_max() const;
    double get_y_min() const;
    inline size_t get_width() const { return width; }

    /// \brief factory method for creating from a json document
    static std::unique_ptr<Square> make_from_json(nlohmann::json& doc);

    nlohmann::json to_json() const;

    std::string to_string() const;

private: // member variables
    const size_t dimension;
    const double width;
    const struct _c {
	double x;
	double y;
    } center;
};




#endif // #ifdef _SQUARE_HPP_
