project(commands-lib CXX)

set(SOURCES ${PROJECT_SOURCE_DIR}/src/commands-implementations.cpp)

add_library(commands-lib ${SOURCES})
set_target_properties(commands-lib PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(commands-lib communications-layer-lib)
target_include_directories(commands-lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

if(UTEST)
  add_subdirectory("test")
endif()
