# ????
#set(BUILD_SHARED_LIBS OFF)
#set(BUILD_EXAMPLE OFF)

macro(tempeh_deps_var _VAR _TYPE _VALUE)
	set(${_VAR} ${_VALUE} CACHE ${_TYPE} "" FORCE)
endmacro()

# GLFW configurations
tempeh_deps_var(GLFW_BUILD_EXAMPLES BOOL OFF)
tempeh_deps_var(GLFW_BUILD_TESTS BOOL OFF)
tempeh_deps_var(GLFW_BUILD_DOCS BOOL OFF)
tempeh_deps_var(GLFW_INSTALL BOOL OFF)

# Dawn configurations
tempeh_deps_var(DAWN_BUILD_EXAMPLES BOOL OFF)
tempeh_deps_var(DAWN_ENABLE_D3D12 BOOL OFF)

# spdlog configuration
tempeh_deps_var(SPDLOG_NO_EXCEPTIONS BOOL ON)

add_subdirectory("GSL")
add_subdirectory("glfw")
add_subdirectory("dawn")
add_subdirectory("sentry-native")
add_subdirectory("spdlog")
add_subdirectory("entt")
add_subdirectory("magic-enum")
add_subdirectory("glm")
add_subdirectory("Vulkan-Headers")

if(TEMPEH_ENABLE_TEST)
	add_subdirectory("googletest")
endif()

# imgui
add_library(imgui STATIC
	"imgui/imgui.cpp"
	"imgui/imgui_demo.cpp"
	"imgui/imgui_draw.cpp"
	"imgui/imgui_tables.cpp"
	"imgui/imgui_widgets.cpp")
	
target_include_directories(imgui
	PUBLIC "imgui"
	PUBLIC "imgui/misc/single_include")

# Glad
add_library(glad STATIC
	"glad/src/glad.c")

target_include_directories(glad
	PUBLIC "glad/include")

# SPIRV-Reflect
add_library(spirv_reflect STATIC
	"SPIRV-Reflect/spirv_reflect.c"
	"SPIRV-Reflect/spirv_reflect.h")

target_include_directories(spirv_reflect PUBLIC "SPIRV-Reflect")
