add_library(tempeh_common INTERFACE)
target_include_directories(
    tempeh_common
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
