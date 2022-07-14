set(OPTIONS
        "DAWN_ENABLE_D3D12 OFF"
        "DAWN_BUILD_EXAMPLES OFF"
        "TINT_BUILD_TESTS OFF"
        "TINT_BUILD_DOCS OFF")
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
    file(COPY_FILE "${Dawn_SOURCE_DIR}/scripts/standalone.gclient" "${Dawn_SOURCE_DIR}/.gclient")
    message(STATUS "Running gclient sync")
    set(ENV{DEPOT_TOOLS_UPDATE} "0")
    set(ENV{DEPOT_TOOLS_WIN_TOOLCHAIN} "0")
    if (WIN32)
        execute_process(
                COMMAND cmd /C gclient sync
                WORKING_DIRECTORY "${Dawn_SOURCE_DIR}"
                RESULTS_VARIABLE result)
    else ()
        execute_process(
                COMMAND bash -c "gclient sync"
                WORKING_DIRECTORY "${Dawn_SOURCE_DIR}"
                RESULTS_VARIABLE result)
    endif ()
    cpm_add_subdirectory(
            "${NAME}"
            FALSE
            "${Dawn_SOURCE_DIR}"
            "${Dawn_BINARY_DIR}"
            ""
            "${OPTIONS}"
    )
else ()
    message(FATAL_ERROR "Dependency not added")
endif ()
