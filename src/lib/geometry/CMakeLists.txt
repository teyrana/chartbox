
# ============= ChartBox-Geometry Library =================
# # this is a header-only library
SET(LIB_NAME chartbox-geometry)
SET(LIB_HEADERS bound-box.hpp
                frame-mapping.hpp
                polygon.hpp
                global-location.hpp
                utm-location.hpp
                )

SET(LIB_SOURCES frame-mapping.cpp
                polygon.cpp
                )

MESSAGE( STATUS "Generating ChartBox Library: ${LIB_NAME}")
MESSAGE( STATUS "    with headers: ${LIB_HEADERS}")
MESSAGE( STATUS "    with sources: ${LIB_SOURCES}")

# Generate the static library from the sources
add_library(${LIB_NAME} STATIC ${LIB_HEADERS} ${LIB_SOURCES})
target_link_libraries(${LIB_NAME} PRIVATE ${LIBRARY_LINKAGE})
target_link_libraries(${LIB_NAME} PRIVATE CONAN_PKG::fmt)
target_link_libraries(${LIB_NAME} PRIVATE CONAN_PKG::gdal)


# ============= ChartBox-Geometry Tests =================
# These tests can use the Catch2-provided main
add_executable(geometry-tests 
                ${LIB_HEADERS}
                bounds.test.cpp
                frame.test.cpp
                polygon.test.cpp
                )
target_link_libraries(geometry-tests PRIVATE ${LIB_NAME})
target_link_libraries(geometry-tests PRIVATE Catch2::Catch2WithMain)
