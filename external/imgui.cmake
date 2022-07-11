CPMAddPackage(
        GITHUB_REPOSITORY "ocornut/imgui"
        GIT_TAG "docking"
)

if (imgui_ADDED)
    add_library(imgui STATIC
            "${imgui_SOURCE_DIR}/imgui.cpp"
            "${imgui_SOURCE_DIR}/imgui_demo.cpp"
            "${imgui_SOURCE_DIR}/imgui_draw.cpp"
            "${imgui_SOURCE_DIR}/imgui_tables.cpp"
            "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
            "${imgui_SOURCE_DIR}/backends/imgui_impl_wgpu.cpp"
            "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp")
    target_include_directories(imgui
            PUBLIC "${imgui_SOURCE_DIR}"
            PUBLIC "${imgui_SOURCE_DIR}/misc/single_include")
    target_link_libraries(imgui
            dawn_headers
            glfw)
else ()
    message(FATAL_ERROR "Dependency not added")
endif ()