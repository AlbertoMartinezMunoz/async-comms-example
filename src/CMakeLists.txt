project(message-manager CXX)

add_subdirectory("${PROJECT_SOURCE_DIR}/communications-layer")
add_subdirectory("${PROJECT_SOURCE_DIR}/remote-command-manager")
add_subdirectory("${PROJECT_SOURCE_DIR}/interactive-console")
add_subdirectory("${PROJECT_SOURCE_DIR}/interactive-commands")
add_subdirectory("${PROJECT_SOURCE_DIR}/arguments-parser")

add_executable(command-manager command-manager.cpp)
target_link_libraries(command-manager remote-command-manager-lib communications-layer-lib interactive-console-lib interactive-commands-lib arguments-parser-lib)
