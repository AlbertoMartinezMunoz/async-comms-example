add_executable(test-command-remote-handler test-command-remote-handler.cpp)
target_link_libraries(test-command-remote-handler command-manager-lib gtest gmock fff)
add_test(NAME test-command-remote-handler COMMAND test-command-remote-handler)
set_target_properties(test-command-remote-handler PROPERTIES FOLDER test)

add_executable(test-command-local-handler test-command-local-handler.cpp)
target_link_libraries(test-command-local-handler command-manager-lib gtest gmock fff)
add_test(NAME test-command-local-handler COMMAND test-command-local-handler)
set_target_properties(test-command-local-handler PROPERTIES FOLDER test)
