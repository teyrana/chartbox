# ============= Fixed-Grid Chart Layer Library =================
SET(LIB_NAME staticgridlayer )
SET(LIB_HEADERS static-grid.hpp
                # z-order-index.hpp z-order-index.inl
                )
SET(LIB_SOURCES 
                static-grid.cpp
                )

MESSAGE( STATUS "Generating StaticGrid Library: ${LIB_NAME}")
MESSAGE( STATUS "    with headers: ${LIB_HEADERS}")
MESSAGE( STATUS "    with sources: ${LIB_SOURCES}")

# header + source static library
add_library(${LIB_NAME} STATIC ${LIB_HEADERS} ${LIB_SOURCES})
target_link_libraries(${LIB_NAME} PRIVATE ${LIBRARY_LINKAGE} )
target_link_libraries(${LIB_NAME} PUBLIC chartbox )
target_link_libraries(${LIB_NAME} PUBLIC CONAN_PKG::gdal )

INCLUDE_DIRECTORIES(${CMAKE_SRC_DIRECTORY}/src/lib/layer/grid)
