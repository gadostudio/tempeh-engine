CPMAddPackage(
        GITHUB_REPOSITORY "g-truc/glm"
        GIT_TAG "0.9.9.8"
)

if (glm_ADDED)
else ()
    message(FATAL_ERROR "Dependency not added")
endif ()