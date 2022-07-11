CPMAddPackage(gh:skypjack/entt@3.10.1)

if (entt_ADDED)
    add_library(entt INTERFACE)
    target_include_directories(entt INTERFACE ${entt_SOURCE_DIR}/src)
else ()
    message(FATAL_ERROR "Dependency not added")
endif ()
