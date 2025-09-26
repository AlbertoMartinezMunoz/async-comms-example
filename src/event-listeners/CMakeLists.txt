project(event-listeners-lib CXX)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/include/event-listeners/event-listener.hpp)

add_library(event-listeners-lib ${SOURCES})
set_target_properties(event-listeners-lib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(event-listeners-lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
