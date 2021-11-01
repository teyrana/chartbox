// GPL v3 (c) 2021, Daniel Williams 

#pragma once


#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "layer/layer-interface.hpp"

#include "leaf-node.hpp"
#include "quad-node.hpp"

namespace chartbox::layer {


///! \brief Quad Tree for representing 2D data
///!     note: the quad tree as a whole is square -- in precision, and real-world-units
///!     note: each node / level / leaf is also square -- in real world units
///! 
///! The implementation currently requires pre-determined bounds for data as it
///! can not rebalance itself to that degree.
///! 
class QuadTreeLayer final : public LayerInterface<QuadTreeLayer> {
public:

    /// \brief number of cells along each dimension of the entire tree
    constexpr static uint32_t dimension = 1024;

    /// \brief name of this layer's type
    constexpr static char type_name_[] = "QuadTreeLayer";

public:
    /**
     * Constructs a new quad tree, centered at 0,0 and 1024 units wide, square
     * 
     * Use this constructor if the tree will be loaded from a config file, and the initialization values do not matter.
     */
    QuadTreeLayer();
    
    /**
     * Constructs a new quad tree.
     *
     * @param x, y coordinates of tree's center point.
     * @param tree width.  Tree is square. (i.e. height === width)
     */
    QuadTreeLayer( const geometry::LocalLocation& center, double width );

    /**
     *  Releases all memory associated with this quad tree.
     */
    ~QuadTreeLayer();

    constexpr size_t calculate_complete_tree(const size_t height) const;

    /**
     * Returns true if the point at (x, y) exists in the tree.
     *
     * @param {double} x The x-coordinate.
     * @param {double} y The y-coordinate.
     * @return {bool} Whether the tree contains a point at (x, y).
     */
    bool contains(const geometry::LocalLocation& p) const;

    /**
     * retrieve the value of the requested point `p`. 
     *
     * @param {double} x The x-coordinate.
     * @param {double} y The y-coordinate.
     * @param {uint8_t} opt_default The default value to return if the node doesn't
     *                 exist.
     * @return {uint8_t} The value of the node, if available; or the default value.
     */
    uint8_t classify(const geometry::LocalLocation& p) const;

    /**
     * Draws a simple debug representation of this tree to the given
     * output stream. 
     *
     */
    void debug_tree( bool show_pointer=false) const;

    ///! \brief sets all leaf nodes to the given value
    ///!       override from LayerInterface
    ///! \param fill_value - value to write
    void fill( uint8_t fill_value);

    bool fill( const uint8_t* const buffer, size_t count );

    uint32_t height() const;
    
    double load_factor() const;
    
    size_t memory_usage() const;

    uint8_t get( const geometry::LocalLocation& p) const;

    uint32_t lookup( const uint32_t i, const uint32_t j ) const;

    // uint32_t lookup( const geometry::LocalLocation& p ) const;

    void prune();

    ///! \brief resets _the tree_ to fully populate the bounds at the given precision
    void reset();

    ///! \brief Classify what value the requested point `p` has.
    ///!
    ///! This method is designed for use with an interpolation algorithm.
    ///! 
    ///! \param location to sample near
    ///! @return the point-value-pair _actually_ contained in the tree.
    uint8_t get(const geometry::LocalLocation& p, uint8_t or_default_value ) const;

    double precision() const;
    
    size_t size() const;

    void split( double width );
    
    ///! \brief store a value in the tree, at point `p`
    ///!
    ///! This is the primary method to populate a useable tree.
    ///!
    ///! \param p - the x,y coordinates to write to
    ///! \param new_value - the value to write at point 'p'
    ///! \return success - fails if out-of-bounds.
    bool store(const geometry::LocalLocation& p, const uint8_t new_value);

    std::string summary() const;
    
    double width() const;

private:
    ///! the data layout this tree represents
    geometry::LocalLocation center_;
    double width_2_;
    double precision_;

    std::unique_ptr<QuadNode> root;

private:
    LayerInterface<QuadTreeLayer>& super() {
        return *static_cast< LayerInterface<QuadTreeLayer>* >(this);
    }

    const LayerInterface<QuadTreeLayer>& super() const {
        return *static_cast< const LayerInterface<QuadTreeLayer>* >(this);
    }
};

}   // namespace chartbox::layer
