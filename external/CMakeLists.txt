set(BUILD_SHARED_LIBS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
#set(GLFW_VULKAN_STATIC OFF)
add_subdirectory("glfw")
set(BUILD_EXAMPLE OFF)
set(DAWN_BUILD_EXAMPLES OFF)

add_subdirectory("dawn")
#if(NOT(EXISTS "dawn/src/dawn/webgpu_cpp.cpp"))
#	file(COPY "${Dawn_BINARY_DIR}/gen/src" DESTINATION "dawn/src")
#endif()

#add_subdirectory("sentry-native")
add_subdirectory("spdlog")
add_subdirectory("predef")
add_subdirectory("glm")

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
