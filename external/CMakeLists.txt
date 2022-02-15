# ????
#set(BUILD_SHARED_LIBS OFF)
#set(BUILD_EXAMPLE OFF)

# GLFW configuration
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)

# Dawn configuration
set(DAWN_BUILD_EXAMPLES OFF)
set(DAWN_ENABLE_D3D12 OFF) # Not stable yet!

# spdlog configuration
set(SPDLOG_NO_EXCEPTIONS ON)

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
