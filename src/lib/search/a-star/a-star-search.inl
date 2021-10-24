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
    // we just constructed this, so OF COURSE it should be empty.
    assert( 0 == fringe_.size());

    // not dynamic.... yet
    assert( context_.dimension() == visited_.dimension() );
    assert( 1.0 == visited_.meters_across_cell() );

    visited_.fill( VACANT_FLAG );
    //for( size_t i = 0; i < visited_.size(); ++i ){
    //    fmt::print("    @[{}]:\n", i );
    //     cell.cost_spent = VisitedCell::maximum_cost;
    //     cell.previous = {0,0};
    //}

}

template<typename layer_t>
float AStarSearch<layer_t>::cost( const LocalLocation& p, const LocalLocation& goal ){
    return (goal - p).norm2();
}

template<typename layer_t>
SearchPath AStarSearch<layer_t>::extract_path(const geometry::LocalLocation& goal) {
    // is the list easy to use, for some reason?
    // geometry::Path<LocalLocation> draft_path;
    std::list<LocalLocation> draft_v1_list;

    {   // Stage 1: Extract Raw Path from `visited_` array
        fmt::print( "====== 2. Trace Gradients: ======\n");
        LocalLocation at = goal;
        uint8_t value = SENTINEL_FLAG;
        do {
            draft_v1_list.push_front(at);
            value = visited_.get(at);
            if( VACANT_FLAG == value ){

            }

            // fmt::print( "    >> @ {} == {:02X} \n", at.to_string(), value );
            const LocalLocation delta = decode_adjacency_flags( value );
            // fmt::print( "       ==>> {} \n", delta.to_string() );
            at += delta;
        } while( value != SENTINEL_FLAG );

        // fmt::print( "====== First Draft Path: ======\n");
        // for( auto& p : draft_v1_list ){
        //     fmt::print( "    - {}\n", p.to_string() );
        // }
    }

    {   // Stage 2: Reduce Excess vertices
        fmt::print( "====== 3. Reduce Vertices ======\n");
//     auto cur = draft_path.begin();
//     LocalLocation p0 = *cur;
//     LocalLocation p1 = *(++cur);
//     LocalLocation p2 = *(++cur);
//     while( cur != draft_path.end() ){
//         const signed int s0x = static_cast<int>(p1.row) - static_cast<int>(p0.row);
//         const signed int s0y = static_cast<int>(p1.column) - static_cast<int>(p0.column);
//         const signed int s1x = static_cast<int>(p2.row) - static_cast<int>(p1.row);
//         const signed int s1y = static_cast<int>(p2.column) - static_cast<int>(p1.column);

//         // fmt::print(stderr, "==> p0:      %3d, %3d\n", p0.row, p0.column);
//         // fmt::print(stderr, "      > s0:      %3d, %3d\n", s0x, s0y);
//         // fmt::print(stderr, "    p1:      %3d, %3d\n", p1.row, p1.column);
//         // fmt::print(stderr, "      > s1:      %3d, %3d\n", s1x, s1y);
//         // fmt::print(stderr, "    p2:      %3d, %3d\n", p2.row, p2.column);

//         if( maximum_separation < std::max(s1x,s1y) ){
//             // noop: just let the loop advance
        
//         // (a) if segments are the same direction:
//         // - *definitely* reduce
//         // - rewind to beginning of reduced segment
//         }else if( (s0x==s1x) && (s0y==s1y) ){
//             cur = draft_path.erase(--cur);
//             p1 = p2;
//             p2 = *(++cur);
//             continue;
//         }

//         p0 = p1;
//         p1 = p2;
//         p2 = *(++cur);
//     }

    //         fmt::print(stderr, "====== Reduced Path: ======\n");
    //         for( auto& p : draft_path ){
    //             fmt::print(stderr, "    %d, %d\n", p.row, p.column);
    //         }
    }

    // convert the list -> path/vector
    Path<LocalLocation> final_path;( draft_v1_list.size() );
    for( auto& at : draft_v1_list ){
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
std::string AStarSearch<layer_t>::to_debug() const {
    static const std::string header("======== ======= ======= ======= ======= ======= ======= ======= ======= =======\n");
    std::ostringstream buf;

    buf << header;
    // print from top-down (max j => min j) to mirror north => south
    for (size_t j = visited_.dimension() - 1; j < visited_.dimension(); --j) {
        for (size_t i = 0; i < visited_.dimension(); ++i) {
            buf << fmt::format(" {:02X}", visited_.get({i,j}) );
        }
        buf << '\n';
    }
    buf << header;
    return buf.str();
}

template<typename layer_t>
SearchPath AStarSearch<layer_t>::compute( const LocalLocation& start_point, const LocalLocation& goal_point ) {

    const auto search_bounds = context_.visible();
    if( ! search_bounds.contains(start_point) || ! search_bounds.contains(goal_point) ){
        return {}; // error condition
    }

    fmt::print( "====== 0. Setup: ======\n");
    fmt::print("    ::Start @ {}    //  {:5.2f}\n", start_point.to_string(), cost(start_point,goal_point) );
    fmt::print("    ::Goal  @ {}    //  {:5.2f}\n", goal_point.to_string(), 0.0 );

    // explicitly -- DO NOT MARK the goal!  it interferes with the control flow, below.
    // visited_.store( goal_point, SENTINEL_FLAG | FLAG_GOAL );

    visited_.store( start_point, SENTINEL_FLAG );
    fringe_.emplace( start_point, cost(start_point,goal_point) );

    // DEBUG:  drastically restrict this value
    // const uint32_t iteration_guard = grid_.size();
    constexpr uint32_t iteration_guard = 12;

    fmt::print( "====== 1. Build Vectors To Goal: ======\n");
    uint32_t iteration = 0;
    while( (0 < fringe_.size()) && (iteration < iteration_guard) ){
         
        // pop next node:
        // ( because we're using a priority heap, it will be the closest available to our goal
        auto next = fringe_.top();
        fringe_.pop();
        const auto& current_center = next.at;

        // {   // DEBUG
        //     const double current_cost = next.cost;
        //     fmt::print("    ::[{:2d} ?<? {:2d}]    @@ {:2.0f}, {:2.0f}  //  {:5.2f}\n", iteration, iteration_guard, current_center.easting, current_center.northing, current_cost );
        // }   // DEBUG

        // Add next nodes (neighbors) to our search list
        const auto& neighbors = get_neighbors( current_center, neighbor_8_offsets );
        // size_t neighbor_index = 0; // debugging-only
        for( auto& each_point : neighbors ) {
            const float each_cost = cost( each_point, goal_point );
            const uint8_t each_delta_flags = encode_adjacency_flags( current_center - each_point );

            // DEBUG
            // fmt::print( "        :[{}] @ {:2.0f}, {:2.0f}  //  {:5.2f}  //  {:02X}h\n", 
            //                         neighbor_index, each_point.easting, each_point.northing,
            //                         each_cost, each_delta_flags );
            // ++neighbor_index;
            // DEBUG

            if( not search_bounds.contains( each_cost )){
                //fmt::print( "            << Out-Of-Bounds\n");
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
        // fmt::print( "    <<({}) fringe points left\n", fringe_.size() );

        ++iteration;
    }

#ifdef DEBUG
    fmt::print("<<< Iteration guard reached !!\n");
#endif // #ifdef DEBUG

    // If we get here, no path was found
    return {};
}


} // close namespace
