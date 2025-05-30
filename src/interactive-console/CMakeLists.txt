project(interactive-console-lib CXX)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/include/interactive-console/interactive-console.hpp
    ${PROJECT_SOURCE_DIR}/src/interactive-console.cpp
)

add_library(interactive-console-lib ${SOURCES})
set_target_properties(interactive-console-lib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(interactive-console-lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

if(UTEST)
	add_subdirectory("test")
endif()
