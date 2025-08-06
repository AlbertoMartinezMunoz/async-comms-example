project(command-manager-lib CXX)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/src/command-remote-handler.cpp
    ${PROJECT_SOURCE_DIR}/src/command-local-handler.cpp)

add_library(command-manager-lib ${SOURCES})
set_target_properties(command-manager-lib PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(command-manager-lib communications-layer-lib)
target_include_directories(command-manager-lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

if(UTEST)
    add_subdirectory("test")
endif()
