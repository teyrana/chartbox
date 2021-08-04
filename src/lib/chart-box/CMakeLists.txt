
# ============= Chart Base Library =================
# # this is a header-only library ???
SET(LIB_NAME chartbox)
SET(LIB_HEADERS chart-box.hpp
                chart-layer-interface.hpp chart-layer-interface.inl
                frame/bound-box.hpp
                frame/frame-mapping.hpp frame/frame-mapping.cpp
                frame/location2ll.hpp
                frame/location2en.hpp
                )
SET(LIB_SOURCES chart-box.cpp
                )

MESSAGE( STATUS "Generating ChartBox Library: ${LIB_NAME}")
MESSAGE( STATUS "    with headers: ${LIB_HEADERS}")
MESSAGE( STATUS "    with sources: ${LIB_SOURCES}")

# Generate the static library from the sources
add_library(${LIB_NAME} STATIC ${LIB_HEADERS} ${LIB_SOURCES})
target_link_libraries(${LIB_NAME} PRIVATE ${LIBRARY_LINKAGE})
target_link_libraries(${LIB_NAME} PRIVATE CONAN_PKG::gdal )
target_link_libraries(${LIB_NAME} PRIVATE CONAN_PKG::fmt)


# ============= Chart Base Tests =================

# These tests can use the Catch2-provided main
add_executable(chartbox-tests 
                frame/bound-box.hpp
                frame/frame-mapping.cpp frame/frame-mapping.hpp
                frame/location2en.hpp
                frame/location2ll.hpp
                frame/location2xy.hpp
                frame/frame.test.cpp
                )
target_link_libraries(chartbox-tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(chartbox-tests PRIVATE CONAN_PKG::fmt)
target_link_libraries(chartbox-tests PRIVATE CONAN_PKG::gdal )
#target_link_libraries(chartbox-tests PRIVATE ${LIBRARY_LINKAGE})
