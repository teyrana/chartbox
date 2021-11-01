// GPL v3 (c) 2020, Daniel Williams 

#include <cmath>
#include <cstdio>
#include <list>
#include <queue>
#include <sstream>

using chartbox::layer::LayerInterface;
using chartbox::geometry::LocalLocation;
using chartbox::geometry::BoundBox;
using chartbox::geometry::Path;

namespace chartbox::search {

template<typename layer_t>
AStarSearch<layer_t>::AStarSearch( const layer_t& _context )
    : context_(_context)
{
    visited_.meters_across_cell( 1.0 );
    assert( 1.0 == visited_.meters_across_cell() );

    visited_.view( context_.visible() );
    assert( context_.visible().height() == visited_.visible().height() );
    assert( context_.visible().width()  == visited_.visible().width() );

    visited_.fill( VACANT_FLAG );
}

template<typename layer_t>
float AStarSearch<layer_t>::cost( const LocalLocation& p, const LocalLocation& goal ){
    return (goal - p).norm2();
}

template<typename layer_t>
SearchPath AStarSearch<layer_t>::extract_path(const geometry::LocalLocation& goal) {
    // std::list is easier to modify than our path (which is based on std::vector)
    std::list<LocalLocation> draft_path;

    {   // Stage 1: Extract Raw Path from `visited_` array
        LocalLocation at = goal;
        uint8_t value = SENTINEL_FLAG;
        do {
            draft_path.push_front(at);
            value = visited_.get(at);
            if( VACANT_FLAG == value ){
                fmt::print(stderr, "<<!!ERROR!!: found a vacant cell while attempting to build the path! Aborting.\n");
                return {};
            }

            const LocalLocation delta = decode_adjacency_flags( value );
            at += delta;
        } while( value != SENTINEL_FLAG );

        // fmt::print( "====== First Draft Path: ======\n");
        // for( auto& p : draft_path ){
        //     fmt::print( "    - {}\n", p.to_string() );
        // }
    }

    {   // Stage 2: Reduce Excess vertices
        auto current_point = draft_path.begin();
        LocalLocation p0 = *current_point;
        LocalLocation p1 = *(++current_point);
        LocalLocation p2 = *(++current_point);
        while( current_point != draft_path.end() ){
            const LocalLocation seg0_delta = (p1 - p0).normalize();
            const LocalLocation seg1_delta = (p2 - p1).normalize();
            const float separation = (p2 - p0).norm2();

            // const std::string prefix = "        ";
            // fmt::print(stderr, "{}==> p0:      {}\n",    prefix, p0.to_string() );
            // fmt::print(stderr, "{}      > s0:     {}\n", prefix, seg0_delta.to_string() );
            // fmt::print(stderr, "{}    p1:      {}\n",    prefix, p1.to_string() );
            // fmt::print(stderr, "{}      > s1:     {}\n", prefix, seg1_delta.to_string() );
            // fmt::print(stderr, "{}    p2:      {}\n",    prefix, p2.to_string() );

            // are we at a small enough scale to check for simplifications
            if( maximum_separation > separation ){
                // (a) if segments are the same direction:
                // - *definitely* reduce
                // - rewind to beginning of reduced segment, and continue
                if( seg0_delta == seg1_delta ){
                    current_point = draft_path.erase(--current_point);

                    // advance the counters, and restart the loop
                    p1 = *(current_point);
                    p2 = *(++current_point);
                    continue;
                }
            }

            p0 = p1;
            p1 = p2;
            p2 = *(++current_point);
        }
    }

    // convert the list -> path/vector
    Path<LocalLocation> final_path;( draft_path.size() );
    for( auto& at : draft_path ){
        final_path.emplace_back( at );
    }
    return final_path;
}

template<typename layer_t>
geometry::LocalLocation AStarSearch<layer_t>::decode_adjacency_flags( uint8_t flags ){
    // most common condition:
    if( 0 < (flags & FLAG_DELTA)){
        LocalLocation delta = {0,0};

        if( 0 < (flags & FLAG_WEST)){
            delta.easting -= 1.0;
        } else if( 0 < (flags & FLAG_EAST)){
            delta.easting += 1.0;
        }

        if( 0 < (flags & FLAG_SOUTH)){
            delta.northing -= 1.0;
        } else if( 0 < (flags & FLAG_NORTH)){
            delta.northing += 1.0;
        }

        return delta;
    } else if( 0 < (flags & SENTINEL_FLAG)){
        // termination condition
        return {0,0};
    } 

    // error condition!
    return LocalLocation::nan();
}

template<typename layer_t>
uint8_t AStarSearch<layer_t>::encode_adjacency_flags( const geometry::LocalLocation& delta ){
    return ( FLAG_DELTA 
            | ( (0<delta.easting)  ? FLAG_EAST  : ( (0>delta.easting)  ? FLAG_WEST: 0) )
            | ( (0<delta.northing) ? FLAG_NORTH : ( (0>delta.northing) ? FLAG_SOUTH: 0) ) );
}

template<typename layer_t>
template<typename array_t>
array_t AStarSearch<layer_t>::get_neighbors( const LocalLocation& center, const array_t& neighbor_offsets ){
    // expect the integer-overflow-check, and bound-check at the call site
    array_t result;
    for( size_t i = 0; i < neighbor_offsets.size(); ++i ){
        result[i] = center + neighbor_offsets[i];
    }
    return result;
}

template<typename layer_t>
SearchPath AStarSearch<layer_t>::compute( const LocalLocation& start_point, const LocalLocation& goal_point ) {

    const auto search_bounds = context_.visible();
    if( ! search_bounds.contains(start_point) || ! search_bounds.contains(goal_point) ){
        return {}; // error condition
    }

    // fmt::print("    ====== 0. Setup: ======\n");
    // fmt::print("        ::Start @ {}    ==>>    Goal @ {}\n", start_point.to_string(), goal_point.to_string() );

    // check if start is passable
    if( context_passable_threshold < context_.get(start_point) ){
        fmt::print(stderr, "    << start point is inaccessible: {} !?\n", start_point.to_string() );
        return {};
    }else if( context_passable_threshold < context_.get(goal_point) ){
        fmt::print(stderr, "    << goal point is inaccessible!?\n");
        return {};
    }

    visited_.store( start_point, SENTINEL_FLAG );
    fringe_.emplace( start_point, cost(start_point,goal_point) );

    const uint32_t iteration_guard = visited_.cells_in_view();

    // fmt::print("    ====== 1. Build Vectors To Goal: ======\n");
    uint32_t iteration = 0;
    while( (0 < fringe_.size()) && (iteration < iteration_guard) ){
        // pop next node: `fringe_` is sorted, therefore it always offers the lowest-cost-node
        auto next = fringe_.top();
        fringe_.pop();
        const auto& current_center = next.at;

        // Add next nodes (neighbors) to our search list
        const auto& neighbors = get_neighbors( current_center, neighbor_8_offsets );
        for( auto& each_point : neighbors ) {
            const float each_cost = cost( each_point, goal_point );
            const uint8_t each_delta_flags = encode_adjacency_flags( current_center - each_point );

            if( not search_bounds.contains( each_cost )){
                continue; // out-of-bounds. ignore.
            }

            // check if neighbor is passable
            if( context_passable_threshold < context_.get(each_point) ){
                continue;
            }

            // this is the canonical step, but because we're mapping orthogonal grids to real-space, it's redundant.
            // // if this path to the neighbor cell is shorter, update the cache
            // if (cost_spent_to_neighbor < visited(neighbor).cost_spent ){

            // only update not-yet-visited points
            if( 0 == visited_.get( each_point ) ){
                visited_.store( each_point, each_delta_flags );

                // If we found the goal, extract the path, and return it
                if( match_threshold > each_cost ){
                    return extract_path( goal_point );
                }

                fringe_.emplace( each_point, each_cost );
            }
        }

        ++iteration;
    }

    fmt::print("<<< Iteration guard reached !!\n");

    // If we get here, no path was found
    return {};
}


} // close namespace
