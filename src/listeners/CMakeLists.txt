project(listeners-lib CXX)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/include/listeners/listener.hpp
    ${PROJECT_SOURCE_DIR}/include/listeners/local-comms-tcp-listener.hpp)

add_library(listeners-lib ${SOURCES})
set_target_properties(listeners-lib PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(listeners-lib communications-layer-lib)
target_include_directories(listeners-lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
