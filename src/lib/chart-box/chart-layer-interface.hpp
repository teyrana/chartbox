// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <memory>

#include <gdal.h>
#include <ogr_geometry.h>

#include "frame/bound-box.hpp"
#include "frame/location2en.hpp"
#include "frame/location2xy.hpp"

namespace chartbox {

using chartbox::frame::BoundBox;
using chartbox::frame::Location2EN;
using chartbox::frame::Location2xy;

// base class of a CRTP pattern, as described here:
// https://eli.thegreenplace.net/2011/05/17/the-curiously-recurring-template-pattern-in-c/
// https://eli.thegreenplace.net/2013/12/05/the-cost-of-dynamic-virtual-calls-vs-static-crtp-dispatch-in-c
template<typename cell_t, typename layer_t>
class ChartLayerInterface {
protected:
    ChartLayerInterface() = default;

    ChartLayerInterface( const BoundBox<Location2EN>& bounds );

public:

    /// \brief how wide each cell is, in real-world navigation units
    constexpr static cell_t blocked_value = 0;
    constexpr static cell_t clear_value = 0;
    
public:
    // /// \brief Retrieve the value at an (x, y) Eigen::Vector2d
    // ///
    // /// \param Eigen::Vector2d - the x,y coordinates to search at
    // /// \note  this version has a default value of '0xFF'
    // /// \return the cell value
    // cell_t classify(const Eigen::Vector2d& p) const {
    //     return static_cast<chart_t*>(this)->classify(p); }

    // /// \brief Test if this data structer contains this index-point
    // /// \param The 2d-index to test
    // /// \return true if this data structure contains this index
    // bool contains(const index::Index2u& index) const {

    /// \brief sets the entire grid to the given value
    /// \param fill_value - fill value for entire grid
    bool fill( const cell_t value ){
        return layer().fill(value); }

    /// \brief Fill the given area with the given value.
    /// 
    /// \param source - bounds defining the fill area, in local (UTM) coordinates
    /// \param fill_value - value to write inside the box
    bool fill( const BoundBox<Location2xy>& box, const cell_t value );

    /// \brief Fills the interior of the given polygon with the given value.
    /// 
    /// \param source - polygon defining the fill araea. Assumed to be in local coordinates, closed, CCW, and non-intersecting
    /// \param fill_value - fill value for polygon interior
    bool fill( const OGRLinearRing& source, cell_t value );

    ///! \brief load a .shp file into this chart.
    // bool load_from_shape_file(target_t& chart, const std::string& filepath);

    /// \brief Retrieve the value at an (x, y) Eigen::Vector2d
    ///
    /// \param Eigen::Vector2d - the x,y coordinates to search at
    /// \return the cell value
    cell_t get(const Location2EN& p) const { return layer().get(p); }

    /// \brief Access the value at an (x, y) Eigen::Vector2d
    ///!
    /// \param Eigen::Vector2d - the x,y coordinates to search at:
    /// \return reference to the cell value
    cell_t& get( const Eigen::Vector2d& point ){
        return layer().get(point); }

    std::string name() const { return name_; }

    layer_t name( const std::string& _name ){ name_ = _name; return layer(); }

    /// \brief reset the layer to its default state
    void reset() { 
        layer().reset(); }

    /// \brief store something at the a given (x, y) coordinate in nav-space
    ///
    /// \param point - the x,y coordinates to write to
    /// \param value - the value to write at the given coordinates
    /// \return true for success; else false
    bool store(const Eigen::Vector2d& point, const cell_t value){
        return layer().store(point,value); }

    std::string print_contents() const { return layer().print_contents(); }

    std::string type() const { return layer().type_name_; }

    void update_precision(){ layer().update_precision(); }

protected:

    layer_t& layer() {
        return *static_cast<layer_t*>(this);
    }

    const layer_t& layer() const {
        return *static_cast<const layer_t*>(this);
    }

    ~ChartLayerInterface() = default;

protected:

    /// \brief the data layout this grid represents
    const BoundBox<Location2EN>& bounds_;

    /// \brief descriptive for this layer's purpose
    std::string name_;

}; // class ChartLayerInterface< cell_t, layer_t >

} // namespace chart

#include "chart-layer-interface.inl"