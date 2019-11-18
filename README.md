Yggdrasil 
===============================================================

Yggdrasil implements a 2d navigation library for autonomous navigation.

More precisely, it implements a probabalistic occupancy grid, split into tiles for performant lookup.  Each tile is the leaf node of a quadtree.  Thus the library provides a quadtree spanning the robot's entire world: that is, a world-tree.

The library also intends to provide an interface for pathfinding algorithms, like A*. 

Purpose
-------

While quadtrees have a wide variety of uses, this implementation is designed to partition space into allowable area, and blocked area.  (Both may be convex, concave, or disjoint.)  The allowable nature of the space is tracked on a square-area basis (in fact, the include reference implementation is a grid.)

For instance, imagine a robot in a room; in order to locate the robot in the room, and the boundaries of the room.


Build
-----
This project uses CMake as its primary build system. `build.sh` in the base directory should automatically build all executables.


### External Dependencies
#### System - Installed (i.e. `apt install ...` or `brew install ...`)
This project makes use of several libraries
- cxxopts [10] - command-line argument parsing libary
- FlatBuffers [12] - Used for tile serialization
- GDAL [5] - Used for file I/O-- including height map as png, and loading shapefiles
- Google Test Framework [6]- used to run all of the development tests

## Handled by Conan:
- Eigen [7] (@3.3.7) - math / linear algebra library
- nlohmann/json [4] (@3.7.3) - json I/O libray

### Included, vendored dependencies
- CNL [11] - fixed point math library

References
----------
- [1] [Quadtree (Wikipedia)](http://en.wikipedia.org/wiki/Quadtree)
- [2] [varunpant/Quadtree (Java Implementation)](https://github.com/varunpant/Quadtree)
- [3] [Jefflarson/quadtree (C Implementation)](https://github.com/thejefflarson/quadtree)
- [4] [nlohmann/json](https://github.com/nlohmann/json)
- [5] [GDAL](https://gdal.org/)
- [6] [Google Test Framework](https://github.com/google/googletest)
- [7] [Eigen library](http://eigen.tuxfamily.org/index.php)
- [8] [Z-Order Curve](https://en.wikipedia.org/wiki/Z-order_curve)
- [9] [Comparable Implementation](https://github.com/google/s2geometry)
- [10] [cxxopts](https://github.com/jarro2783/cxxopts)
- [11] [CNL](https://github.com/johnmcfarlane/cnl)
- [12] Flatbuffers](https://google.github.io/flatbuffers/)