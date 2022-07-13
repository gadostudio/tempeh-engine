set(TEMPEH_ENGINE_EDITOR_HEADERS
        "src/core/application.hpp"

        "src/renderer/render_context.hpp")

set(TEMPEH_ENGINE_EDITOR_SOURCES
        "src/main.cpp"
        "src/core/application.cpp"

        "src/renderer/render_context.cpp"
        "src/renderer/render_context.hpp"
        src/renderer/gui/imgui_renderer.cpp
        src/renderer/gui/renderer.hpp
        src/renderer/renderer.hpp
        src/renderer/renderer.cpp
        src/renderer/gui/imgui_renderer.hpp src/gui/scene/scene.hpp)

add_executable("tempeh_editor"
        ${TEMPEH_ENGINE_EDITOR_SOURCES}
        ${TEMPEH_ENGINE_EDITOR_HEADERS}
#        "src/gui/panel.hpp"
#        "src/gui/panels/scene_panel.hpp"
#        "src/gui/panels/scene_panel.cpp"
#        "src/gui/window_menubar_panel.cpp"
#        "src/gui/window_menubar_panel.hpp"
        )

target_include_directories("tempeh_editor"
        #PUBLIC "../common"
        PUBLIC "${Dawn_SOURCE_DIR}/src"
        PUBLIC "$ENV{MONO_INCLUDE_DIR}"
        PUBLIC ${TEMPEH_COMMON_INCLUDE})

target_link_libraries("tempeh_editor"
        # Private, because anyone ain't gonna touch the editor
        PRIVATE "glfw"
        PRIVATE "tempeh_common"
        PRIVATE "tempeh_core"
        PRIVATE "tempeh_log"
        PRIVATE "tempeh_math"
        PRIVATE "spdlog::spdlog"
        PRIVATE "tempeh_telemetry"
        PRIVATE "webgpu_dawn"
        PRIVATE "dawn_native"
        PRIVATE "dawn_utils"
        PRIVATE "dawn_proc"
        PRIVATE "dawncpp"
        PRIVATE "dawncpp_headers"
        PRIVATE "imgui")

#add_dependencies("tempeh_editor" webgpu_headers_gen)