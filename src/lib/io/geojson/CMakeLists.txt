# ============= Chart GeoJSON I/O Library =================
SET(LIB_NAME chart-geojson-io)
SET(LIB_HEADERS boundary.inl 
                contour.inl
                geojson.hpp
                )
SET(LIB_SOURCES geojson.cpp
                )

MESSAGE( STATUS "Generating ChartBox GeoJSON Library: ${LIB_NAME}")
MESSAGE( STATUS "    with headers: ${LIB_HEADERS}")
MESSAGE( STATUS "    with sources: ${LIB_SOURCES}")

# header + source static library
add_library(${LIB_NAME} STATIC ${LIB_HEADERS} ${LIB_SOURCES})
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SRC_DIRECTORY}/src/lib/chart-box)
target_link_libraries(${LIB_NAME} PRIVATE ${LIBRARY_LINKAGE})
