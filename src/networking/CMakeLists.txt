project(networking-lib CXX)

 set(SOURCES
     ${PROJECT_SOURCE_DIR}/include/networking/networking.hpp
     ${PROJECT_SOURCE_DIR}/include/networking/local-tcp-networking.hpp
     ${PROJECT_SOURCE_DIR}/src/local-tcp-networking-client.cpp)

add_library(networking-lib ${SOURCES})
set_target_properties(networking-lib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(networking-lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

if(UTEST)
  add_subdirectory("test")
endif()
