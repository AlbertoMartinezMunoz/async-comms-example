add_executable(test-remote-command-manager-recv-command test-command-manager-recv-command.cpp)
target_link_libraries(test-remote-command-manager-recv-command remote-command-manager-lib gtest gmock)
add_test(NAME test-remote-command-manager-recv-command COMMAND test-remote-command-manager-recv-command)
set_target_properties(test-remote-command-manager-recv-command PROPERTIES FOLDER test)

add_executable(test-remote-command-manager test-command-manager.cpp)
target_link_libraries(test-remote-command-manager remote-command-manager-lib gtest gmock)
add_test(NAME test-remote-command-manager COMMAND test-remote-command-manager)
set_target_properties(test-remote-command-manager PROPERTIES FOLDER test)
