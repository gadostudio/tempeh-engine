enable_testing()

add_executable(
	tempeh_core_test
	"core/main.cpp"
)
target_link_libraries(
  tempeh_core_test
  PUBLIC "gtest_main"
)

include(GoogleTest)
gtest_discover_tests(tempeh_core_test)
