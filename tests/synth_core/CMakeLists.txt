# Tests of the synth core without the editor (see README.md):
#   cmake -S tests/synth_core -B build/synth_core_tests -DSANITIZER=thread && cmake --build build/synth_core_tests

cmake_minimum_required(VERSION 3.16)
project(polyphone_synth_core_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# "thread" (ThreadSanitizer) or "address" (AddressSanitizer), for the synth core too
set(SANITIZER "" CACHE STRING "Sanitizer: thread, address or empty")
if(SANITIZER)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
    endif()
    add_compile_options(-fsanitize=${SANITIZER} -fno-omit-frame-pointer -g)
    add_link_options(-fsanitize=${SANITIZER})
endif()

if(NOT TARGET polyphone_synth_core) # Already there if built from sources/CMakeLists.txt
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../sources/synth_core synth_core)
endif()

find_package(Threads REQUIRED)
add_executable(stress_threads stress_threads.cpp)
target_link_libraries(stress_threads PRIVATE polyphone_synth_core Threads::Threads)
