add_executable(test-slip-application-layer test-slip-application-layer.cpp)
target_link_libraries(test-slip-application-layer communications-layer-lib gtest gmock)
add_test(NAME test-slip-application-layer COMMAND test-slip-application-layer)
set_target_properties(test-slip-application-layer PROPERTIES FOLDER test)

# add_executable(test-socket-transport-layer test-socket-transport-layer.cpp)
# target_link_libraries(test-socket-transport-layer communications-layer-lib gtest gmock fff)
# add_test(NAME test-socket-transport-layer COMMAND test-socket-transport-layer)
# set_target_properties(test-socket-transport-layer PROPERTIES FOLDER test)
