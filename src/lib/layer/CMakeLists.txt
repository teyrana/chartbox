ADD_SUBDIRECTORY(dynamic-grid)
ADD_SUBDIRECTORY(static-grid)
ADD_SUBDIRECTORY(rolling-grid)
# ADD_SUBDIRECTORY(quad-tree)
# ADD_SUBDIRECTORY(view)

set( COMMON_LAYER_INCLUDES layer-interface.hpp
                            layer-interface.inl
                            grid-index.hpp )

# ============= Chart Base Library =================
# These tests can use the Catch2-provided main
set(TEST_BIN_NAME common-layer-tests)
add_executable( ${TEST_BIN_NAME}
                grid-index.test.cpp
                )
target_link_libraries(${TEST_BIN_NAME} PRIVATE ${LIB_NAME})
target_link_libraries(${TEST_BIN_NAME} PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${TEST_BIN_NAME} PRIVATE CONAN_PKG::fmt)
