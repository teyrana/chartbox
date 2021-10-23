# ============= Fixed-Grid Chart Layer Library =================
SET(LIB_NAME staticgridlayer )
SET(LIB_HEADERS static-grid-layer.hpp
                # z-order-index.hpp z-order-index.inl
                )
SET(LIB_SOURCES 
                static-grid-layer.cpp
                )

MESSAGE( STATUS "Generating StaticGrid Library: ${LIB_NAME}")
MESSAGE( STATUS "    with headers: ${LIB_HEADERS}")
MESSAGE( STATUS "    with sources: ${LIB_SOURCES}")

# header + source static library
add_library(${LIB_NAME} STATIC ${LIB_HEADERS} ${LIB_SOURCES})
target_link_libraries(${LIB_NAME} PRIVATE ${LIBRARY_LINKAGE} )
target_link_libraries(${LIB_NAME} PRIVATE CONAN_PKG::gdal )

# ============= Chart Base Library =================
# These tests can use the Catch2-provided main
add_executable( static-grid-tests
                static-grid.test.cpp
                )

target_link_libraries(static-grid-tests PRIVATE ${LIB_NAME})
target_link_libraries(static-grid-tests PRIVATE ${LIBRARY_LINKAGE} )
target_link_libraries(static-grid-tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(static-grid-tests PRIVATE CONAN_PKG::fmt)
