project(thread-pool-lib CXX)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/include/thread-pool/thread-pool.hpp
    ${PROJECT_SOURCE_DIR}/src/thread-pool.cpp)

add_library(thread-pool-lib ${SOURCES})
set_target_properties(thread-pool-lib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(thread-pool-lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

if(UTEST)
  add_subdirectory("test")
endif()
