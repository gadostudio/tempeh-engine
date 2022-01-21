add_library("tempeh_math" "math.hpp" "math.cpp")

target_include_directories("tempeh_math"
	PUBLIC "${CMAKE_SOURCE_DIR}/external/glm")
