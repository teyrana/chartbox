CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT( chartbox )

set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CPP_COMPILER /usr/bin/clang)

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
conan_basic_setup(TARGETS)

# Required, even with conan
find_package(Catch2 3 REQUIRED)

# ============= PDal =================
SET( APPEND LIBRARY_LINKAGE CONAN_PKG::pdal CONAN_PKG::fmt )


# Set Compile Flags
#=============================================================================
# specify as CMAKE_CXX_FLAGS, instead of 'TARGET_COMPILE_OPTIONS(...' because we have multiple compilation targets.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a  -fno-exceptions -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-unused-result")

# set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")
# # Clang-Tidy doesn't check headers by default.  Enable that:
# set(CMAKE_CXX_CLANG_TIDY clang-tidy;-header-filter=.;-checks=*;)

set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -Og -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG")

MESSAGE( STATUS ">>> Configuring Target Architecture: ${CMAKE_TARGET_SYSTEM_ARCHITECTURE}")
IF(NOT DEFINED TARGET_SYSTEM_PROCESSOR)
    MESSAGE( STATUS ".... Target Architecture not defined. Using: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
    SET(TARGET_SYSTEM_PROCESSOR ${CMAKE_HOST_SYSTEM_PROCESSOR})
ELSEIF(TARGET_SYSTEM_PROCESSOR STREQUAL "Host")
    MESSAGE( STATUS ".... Configuring 'Host' Mode: ${CMAKE_HOST_SYSTEM_ARCHITECTURE}")
    SET(TARGET_SYSTEM_PROCESSOR ${CMAKE_HOST_SYSTEM_PROCESSOR})
ENDIF()


MESSAGE( STATUS ">>> Configuring Build Type: ${CMAKE_BUILD_TYPE}")
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    MESSAGE( STATUS ".... Configuring Debug Mode.")
ELSEIF(CMAKE_BUILD_TYPE STREQUAL "Release")
    MESSAGE( STATUS ".... Configuring Release Mode.")
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

# ============= Enable Testing  =================
#ExternalProject_Add(GTest
#    TMP_DIR ${PROJECT_SOURCE_DIR}/vendor/googletest 
#    GIT_REPOSITORY https://github.com/google/googletest.git) 
#
# INCLUDE_DIRECTORIES(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR}) 

ENABLE_TESTING()

INCLUDE_DIRECTORIES(${CONAN_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(extern)
INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(src/lib)

ADD_SUBDIRECTORY(src/lib/chart-box)
SET(LIBRARY_LINKAGE ${LIBRARY_LINKAGE} chartbox )

# depend on chart-box:
ADD_SUBDIRECTORY(src/lib/io)
SET(LIBRARY_LINKAGE ${LIBRARY_LINKAGE} chartloaders chartsavers )

ADD_SUBDIRECTORY(src/lib/layer/static-grid)
SET(LIBRARY_LINKAGE ${LIBRARY_LINKAGE} staticgridlayer )

# ADD_SUBDIRECTORY(src/lib/layer/quad-tree)
# ADD_SUBDIRECTORY(src/lib/layer/view)

# ADD_SUBDIRECTORY(src/process/profile)
ADD_SUBDIRECTORY(src/process/merge)
