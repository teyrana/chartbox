
# ============= Chart Loaders Library =================
SET(LIB_NAME chartloaders)
SET(LIB_HEADERS geojson.hpp geojson-boundary-loader.inl geojson-contour-loader.inl
                flatbuffer.hpp flatbuffer-loader.inl
                )
SET(LIB_SOURCES geojson.cpp
                )

MESSAGE( STATUS "Generating ChartBox Loaders Library: ${LIB_NAME}")
MESSAGE( STATUS "    with headers: ${LIB_HEADERS}")
MESSAGE( STATUS "    with sources: ${LIB_SOURCES}")

# header-only library variant
# # Generate the static library from the sources
# add_library(${LIB_NAME} INTERFACE ${LIB_HEADERS} ${LIB_SOURCES})
# target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_SRC_DIRECTORY}/src/lib/io)

# header + source static library
add_library(${LIB_NAME} ${LIB_HEADERS} ${LIB_SOURCES})
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SRC_DIRECTORY}/src/lib/io)
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SRC_DIRECTORY}/src/lib/chart-box)
target_link_libraries(${LIB_NAME} PRIVATE chartbox)
target_link_libraries(${LIB_NAME} PRIVATE ${LIBRARY_LINKAGE})



# ============= Chart Base Library =================
SET(LIB_NAME chartsavers)
SET(LIB_HEADERS png.hpp png-saver.inl
                flatbuffer.hpp flatbuffer-saver.inl
                )
SET(LIB_SOURCES 
                )

MESSAGE( STATUS "Generating ChartBox Writers Library: ${LIB_NAME}")
MESSAGE( STATUS "    with headers: ${LIB_HEADERS}")
MESSAGE( STATUS "    with sources: ${LIB_SOURCES}")

# header-only library variant
add_library(${LIB_NAME} INTERFACE)
target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_SRC_DIRECTORY}/src/lib/io)
target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_SRC_DIRECTORY}/src/lib/chart-box)

# header + source static library
# add_library(${LIB_NAME} ${LIB_HEADERS} ${LIB_SOURCES})
# target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SRC_DIRECTORY}/src/lib/io)
# target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SRC_DIRECTORY}/src/lib/chart-box)
# target_link_libraries(${LIB_NAME} PRIVATE chartbox)
# TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE ${LIBRARY_LINKAGE})
