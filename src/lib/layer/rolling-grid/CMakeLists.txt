
# ============= Chart Base Library =================
SET(LIB_NAME rollinggridlayer )
SET(LIB_HEADERS 
                rolling-grid-index.hpp
                rolling-grid-layer.hpp
                rolling-grid-sector.hpp
                )
SET(LIB_SOURCES 
                rolling-grid-layer.cpp
                )

MESSAGE( STATUS "Generating RollingGrid Library: ${LIB_NAME}")
MESSAGE( STATUS "    with headers: ${LIB_HEADERS}")
MESSAGE( STATUS "    with sources: ${LIB_SOURCES}")

# header + source static library
add_library(${LIB_NAME} STATIC ${LIB_HEADERS} ${LIB_SOURCES})

target_include_directories( ${LIB_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/lib )

target_link_libraries(${LIB_NAME} PRIVATE ${LIBRARY_LINKAGE} )
target_link_libraries(${LIB_NAME} PUBLIC chartbox )
target_link_libraries(${LIB_NAME} PUBLIC CONAN_PKG::gdal )


# ============= Chart Base Library =================
# These tests can use the Catch2-provided main
add_executable( rolling-grid-tests
                    ${LIB_SOURCES}
                    rolling-grid.test.cpp
                )

target_link_libraries(rolling-grid-tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(rolling-grid-tests PRIVATE CONAN_PKG::fmt)
