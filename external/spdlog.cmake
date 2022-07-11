CPMAddPackage(
        GITHUB_REPOSITORY "gabime/spdlog"
        VERSION "1.10.0"
        OPTIONS
        "SPDLOG_NO_EXCEPTIONS ON"
)

if (spdlog_ADDED)

else ()
    message(FATAL_ERROR "Dependency not added")
endif ()