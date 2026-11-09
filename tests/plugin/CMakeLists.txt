# Host loading the plugin built (sources/plugin) as a DAW does (see README.md):
#   cmake -S tests/plugin -B build/plugin_host -DPOLYPHONE_JUCE_DIR=<path of JUCE> && cmake --build build/plugin_host

cmake_minimum_required(VERSION 3.22)
project(PolyphonePluginHost VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Same JUCE as the plugin
set(POLYPHONE_JUCE_DIR "" CACHE PATH "Local copy of JUCE (downloaded otherwise)")
include(FetchContent)
if(POLYPHONE_JUCE_DIR)
    add_subdirectory(${POLYPHONE_JUCE_DIR} JUCE EXCLUDE_FROM_ALL)
else()
    FetchContent_Declare(JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 9.0.4
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(JUCE)
endif()

# Headers of the CLAP format (same version as the one of clap-juce-extensions)
FetchContent_Declare(clap
    GIT_REPOSITORY https://github.com/free-audio/clap.git
    GIT_TAG 1.2.7
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(clap)

juce_add_console_app(plugin_host PRODUCT_NAME "plugin_host")
target_sources(plugin_host PRIVATE plugin_host.cpp clap_instance.cpp)
target_compile_definitions(plugin_host PRIVATE
    JUCE_PLUGINHOST_VST3=1
    JUCE_PLUGINHOST_LV2=1
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0)
target_link_libraries(plugin_host PRIVATE
    clap
    ${CMAKE_DL_LIBS}
    juce::juce_audio_processors
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags)

# Tools compiling the sources of the plugin: its interface drawn in images, and its threads (possibly with
# -DSANITIZER=thread or address, applied to these tools and to the synth core)
set(SANITIZER "" CACHE STRING "Sanitizer of plugin_threads and of the synth core: thread, address or empty")
if(SANITIZER)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${SANITIZER} -fno-omit-frame-pointer -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=${SANITIZER}")
endif()
if(NOT TARGET polyphone_synth_core) # Already there if built from sources/CMakeLists.txt
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../sources/synth_core synth_core)
endif()
juce_add_binary_data(PluginSnapshotData SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../logo.png
    ${CMAKE_CURRENT_SOURCE_DIR}/../../sources/resources/misc/temperaments.csv)
juce_add_console_app(plugin_snapshot PRODUCT_NAME "plugin_snapshot")
target_sources(plugin_snapshot PRIVATE
    plugin_snapshot.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../sources/plugin/pluginengine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../sources/plugin/pluginsoundfont.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../sources/plugin/pluginprocessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../sources/plugin/plugineditor.cpp)
target_include_directories(plugin_snapshot PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../sources/plugin)
target_compile_definitions(plugin_snapshot PRIVATE
    JucePlugin_Name="Polyphone"
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0)
target_link_libraries(plugin_snapshot PRIVATE
    polyphone_synth_core
    PluginSnapshotData
    juce::juce_audio_utils
    juce::juce_gui_extra
    juce::juce_recommended_config_flags)

juce_add_console_app(plugin_threads PRODUCT_NAME "plugin_threads")
target_sources(plugin_threads PRIVATE
    plugin_threads.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../sources/plugin/pluginengine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../sources/plugin/pluginsoundfont.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../sources/plugin/pluginprocessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../sources/plugin/plugineditor.cpp)
target_include_directories(plugin_threads PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../sources/plugin)
target_compile_definitions(plugin_threads PRIVATE
    JucePlugin_Name="Polyphone"
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0)
target_link_libraries(plugin_threads PRIVATE
    polyphone_synth_core
    PluginSnapshotData
    juce::juce_audio_utils
    juce::juce_gui_extra
    juce::juce_recommended_config_flags)
