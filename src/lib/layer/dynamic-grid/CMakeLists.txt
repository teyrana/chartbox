
# ============= Chart Base Library =================
SET(LIB_NAME dynamicgridlayer )
SET(LIB_HEADERS ../grid-index.hpp
                dynamic-grid-layer.hpp
                )
SET(LIB_SOURCES dynamic-grid-layer.cpp
                )

MESSAGE( STATUS "Generating Dynamic-Grid-Library: ${LIB_NAME}")
MESSAGE( STATUS "    with headers: ${LIB_HEADERS}")
MESSAGE( STATUS "    with sources: ${LIB_SOURCES}")

# header + source static library
add_library(${LIB_NAME} STATIC ${LIB_HEADERS} ${LIB_SOURCES})

target_include_directories( ${LIB_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/lib )

target_link_libraries(${LIB_NAME} PRIVATE ${LIBRARY_LINKAGE} )
target_link_libraries(${LIB_NAME} PUBLIC CONAN_PKG::gdal )

# ============= Chart Base Library =================
# These tests can use the Catch2-provided main
set(TEST_BIN_NAME dynamic-layer-tests)
add_executable( ${TEST_BIN_NAME}
                dynamic-grid-layer.test.cpp
                )
target_link_libraries(${TEST_BIN_NAME} PRIVATE ${LIB_NAME})
target_link_libraries(${TEST_BIN_NAME} PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${TEST_BIN_NAME} PRIVATE CONAN_PKG::fmt)
