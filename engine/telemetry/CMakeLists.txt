add_library("tempeh_telemetry" STATIC
        "src/telemetry.cpp"
        "include/tempeh/telemetry.hpp")

if (NOT (SENTRY_DSN))
    message(FATAL_ERROR "Sentry DSN is not defined!")
endif ()

target_compile_definitions("tempeh_telemetry"
        PUBLIC SENTRY_DSN="${SENTRY_DSN}")

target_include_directories("tempeh_telemetry"
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries("tempeh_telemetry"
        PUBLIC "sentry::sentry")
