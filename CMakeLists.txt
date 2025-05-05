project(async-comms-example CXX)
cmake_minimum_required(VERSION 3.1...3.14)

# configuration options
option(UTEST "Compile unitary test" OFF)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

set(CMAKE_SKIP_RULE_DEPENDENCY TRUE)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_CXX_STANDARD 20)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    message("CCache found!")
endif(CCACHE_FOUND)

# place binaries and libraries according to GNU standards
include(GNUInstallDirs)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

# If you set any CMAKE_ variables, that can go here.
# We use this to get code coverage
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -fstack-protector-all -Wall -Wno-psabi -Wextra -Wpedantic -Werror -s")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

if(UTEST)
	include(CTest)
	enable_testing()
	add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/gtest")
	add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/fff")
endif()

add_subdirectory("${PROJECT_SOURCE_DIR}/src")
