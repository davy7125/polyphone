# Polyphone as a plugin: VST3, CLAP, LV2, AU (macOS) and a standalone application playing soundfonts with the synth of
# the editor (sources/synth_core). See README.md. Build:
#   cmake -S sources/plugin -B build/plugin && cmake --build build/plugin
# JUCE (AGPLv3) and clap-juce-extensions (MIT) are downloaded at the configuration, unless local copies are given with
# -DPOLYPHONE_JUCE_DIR=<path> and -DPOLYPHONE_CLAP_JUCE_EXTENSIONS_DIR=<path>.

cmake_minimum_required(VERSION 3.22)

# Same version as the editor, read in sources/version.txt ("2.7.0-BETA": the numbers only)
file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/../version.txt POLYPHONE_VERSION_FULL LIMIT_COUNT 1)
string(REGEX MATCH "^[0-9]+(\\.[0-9]+)*" POLYPHONE_VERSION "${POLYPHONE_VERSION_FULL}")
if(NOT POLYPHONE_VERSION)
    message(FATAL_ERROR "No version found in sources/version.txt")
endif()
project(PolyphonePlugin VERSION ${POLYPHONE_VERSION} LANGUAGES C CXX)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../version.txt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(POLYPHONE_JUCE_DIR "" CACHE PATH "Local copy of JUCE (downloaded otherwise)")
set(POLYPHONE_CLAP_JUCE_EXTENSIONS_DIR "" CACHE PATH "Local copy of clap-juce-extensions (downloaded otherwise)")
option(POLYPHONE_FETCH_VORBIS "Download and build libogg and libvorbis (if they are not installed, Windows and macOS)" OFF)
option(POLYPHONE_FETCH_SNDFILE "Download and build libsndfile (if it is not installed, Windows and macOS)" OFF)

include(FetchContent)

# JUCE
if(POLYPHONE_JUCE_DIR)
    add_subdirectory(${POLYPHONE_JUCE_DIR} JUCE EXCLUDE_FROM_ALL)
