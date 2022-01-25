if(NOT(WIN32))
	set(ENV{MONO_INCLUDE_DIR} "/usr/include/mono-2.0/")
endif()

if(NOT(DEFINED ENV{MONO_INCLUDE_DIR}))
	message(FATAL_ERROR "Mono environment variable is not defined")
endif()

add_library("tempeh_core" STATIC
	"include/tempeh/scripting/mono.hpp"
	"src/scripting/mono.cpp"
	"include/tempeh/renderer/camera.hpp")

target_include_directories("tempeh_core"
	PUBLIC "$ENV{MONO_INCLUDE_DIR}"
	PUBLIC "${CMAKE_SOURCE_DIR}/engine/core/include")

#target_link_libraries("tempeh_core"
#	PUBLIC "include")
