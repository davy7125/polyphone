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
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(SYNTH_CORE_OPTIMIZATION -O3)
    endif()
endif()

# Reverb and chorus from STK (copy in the sources of Polyphone)
set(STK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../lib/_option_stk)

add_library(polyphone_synth_core STATIC
    attributebase.cpp
    enveloppevol.cpp
    fastmaths.cpp
    modulatedparameter.cpp
    modulatordata.cpp
    modulatorgroup.cpp
    osctriangle.cpp
    parametermodulator.cpp
    synthbuses.cpp
    synthcore.cpp
    syntheffects.cpp
    synthmath.cpp
    midiprocessor.cpp
    synthmidi.cpp
    voice.cpp
    voicecontext.cpp
    voicelist.cpp
    voiceparam.cpp
    ${STK_DIR}/stk/Stk.cpp
    ${STK_DIR}/stk/SineWave.cpp
    ${STK_DIR}/stk/OnePole.cpp
    ${STK_DIR}/stk/Delay.cpp
    ${STK_DIR}/stk/DelayL.cpp
    ${STK_DIR}/stk/Chorus.cpp
    ${STK_DIR}/stk/FreeVerb.cpp
)

target_include_directories(polyphone_synth_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${STK_DIR}
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(polyphone_synth_core PRIVATE -Wall -Wextra ${SYNTH_CORE_OPTIMIZATION})
endif()
