// GPL v3 (c) 2020

#ifndef _NODE_HPP_
#define _NODE_HPP_

#include <cstdio>
#include <string>
#include <vector>

#include <Eigen/Geometry>

#include <nlohmann/json.hpp>

#include "geometry/cell_value.hpp"
#include "geometry/layout.hpp"
#include "geometry/polygon.hpp"


namespace yggdrasil::node {

///! \brief defines a template interface for all nodes in the world tree.
template<typename T>
class yggdrasil {
public:
    T& impl;
    std::string error_message;

    yggdrasil();

    yggdrasil(T& _ref);

    //copy constructor
    yggdrasil(const yggdrasil& t) = delete;

    //move constructor
    yggdrasil(yggdrasil&& t) = delete;

    // copy assignment operator
    yggdrasil& operator=(const yggdrasil& t) = delete;

    //move assignment operator
    yggdrasil& operator=(yggdrasil&& t) = delete;

    ~yggdrasil();

    geometry::cell_value_t classify(const Eigen::Vector2d& p) const;

    ///! \brief writes debug information to std err
    void debug() const;

    // sets grid to all zeros
    void inline fill(const geometry::cell_value_t _value);

    /**
     * Loads the vector of points as a CCW polygon.
     *
     * @param {std::istream} input stream containing the serialization text
     */
    void inline fill(const geometry::Polygon& source, const geometry::cell_value_t fill_value);

    ///! \brief counts the number of cells *actually* tracked
    size_t get_count() const;

    ///! \brief describes the number of divisions along each of the x, y dimensions
    size_t get_dimension() const;

    const std::string& get_error() const;

    ///! \brief retreives the boundary information
    const geometry::Layout& get_layout() const;

    ///! \brief describes how much of the data structure is actively
    ///! being used
    double get_load_factor() const;

    ///! \brief gets the total count of cells in this data structure
    size_t get_size() const;

    void print() const;

    void inline reset();
    void inline reset(const geometry::Layout& _layout);

    std::string summary() const;

}; // class yggdrasil<T>

}; // namespace yggdrasil::node

#include "yggdrasil.inl"

#endif  // #ifdef _NODE_HPP_
