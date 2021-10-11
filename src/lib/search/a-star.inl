// GPL v3 (c) 2020, Daniel Williams 

#include <cmath>
#include <cstdio>
#include <list>
#include <queue>
#include <sstream>

#include "search/cost.hpp"

using chartbox::ChartInterfaceLayer;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::Bounds;
using chartbox::geometry::Path;
using chartbox::index::Index2u;
using chartbox::search::AStar;

using chart::search::euclideanCost;

template<typename chart_t>
AStar<chart_t>::AStar( const chart_t& _grid ) 
    : grid_(_grid) 
{
    for(uint32_t i=0; i < grid_.dimension(); ++i){
        for(uint32_t j=0; j < grid_.dimension(); ++j){
            visited_[i][j] = { VisitedCell::maximum_cost, Index2u(i,j) };
        }
    }
}

template<typename chart_t>
chart::geometry::Path AStar<chart_t>::extract_path(const index::Index2u& goal) {
    std::list<Index2u> draft_path;
    draft_path.push_front(goal);

    Index2u current_index = goal;
    VisitedCell current_cell = visited(goal);

    // the start cell points to itself, so that's the termination condition
    while( current_index != current_cell.previous ){
        current_index = current_cell.previous;
        current_cell = visited(current_index);
        draft_path.push_front(current_index);
    }

// #ifdef DEBUG
//         fprintf(stderr, "====== First Draft Path: ======\n");
//         for( auto& p : draft_path ){
//             fprintf(stderr, "    %d, %d\n", p.i, p.j);
//         }
// #endif // DEBUG

    // Reduce redundant points
    auto cur = draft_path.begin();
    Index2u p0 = *cur;
    Index2u p1 = *(++cur);
    Index2u p2 = *(++cur);
    while( cur != draft_path.end() ){
        const signed int s0x = static_cast<int>(p1.i) - static_cast<int>(p0.i);
        const signed int s0y = static_cast<int>(p1.j) - static_cast<int>(p0.j);
        const signed int s1x = static_cast<int>(p2.i) - static_cast<int>(p1.i);
        const signed int s1y = static_cast<int>(p2.j) - static_cast<int>(p1.j);

        // fprintf(stderr, "==> p0:      %3d, %3d\n", p0.i, p0.j);
        // fprintf(stderr, "      > s0:      %3d, %3d\n", s0x, s0y);
        // fprintf(stderr, "    p1:      %3d, %3d\n", p1.i, p1.j);
        // fprintf(stderr, "      > s1:      %3d, %3d\n", s1x, s1y);
        // fprintf(stderr, "    p2:      %3d, %3d\n", p2.i, p2.j);

        if( maximum_separation < std::max(s1x,s1y) ){
            // noop: just let the loop advance
        
        // (a) if segments are the same direction:
        // - *definitely* reduce
        // - rewind to beginning of reduced segment
        }else if( (s0x==s1x) && (s0y==s1y) ){
            cur = draft_path.erase(--cur);
            p1 = p2;
            p2 = *(++cur);
            continue;
        }

        p0 = p1;
        p1 = p2;
        p2 = *(++cur);
    }

// #ifdef DEBUG
//         fprintf(stderr, "====== Reduced Path: ======\n");
//         for( auto& p : draft_path ){
//             fprintf(stderr, "    %d, %d\n", p.i, p.j);
//         }
// #endif // DEBUG

    // convert the list -> path/vector
    chart::geometry::Path final_path;
    for( auto& index : draft_path){
        final_path.push_back(grid_.as_location(index));
    }

// #ifdef DEBUG
//         fprintf(stderr, "====== Final Path: ======\n");
//         for( auto& p : final_path ){
//             fprintf(stderr, "    %5.2f, %5.2f\n", p.x(), p.y() );
//         }
// #endif // DEBUG

    return final_path;
}

template<typename chart_t>
std::array<Index2u, 8> AStar<chart_t>::get_8_neighbors(const Index2u& center){
    const uint32_t i = center.i;
    const uint32_t j = center.j;
    // expect the integer-overflow-check, and bound-check at the call site
    return {Index2u(i+1,j),
            Index2u(i+1,j+1),
            Index2u(i,j+1),
            Index2u(i-1,j+1),
            Index2u(i-1,j),
            Index2u(i-1,j-1),
            Index2u(i,j-1),
            Index2u(i+1,j-1)};
}

