add_executable(test-local-command-manager test-local-command-manager.cpp)
target_link_libraries(test-local-command-manager local-command-manager-lib gtest gmock)
add_test(NAME test-local-command-manager COMMAND test-local-command-manager)
set_target_properties(test-local-command-manager PROPERTIES FOLDER test)
