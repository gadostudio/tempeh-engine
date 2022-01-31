set(TEMPEH_ENGINE_EDITOR_HEADERS
	"src/core/application.hpp"

	"src/renderer/render_context.hpp"
	
	"src/renderer/gui.hpp"
	"src/renderer/gui_imgui_renderer.hpp"
	"src/renderer/gui_renderer.hpp"
	"src/renderer/editor_camera.hpp"
	"src/renderer/gui_imgui_renderer.cpp")

set(TEMPEH_ENGINE_EDITOR_SOURCES
	"src/main.cpp"
	"src/core/application.cpp"

	"src/renderer/render_context.cpp"
	
	"${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_wgpu.cpp"
	"${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_glfw.cpp" src/core/projects.cpp src/core/projects.hpp)

add_executable("tempeh_engine_editor"
	${TEMPEH_ENGINE_EDITOR_SOURCES}
	${TEMPEH_ENGINE_EDITOR_HEADERS}
	"src/gui/panel.hpp"
	"src/gui/panels/scene_panel.hpp"
	"src/gui/panels/scene_panel.cpp"
	"src/gui/window_menubar_panel.cpp"
	"src/gui/window_menubar_panel.hpp")

target_compile_definitions("tempeh_engine_editor"
	PUBLIC SENTRY_DSN="https://8e07fe63b9c94200a0926d0d28b54ad2@o1113293.ingest.sentry.io/6143604")

target_include_directories("tempeh_engine_editor"
	#PUBLIC "../common"
	PUBLIC "${CMAKE_SOURCE_DIR}/external/dawn/src"
	PUBLIC "$ENV{MONO_INCLUDE_DIR}"
	PUBLIC ${TEMPEH_COMMON_INCLUDE})

target_link_libraries("tempeh_engine_editor"
	# Private, because anyone ain't gonna touch the editor
	PRIVATE "glfw"
	PRIVATE "tempeh_common"
	PRIVATE "tempeh_core"
	PRIVATE "tempeh_log"
	PRIVATE "tempeh_math"
	PRIVATE "spdlog::spdlog"
	#PUBLIC "sentry::sentry"
	PRIVATE "webgpu_dawn"
	PRIVATE "dawn_native"
	PRIVATE "dawn_utils"
	PRIVATE "dawn_proc"
	PRIVATE "dawncpp"
	PRIVATE "dawncpp_headers"
	PRIVATE "imgui")

add_dependencies(tempeh_engine_editor webgpu_headers_gen)