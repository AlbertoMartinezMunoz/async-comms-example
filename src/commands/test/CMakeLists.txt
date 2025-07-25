add_executable(test-commands test-commands.cpp)
target_link_libraries(test-commands commands-lib gtest gmock fff)
add_test(NAME test-commands COMMAND test-commands)
set_target_properties(test-commands PROPERTIES FOLDER test)
