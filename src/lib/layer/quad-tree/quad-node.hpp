// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

#include "leaf-node.hpp"

namespace chartbox::layer {

class QuadNode;

class QuadNode {
public:
    enum Quadrant { NW, NE, SW, SE };

public:
    QuadNode();
    QuadNode(const uint8_t value);
    ~QuadNode();

    size_t count() const;

    void draw(std::ostream& sink, const std::string& prefix,
              const std::string& as, const bool show_pointers) const;

    void fill( uint8_t fill_value);

    size_t height() const;

    QuadNode* get(QuadNode::Quadrant quad) const;

    uint8_t get() const;

    bool operator==(const QuadNode& other) const;

    ///! \brief coalesce leaf nodes that have the same value
    void prune();

    bool is_leaf() const;

    void set(uint8_t new_value);

    std::string to_string() const;

    void join();
    void split();

    // typedef union {
    //     QuadNode quad;
    //     LeafNode<8> tile;
    //     // LeafNode<32> tile;
    // } child_t;

    // By design, any given node will only cantain (a) children or (b) a value.
    // => If the following uptr, `northeast` has a value, the union will contain
    // pointers.
    // => if 'northeast' is empty / null => the union contains leaf-node-values
    // defined in CCW order:  NE -> NW -> SW -> SE
    std::unique_ptr<QuadNode> northeast; // ne;
    std::unique_ptr<QuadNode> northwest; // nw;
    std::unique_ptr<QuadNode> southwest; // sw;
    std::unique_ptr<QuadNode> southeast; // se;

    uint8_t value_; 

};

} // namespace chartbox::layer
