add_executable(test-socket-comms test-socket-comms.cpp)
target_link_libraries(test-socket-comms socket-comms-lib gtest gmock fff)
add_test(NAME test-socket-comms COMMAND test-socket-comms)
set_target_properties(test-socket-comms PROPERTIES FOLDER test)
