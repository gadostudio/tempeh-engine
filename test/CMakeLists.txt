enable_testing()

include(GoogleTest)

macro(tempeh_add_test _NAME _TEST_SRC)
    add_executable(tempeh_${_NAME}_test ${_TEST_SRC})
    target_link_libraries(
        tempeh_${_NAME}_test
        PUBLIC tempeh_core
        PUBLIC gtest_main)
    gtest_discover_tests(tempeh_${_NAME}_test)
endmacro()

if (TEMPEH_ENABLE_GPU_TEST)
    tempeh_add_test(core_gpu_vulkan "core/gpu_vulkan.cpp")
endif()

tempeh_add_test(core_main "core/main.cpp")
