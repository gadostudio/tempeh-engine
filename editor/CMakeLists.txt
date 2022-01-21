set(tempeh_engine_editor_headers
	"src/window/window.hpp"
	"src/core/application.hpp"
	"src/renderer/renderer.hpp"
	"src/event/input_manager.hpp")

set(tempeh_engine_editor_sources
	"src/main.cpp"
	"src/core/application.cpp"
	"src/renderer/renderer.cpp"
	"src/renderer/camera.cpp"
	"src/window/window_glfw.cpp"
	"src/window/input_manager.cpp")

add_executable("tempeh_engine_editor"
	${tempeh_engine_editor_sources}
	${tempeh_engine_editor_headers})

target_compile_definitions("tempeh_engine_editor"
	PUBLIC SENTRY_DSN="https://8e07fe63b9c94200a0926d0d28b54ad2@o1113293.ingest.sentry.io/6143604")

target_include_directories("tempeh_engine_editor"
	PUBLIC "../common"
	PUBLIC "${CMAKE_SOURCE_DIR}/external/glfw/include"
	PUBLIC "${CMAKE_SOURCE_DIR}/external/dawn/src/include"
	PUBLIC "${CMAKE_SOURCE_DIR}/external/imgui/misc/single_include"
	PUBLIC "${CMAKE_SOURCE_DIR}/external/sentry-native/include"
	PUBLIC "${CMAKE_SOURCE_DIR}/engine/log/include"
	PUBLIC ${TEMPEH_COMMON_INCLUDE})

target_link_libraries("tempeh_engine_editor"
	PUBLIC "glfw"
	PUBLIC "tempeh_log"
	PUBLIC "sentry::sentry"
	PUBLIC "webgpu_dawn"
	PUBLIC "dawncpp")
