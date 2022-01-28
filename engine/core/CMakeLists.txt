if(NOT(WIN32))
	set(ENV{MONO_INCLUDE_DIR} "/usr/include/mono-2.0/")
endif()

if(NOT(DEFINED ENV{MONO_INCLUDE_DIR}))
	message(FATAL_ERROR "Mono environment variable is not defined")
endif()

add_library("tempeh_core" STATIC
	"include/tempeh/scripting/mono.hpp"
	"src/scripting/mono.cpp"
	"include/tempeh/renderer/camera.hpp"
	"include/tempeh/renderer/backend/framebuffer.hpp"
	"include/tempeh/renderer/backend/texture.hpp"
	"include/tempeh/renderer/backend/uniform_buffer.hpp"
	"include/tempeh/renderer/camera.hpp"
	"include/tempeh/renderer/backend/vulkan/texture.hpp"
	"include/tempeh/renderer/backend/vulkan/instance.hpp"
	"include/tempeh/renderer/backend/opengl_es/texture.hpp"
	"include/tempeh/renderer/backend/opengl_es/instance.hpp"
	"include/tempeh/application.hpp"
	"include/tempeh/renderer/orthogonal_camera.hpp"
	"include/tempeh/renderer/perspective_camera.hpp"
	"src/renderer/orthogonal_camera.cpp"
	"src/renderer/perspective_camera.cpp"
	)

target_include_directories("tempeh_core"
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PUBLIC "$ENV{MONO_INCLUDE_DIR}"
	PUBLIC "${CMAKE_SOURCE_DIR}/external/glad/include"
	PUBLIC "${CMAKE_SOURCE_DIR}/engine/core/include"
	PUBLIC "${CMAKE_SOURCE_DIR}/engine/common/include"
	PUBLIC "${Vulkan_INCLUDE_DIRS}"
	)

target_link_libraries("tempeh_core"
	PUBLIC "${Vulkan_LIBRARIES}"
	PUBLIC "tempeh_math"
	PUBLIC "glfw"
	)
