if(TEMPEH_ENABLE_TEST)
	enable_testing()

	add_executable(
		"tempeh_core_test"
		"core/main.cpp"
			core/common/collections/capped_vector.cpp)
	target_link_libraries(
	  "tempeh_core_test"
	  PUBLIC "gtest_main"
	  PUBLIC "tempeh_common"
	)

	include(GoogleTest)
	gtest_discover_tests(tempeh_core_test)
endif()
