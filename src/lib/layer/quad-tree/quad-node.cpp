// GPL v3 (c) 2020, Daniel Williams 

#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "chart-box/geometry/local-location.hpp"

#include "quad-node.hpp"

using chartbox::geometry::LocalLocation;

using chartbox::layer::QuadNode;

QuadNode::QuadNode() 
    : QuadNode(0)
{}

QuadNode::QuadNode(const uint8_t _value)
    : northeast(nullptr), northwest(nullptr), southwest(nullptr), southeast(nullptr)
    , value_(_value)
{}

void QuadNode::draw(std::ostream& sink, 
                    const std::string& prefix, 
                    const std::string& as,
                    const bool show_pointers) const 
{

    sink << prefix << "[" << as << "]: ";
    if (is_leaf()) {
        sink << static_cast<int>(get());
    }
    // sink << ' ' << bounds.str();
    if (show_pointers) {
        sink << "   @:" << this << std::endl;
    }
    sink << std::endl;

    if (!is_leaf()) {
        auto next_prefix = prefix + "    ";
        northeast->draw(sink, next_prefix, "NE", show_pointers);
        northwest->draw(sink, next_prefix, "NW", show_pointers);
        southwest->draw(sink, next_prefix, "SW", show_pointers);
        southeast->draw(sink, next_prefix, "SE", show_pointers);
    }
}

void QuadNode::fill(const uint8_t fill_value) {
    if (is_leaf()) {
        set(fill_value);
    } else {
        northeast->fill(fill_value);
        northwest->fill(fill_value);
        southeast->fill(fill_value);
        southwest->fill(fill_value);
    }
}

size_t QuadNode::count() const {
    if (is_leaf()) {
        return 1;
    } else {
        size_t count = 0;
        count += northeast->count();
        count += northwest->count();
        count += southeast->count();
        count += southwest->count();
        return count + 1;
    }
}

size_t QuadNode::height() const {
    if (is_leaf()) {
        return 1;
    } else {
        const size_t ne_height = northeast->height();
        const size_t nw_height = northwest->height();
        const size_t se_height = southeast->height();
        const size_t sw_height = southwest->height();

        const size_t max_height = std::max(
            ne_height, std::max(nw_height, std::max(se_height, sw_height)));
        return max_height + 1;
    }
}

bool QuadNode::is_leaf() const { 
    return (nullptr == northeast);
}


uint8_t QuadNode::get() const { 
    return value_;
}

void QuadNode::join() {
    if( is_leaf() ){
        return;
    }

    if( northeast ){
        northeast->join();
        northeast.release();

        northwest->join();
        northwest.release();

        southeast->join();
        southeast.release();

        southwest->join();
        southwest.release();
    }
}

bool QuadNode::operator==(const QuadNode& other) const {
    return (value_ == other.value_);
}

void QuadNode::prune() {
    if (is_leaf()) {
        return;
    }

    northeast->prune();
    northwest->prune();
    southeast->prune();
    southwest->prune();

    const bool has_only_leaves = northeast->is_leaf()
                                && northwest->is_leaf()
                                && southeast->is_leaf()
                                && southwest->is_leaf();

    if (has_only_leaves) {
        auto nev = northeast->get();
        auto nwv = northwest->get();
        auto sev = southeast->get();
        auto swv = southwest->get();

        if ((nev == nwv) && (nwv == sev) && (sev == swv)) {
            set(nev);
            join();
        }
    }
}

void QuadNode::set(uint8_t new_value) {
    value_ = new_value;
}

void QuadNode::split() {
    if (is_leaf()) {
        this->northeast = std::make_unique<QuadNode>(value_);
        this->northwest = std::make_unique<QuadNode>(value_);
        this->southeast = std::make_unique<QuadNode>(value_);
        this->southwest = std::make_unique<QuadNode>(value_);
    }
}

std::string QuadNode::to_string() const {
    std::ostringstream buf;

    if (is_leaf()) {
//         doc = json({this->value}, false, json::value_t::number_integer)[0];
    } else {
        assert(northeast);
        assert(northwest);
        assert(southeast);
        assert(southwest);
//         doc["NE"] = northeast->to_json();
//         doc["NW"] = northwest->to_json();
//         doc["SE"] = southeast->to_json();
//         doc["SW"] = southwest->to_json();
    }

    return buf.str();
}

QuadNode::~QuadNode() { 
    join();
}
