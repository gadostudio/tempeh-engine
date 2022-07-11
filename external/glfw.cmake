CPMAddPackage(
        GITHUB_REPOSITORY "glfw/glfw"
        GIT_TAG "3.3.7"
        OPTIONS
        "ENKITS_BUILD_EXAMPLES OFF"
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_BUILD_TESTS OFF"
        "GLFW_BUILD_DOCS OFF"
        "GLFW_BUILD_INSTALL OFF"
)

if (glfw_ADDED)
else ()
    message(FATAL_ERROR "Dependency not added")
endif ()