// GPL v3 (c) 2020
#ifndef _QUAD_NODE_HPP_
#define _QUAD_NODE_HPP_

#include <cstdint>
#include <iostream>
#include <string>
#include <memory>

#include <nlohmann/json.hpp>

#include "geometry/cell_value.hpp"

#include "node/tile_node.hpp"

using yggdrasil::geometry::cell_value_t;

namespace yggdrasil::node {

class TileNode;

class QuadNode {
    enum Quadrant {NW, NE, SW, SE};
    typedef union{
        std::unique_ptr<QuadNode> quad;
        std::unique_ptr<TileNode> tile;
    } child_t;

public:
    QuadNode();
    // QuadNode(const cell_value_t value);

    ~QuadNode();

    void draw(std::ostream& sink, const std::string& prefix, const std::string& as, const bool show_pointers) const;

    void fill(const cell_value_t fill_value);

    size_t get_count() const;
    size_t get_height() const;

    QuadNode* get(QuadNode::Quadrant quad) const;
    QuadNode* get_northeast() const;
    QuadNode* get_northwest() const;
    QuadNode* get_southeast() const;
    QuadNode* get_southwest() const;

    cell_value_t& get_value();
    cell_value_t get_value() const;

    bool load(const nlohmann::json& doc);

    bool operator==(const QuadNode& other) const;

    ///! \brief coalesce groups of leaf nodes with identice values (for some value of "identical")
    void prune();

    void split(const double precision, const double width);

    void reset();

    bool is_leaf() const;

    void set_value(cell_value_t new_value);

    nlohmann::json to_json() const;

    std::string to_string() const;

private:
    void split();

private:
    // By design, any given node will only cantain (a) children or (b) a value.
    // => If the following uptr, `northeast` has a value, the union will contain pointers.
    // => if 'northeast' is empty / null => the union contains leaf-node-values
    // defined in CCW order:  NE -> NW -> SW -> SE
    std::unique_ptr<QuadNode> northeast; //ne;
    std::unique_ptr<QuadNode> northwest; //nw;
    std::unique_ptr<QuadNode> southwest; //sw;
    std::unique_ptr<QuadNode> southeast; //se;

    std::unique_ptr<TileNode> value;

private:
    friend class QuadNodeTest_ConstructDefault_Test;
    friend class QuadNodeTest_ConstructWithValue_Test;
    friend class QuadNodeTest_SetGet_Test;
    friend class QuadNodeTest_SplitNodeImperative_Test;

};

} // namespace yggdrasil::node

#endif // #ifndef _QUAD_NODE_HPP_
