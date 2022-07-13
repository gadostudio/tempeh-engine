set(TEMPEH_COMMON_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/common")

set(SENTRY_DSN "https://8e07fe63b9c94200a0926d0d28b54ad2@o1113293.ingest.sentry.io/6143604")

add_subdirectory("common")
add_subdirectory("math")
add_subdirectory("log")
add_subdirectory("core")
add_subdirectory("telemetry")
