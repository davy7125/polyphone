# Synth core of Polyphone: plays soundfonts without Qt, for a plugin or a web player.
# The editor compiles the same sources through polyphone.pro. Alone:
#   cmake -S sources/synth_core -B build/synth_core && cmake --build build/synth_core
# (with a multi-configuration generator such as Visual Studio or Xcode: cmake --build ... --config Release)

cmake_minimum_required(VERSION 3.16)
project(polyphone_synth_core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The synth is far too slow without optimizations: Release if no build type is given, or optimizations for this
# library only if the project including it has no build type
set(SYNTH_CORE_OPTIMIZATION "")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND NOT MSVC)
        set(SYNTH_CORE_OPTIMIZATION -O3)
    endif()
endif()

add_library(polyphone_synth_core STATIC
    attributebase.cpp
    dlsarticulation.cpp
    dlsreader.cpp
    enveloppevol.cpp
    fastmaths.cpp
    modulatedparameter.cpp
    modulatordata.cpp
    modulatorgroup.cpp
    osctriangle.cpp
    parametermodulator.cpp
    samplefilereader.cpp
    sf2reader.cpp
    sfzparameter.cpp
    sfzparametergroup.cpp
    sfzparameterregion.cpp
    sfzreader.cpp
    soundfontbuilder.cpp
    synthbuses.cpp
    synthcore.cpp
    syntheffects.cpp
    synthmath.cpp
    midiprocessor.cpp
    synthchannels.cpp
    synthmidi.cpp
    synthsoundfontfile.cpp
    synthsoundfontset.cpp
    textconversions.cpp
    voice.cpp
    voicecontext.cpp
    voicelist.cpp
    voiceparam.cpp
)

target_include_directories(polyphone_synth_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Possibly linked in a shared library (plugin). -std=c++17 as polyphone.pro, not gnu++17: with GNU extensions, GCC
# may fuse multiplications and additions (FMA on ARM) and the plugin would not sound exactly as the editor
set_target_properties(polyphone_synth_core PROPERTIES POSITION_INDEPENDENT_CODE ON CXX_EXTENSIONS OFF)

# libvorbisfile decompresses the samples of the sf3 files (SynthSoundfontFile): the target Vorbis::vorbisfile if the
# project including this library provides it, otherwise the package found by CMake or by pkg-config
if(NOT TARGET Vorbis::vorbisfile)
    find_package(Vorbis CONFIG QUIET)
endif()
if(TARGET Vorbis::vorbisfile)
    target_link_libraries(polyphone_synth_core PUBLIC Vorbis::vorbisfile)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(VORBISFILE REQUIRED IMPORTED_TARGET vorbisfile)
    target_link_libraries(polyphone_synth_core PUBLIC PkgConfig::VORBISFILE)
endif()

# libsndfile reads the samples of the sfz files and of the DLS files (SampleFileReader): the target SndFile::sndfile if
# the project including this library provides it, otherwise the package found by CMake or by pkg-config
if(NOT TARGET SndFile::sndfile)
    find_package(SndFile CONFIG QUIET)
endif()
if(TARGET SndFile::sndfile)
    target_link_libraries(polyphone_synth_core PUBLIC SndFile::sndfile)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)
    target_link_libraries(polyphone_synth_core PUBLIC PkgConfig::SNDFILE)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND NOT MSVC)
    target_compile_options(polyphone_synth_core PRIVATE -Wall -Wextra ${SYNTH_CORE_OPTIMIZATION})
endif()

# MSVC and clang-cl (Windows, plugin): M_PI_2 and the other constants of <cmath>
if(MSVC)
    target_compile_definitions(polyphone_synth_core PRIVATE _USE_MATH_DEFINES)
endif()
