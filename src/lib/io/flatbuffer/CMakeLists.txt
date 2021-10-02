
# ============= Chart Flatbuffer I/O Library =================
SET(LIB_NAME chart-flatbuffer-io)

SET(GENERATED_HEADERS  "tile-cache-generated.hpp" )
                
SET(LIB_HEADERS 
                flatbuffer.hpp
                ${GENERATED_HEADERS}
                )
SET(LIB_SOURCES 
                flatbuffer.cpp
                )

MESSAGE( STATUS "Generating ChartBox Flatbuffers I/O Library: ${LIB_NAME}")
MESSAGE( STATUS "    with headers: ${LIB_HEADERS}")
MESSAGE( STATUS "    with sources: ${LIB_SOURCES}")

# header + source static library
add_library(${LIB_NAME} ${LIB_HEADERS} ${LIB_SOURCES})
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SRC_DIRECTORY}/src/lib/io)
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SRC_DIRECTORY}/src/lib/chart-box)
target_link_libraries(${LIB_NAME} PRIVATE ${LIBRARY_LINKAGE})