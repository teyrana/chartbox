// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <memory>

#include "geometry/bound-box.hpp"
#include "geometry/global-location.hpp"
#include "geometry/local-location.hpp"
#include "geometry/path.hpp"
#include "geometry/polygon.hpp"
#include "geometry/utm-location.hpp"

namespace chartbox::layer {

using chartbox::geometry::BoundBox;
using chartbox::geometry::GlobalLocation;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::Path;
using chartbox::geometry::Polygon;
using chartbox::geometry::UTMLocation;

typedef enum {
       BOUNDARY, 
       CONTOUR,
       TARGET,
       LIDAR,
       RADAR,
       VIEW
} role_t;


constexpr uint8_t block_cell_value = 0xFF;
constexpr uint8_t clear_cell_value = 0;
constexpr uint8_t unknown_cell_value = 128u;
constexpr uint8_t default_cell_value = unknown_cell_value;

// base class of a CRTP pattern, as described here:
// https://eli.thegreenplace.net/2011/05/17/the-curiously-recurring-template-pattern-in-c/
// https://eli.thegreenplace.net/2013/12/05/the-cost-of-dynamic-virtual-calls-vs-static-crtp-dispatch-in-c
template< typename layer_t >
class LayerInterface {
protected:
    LayerInterface() = default;

public:

    // /// \brief Test if this data structer contains this index-point
    // /// \param The 2d-index to test
    // /// \return true if this data structure contains this index
    bool contains(const LocalLocation& p ) const {
        return layer().contains( p ); }

    /// \brief sets the entire layer to the given value
    /// \param fill_value - fill value for entire grid
    bool fill( uint8_t value ){
        return layer().fill(value); }

    /// \brief Fill the given area with the given value.
    /// 
    /// \param source - bounds defining the fill area, in local coordinates
    /// \param fill_value - value to write inside the box
    bool fill( const BoundBox<LocalLocation>& box, uint8_t value );

    /// \brief Fills the points along the path with the given value 
    /// 
    /// \param source - polygon defining the fill araea. Assumed to be in local coordinates, closed, CCW, and non-intersectings
    /// \param fill_value - fill value for polygon interior
    bool fill( const geometry::Path<LocalLocation>& source, const BoundBox<LocalLocation>& bounds, uint8_t value );

    /// \brief Fills the interior of the given polygon with the given value.
    /// 
    /// \param source - polygon defining the fill araea. Assumed to be in local coordinates, closed, CCW, and non-intersectings
    /// \param fill_value - fill value for polygon interior
    bool fill( const Polygon<LocalLocation>& source, const BoundBox<LocalLocation>& box, uint8_t value );

    /// \brief Access the value at an (x, y) LocalLocation
    ///!
    /// \param LocalLocation - the x,y coordinates to search at:
    /// \return reference to the cell value
    uint8_t& get( const LocalLocation& p ){
        return layer().get(p); }

    /// \brief Retrieve the value at an (x, y) LocalLocation
    ///
    /// \param LocalLocation - the x,y coordinates to search at
    /// \return the cell value
    uint8_t get(const LocalLocation& p) const { 
        return layer().get(p); }

    std::string name() const { 
        return name_; }

    layer_t name( const std::string& _name ){ 
        name_ = _name;
        return layer(); }

    /// \brief reset the layer to its default state
    void reset() { 
        layer().reset(); }

    /// \brief store something at the a given (x, y) coordinate in nav-space
    ///
    /// \param point - the x,y coordinates to write to
    /// \param value - the value to write at the given coordinates
    /// \return true for success; else false
    bool store(const LocalLocation& point, const uint8_t value){
        return layer().store(point,value); }
 
    double precision() const {
        return layer().precision(); }

    /// \brief Draws a simple debug representation of this grid to stderr
    std::string to_cell_content_string( uint32_t indent ) const {
        return layer().to_cell_content_string(indent); }

    std::string to_location_content_string( uint32_t indent ) const;

    std::string to_property_string( uint32_t indent ) const {
        return layer().to_property_string(); }

    std::string type() const { 
        return layer().type_name_; }

    /// \brief track from the given location  (... + the native width)
    bool track( const BoundBox<LocalLocation>& bounds ){
        return layer().track(bounds); }
    /// \brief Get the bounds of the tracked (overall) area
    const BoundBox<LocalLocation>& tracked() const {
        return layer().tracked(); }
    bool tracked(const LocalLocation& p) const {
        return layer().tracked(p); }

    /// \brief set the active / visible bounds for this layer
    bool view(const BoundBox<LocalLocation>& p) {
        return layer().view(p); }
    /// \brief Get the currently visible (active) bounds of this layer
    const BoundBox<LocalLocation>& visible() const { 
        return layer().visible(); }
    bool visible(const LocalLocation& p) const {
        return layer().contains(p); }

protected:

    layer_t& layer() {
        return *static_cast<layer_t*>(this);
    }

    const layer_t& layer() const {
        return *static_cast<const layer_t*>(this);
    }

    ~LayerInterface() = default;

protected:

    /// \brief descriptive for this layer's purpose
    std::string name_;

}; // class LayerInterface< uint8_t, layer_t >

} // namespace chart

#include "layer-interface.inl"