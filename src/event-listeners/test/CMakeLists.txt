add_executable(test-socket-event-listener test-socket-event-listener.cpp)
target_link_libraries(test-socket-event-listener event-listeners-lib gtest gmock fff)
add_test(NAME test-socket-event-listener COMMAND test-socket-event-listener)
set_target_properties(test-socket-event-listener PROPERTIES FOLDER test)
