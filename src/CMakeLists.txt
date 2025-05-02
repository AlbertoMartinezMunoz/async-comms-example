project(message-manager CXX)

add_subdirectory("${PROJECT_SOURCE_DIR}/communications-layer")

add_executable(message-manager message-manager.cpp)
target_link_libraries(message-manager communications-layer-lib)