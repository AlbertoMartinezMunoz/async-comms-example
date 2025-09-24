project(listeners-lib CXX)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/include/listeners/listener.hpp)

add_library(listeners-lib ${SOURCES})
set_target_properties(listeners-lib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(listeners-lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
