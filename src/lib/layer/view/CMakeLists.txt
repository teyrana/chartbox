# ============= Fixed-Grid Chart Layer Library =================
SET(LIB_NAME viewlayer )
SET(LIB_HEADERS 
                view.hpp view.inl
                )
SET(LIB_SOURCES 
                )

MESSAGE( STATUS "Generating Composite View Library: ${LIB_NAME}")
MESSAGE( STATUS "    with headers: ${LIB_HEADERS}")
MESSAGE( STATUS "    with sources: ${LIB_SOURCES}")

# Generate the static library from the sources
add_library(${LIB_NAME} INTERFACE)
# add_library(${LIB_NAME} STATIC ${LIB_HEADERS} ${LIB_SOURCES})

# internal library dependency
target_link_libraries(${LIB_NAME} INTERFACE )
# target_link_libraries(${LIB_NAME} PRIVATE ${LIBRARY_LINKAGE} )
# target_link_libraries(${LIB_NAME} PUBLIC chartbox )
# target_link_libraries(${LIB_NAME} PUBLIC fixedgridlayer )
# target_link_libraries(${LIB_NAME} PUBLIC CONAN_PKG::gdal )

INCLUDE_DIRECTORIES(${CMAKE_SRC_DIRECTORY}/src/lib/layer/view)
