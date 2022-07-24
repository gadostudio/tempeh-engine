cmake_minimum_required(VERSION 3.10)

project("tempeh_engine"
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VERBOSE_MAKEFILE ON)
find_program(SCCACHE "sccache")
if (SCCACHE)
    set(CMAKE_C_COMPILER_LAUNCHER "${SCCACHE}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${SCCACHE}")
    if (MSVC)
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
            string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
        elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
            string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
            string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
        elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
            string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
            string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
        endif ()
    endif ()
endif ()

option(TEMPEH_ENABLE_TEST "Test" OFF)

include("cmake/CPM.cmake")
add_subdirectory("external")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# https://github.com/izenecloud/cmake/blob/master/SetCompilerWarningAll.cmake
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Use the highest warning level for Visual Studio.
    set(CMAKE_CXX_WARNING_LEVEL 4)
    if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    # Disable C++ exceptions.
    string(REGEX REPLACE "/EH[a-z]+" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    # Disable RTTI.
    string(REGEX REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
else (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Use -Wall for clang and gcc.
    if (NOT CMAKE_CXX_FLAGS MATCHES "-Wall")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    endif (NOT CMAKE_CXX_FLAGS MATCHES "-Wall")
    # Use -Wextra for clang and gcc.
    if (NOT CMAKE_CXX_FLAGS MATCHES "-Wextra")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    endif (NOT CMAKE_CXX_FLAGS MATCHES "-Wextra")
    # Use -Werror for clang and gcc.
    #    if(NOT CMAKE_CXX_FLAGS MATCHES "-Werror")
    #      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
    #    endif(NOT CMAKE_CXX_FLAGS MATCHES "-Werror")
    # Disable C++ exceptions.
    #    string(REGEX REPLACE "-fexceptions" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
    # Disable RTTI.
    string(REGEX REPLACE "-frtti" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif ()

#find_program(SCCACHE_FOUND sccache)
#if (SCCACHE_FOUND)
#    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE sccache)
#    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK sccache)
#endif ()

add_subdirectory("engine")
#add_subdirectory("test/vulkan_backend")
add_subdirectory("sample")
if (ANDROID)
    add_subdirectory("platform/android/bridge")
else ()
    add_subdirectory("editor")
endif ()

if (TEMPEH_ENABLE_TEST)
    add_subdirectory("test")
endif ()
