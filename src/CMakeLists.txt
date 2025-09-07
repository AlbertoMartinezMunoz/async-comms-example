project(message-manager CXX)

add_subdirectory("${PROJECT_SOURCE_DIR}/communications-layer")
add_subdirectory("${PROJECT_SOURCE_DIR}/command-manager")
add_subdirectory("${PROJECT_SOURCE_DIR}/interactive-console")
add_subdirectory("${PROJECT_SOURCE_DIR}/commands")
add_subdirectory("${PROJECT_SOURCE_DIR}/arguments-parser")
add_subdirectory("${PROJECT_SOURCE_DIR}/networking")
add_subdirectory("${PROJECT_SOURCE_DIR}/thread-pool")

add_executable(command-manager command-manager.cpp)
target_link_libraries(
  command-manager command-manager-lib communications-layer-lib
  interactive-console-lib arguments-parser-lib)
