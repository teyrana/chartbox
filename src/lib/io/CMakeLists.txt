
# ============= Chart Loaders Library =================
SET(LIB_NAME chartloaders)
SET(LIB_HEADERS chart-base-loader.hpp chart-debug-loader.hpp
                chart-json-loader.hpp chart-json-loader.inl
                # chart-shapefile-loader.hpp chart-shapefile-loader.inl
                )
SET(LIB_SOURCES 
                )

MESSAGE( STATUS "Generating ChartBox Loaders Library: ${LIB_NAME}")
MESSAGE( STATUS "    with headers: ${LIB_HEADERS}")
MESSAGE( STATUS "    with sources: ${LIB_SOURCES}")

# internal library dependency
target_link_libraries(chartbox)

add_library(${LIB_NAME} INTERFACE)
target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_SRC_DIRECTORY}/src/lib/io)


# # Generate the static library from the sources
# add_library(${LIB_NAME} INTERFACE ${LIB_HEADERS} ${LIB_SOURCES})
# TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE ${LIBRARY_LINKAGE})

# include_directories(${CMAKE_SRC_DIRECTORY}/src/lib/io)



# ============= Chart Base Library =================
SET(LIB_NAME chartwriters)
SET(LIB_HEADERS chart-base-writer.hpp chart-debug-writer.hpp
                chart-png-writer.hpp chart-png-writer.inl
                # src/base/readers.inl
                # src/base/writers.inl
                )
SET(LIB_SOURCES 
                )

MESSAGE( STATUS "Generating ChartBox Writers Library: ${LIB_NAME}")
MESSAGE( STATUS "    with headers: ${LIB_HEADERS}")
MESSAGE( STATUS "    with sources: ${LIB_SOURCES}")

# internal library dependency
target_link_libraries(chartbox)

add_library(${LIB_NAME} INTERFACE)
target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_SRC_DIRECTORY}/src/lib/io)


# # Generate the static library from the sources
# add_library(${LIB_NAME} INTERFACE ${LIB_HEADERS} ${LIB_SOURCES})
# TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE ${LIBRARY_LINKAGE})

# include_directories(${CMAKE_SRC_DIRECTORY}/src/lib/io)
