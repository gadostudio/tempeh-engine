cmake_minimum_required(VERSION 3.16)

project("tempeh_engine"
	LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)

option(TEMPEH_ENABLE_TEST "Enable unit test" OFF)
option(TEMPEH_ENABLE_GPU_TEST "Enable unit test for GPU backends (requires TEMPEH_ENABLE_TEST to be enabled)" OFF)

add_subdirectory("external")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# https://github.com/izenecloud/cmake/blob/master/SetCompilerWarningAll.cmake

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Use the highest warning level for Visual Studio.
    set(CMAKE_CXX_WARNING_LEVEL 4)
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    # Disable C++ exceptions.
    string(REGEX REPLACE "/EH[a-z]+" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    # Disable RTTI.
    string(REGEX REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Use -Wall for clang and gcc.
    if(NOT CMAKE_CXX_FLAGS MATCHES "-Wall")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    endif(NOT CMAKE_CXX_FLAGS MATCHES "-Wall")
    # Use -Wextra for clang and gcc.
    if(NOT CMAKE_CXX_FLAGS MATCHES "-Wextra")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    endif(NOT CMAKE_CXX_FLAGS MATCHES "-Wextra")
    # Disable C++ exceptions.
    string(REGEX REPLACE "-fexceptions" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
    # Disable RTTI.
    string(REGEX REPLACE "-frtti" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

add_subdirectory("engine")
add_subdirectory("editor")
add_subdirectory("test/vulkan_backend")

if(TEMPEH_ENABLE_TEST)
    add_subdirectory("test")
endif()
