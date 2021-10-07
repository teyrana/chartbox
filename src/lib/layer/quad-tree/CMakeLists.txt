# ============= Quad-Tree Layer Library =================
SET(LIB_NAME quadtreelayer )
SET(LIB_HEADERS quad-node.hpp
                leaf-node.hpp
                quad-tree-layer.hpp
                )
SET(LIB_SOURCES quad-node.cpp
                leaf-node.cpp
                quad-tree-layer.cpp
                )

MESSAGE( STATUS "Generating GridChart Library: ${LIB_NAME}")
MESSAGE( STATUS "    with headers: ${LIB_HEADERS}")
MESSAGE( STATUS "    with sources: ${LIB_SOURCES}")

# internal library dependency
target_link_libraries(chartbox)

# header + source static library
add_library(${LIB_NAME} STATIC ${LIB_HEADERS} ${LIB_SOURCES})
target_link_libraries(${LIB_NAME} PRIVATE ${LIBRARY_LINKAGE})
target_link_libraries(${LIB_NAME} PRIVATE chartbox )
target_link_libraries(${LIB_NAME} PRIVATE CONAN_PKG::gdal )

# ============= Quad-Tree Tests =================
# These tests can use the Catch2-provided main
add_executable( quad-tree-tests
                leaf-node.test.cpp
                quad-node.test.cpp
                quad-tree-layer.test.cpp
                )

target_link_libraries( quad-tree-tests PRIVATE ${LIB_NAME})
target_link_libraries( quad-tree-tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries( quad-tree-tests PRIVATE CONAN_PKG::fmt)
