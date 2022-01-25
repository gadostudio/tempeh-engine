# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("dawnwebgpu")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

file(GLOB dawn_sources "external/dawn/src/dawn/*.cpp" "external/dawn/src/dawn/*.c")
file(GLOB dawn_native_sources "external/dawn/src/dawn_native/*.cpp" "external/dawn/src/dawn_native/*.c")
file(GLOB dawn_wire_sources "external/dawn/src/dawn_wire/*.cpp" "external/dawn/src/dawn_wire/*.c")

add_library(dawn_wire ${dawn_wire_sources})
target_include_directories(
        dawn_wire
        PUBLIC "external/dawn/src"
        PUBLIC "external/dawn/src/include")
add_library(dawn ${dawn_sources})
target_include_directories(
        dawn
        PUBLIC "external/dawn/src"
        PUBLIC "external/dawn/src/include")
add_library(dawn_native ${dawn_native_sources})
target_include_directories(
        dawn_native
        PUBLIC "external/dawn/src"
        PUBLIC "external/dawn/src/include"
        PUBLIC "external/dawn/third_party/abseil-cpp")

add_library( # Sets the name of the library.
        dawnwebgpu

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        dawnwebgpu

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        dawn_wire
        dawn
        dawn_native
        )

target_include_directories(
        dawnwebgpu

        PUBLIC "external/dawn/src"
        PUBLIC "external/dawn/include"
        PUBLIC "external/dawn/third_party/abseil-cpp"
)
