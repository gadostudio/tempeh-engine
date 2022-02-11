add_library(tempeh_log STATIC
	"src/logger.cpp"
	"include/tempeh/logger.hpp")

target_include_directories(tempeh_log
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(tempeh_log
	PUBLIC spdlog::spdlog)
