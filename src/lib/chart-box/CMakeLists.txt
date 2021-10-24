# ============= Chart Base Library =================
SET(LIB_NAME chartbox-base)
SET(LIB_HEADERS chart-box.hpp
                )
SET(LIB_SOURCES chart-box.cpp
                )

MESSAGE( STATUS "Generating ChartBox Library: ${LIB_NAME}")
MESSAGE( STATUS "    with headers: ${LIB_HEADERS}")
MESSAGE( STATUS "    with sources: ${LIB_SOURCES}")

# Generate the static library from the sources
add_library(${LIB_NAME} STATIC ${LIB_HEADERS} ${LIB_SOURCES})
target_link_libraries(${LIB_NAME} PRIVATE ${LIBRARY_LINKAGE})
