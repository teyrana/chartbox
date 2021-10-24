ADD_SUBDIRECTORY(a-star)
# ADD_SUBDIRECTORY(rrt)

set( COMMON_SEARCH_INCLUDES
                        #cost.hpp
                        )
set( COMMON_SEARCH_SEARCHES  
                        #cost.cpp
                        )

# # ============= Search Tests =================
# # These tests can use the Catch2-provided main
# set(TEST_BIN_NAME common-search-tests)
# add_executable( ${TEST_BIN_NAME}
#                 cost.test.cpp
#                 )
# target_link_libraries(${TEST_BIN_NAME} PRIVATE ${LIB_NAME})
# target_link_libraries(${TEST_BIN_NAME} PRIVATE Catch2::Catch2WithMain)
# target_link_libraries(${TEST_BIN_NAME} PRIVATE CONAN_PKG::fmt)