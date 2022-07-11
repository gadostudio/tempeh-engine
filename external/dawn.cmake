set(OPTIONS
        "DAWN_ENABLE_D3D12 OFF"
        "DAWN_BUILD_EXAMPLES OFF"
        "TINT_BUILD_TESTS OFF")
set(NAME "Dawn")

CPMAddPackage(
        NAME "${NAME}"
        GIT_REPOSITORY "https://dawn.googlesource.com/dawn"
        GIT_TAG "chromium/5172"
        DOWNLOAD_ONLY TRUE
        OPTIONS
        "${OPTIONS}"
)

if (Dawn_ADDED)
    if (WIN32)
        set(ENV{DEPOT_TOOLS_WIN_TOOLCHAIN} "0")
    endif ()
    file(COPY_FILE "${Dawn_SOURCE_DIR}/scripts/standalone.gclient" "${Dawn_SOURCE_DIR}/.gclient")
    execute_process(
            COMMAND cmd /C gclient sync # Windows only
            WORKING_DIRECTORY "${Dawn_SOURCE_DIR}"
            RESULTS_VARIABLE result)
    if (result OR result EQUAL 1)
        cpm_add_subdirectory(
                "${NAME}"
                FALSE
                "${Dawn_SOURCE_DIR}"
                "${Dawn_BINARY_DIR}"
                ""
                "${OPTIONS}"
        )
    endif ()
else ()
    message(FATAL_ERROR "Dependency not added")
endif ()
