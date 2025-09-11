add_executable(test-thread-pool test-thread-pool.cpp)
target_link_libraries(test-thread-pool thread-pool-lib gtest gmock)
add_test(NAME test-thread-pool COMMAND test-thread-pool)
set_target_properties(test-thread-pool PROPERTIES FOLDER test)
