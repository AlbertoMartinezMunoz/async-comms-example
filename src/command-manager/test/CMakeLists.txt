add_executable(test-command-manager-send-command test-command-manager-send-command.cpp)
target_link_libraries(test-command-manager-send-command command-manager-lib gtest gmock)
add_test(NAME test-command-manager-send-command COMMAND test-command-manager-send-command)
set_target_properties(test-command-manager-send-command PROPERTIES FOLDER test)

