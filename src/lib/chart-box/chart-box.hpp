// GPL v3 (c) 2021, Daniel Williams

#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Geometry>

#include "chart-layer-interface.hpp"
#include "frame/frame-mapping.hpp"
#include "layer/fixed-grid/fixed-grid.hpp"

namespace chartbox {

using frame::FrameMapping;

/// \brief A container of containers for various types of map data structures
class ChartBox {
public:
    typedef chartbox::layer::FixedGridLayer boundary_layer_t;
    typedef chartbox::layer::FixedGridLayer contour_layer_t;
    
public:
    ChartBox();

    //// \brief retrieve the value of the requested point `p`. 
    ///
    /// @param {double} x The x-coordinate; in real-world units; meters
    /// @param {double} y The y-coordinate; in real-world units; meters
    /// @return {cell_value_t} The value of the node, if available; or the default value.
    int classify(const Eigen::Vector2d& p) const;

    boundary_layer_t& get_boundary_layer();

    contour_layer_t& get_contour_layer();
    
    inline FrameMapping& mapping() { return mapping_; }

    void print_layers() const;

    /// \brief Releases all memory associated with this quad tree.
    ~ChartBox() = default;

public:
    // NYI -- this should include all the to-global, and to-local code...
    // Transform transform;

private:
    FrameMapping mapping_;

    boundary_layer_t boundary_layer_;
    contour_layer_t contour_layer_;
    // TreeLayer _tree_layer;

    // manually hard-coded. Unfortunately.
    constexpr static size_t layer_count = 2;

// private:
//     friend class QuadTreeTest_ConstructDefault_Test;
//     friend class QuadTreeTest_LoadValidTree_Test;
//     friend class QuadTreeTest_CalculateLoadFactor_Test;
//     friend class QuadTreeTest_LoadGridFromJSON_Test;
//     friend class QuadTreeTest_WriteLoadCycle_Test;
//     friend class QuadTreeTest_SearchExplicitTree_Test;
//     friend class QuadTreeTest_InterpolateTree_Test;

};
} // namespace chart

