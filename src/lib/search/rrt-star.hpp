// GPL v3 (c) 2020, Daniel Williams 

#ifndef _CHART_SEARCH_RRT_STAR_HPP_
#define _CHART_SEARCH_RRT_STAR_HPP_

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
class RRTStar {
public:

    RRTStar() = delete;

    RRTStar( const chart_t & chart );


    /// \brief Performs an RRT* search on the given chart data structure
    /// 
    /// ## Implementation Specifics
    ///
    ///
    /// ### Data Structures 
    ///     ????
    ///
    /// ### See Also:
    ///   - https://en.wikipedia.org/wiki/Rapidly-exploring_random_tree
    ///
    /// \param start - location to start searching from
    /// \param end  - location to searching to
    chart::geometry::Path compute( const LocalLocation& start, const LocalLocation& end );

private:

    std::string to_debug() const ;

private:
    typedef float cost_t;

    const chart_t & grid_;

};

} // namespace chart::search

#include "search/rrt-star.inl"

#endif // #define _CHART_SEARCH_RRT_STAR_HPP_
