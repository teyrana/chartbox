// GPL v3 (c) 2020

#include <cmath>

#include "geometry/point.hpp"
#include "geometry/segment.hpp"

using yggdrasil::geometry::Point;
using yggdrasil::geometry::Segment;


Segment::Segment() {}

Segment::Segment(const Point& _start, const Point& _end):
  start(_start), end(_end)
{}

void Segment::clear(){
  this->start.clear();
  this->end.clear();
}
