# ============= Build Profiling Program  =================
SET(EXE_NAME mapmerge)
SET(EXE_SOURCES main.cpp)

MESSAGE( STATUS "Generating Map-Merge program: ${EXE_NAME}")
MESSAGE( STATUS "    with sources: ${EXE_SOURCES}")
MESSAGE( STATUS "    with linkage: ${LIBRARY_LINKAGE}")

# DEVEL
ADD_EXECUTABLE( ${EXE_NAME} ${EXE_SOURCES})

target_include_directories( ${EXE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/lib/chart-box )
target_include_directories( ${EXE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/lib/io )
target_include_directories( ${EXE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/lib/layer )

TARGET_LINK_LIBRARIES(${EXE_NAME} PRIVATE ${EXE_LINKAGE} ${LIBRARY_LINKAGE}) 
target_link_libraries(${EXE_NAME} PRIVATE CONAN_PKG::gdal)
target_link_libraries(${EXE_NAME} PRIVATE CONAN_PKG::fmt)

