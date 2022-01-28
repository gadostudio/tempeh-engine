set(TEMPEH_ENGINE_EDITOR_HEADERS
	"src/window/window.hpp"

	"src/core/application.hpp"

	"src/renderer/render_context.hpp"

	"src/event/event.hpp"
	"src/renderer/gui.hpp"
	"src/renderer/gui_imgui_renderer.hpp"
	"src/renderer/gui_renderer.hpp"
	"src/renderer/editor_camera.hpp"
	"src/window/keycode_glfw.hpp"
	"src/filesystem/filetree.hpp"
	)

set(TEMPEH_ENGINE_EDITOR_SOURCES
	"src/core/application.cpp"

	"src/renderer/render_context.cpp"
	"src/renderer/gui_imgui_renderer.cpp"

	"src/window/window_glfw.cpp"
	"src/window/input_manager.cpp"
	"src/filesystem/filetree.cpp"
	)

add_library(
	"tempeh_engine_editor_lib"
	"${TEMPEH_ENGINE_EDITOR_HEADERS}"
	"${TEMPEH_ENGINE_EDITOR_SOURCES}")

target_link_libraries("tempeh_engine_editor_lib"
	PUBLIC "glfw"
	PUBLIC "boost_predef"
	PUBLIC "tempeh_common"
	PUBLIC "tempeh_core"
	PUBLIC "tempeh_log"
	PUBLIC "tempeh_math"
	PUBLIC "spdlog::spdlog"
	#PUBLIC "sentry::sentry"
	PUBLIC "webgpu_dawn"
	PUBLIC "dawn_native"
	PUBLIC "dawn_utils"
	PUBLIC "dawn_proc"
	PUBLIC "dawncpp"
	PUBLIC "dawncpp_headers"
	PUBLIC "imgui"
	)

add_executable("tempeh_engine_editor"
	"src/main.cpp"
	"${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_wgpu.cpp"
	"${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_glfw.cpp"
	)

target_compile_definitions("tempeh_engine_editor"
	PUBLIC SENTRY_DSN="https://8e07fe63b9c94200a0926d0d28b54ad2@o1113293.ingest.sentry.io/6143604"
	)

target_include_directories("tempeh_engine_editor"
	#PUBLIC "../common"
	PUBLIC "${CMAKE_SOURCE_DIR}/external/dawn/src/utils"
	PUBLIC "$ENV{MONO_INCLUDE_DIR}"
	PUBLIC ${TEMPEH_COMMON_INCLUDE}
	)
	
target_link_libraries("tempeh_engine_editor"
	# Private, because anyone ain't gonna touch the editor
	PRIVATE "tempeh_engine_editor_lib"
	)

add_dependencies(tempeh_engine_editor webgpu_headers_gen)