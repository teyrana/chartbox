// GPL v3 (c) 2020, Daniel Williams 

#ifndef _GEOMETRY_PATH_HPP_
#define _GEOMETRY_PATH_HPP_

#include <initializer_list>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "local-location.hpp"

namespace chart::geometry {

class Path {
public:
    Path();
    Path(std::vector<LocalLocation>& init);
    Path(std::initializer_list<LocalLocation> init);

    // clears the internal point vector
    void clear();

    void emplace_back(const double x, const double y);

    bool empty() const;

    LocalLocation segment(const size_t index) const;

    double length() const;

    bool load(std::vector<LocalLocation> source);

    LocalLocation& operator[](const size_t index);

    const LocalLocation& operator[](const size_t index) const;

    void push_back(const LocalLocation p);

    void resize(size_t capacity);

    size_t size() const;

    // dumps the contains points to stderr
    // \param title - text to print in the output header
    // \param pts - set of points to dump
    std::string to_yaml(const std::string& indent = "") const;

private:
    double calculate_length();

// ====== ====== ====== Class Attributes ====== ====== ====== 
private:
    /// Main data store for this class.
    std::vector<LocalLocation> points;

    double length_;

public:
    // these need to be declared after the 'points' property
    auto begin() -> decltype(points.begin());
    auto end() -> decltype(points.end());

};

} // namespace chart::geometry

#endif // #endif _GEOMETRY_POLYGON_HPP_
