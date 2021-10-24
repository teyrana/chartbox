# ============= Build Profiling Program  =================
SET(EXE_NAME sandbox)
SET(EXE_SOURCES main.cpp)

MESSAGE( STATUS "Generating Sandbox program: ${EXE_NAME}")
MESSAGE( STATUS "    with sources: ${EXE_SOURCES}")
MESSAGE( STATUS "    with linkage: ${LIBRARY_LINKAGE}")

ADD_EXECUTABLE( ${EXE_NAME} ${EXE_SOURCES})
TARGET_LINK_LIBRARIES(${EXE_NAME} PRIVATE ${EXE_LINKAGE} ${LIBRARY_LINKAGE}) 
