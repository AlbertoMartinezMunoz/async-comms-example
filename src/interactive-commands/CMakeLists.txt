project(interactive-commands-lib CXX)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/include/interactive-commands/interactive-commands.hpp
    ${PROJECT_SOURCE_DIR}/src/interactive-commands.cpp
)

add_library(interactive-commands-lib ${SOURCES})
set_target_properties(interactive-commands-lib PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(interactive-commands-lib remote-command-manager-lib communications-layer-lib interactive-console-lib)
target_include_directories(interactive-commands-lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
