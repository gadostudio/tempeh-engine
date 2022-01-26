cmake_minimum_required(VERSION 3.16)

project("tempeh_engine"
	LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

option(TEMPEH_ENABLE_TEST "Test" OFF)

add_subdirectory("external")
add_subdirectory("engine")
add_subdirectory("editor")
add_subdirectory("test")
