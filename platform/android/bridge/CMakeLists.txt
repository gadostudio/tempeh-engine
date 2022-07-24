set(TEMPEH_ANDROID_BRIDGE_SOURCES
        "src/bridge.cpp")
set(TEMPEH_ANDROID_BRIDGE_HEADERS
        "include/tempeh/android-bridge/bridge.hpp")

find_library(android-lib android)

add_library(tempeh_android_bridge SHARED
        ${TEMPEH_ANDROID_BRIDGE_SOURCES}
        ${TEMPEH_ANDROID_BRIDGE_HEADERS})

target_include_directories(tempeh_android_bridge PUBLIC
        "include")

target_link_libraries(tempeh_android_bridge
        tempeh_core
        ${android-lib})
