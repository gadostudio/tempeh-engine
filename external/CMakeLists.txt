set(BUILD_SHARED_LIBS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
#set(GLFW_VULKAN_STATIC OFF)
add_subdirectory("glfw")
set(BUILD_EXAMPLE OFF)

set(DAWN_BUILD_EXAMPLES OFF)
set(DAWN_ENABLE_D3D12 OFF) # Not stable yet!
add_subdirectory("dawn")

add_subdirectory("sentry-native")
add_subdirectory("spdlog")
add_subdirectory("entt")
add_subdirectory("glm")
add_subdirectory("predef")

add_library("imgui" STATIC
		"imgui/imgui.cpp"
		"imgui/imgui_demo.cpp"
		"imgui/imgui_draw.cpp"
		"imgui/imgui_tables.cpp"
		"imgui/imgui_widgets.cpp")
	
target_include_directories("imgui"
		PUBLIC "imgui"
		PUBLIC "imgui/misc/single_include")

add_library("glad" STATIC
		"glad/src/glad.c")

target_include_directories("glad"
		PUBLIC "glad/include")

find_package(Vulkan REQUIRED)

if(TEMPEH_ENABLE_TEST)
	add_subdirectory("googletest")
endif()