template<typename chart_t>
std::array<Index2u, 4> AStar<chart_t>::get_4_neighbors(const Index2u& p){
    // expect the integer-overflow-check, and bound-check at the call site
    return {Index2u( p.i+1, p.j  ),
            Index2u( p.i,   p.j+1),
            Index2u( p.i-1, p.j  ),
            Index2u( p.i,   p.j-1) };
}

#ifdef DEBUG
template<typename chart_t>
std::string AStar<chart_t>::to_debug() const {
    static const std::string header("======== ======= ======= ======= ======= ======= ======= ======= ======= =======\n");
    std::ostringstream buf;

    buf << header;
    for (size_t j = grid_.dimension() - 1; j < grid_.dimension(); --j) {
        for (size_t i = 0; i < grid_.dimension(); ++i) {
            buf << std::setw(3) << std::min(static_cast<float>(99), visited_[i][j].cost_spent);
        }
        buf << '\n';
    }
    buf << header;
    return buf.str();
}
#endif // #ifdef DEBUG

template<typename chart_t>
Path AStar<chart_t>::compute( const Vector2d& start_point, const Vector2d& goal_point ) {//const {
    if( ! grid_.contains(start_point) || ! grid_.contains(goal_point) ){
        return {}; // error condition
    }

    const Index2u start_cell = grid_.as_index(start_point);
    visited(start_cell).cost_spent = 0;
    // already true; but explicitly re-stated for emphasis:
    // -- because this is the termination condition for generating the path
    visited(start_cell).previous = start_cell;

    const Index2u goal_cell = grid_.as_index(goal_point);

    std::priority_queue<FringeCell> upcoming;
    // since we start the loop with only the start cell, this cost is arbitrary.  (except for zero-- that indicates goal-arrival)
    // upcoming.emplace( start_cell, 0, 1);

     // this cost doesn't _really_ matter, but it helps for debugging.
    upcoming.emplace( start_cell, euclideanCost(start_cell, goal_cell));

    const uint32_t iteration_guard = grid_.size();
    // constexpr uint32_t iteration_guard = 12;
    uint32_t iteration = 0;
    while( (0 < upcoming.size()) && (iteration < iteration_guard) ){
         
        // pop next node, closest available to our goal
        // const FringeCell fringe_cell = upcoming.top().at;
        // const VisitedCell& visited_cell = visited(fringe_cell.at);
        const Index2u at = upcoming.top().at;
// #ifdef DEBUG
//         // const float cost_spent_to_here = visited(at).cost_spent;
//         const float cost_from_here_to_goal = upcoming.top().cost_to_goal;
//         fprintf(stderr, "    @ %d, %d  // # %3d", at.i, at.j, iteration);
//         // fprintf(stderr, "    // %5.2d", cost_spent_to_here );
//         fprintf(stderr, "    + %5.2f ", std::min(static_cast<float>(99), cost_from_here_to_goal));
//         fputc('\n', stderr);
// #endif // #ifdef DEBUG
        upcoming.pop();

        const float cost_spent_to_neighbor = visited(at).cost_spent + 1;
        // Add next nodes (neighbors) to our search list
        for( auto& neighbor : get_8_neighbors(at)) {
// #ifdef DEBUG
//             cerr << "        @> " << neighbor.i << ", " << neighbor.j << endl;
// #endif // #ifdef DEBUG

            if( ! grid_.contains(neighbor) ){
                continue; // out-of-bounds. ignore.
            }

            // check if neighbor is passable 
            if( grid_.blocked(neighbor) ){
                continue;
            }

            // if this path to the neighbor cell is shorter, update the cache
            if (cost_spent_to_neighbor < visited(neighbor).cost_spent ){
                const float cost_to_goal = euclideanCost(neighbor, goal_cell);

                // If we found the goal, extract the path, and return it
                if( 0 == cost_to_goal ){
                    visited(neighbor).previous = at;
                    return extract_path(goal_cell);
                }

// #ifdef DEBUG
//                 cerr << "            ++ " << " => " << cost_spent_to_neighbor << " + " << cost_to_goal << endl;
// #endif // #ifdef DEBUG

                visited(neighbor).cost_spent = cost_spent_to_neighbor;
                visited(neighbor).previous = at;

                // TODO: FIX ME!
                // if we update the visited _after_ adding cell to the fringe tree, 
                // the fringe tree will be out-of-date.
                // .... we need to keep them both synced.
                upcoming.emplace(neighbor, cost_to_goal);
            }
        }

        ++iteration;
    }
#ifdef DEBUG
    cerr << "hit iteration guard :/";
#endif // #ifdef DEBUG

    // If we get here, no path was found
    return {};
}
