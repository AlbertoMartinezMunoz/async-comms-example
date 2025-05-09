add_executable(test-command-manager test-command-manager.cpp)
target_link_libraries(test-command-manager command-manager-lib gtest gmock)
add_test(NAME test-command-manager COMMAND test-command-manager)
set_target_properties(test-command-manager PROPERTIES FOLDER test)

# add_executable(test-socket-transport-layer test-socket-transport-layer.cpp)
# target_link_libraries(test-socket-transport-layer communications-layer-lib gtest gmock fff)
# add_test(NAME test-socket-transport-layer COMMAND test-socket-transport-layer)
# set_target_properties(test-socket-transport-layer PROPERTIES FOLDER test)
