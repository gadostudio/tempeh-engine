if(NOT(DEFINED ENV{MONO_PROJECT}))
	message(FATAL_ERROR "Mono environment variable is not defined")
endif()

add_library("tempeh_core" STATIC
	"include/tempeh/scripting/Mono.hpp"
	"src/scripting/Mono.cpp" "include/tempeh/renderer/camera.hpp")

target_include_directories("tempeh_core"
	PUBLIC "$ENV{MONO_PROJECT}/include/mono-2.0"
	PUBLIC "${CMAKE_SOURCE_DIR}/engine/core/include")

#target_link_libraries("tempeh_core"
#	PUBLIC "include")
