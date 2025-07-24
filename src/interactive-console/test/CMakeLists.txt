add_executable(test-interactive-console test-interactive-console.cpp)
target_link_libraries(test-interactive-console interactive-console-lib gtest
                      gmock fff)
add_test(NAME test-interactive-console COMMAND test-interactive-console)
set_target_properties(test-interactive-console PROPERTIES FOLDER test)
