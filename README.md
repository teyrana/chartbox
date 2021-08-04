Chartbox 
===============================================================

Chartbox implements .... a whole bunch of random stuff... for autonomous navigation.

More precisely, it implements a probabalistic occupancy grid, split into tiles for performant lookup.  Each tile is the leaf node of a quadtree.  Thus the library provides a quadtree spanning the robot's entire world: that is, a world-tree.

The library also intends to provide an interface for pathfinding algorithms, like A*. 

Purpose
-------

While quadtrees have a wide variety of uses, this implementation is designed to partition space into allowable area, and blocked area.  (Both may be convex, concave, or disjoint.)  The allowable nature of the space is tracked on a square-area basis (in fact, the include reference implementation is a grid.)

For instance, imagine a robot in a room; in order to locate the robot in the room, and the boundaries of the room.

Project Directories
-------------------
``` bash
build/              # Used for storing ephemeral build results.  Not in version control.
data/               # non-source code resources.  i.e. graphics and markup files
docs/               # Contains project documentation.
examples/           # samples and examples.
external/           # All 3rd party projects used by the project, but not edited / maintained
include/            # _Public_ headers.
optional/           # optional modules.  Not required for builds.
src/                # Main compilable source location.
src/A/              # each module contains its: 
                    #  - source (.cpp)
                    #  - private headers (*.hpp)
                    #  - tests (.test.cpp)
tests/              # Integration tests.
tools/              # Development utilities, such as build and refactoring scripts.
                    # Should be standalone executables.
```


Build
-----
This project uses CMake as its primary build system. `make debug` in the base directory should automatically build all executables.


External Dependencies
-----

### System - Installed (i.e. `apt install ...` )

This project makes use of several libraries
<!-- - [Flatbuffers](https://google.github.io/flatbuffers/)
    - Used for tile serialization -->
- [Conan](https://docs.conan.io/en/latest/introduction.html)
    - Use this to manage certain dependencies 

### Handled by Conan
- [Catch2](https://github.com/catchorg/Catch2) 
    - used to run all of the development tests
- [Eigen library](http://eigen.tuxfamily.org/index.php) @ 3.3.7
    - math / linear algebra library
- [fmt](https://github.com/fmtlib/fmt) @ 6.1.2 
    - output formatting library
- [GDAL](https://gdal.org/)
    - load shapefiles, PNG - height-maps 
- [PDAL](https://pdal.io/) - load LAS files
    - helps with point clouds


References
----------
- [1] [Quadtree (Wikipedia)](http://en.wikipedia.org/wiki/Quadtree)
- [2] [varunpant/Quadtree (Java Implementation)](https://github.com/varunpant/Quadtree)
- [3] [Jefflarson/quadtree (C Implementation)](https://github.com/thejefflarson/quadtree)
- [4] [Pitchfork Layout](https://github.com/vector-of-bool/pitchfork/)

