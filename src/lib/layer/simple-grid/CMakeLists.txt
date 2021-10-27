# ============= Fixed-Grid Chart Layer Library =================
SET(LIB_NAME simple-grid-layer )

SET(LIB_HEADERS ${COMMON_LAYER_INCLUDES}
                simple-grid-layer.hpp
                )
SET(LIB_SOURCES 
                simple-grid-layer.cpp
                )

MESSAGE( STATUS "Generating StaticGrid Library: ${LIB_NAME}")
MESSAGE( STATUS "    with headers: ${LIB_HEADERS}")
MESSAGE( STATUS "    with sources: ${LIB_SOURCES}")

# header + source static library
add_library(${LIB_NAME} STATIC ${LIB_HEADERS} ${LIB_SOURCES})
target_link_libraries(${LIB_NAME} PRIVATE ${LIBRARY_LINKAGE} )

# ============= Chart Base Library =================
# These tests can use the Catch2-provided main
set( TEST_BIN_NAME simple-layer-tests )
add_executable( ${TEST_BIN_NAME}
                simple-grid.test.cpp
                )

target_link_libraries(${TEST_BIN_NAME} PRIVATE ${LIB_NAME})
target_link_libraries(${TEST_BIN_NAME} PRIVATE ${LIBRARY_LINKAGE} )
target_link_libraries(${TEST_BIN_NAME} PRIVATE Catch2::Catch2WithMain)
