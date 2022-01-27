cmake_minimum_required(VERSION 3.16)

project("tempeh_engine"
	LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory("external")
add_subdirectory("engine")
add_subdirectory("editor")
add_subdirectory("test/vulkan_backend")
