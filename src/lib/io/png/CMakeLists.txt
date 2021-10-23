# ============= Chart PNG I/O Library =================
SET(LIB_NAME chartbox-io-png )
SET(LIB_HEADERS png.hpp png.inl )
SET(LIB_SOURCES png.cpp )

MESSAGE( STATUS "Generating ChartBox PNG Library: ${LIB_NAME}")
MESSAGE( STATUS "    with headers: ${LIB_HEADERS}")
MESSAGE( STATUS "    with sources: ${LIB_SOURCES}")

# header + source static library
add_library(${LIB_NAME} ${LIB_HEADERS} ${LIB_SOURCES})
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SRC_DIRECTORY}/src/lib/io)
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SRC_DIRECTORY}/src/lib/chart-box)
target_link_libraries(${LIB_NAME} PRIVATE ${LIBRARY_LINKAGE})
