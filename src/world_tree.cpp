// GPL v3 (c) 2020

#include <Eigen/Geometry>

// #include <nlohmann/json.hpp>

#include "geometry/polygon.hpp"

#include "world_tree.hpp"

using yggdrasilgeometry::Polygon;

using nlohmann::json;


namespace yggdrasil {

nlohmann::json generate_diamond( const double width,
                                 const double desired_precision)
{
    const double width_2 = width /2;
    return {{"layout",  {{"precision", desired_precision},
                         {"x", width_2},
                         {"y", width_2},
                         {"width", width}}},
            {"allow", {{{  width, width_2},
                        {width_2,   width},
                        {      0, width_2},
                        {width_2,       0}}}}};
}

}; // namespace yggdrasil
