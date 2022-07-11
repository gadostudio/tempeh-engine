CPMAddPackage(
        GITHUB_REPOSITORY "google/googletest"
        GIT_TAG "release-1.12.1"
        OPTIONS
        "INSTALL_GTEST OFF"
        "gtest_force_shared_crt ON"
)

if (googletest_ADDED)
else ()
    message(FATAL_ERROR "Dependency not added")
endif ()