project(message-manager CXX)

add_subdirectory("${PROJECT_SOURCE_DIR}/communications-layer")
add_subdirectory("${PROJECT_SOURCE_DIR}/command-manager")
add_subdirectory("${PROJECT_SOURCE_DIR}/socket-comms")

add_executable(message-manager message-manager.cpp)
target_link_libraries(message-manager communications-layer-lib)

add_executable(message-sender message-sender.cpp)
target_link_libraries(message-sender socket-comms-lib)

add_executable(command-sender command-sender.cpp)
target_link_libraries(command-sender command-manager-lib communications-layer-lib)

add_executable(command-receiver command-receiver.cpp)
target_link_libraries(command-receiver command-manager-lib communications-layer-lib)

add_executable(message-receiver message-receiver.cpp)
target_link_libraries(message-receiver socket-comms-lib)
