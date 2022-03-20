add_library(tempeh_common INTERFACE)

target_include_directories(
    tempeh_common
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(
    tempeh_common
    INTERFACE "magic_enum")
