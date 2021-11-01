// GPL v3 (c) 2021, Daniel Williams 

#pragma once

#include <array>
#include <cstdlib>
#include <limits>
#include <memory>
#include <queue>

#include "geometry/bound-box.hpp"
#include "geometry/local-location.hpp"
#include "geometry/path.hpp"
#include "geometry/polygon.hpp"
#include "layer/dynamic-grid/dynamic-grid-layer.hpp"

namespace chartbox::search {

typedef chartbox::geometry::Path<chartbox::geometry::LocalLocation> SearchPath;

template<typename layer_t>
class AStarSearch {
public:
    constexpr static char name[] = "A* Search";

    AStarSearch() = delete;

    AStarSearch( const layer_t& search_space );

    ~AStarSearch() = default;

    /// \brief Performs an A* search on the given layer data structure
    /// 
    /// ## Implementation Specifics
    /// The default implementation of A* assumes a regularly spaced grid, with index-lookups. 
    /// This could easily be extended to real-space by assuming an implicit grid of 
    /// regularly spaced points... but this latter option has not yet been implemented.
    ///
    ///
    /// ### Data Structures 
    ///   - metadata grid
    ///      - provides lookup for min-cost to reach given cell 
    ///      - stores heuristic: distance-to-goal
    ///   - fringe
    ///      - implements the fringe of our mapped space (within the overall search-space)
    ///      - std::priority_queue with pointers into the above metadata
    ///   - path-construction: 
    ///      - at first, implicit in the metadata grid
    ///      - next, constructed as a doubly-linked list.
    ///      - finally returned as a `geometry::Path`, aka: `vector< pair< double, double>>`
    ///
    /// ### Modifications:
    ///   - the general a-star assumes path-dependent costs vs this implementation, which does not
    ///      - our cost function is the Euclidian-norm (aka 2-norm) from any point to the goal
    ///      - because of this, we (currently) (over?) simplify, by ignoring the path to a point
    ///      - therefore, we merely record the first-path-to-point
    ///
    /// ### See Also:
    ///   - https://en.wikipedia.org/wiki/A*_search_algorithm
    ///   - https://gabrielgambetta.com/astar-demystified.html
    ///
    /// \param start - find a path from here
    /// \param goal  - find a path to here
    SearchPath compute( const geometry::LocalLocation& start, const geometry::LocalLocation& end );

    inline double precision() const { return visited_.meters_across_cell(); }

    /// \brief calculates the cost-to-goal for this point
    /// 
    /// Note: Current implementation is just the 2-norm distance between the search-space coordinates
    /// 
    /// \param p -- the location to measure
    static float cost( const geometry::LocalLocation& p, const geometry::LocalLocation& goal );

    const geometry::BoundBox<geometry::LocalLocation>& searchable() const { return visited_.visible(); }

public:
// public only for development -- hide again, once this is working
    static geometry::LocalLocation decode_adjacency_flags( uint8_t flags );
    static uint8_t encode_adjacency_flags( const geometry::LocalLocation& delta );

// ====== ====== Private Function Declarations ====== ======
private:
    // constexpr static bool smooth_corners = true;  // meh.
    constexpr static bool simplify_straights = true;
    constexpr static float minimum_separation = 1.0;
    constexpr static float maximum_separation = 4.0;
    SearchPath extract_path(const geometry::LocalLocation& goal);

    /// \brief offsets for the 4 neighbors directly adjacent to the center coordinate
    //      +---+---+---+
    //      |   | 3 |   |
    //      +---+---+---+
    //      | 2 |[C]| 0 |
    //      +---+---+---+
    //      |   | 1 |   |
    //      +---+---+---+
    constexpr static std::array<geometry::LocalLocation,4> neighbor_4_offsets = {
            geometry::LocalLocation( +1,  0 ),
            geometry::LocalLocation(  0, -1 ),
            geometry::LocalLocation( -1,  0 ),
            geometry::LocalLocation(  0, +1 )};

    /// \brief offsets for the 8 neighbors directly adjacent to the center coordinate
    //      +---+---+---+
    //      | 5 | 6 | 7 |
    //      +---+---+---+
    //      | 4 |[C]| 0 |
    //      +---+---+---+
    //      | 3 | 2 | 1 |
    //      +---+---+---+
    constexpr static std::array<geometry::LocalLocation,8> neighbor_8_offsets = {
            geometry::LocalLocation( +1,  0 ),
            geometry::LocalLocation( +1, -1 ),
            geometry::LocalLocation(  0, -1 ),
            geometry::LocalLocation( -1, -1 ),
            geometry::LocalLocation( -1,  0 ),
            geometry::LocalLocation( -1, +1 ),
            geometry::LocalLocation(  0, +1 ),
            geometry::LocalLocation( +1, +1 )};

    /// \brief calculate the n neighbors directly adjacent to the center coordinate
    //      +---+---+---+
    //      |   |   |   |
    //      +---+---+---+
    //      |   |[C]|   |
    //      +---+---+---+
    //      |   |   |   |
    //      +---+---+---+
    /// \param center -- coordinate of center box
    /// \return returns the array of neighbors, in the order shown
    template<typename array_t>
    static array_t get_neighbors( const geometry::LocalLocation& center, const array_t& neighbor_offsets );


// ====== ====== Private Type Definitions ====== ======
private:
    constexpr static float match_threshold = 0.1f;

    // values lower than this value are considered "passable"
    constexpr static uint8_t context_passable_threshold = 0x80;

    // defined for the fringe next-cell-to-explore data structure
    struct FringeCell {
        geometry::LocalLocation at;
        float cost;    ///< distance-to-goal heuristic

        static constexpr float maximum_cost = std::numeric_limits<decltype(cost)>::max();

        FringeCell( geometry::LocalLocation _p, float _c ) : at(_p), cost(_c) {}

        inline bool operator<(const FringeCell& rhs) const {
            // this ordering is be reversed in order to induce bubble the lowest-cost options to the top
            return (this->cost > rhs.cost); }
    };

    // these fields define bit-packing for the values in `visited_`

    // default value.  Signals that the cell has not been visited.
    constexpr static uint8_t VACANT_FLAG =   0x00;

    // sentinel to signal the start point
    constexpr static uint8_t SENTINEL_FLAG = 0xCF;

    // this group may be combined:.
    // Example:  (defines a move one cell to the Northeast:
    //     `visited_[p] = FLAG_DELTA | FLAG_NORTH | FLAG_EAST;`
    constexpr static uint8_t FLAG_DELTA =    0x30;
    constexpr static uint8_t FLAG_NORTH =    0x08;
    constexpr static uint8_t FLAG_SOUTH =    0x04;
    constexpr static uint8_t FLAG_EAST =     0x02;
    constexpr static uint8_t FLAG_WEST =     0x01;


// ====== ====== Private Member Variables ====== ======
private:
    const layer_t & context_;

    std::priority_queue<FringeCell> fringe_;  // aka 'open set'

    // holds the return path via a list of local-deltas
    // aka 'closed set'
    // layer::simple::SimpleGridLayer<uint8_t,1024,1000> visited_;
    layer::dynamic::DynamicGridLayer visited_;

};

} // namespace

#include "a-star-search.inl"
