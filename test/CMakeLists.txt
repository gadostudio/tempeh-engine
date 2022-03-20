enable_testing()

include(GoogleTest)

add_subdirectory(assets) # Assets for unit tests

macro(tempeh_add_test _NAME _TEST_SRC)
    add_executable(${_NAME} ${_TEST_SRC})
    target_link_libraries(
        ${_NAME}
        PUBLIC tempeh_common
        PUBLIC tempeh_core
        PUBLIC gtest_main)
    gtest_discover_tests(${_NAME})
endmacro()

# Define assets for a unit test target
macro(tempeh_test_define_assets _TARGET_NAME)
    get_directory_property(_ASSETS_BIN_DIR DIRECTORY "assets" BINARY_DIR)
    target_compile_definitions(${_TARGET_NAME} PRIVATE "TEMPEH_UNIT_TEST_ASSETS_DIRECTORY=\"${_ASSETS_BIN_DIR}\"")
    add_dependencies(${_TARGET_NAME} tempeh_test_shaders)
endmacro()

tempeh_add_test(tempeh_container_static_vector_test "common/container/test_static_vector.cpp")

if (TEMPEH_ENABLE_GPU_TEST)
    tempeh_add_test(tempeh_core_gpu_vulkan_test "core/gpu_vulkan.cpp")
    tempeh_test_define_assets(tempeh_core_gpu_vulkan_test)
endif()
