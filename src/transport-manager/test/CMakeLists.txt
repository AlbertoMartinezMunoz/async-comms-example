add_library(transport-manager ${PROJECT_SOURCE_DIR}/src/transport-manager.cpp)  
target_include_directories(transport-manager PUBLIC ${PROJECT_SOURCE_DIR}/include)
# target_link_libraries(transport-manager)

add_executable(test-transport-manager test-transport-manager.cpp)
target_link_libraries(test-transport-manager transport-manager gtest fff)
add_test(NAME test-transport-manager COMMAND test-transport-manager)
set_target_properties(test-transport-manager PROPERTIES FOLDER test)