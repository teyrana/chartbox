CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT( libyggdrasil )

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake) 
MESSAGE(STATUS "bindir: " ${CMAKE_BINARY_DIR})

#=============================================================================
# Set the output directories for the binary and library files
#=============================================================================

#=============================================================================
# unknown block purpose
#=============================================================================
# set(CMakeScripts_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
# set(CMAKE_MODULE_PATH ${CMakeScripts_MODULE_DIR})

# ============= Conan-Managed Dependencies =================
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
SET(LIBRARY_LINKAGE ${LIBRARY_LINKAGE} ${CONAN_LIBS})

# ============= GDal =================
# Used for most file I/O
SET(GDAL_PATH "/usr/local/Cellar/gdal/2.4.2/lib/")
# SET(GDAL_LINKAGE "/usr/local/Cellar/gdal/2.4.2/lib/libgdal.dylib")
SET(GDAL_LINKAGE gdal)
SET(LIBRARY_LINKAGE ${LIBRARY_LINKAGE} ${GDAL_LINKAGE})
ADD_DEFINITIONS(-DENABLE_GDAL)

MESSAGE( STATUS "Configured GDal... ")
MESSAGE( STATUS "    with path: ${GDAL_PATH}")

# Set Compile Flags
#=============================================================================
# specify as CMAKE_CXX_FLAGS, instead of 'TARGET_COMPILE_OPTIONS(...' because we have multiple compilation targets.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17  -fno-exceptions -fno-rtti")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic ")

MESSAGE( STATUS ">> Configuring Build Type: ${CMAKE_BUILD_TYPE}")
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    MESSAGE( STATUS ".... Configuring Debug Mode.")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -g")
ELSEIF(CMAKE_BUILD_TYPE STREQUAL "Release")
    MESSAGE( STATUS ".... Configuring Release Mode.")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O2")
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

# ============= Yggdrasil (World Tree) Library =================
SET(YGGDRASIL_LIB_NAME yggdrasil)

# debug include dirs:
#=============================================================================
MESSAGE(STATUS "==== Conan Include Dirs ====")
MESSAGE(STATUS ${CONAN_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CONAN_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(vendor)
INCLUDE_DIRECTORIES(include)
SET(LIB_HEADERS #include/terrain.hpp include/terrain.inl
                include/geometry/interpolate.hpp
                include/geometry/layout.hpp
                include/geometry/polygon.hpp
                # include/io/json.hpp
                # include/io/readers.hpp include/io/readers.inl
                # include/io/writers.hpp include/io/writers.inl
                # include/node/quad_node.hpp
                include/node/tile_node.hpp
                # include/quadtree/tree.hpp
                )

SET(LIB_SOURCES
                src/geometry/interpolate.cpp
                src/geometry/layout.cpp
                src/geometry/polygon.cpp
                # src/node/quad_node.cpp
                src/node/tile_node.cpp
                # src/quadtree/tree.cpp
                )

#Generate the static library from the sources
add_library(${YGGDRASIL_LIB_NAME} STATIC ${LIB_HEADERS} ${LIB_SOURCES})

TARGET_LINK_LIBRARIES(${YGGDRASIL_LIB_NAME} PRIVATE ${LIBRARY_LINKAGE})

# ============= Enable Testing  =================

#ExternalProject_Add(GTest
#    TMP_DIR ${PROJECT_SOURCE_DIR}/vendor/googletest 
#    GIT_REPOSITORY https://github.com/google/googletest.git) 
#
# INCLUDE_DIRECTORIES(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR}) 

enable_testing()

# ============= Build Project Tests  =================
# https://cmake.org/cmake/help/v3.0/module/FindGTest.html 
SET(TEST_EXE testall) 
SET(TEST_SOURCES
                    test/geometry/interpolate.cpp
                    test/geometry/layout.cpp
                    test/geometry/polygon.cpp
                    # test/node/quad_node.cpp
                    test/node/tile_node.cpp
                    # test/quadtree/tree.cpp
                    )

SET(TEST_LINKAGE ${YGGDRASIL_LIB_NAME} ${LIBRARY_LINKAGE} ${CONAN_LIBS} gtest gtest_main )

MESSAGE( STATUS "Generating tests for ${LIB_NAME}") 
MESSAGE( STATUS "    with name: ${TEST_EXE}") 
MESSAGE( STATUS "    with sources: ${TEST_SOURCES}") 
MESSAGE( STATUS "    with linkage: ${TEST_LINKAGE}") 

ADD_EXECUTABLE( ${TEST_EXE} ${TEST_SOURCES}) 
ADD_TEST(AllTestsInFoo ${TEST_EXE}) 


TARGET_LINK_LIBRARIES(${TEST_EXE} ${TEST_LINKAGE})


# ============= Build Profiling Program  =================
SET(PROFILE_EXE profile)
SET(PROFILE_SOURCES src/profile/main.cpp)

MESSAGE( STATUS "Generating profiling program: ${PROFILE_EXE}")
MESSAGE( STATUS "    with sources: ${PROFILE_SOURCES}")
MESSAGE( STATUS "    with linkage: ${PROFILE_LINKAGE}") 

# # DEVEL
# ADD_EXECUTABLE( ${PROFILE_EXE} ${PROFILE_SOURCES})
# TARGET_INCLUDE_DIRECTORIES(${PROFILE_EXE} PRIVATE vendor)

# TARGET_LINK_LIBRARIES(${PROFILE_EXE} PRIVATE ${YGGDRASIL_LIB_NAME} ${PROFILE_LINKAGE}) 
