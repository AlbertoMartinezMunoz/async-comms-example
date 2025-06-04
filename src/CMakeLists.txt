project(message-manager CXX)

add_subdirectory("${PROJECT_SOURCE_DIR}/communications-layer")
add_subdirectory("${PROJECT_SOURCE_DIR}/command-manager")
add_subdirectory("${PROJECT_SOURCE_DIR}/interactive-console")

add_executable(command-sender command-sender.cpp)
target_link_libraries(command-sender command-manager-lib communications-layer-lib interactive-console-lib)

add_executable(command-receiver command-receiver.cpp)
target_link_libraries(command-receiver command-manager-lib communications-layer-lib)

add_executable(command-manager command-manager.cpp)
target_link_libraries(command-manager command-manager-lib communications-layer-lib interactive-console-lib)
