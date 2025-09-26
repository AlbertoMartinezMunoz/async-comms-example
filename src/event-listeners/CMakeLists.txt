project(event-listeners-lib CXX)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/include/event-listeners/event-listener.hpp
    ${PROJECT_SOURCE_DIR}/include/event-listeners/socket-event-listener.hpp
    ${PROJECT_SOURCE_DIR}/src/socket-event-listener.cpp)

add_library(event-listeners-lib ${SOURCES})
set_target_properties(event-listeners-lib PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(event-listeners-lib commands-lib)
target_include_directories(event-listeners-lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

if(UTEST)
  add_subdirectory("test")
endif()
