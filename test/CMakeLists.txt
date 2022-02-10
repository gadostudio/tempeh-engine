enable_testing()

add_executable(
    tempeh_core_test
    "core/gpu_vulkan.cpp"
    "core/main.cpp"
)
target_link_libraries(
  tempeh_core_test
  PUBLIC gtest_main
  PUBLIC tempeh_core
)

include(GoogleTest)
gtest_discover_tests(tempeh_core_test)
