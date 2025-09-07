add_executable(test-local-tcp-networking-client test-local-tcp-networking-client.cpp)
target_link_libraries(test-local-tcp-networking-client networking-lib gtest gmock fff)
add_test(NAME test-local-tcp-networking-client COMMAND test-local-tcp-networking-client)
set_target_properties(test-local-tcp-networking-client PROPERTIES FOLDER test)