else()
    FetchContent_Declare(JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 9.0.4
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(JUCE)
endif()

# CLAP format for the plugins made with JUCE
if(POLYPHONE_CLAP_JUCE_EXTENSIONS_DIR)
    add_subdirectory(${POLYPHONE_CLAP_JUCE_EXTENSIONS_DIR} clap-juce-extensions EXCLUDE_FROM_ALL)
else()
    FetchContent_Declare(clap-juce-extensions
        GIT_REPOSITORY https://github.com/free-audio/clap-juce-extensions.git
        GIT_TAG 6bde108329ff9fc24404b79b8b397261a75cf6a8
        GIT_SUBMODULES_RECURSE TRUE)
    FetchContent_MakeAvailable(clap-juce-extensions)
endif()

# libvorbisfile for the sf3 files, possibly built here (the synth core uses the target Vorbis::vorbisfile if it exists)
if(POLYPHONE_FETCH_VORBIS)
    set(BUILD_SHARED_LIBS OFF)
    set(INSTALL_DOCS OFF CACHE BOOL "" FORCE)
    set(INSTALL_PKG_CONFIG_MODULE OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5) # Their CMakeLists.txt ask for older versions, refused by CMake 4
    FetchContent_Declare(ogg
        GIT_REPOSITORY https://github.com/xiph/ogg.git
        GIT_TAG v1.3.5
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(ogg)
    FetchContent_Declare(vorbis
        GIT_REPOSITORY https://github.com/xiph/vorbis.git
        GIT_TAG v1.3.7
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(vorbis)
    unset(CMAKE_POLICY_VERSION_MINIMUM)
endif()

# libsndfile for the samples of the sfz files and the waves of the DLS files, possibly built here (the synth core uses the
# target SndFile::sndfile if it exists). Built here, it has no external library: the samples in flac, ogg or opus cannot
# be read (wav, aiff and the other formats of libsndfile can).
if(POLYPHONE_FETCH_SNDFILE)
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON) # Linked in the plugins
    set(BUILD_PROGRAMS OFF CACHE BOOL "" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(ENABLE_CPACK OFF CACHE BOOL "" FORCE)
    set(ENABLE_PACKAGE_CONFIG OFF CACHE BOOL "" FORCE)
    set(INSTALL_PKGCONFIG_MODULE OFF CACHE BOOL "" FORCE)
    set(INSTALL_MANPAGES OFF CACHE BOOL "" FORCE)
    set(ENABLE_EXTERNAL_LIBS OFF CACHE BOOL "" FORCE)
    set(ENABLE_MPEG OFF CACHE BOOL "" FORCE)
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5) # Its CMakeLists.txt asks for an older version, refused by CMake 4
    FetchContent_Declare(sndfile
        GIT_REPOSITORY https://github.com/libsndfile/libsndfile.git
        GIT_TAG 1.2.2
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(sndfile)
    unset(CMAKE_POLICY_VERSION_MINIMUM)
endif()

# Synth core of the editor
if(NOT TARGET polyphone_synth_core) # Already there if built from sources/CMakeLists.txt
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../synth_core synth_core)
endif()

# Logo and temperaments of the editor
juce_add_binary_data(PolyphonePluginData SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../logo.png
    ${CMAKE_CURRENT_SOURCE_DIR}/../resources/misc/temperaments.csv)

# Translations, extracted from the ones of the editor
include(${CMAKE_CURRENT_SOURCE_DIR}/translations.cmake)
polyphone_add_plugin_translations(PolyphonePluginTranslations)

# The codes identify the plugin in the hosts: they must never change once the plugin is published
juce_add_plugin(PolyphonePlugin
    PRODUCT_NAME "Polyphone"
    COMPANY_NAME "Polyphone"
    COMPANY_WEBSITE "https://www.polyphone.io"
    DESCRIPTION "Soundfont player (sf2, sf3, sfz and dls) with the synth of Polyphone"
    BUNDLE_ID io.polyphone.plugin
    PLUGIN_MANUFACTURER_CODE Plph
    PLUGIN_CODE Pply
    ICON_BIG ${CMAKE_CURRENT_SOURCE_DIR}/../../logo.png
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES Instrument Sampler
    AU_MAIN_TYPE kAudioUnitType_MusicDevice
    LV2URI https://www.polyphone.io/plugin
    FORMATS VST3 AU LV2 Standalone
    NEEDS_WEB_BROWSER FALSE
    NEEDS_CURL FALSE)

# Standalone application: another name than the editor (Polyphone.app, Polyphone.exe, polyphone), the plugins keep
# the name "Polyphone"
if(APPLE OR WIN32)
    set_target_properties(PolyphonePlugin_Standalone PROPERTIES OUTPUT_NAME "Polyphone Player")
else()
    set_target_properties(PolyphonePlugin_Standalone PROPERTIES OUTPUT_NAME "polyphone-player")
endif()

clap_juce_extensions_plugin(TARGET PolyphonePlugin
    CLAP_ID io.polyphone.plugin
    CLAP_FEATURES instrument sampler stereo)

# Windows: version information (product name and version, checked when signing) and icon generated by JUCE for its
# formats, not added by clap-juce-extensions to the CLAP library
if(WIN32)
    target_link_libraries(PolyphonePlugin_CLAP PRIVATE PolyphonePlugin_rc_lib)
endif()

target_sources(PolyphonePlugin PRIVATE
    pluginengine.cpp
    pluginsoundfont.cpp
    pluginprocessor.cpp
    plugineditor.cpp
    plugintranslator.cpp)

target_compile_definitions(PolyphonePlugin PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_USE_OPUS=0) # Codec not used (its code triggers -Wstringop-overflow warnings with GCC)

target_link_libraries(PolyphonePlugin
    PRIVATE
        polyphone_synth_core
        PolyphonePluginData
        PolyphonePluginTranslations
        juce::juce_audio_utils
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# GCC: parallel LTO link even without a make jobserver (Ninja), otherwise the link is serial with a warning
target_link_options(PolyphonePlugin INTERFACE $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU>>:-flto=auto>)
