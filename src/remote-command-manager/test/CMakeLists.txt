add_executable(test-command-manager-recv-command test-command-manager-recv-command.cpp)
target_link_libraries(test-command-manager-recv-command command-manager-lib gtest gmock)
add_test(NAME test-command-manager-recv-command COMMAND test-command-manager-recv-command)
set_target_properties(test-command-manager-recv-command PROPERTIES FOLDER test)

add_executable(test-command-manager test-command-manager.cpp)
target_link_libraries(test-command-manager command-manager-lib gtest gmock)
add_test(NAME test-command-manager COMMAND test-command-manager)
set_target_properties(test-command-manager PROPERTIES FOLDER test)
