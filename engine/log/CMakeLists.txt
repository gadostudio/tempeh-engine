add_library("tempeh_log" STATIC
	"src/logger.cpp"
	"include/tempeh/logger.hpp")

target_include_directories("tempeh_log"
	PUBLIC "include"
	PUBLIC "${CMAKE_SOURCE_DIR}/external/spdlog/include")

target_link_libraries("tempeh_log"
	PUBLIC "spdlog::spdlog")
