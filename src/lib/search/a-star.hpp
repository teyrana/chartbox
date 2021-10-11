// GPL v3 (c) 2020, Daniel Williams 

#ifndef _CHART_SEARCH_A_STAR_HPP_
#define _CHART_SEARCH_A_STAR_HPP_

#include <limits>
#include <memory>
#include <cstdlib>
#include <string>
#include <vector>


#include "geometry/path.hpp"
#include "geometry/bounds.hpp"
#include "geometry/polygon.hpp"
#include "index/row-index.hpp"

namespace chart::search {

// currently, this is only implemented for grids.
template<typename chart_t>
class AStar {
public:

    AStar() = delete;

    AStar( const chart_t & chart );


    /// \brief Performs an A* search on the given chart data structure
    /// 
    /// ## Implementation Specifics
    /// The default implementation of A* assumes a regularly spaced grid, with index-lookups. 
    /// This could easily be extended to real-space by assuming an implicit grid of 
    /// regularly spaced points... but this latter option has not yet been implemented.
    ///
    ///
    /// ### Data Structures 
    ///   - metadata grid
    ///       - provides lookup for min-cost to reach given cell 
    ///       - stores heuristic: distance-to-goal
    ///   - fringe
    ///     -- std::priority_queue with pointers into the above metadata
    ///   - path-construction: 
    ///      -- at first, implicit in the metadata grid
    ///      -- next, constructed as a doubly-linked list.
    ///      -- finally returned as a `geometry::Path`, aka: `vector< pair< double, double>>`
    ///
    /// ### See Also:
    ///   - https://gabrielgambetta.com/astar-demystified.html
    ///   - https://en.wikipedia.org/wiki/A*_search_algorithm
    ///
    /// \param start - location to start searching from
    /// \param end  - location to searching to
    chart::geometry::Path compute( const LocalLocation& start, const LocalLocation& end );

private:
    constexpr static bool simplify_elbows = true;
    constexpr static float maximum_separation = 4.0;
    chart::geometry::Path extract_path(const index::Index2u& goal);

    static std::array<index::Index2u, 8> get_8_neighbors(const index::Index2u& center);

    static std::array<index::Index2u, 4> get_4_neighbors(const index::Index2u& center);

    std::string to_debug() const ;

private:
    typedef float cost_t;

    const chart_t & grid_;

    struct FringeCell {
        index::Index2u at;
        cost_t cost_to_goal; ///< distance-to-goal heuristic
        // cost_t cost_spent;   ///< cost to travel from the start to here

        inline FringeCell( const index::Index2u& _index, /*cost_t _cost_spent,*/ cost_t _cost_to_goal)
            : at(_index)
            // , cost_spent(_cost_spent)
            , cost_to_goal(_cost_to_goal)
        {}

        // defined for the fringe next-cell-to-explore data structure
        inline bool operator<(const FringeCell& rhs) const {
                return (this->cost_to_goal > rhs.cost_to_goal); }
    };

    // plain-old-data struct
    struct VisitedCell {
        static constexpr cost_t maximum_cost = std::numeric_limits<cost_t>::max();

        cost_t cost_spent; // travel-distance from start-cell
        // cost_t cost_to_goal;

        /// \brief i,j location of the previous node in this path
        /// \note initialized to 0,0, or the start-point by default
        index::Index2u previous;
    };
    std::array<std::array<VisitedCell, chart_t::dimension()>, chart_t::dimension()> visited_;

private:
    inline VisitedCell& visited(const index::Index2u& i) { return visited_[i.i][i.j]; }

};

} // namespace chart::search

#include "search/a-star.inl"

#endif // #define _CHART_SEARCH_A_STAR_HPP_
