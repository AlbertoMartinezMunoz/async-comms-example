add_executable(test-shutdown-commands test-shutdown-commands.cpp)
target_link_libraries(test-shutdown-commands commands-lib gtest gmock fff)
add_test(NAME test-shutdown-commands COMMAND test-shutdown-commands)
set_target_properties(test-shutdown-commands PROPERTIES FOLDER test)

add_executable(test-local-commands test-local-commands.cpp)
target_link_libraries(test-local-commands commands-lib gtest gmock fff)
add_test(NAME test-local-commands COMMAND test-local-commands)
set_target_properties(test-local-commands PROPERTIES FOLDER test)
