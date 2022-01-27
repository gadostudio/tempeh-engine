if(TEMPEH_ENABLE_TEST)
	enable_testing()

	add_executable(
		"tempeh_core_test"
		"core/main.cpp"
	)

	target_link_libraries(
		"tempeh_core_test"
		PUBLIC "gtest_main"
	)

	add_executable(
		"tempeh_editor_test"
		"editor/filesystem.cpp"
	)

	target_include_directories("tempeh_editor_test"
		PUBLIC "${CMAKE_SOURCE_DIR}/editor/src"
		PUBLIC "${CMAKE_SOURCE_DIR}/external/dawn/src/utils"
		PUBLIC "${CMAKE_SOURCE_DIR}/external/predef/include"
		PUBLIC "$ENV{MONO_INCLUDE_DIR}"
		PUBLIC ${TEMPEH_COMMON_INCLUDE}
	)

	target_link_libraries(
		"tempeh_editor_test"
		PRIVATE "tempeh_engine_editor_lib"
		PUBLIC "gtest_main"
	)

	include(GoogleTest)
	gtest_discover_tests(tempeh_core_test)
	gtest_discover_tests(tempeh_editor_test)

endif()
