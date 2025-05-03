add_executable(test-slip-transport-layer test-slip-transport-layer.cpp)
target_link_libraries(test-slip-transport-layer communications-layer-lib gtest gmock)
add_test(NAME test-slip-transport-layer COMMAND test-slip-transport-layer)
set_target_properties(test-slip-transport-layer PROPERTIES FOLDER test)