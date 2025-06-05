project(arguments-parser-lib CXX)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/include/arguments-parser/arguments-parser.hpp
    ${PROJECT_SOURCE_DIR}/src/arguments-parser.cpp
)

add_library(arguments-parser-lib ${SOURCES})
set_target_properties(arguments-parser-lib PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(arguments-parser-lib)
target_include_directories(arguments-parser-lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
