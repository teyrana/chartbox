// GPL v3 (c) 2020, Daniel Williams 

#ifndef _INTERPOLATE_HPP_
#define _INTERPOLATE_HPP_

#include "local-location.hpp"

#include "sample.hpp"

namespace chart::geometry {

/**
 * Performs the low-level interpolation between this node and another node, at
 * the requested location
 *
 * @param {Point} the x,y coordinates to interpolate at.
 * @param {quadtree::Node} n2 the other node to interpolate
 * @return The resultant value
 */
template <typename T>
T interpolate_linear(const LocalLocation& at, const Sample<T>& s1,
                     const Sample<T>& s2);

/**
 * Performs bilinear-interpolation:
 * http://en.wikipedia.org/wiki/Bilinear_Interpolation
 *
 * Warning: Undefined behavior, if interpolation happens outside of the
 * rectangle defined by the 4 samples
 *
 * @param the x,y coordinates to interpolate at.
 * @param ne quadrant sample point
 * @param nw quadrant sample point
 * @param sw quadrant sample point
 * @param se quadrant sample point
 * @return resultant value
 */
template <typename T>
T interpolate_bilinear(const LocalLocation& at, const Sample<T>& ne,
                       const Sample<T>& nw, const Sample<T>& sw,
                       const Sample<T>& se);

} // namespace chart::geometry

#include "interpolate.inl"

#endif // #ifndef _INTERPOLATE_HPP_
