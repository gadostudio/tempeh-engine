add_library("tempeh_math" "include/tempeh/math.hpp" "src/math.cpp")

target_include_directories("tempeh_math"
	PUBLIC "include"
	PUBLIC "${CMAKE_SOURCE_DIR}/external/glm")
