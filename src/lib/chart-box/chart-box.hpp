// GPL v3 (c) 2021, Daniel Williams

#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "chart-layer-interface.hpp"
#include "geometry/frame-mapping.hpp"
#include "layer/static-grid/static-grid-layer.hpp"
#include "layer/rolling-grid/rolling-grid-layer.hpp"

namespace chartbox {

using chartbox::geometry::FrameMapping;
using chartbox::geometry::LocalLocation;

/// \brief A container of containers for various types of map data structures
class ChartBox {
public:
    typedef chartbox::layer::StaticGridLayer boundary_layer_t;

    // note: this template must be explicitly instantiated at the bottom of `rolling-grid-layer.cpp`
    typedef chartbox::layer::RollingGridLayer<64,5> contour_layer_t;

public:
    ChartBox();

    //// \brief retrieve the value of the requested point `p`. 
    ///
    /// @param {double} x The x-coordinate; in real-world units; meters
    /// @param {double} y The y-coordinate; in real-world units; meters
    /// @return {cell_value_t} The value of the node, if available; or the default value.
    uint8_t classify(const LocalLocation& p) const;

    inline boundary_layer_t& get_boundary_layer() {  return boundary_layer_; }

    inline contour_layer_t& get_contour_layer() {  return contour_layer_; }

    inline FrameMapping& mapping() { return mapping_; }

    inline const FrameMapping& mapping() const { return mapping_; }

    void print_layers() const;

    /// \brief Releases all memory associated with this quad tree.
    ~ChartBox() = default;

private:
    FrameMapping mapping_;

    boundary_layer_t boundary_layer_;
    contour_layer_t contour_layer_;

    // manually hard-coded. Unfortunately.
    constexpr static size_t layer_count = 2;

};
} // namespace chart

