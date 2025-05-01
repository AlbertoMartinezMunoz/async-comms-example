cmake_minimum_required(VERSION 3.1...3.14)
project(transport-manager-lib CXX)

if(UTEST)
	add_subdirectory("test")
endif()