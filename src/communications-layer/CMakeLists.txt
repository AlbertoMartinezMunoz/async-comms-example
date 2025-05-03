project(communications-layer CXX)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/include/communications-layer/communications-layer.hpp
    ${PROJECT_SOURCE_DIR}/src/slip-transport-layer.cpp
    ${PROJECT_SOURCE_DIR}/src/socket-application-layer.cpp
)

add_library(communications-layer-lib ${SOURCES})
set_target_properties(communications-layer-lib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(communications-layer-lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

if(UTEST)
	add_subdirectory("test")
endif()
