if(TEMPEH_ENABLE_TEST)
	enable_testing()

	add_executable(
		"tempeh_core_test"
		"core/main.cpp"
	)

	add_executable(
		"tempeh_editor_test"
		"editor/filesystem.cpp"
		"${TEMPEH_ENGINE_EDITOR_SOURCES}"
		"${TEMPEH_ENGINE_EDITOR_HEADERS}"
	)

	target_link_libraries(
	  "tempeh_core_test"
	  PUBLIC "gtest_main"
	)

	target_link_libraries(
	  "tempeh_editor_test"
	  PUBLIC "gtest_main"
		# Private, because anyone ain't gonna touch the editor
		PRIVATE "glfw"
		PRIVATE "boost_predef"
		PRIVATE "tempeh_common"
		PRIVATE "tempeh_core"
		PRIVATE "tempeh_log"
		PRIVATE "tempeh_math"
		PRIVATE "spdlog::spdlog"
		#PUBLIC "sentry::sentry"
		PRIVATE "webgpu_dawn"
		PRIVATE "dawn_native"
		PRIVATE "dawn_utils"
		PRIVATE "dawn_proc"
		PRIVATE "dawncpp"
		PRIVATE "dawncpp_headers"
		PRIVATE "imgui"
	)

	include(GoogleTest)
	gtest_discover_tests(tempeh_core_test)
	gtest_discover_tests(tempeh_editor_test)

endif()
