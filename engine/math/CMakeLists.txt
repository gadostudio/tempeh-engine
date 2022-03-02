add_library("tempeh_math" "include/tempeh/math.hpp" "src/math.cpp")

target_include_directories(tempeh_math
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(tempeh_math PUBLIC glm)
