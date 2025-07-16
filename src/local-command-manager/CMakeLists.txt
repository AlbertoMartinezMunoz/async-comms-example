project(local-command-manager CXX)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/include/local-command-manager/local-command-manager.hpp
    ${PROJECT_SOURCE_DIR}/src/local-command-manager.cpp
)

add_library(local-command-manager-lib ${SOURCES})
set_target_properties(local-command-manager-lib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(local-command-manager-lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

if(UTEST)
	add_subdirectory("test")
endif()
