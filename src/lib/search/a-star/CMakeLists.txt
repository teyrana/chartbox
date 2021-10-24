
# ============= Chart Base Library =================
SET(LIB_NAME a-star-search )
SET(LIB_HEADERS ${COMMON_SEARCH_INCLUDES}
                a-star-search.hpp
                a-star-search.inl
                )
# SET(LIB_SOURCES ${COMMON_SEARCH_SOURCES}
#                 a-star-search.cpp
#                 )

MESSAGE( STATUS "Generating A* Search Library: ${LIB_NAME}")
MESSAGE( STATUS "    with headers: ${LIB_HEADERS}")
MESSAGE( STATUS "    with sources: ${LIB_SOURCES}")

# # header + source static library
# add_library(${LIB_NAME} STATIC ${LIB_HEADERS} ${LIB_SOURCES})
# target_link_libraries(${LIB_NAME} PRIVATE ${LIBRARY_LINKAGE} )

# header only library
add_library(${LIB_NAME} INTERFACE )

# ============= Chart Base Library =================
# These tests can use the Catch2-provided main
set( TEST_BIN_NAME a-star-search-tests )
add_executable( ${TEST_BIN_NAME}
                ${LIB_HEADERS}
                ${LIB_SOURCES}
                a-star-search.test.cpp
                )

target_link_libraries(${TEST_BIN_NAME} PRIVATE ${LIB_NAME})
target_link_libraries(${TEST_BIN_NAME} PRIVATE ${LIBRARY_LINKAGE} )
target_link_libraries(${TEST_BIN_NAME} PRIVATE Catch2::Catch2WithMain)
