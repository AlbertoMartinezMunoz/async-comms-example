project(socket-comms CXX)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/src/socket-comms.cpp
)

add_library(socket-comms-lib ${SOURCES})
set_target_properties(socket-comms-lib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(socket-comms-lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

if(UTEST)
	add_subdirectory("test")
endif()
