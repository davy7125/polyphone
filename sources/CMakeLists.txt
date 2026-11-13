# Polyphone built with CMake: the editor, and possibly the plugin and the tests of the synth core. The official builds
# use it (GitHub workflows, Debian and Fedora packages).
#
#   cmake -S sources -B build/polyphone            (Qt 6 if found, otherwise Qt 5: -DPOLYPHONE_QT_VERSION=5 or 6)
#   cmake --build build/polyphone --parallel 4     (executable: build/polyphone/bin/polyphone)
#
# Qt Creator: open this file, choose the kits (Qt 5 or Qt 6), the options are in Projects > Build > CMake.
# Translations: "cmake --build <build> --target update_translations" updates the .ts files with lupdate (texts of the
# editor and of the plugin).

cmake_minimum_required(VERSION 3.16)

# Version read in version.txt: "2.7.0-BETA" gives the version 2.7.0 and the identifier BETA. Not "VERSION": this
# folder is in the include path and, on the case-insensitive file systems (macOS, Windows), <version> of the C++
# library would be this file
file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/version.txt POLYPHONE_VERSION_FULL LIMIT_COUNT 1)
string(STRIP "${POLYPHONE_VERSION_FULL}" POLYPHONE_VERSION_FULL)
string(REGEX MATCH "^[0-9]+(\\.[0-9]+)*" POLYPHONE_VERSION "${POLYPHONE_VERSION_FULL}")
# Not REGEX REPLACE "^[^-]*-?": before CMake 4.1 (CMP0186), "^" matches again after each replacement ("BETA" removed
# too, or "matched an empty string" error with CMake 3.28)
set(POLYPHONE_IDENTIFIER "")
if(POLYPHONE_VERSION_FULL MATCHES "-(.+)$")
    set(POLYPHONE_IDENTIFIER "${CMAKE_MATCH_1}")
endif()
if(NOT POLYPHONE_VERSION)
    message(FATAL_ERROR "No version found in version.txt")
endif()

if(APPLE AND NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0 CACHE STRING "Minimum version of macOS")
endif()

project(Polyphone VERSION ${POLYPHONE_VERSION} LANGUAGES C CXX)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/version.txt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# -std=c++17, not gnu++17: with GNU extensions, GCC may fuse multiplications and additions (FMA, ARM) and the
# renderings would differ
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(POLYPHONE_BUILD_EDITOR "Build the editor (Qt 5 or 6)" ON)
option(POLYPHONE_BUILD_PLUGIN "Build the plugin (JUCE downloaded, see plugin/README.md)" OFF)
option(POLYPHONE_BUILD_TESTS "Build the tests of the synth core (see tests/synth_core/README.md)" OFF)
option(POLYPHONE_OFFICIAL_BUILD "Official build (GitHub workflows, packaging scripts): the new versions are notified. \
Not for the package of a distribution, which updates the software itself" OFF)
option(POLYPHONE_LOCAL_RTAUDIO "Use the copy of RtAudio in lib/ (always on Windows and macOS)" OFF)
option(POLYPHONE_LOCAL_RTMIDI "Use the copy of RtMidi in lib/ (always on Windows and macOS)" OFF)
option(POLYPHONE_WOLFSSL "Use wolfSSL instead of OpenSSL" OFF)
option(POLYPHONE_NO_SF2_REPOSITORY "Hide the soundfont repository" OFF)
option(POLYPHONE_NO_SF2_CREATION "Hide the creation of soundfonts" OFF)
if(WIN32 OR APPLE)
    set(POLYPHONE_LOCAL_RTAUDIO ON)
    set(POLYPHONE_LOCAL_RTMIDI ON)
endif()

# MinGW: option needed by sfArk, given to all sources
if(MINGW)
    add_compile_options(-ffloat-store)
endif()

# Synth core, shared with the plugin
add_subdirectory(synth_core)

if(POLYPHONE_BUILD_EDITOR)
    set(POLYPHONE_QT_VERSION "" CACHE STRING "Major version of Qt: 5 or 6 (the most recent one found if empty)")
    if(POLYPHONE_QT_VERSION)
        set(QT_VERSION_MAJOR ${POLYPHONE_QT_VERSION})
    else()
        find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
    endif()
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Svg Network PrintSupport Concurrent)
    find_package(Qt${QT_VERSION_MAJOR} QUIET COMPONENTS LinguistTools) # Optional: lrelease is enough
    message(STATUS "Polyphone ${POLYPHONE_VERSION_FULL} with Qt ${Qt${QT_VERSION_MAJOR}_VERSION}")

    include(cmake/files.cmake)
    string(TIMESTAMP POLYPHONE_YEAR "%Y")

    # Libraries of the system
    add_library(polyphone_dependencies INTERFACE)
    if(WIN32)
        target_link_libraries(polyphone_dependencies INTERFACE
            z winmm ogg vorbis vorbisfile vorbisenc crypto FLAC sndfile
            ole32 ksuser mfplat mfuuid wmcodecdspuuid) # The last ones for RtAudio
    elseif(APPLE)
        target_include_directories(polyphone_dependencies INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/../macos/include /opt/homebrew/include /usr/local/include)
        target_link_directories(polyphone_dependencies INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/../macos/lib /opt/homebrew/lib /usr/local/lib)
        target_link_libraries(polyphone_dependencies INTERFACE
            ssl crypto z sndfile opus mpg123 ogg FLAC vorbis vorbisfile vorbisenc mp3lame
            "-framework CoreAudio" "-framework CoreMIDI" "-framework CoreFoundation"
            "-framework AudioUnit" "-framework AudioToolbox" "-framework Cocoa")
    else()
        find_package(PkgConfig REQUIRED)
        set(modules alsa jack zlib ogg vorbis vorbisfile vorbisenc sndfile)
        if(POLYPHONE_WOLFSSL)
            list(APPEND modules wolfssl)
        else()
            list(APPEND modules openssl)
        endif()
        if(NOT POLYPHONE_LOCAL_RTAUDIO)
            list(APPEND modules rtaudio)
        endif()
        if(NOT POLYPHONE_LOCAL_RTMIDI)
            list(APPEND modules rtmidi)
        endif()
        pkg_check_modules(POLYPHONE_SYSTEM REQUIRED IMPORTED_TARGET ${modules})
        target_link_libraries(polyphone_dependencies INTERFACE PkgConfig::POLYPHONE_SYSTEM)
    endif()

    # Definitions given to the sources of the editor
    add_library(polyphone_definitions INTERFACE)
    target_compile_definitions(polyphone_definitions INTERFACE
        SOFT_VERSION="${POLYPHONE_VERSION}"
        IDENTIFIER="${POLYPHONE_IDENTIFIER}"
        CURRENT_YEAR="${POLYPHONE_YEAR}"
        SFTOOLS_NOXML
        $<$<BOOL:${POLYPHONE_OFFICIAL_BUILD}>:OFFICIAL_BUILD>
        $<$<BOOL:${POLYPHONE_LOCAL_RTAUDIO}>:USE_LOCAL_RTAUDIO>
        $<$<BOOL:${POLYPHONE_LOCAL_RTMIDI}>:USE_LOCAL_RTMIDI>
        $<$<BOOL:${POLYPHONE_WOLFSSL}>:USE_WOLFSSL>
        $<$<BOOL:${POLYPHONE_NO_SF2_REPOSITORY}>:NO_SF2_REPOSITORY>
        $<$<BOOL:${POLYPHONE_NO_SF2_CREATION}>:NO_SF2_CREATION>)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND NOT MSVC)
        target_compile_options(polyphone_definitions INTERFACE -Wall -Wextra)
    endif()
    if(WIN32)
        target_compile_definitions(polyphone_definitions INTERFACE __WINDOWS_MM__ __WINDOWS_WASAPI__ __WINDOWS_ASIO__)
    elseif(APPLE)
        target_compile_definitions(polyphone_definitions INTERFACE __MACOSX_CORE__ TARGET_OS_IPHONE=0)
    else()
        target_compile_definitions(polyphone_definitions INTERFACE __LINUX_ALSASEQ__ __UNIX_JACK__ __LINUX_ALSA__)
    endif()

    # Soundfonts, samples, import and export: without the widgets of Qt (the build fails if they are included), the
    # user interface deriving its classes to ask questions and giving the preferences it reads (CoreSettings)
    add_library(polyphone_core STATIC ${POLYPHONE_CORE_FILES})
    set_target_properties(polyphone_core PROPERTIES AUTOMOC ON)
    target_include_directories(polyphone_core PUBLIC
        core core/input core/output core/model core/sample core/types lib)
    target_link_libraries(polyphone_core
        PUBLIC
            polyphone_synth_core
            polyphone_definitions
            polyphone_dependencies
            Qt${QT_VERSION_MAJOR}::Core
            Qt${QT_VERSION_MAJOR}::Gui
            Qt${QT_VERSION_MAJOR}::Concurrent)
    target_precompile_headers(polyphone_core PRIVATE <QtCore> <QtGui>)

    # Application
    set(POLYPHONE_PLATFORM_FILES "")
    if(POLYPHONE_LOCAL_RTAUDIO)
        list(APPEND POLYPHONE_PLATFORM_FILES lib/_option_rtaudio/rtaudio/RtAudio.h lib/_option_rtaudio/rtaudio/RtAudio.cpp)
    endif()
    if(POLYPHONE_LOCAL_RTMIDI)
        list(APPEND POLYPHONE_PLATFORM_FILES lib/_option_rtmidi/rtmidi/RtMidi.h lib/_option_rtmidi/rtmidi/RtMidi.cpp)
    endif()
    if(WIN32)
        # ASIO with RtAudio, icons of the executable and of the files
        list(APPEND POLYPHONE_PLATFORM_FILES
            lib/_option_rtaudio/rtaudio/include/asio.h
            lib/_option_rtaudio/rtaudio/include/asiodrivers.h
            lib/_option_rtaudio/rtaudio/include/asiodrvr.h
            lib/_option_rtaudio/rtaudio/include/asiolist.h
            lib/_option_rtaudio/rtaudio/include/asiosys.h
            lib/_option_rtaudio/rtaudio/include/dsound.h
            lib/_option_rtaudio/rtaudio/include/functiondiscoverykeys_devpkey.h
            lib/_option_rtaudio/rtaudio/include/ginclude.h
            lib/_option_rtaudio/rtaudio/include/iasiodrv.h
            lib/_option_rtaudio/rtaudio/include/iasiothiscallresolver.h
            lib/_option_rtaudio/rtaudio/include/soundcard.h
            lib/_option_rtaudio/rtaudio/include/asio.cpp
            lib/_option_rtaudio/rtaudio/include/asiodrivers.cpp
            lib/_option_rtaudio/rtaudio/include/asiolist.cpp
            lib/_option_rtaudio/rtaudio/include/iasiothiscallresolver.cpp
            ../packaging/windows/polyphone.rc)
    elseif(APPLE)
        set(POLYPHONE_BUNDLE_FILES
            ../packaging/mac/polyphone.icns
            ../packaging/mac/file_sf2.icns
            ../packaging/mac/file_sf3.icns
            ../packaging/mac/file_sfArk.icns
            ../packaging/mac/file_sfz.icns
            ../packaging/mac/file_organ.icns
            ../packaging/mac/file_dls.icns)
        set_source_files_properties(${POLYPHONE_BUNDLE_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        list(APPEND POLYPHONE_PLATFORM_FILES ${POLYPHONE_BUNDLE_FILES})
    endif()

    add_executable(polyphone WIN32 MACOSX_BUNDLE
        ${POLYPHONE_APP_FILES} ${POLYPHONE_PLATFORM_FILES} resources.qrc)
    set_target_properties(polyphone PROPERTIES
        AUTOMOC ON
        AUTOUIC ON
        AUTORCC ON
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    target_include_directories(polyphone PRIVATE
        . clavier context context/interface dialogs directory editor editor/footers editor/graphics editor/modulator
        editor/overview editor/timbre editor/timbre/model editor/tools editor/tree editor/widgets extensions lib
        lib/qtsingleapplication mainwindow mcp midi_player player repository repository/browser repository/daily
        repository/soundfont repository/soundfont/viewer repository/soundfont/editor repository/user repository/widgets
        resources sound_engine sound_engine/elements)
    if(POLYPHONE_LOCAL_RTAUDIO)
        target_include_directories(polyphone PRIVATE lib/_option_rtaudio lib/_option_rtaudio/rtaudio/include)
    endif()
    if(POLYPHONE_LOCAL_RTMIDI)
        target_include_directories(polyphone PRIVATE lib/_option_rtmidi)
    endif()
    if(WIN32)
        # Icons of the resource file
        target_include_directories(polyphone PRIVATE ../packaging/windows)
    endif()
    target_link_libraries(polyphone PRIVATE
        polyphone_core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Svg
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::PrintSupport)
    target_precompile_headers(polyphone PRIVATE precompiled_header.h)

    if(APPLE)
        set_target_properties(polyphone PROPERTIES
            OUTPUT_NAME Polyphone
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/../packaging/mac/polyphone.plist)
    endif()

    # Translations compiled by lrelease and embedded (prefix ":/i18n")
    file(GLOB POLYPHONE_TS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/translations/polyphone_*.ts)
    if(TARGET Qt${QT_VERSION_MAJOR}::lrelease)
        set(POLYPHONE_LRELEASE $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::lrelease>)
    else()
        find_program(POLYPHONE_LRELEASE NAMES lrelease lrelease-qt${QT_VERSION_MAJOR}
            HINTS ${QT${QT_VERSION_MAJOR}_INSTALL_PREFIX}/${QT${QT_VERSION_MAJOR}_INSTALL_BINS}
                  ${QT${QT_VERSION_MAJOR}_INSTALL_PREFIX}/${QT${QT_VERSION_MAJOR}_INSTALL_LIBEXECS}
                  /usr/lib/qt${QT_VERSION_MAJOR}/bin)
        if(NOT POLYPHONE_LRELEASE)
            message(FATAL_ERROR "lrelease not found (Qt Linguist tools), give its path with -DPOLYPHONE_LRELEASE=<path>")
        endif()
    endif()
    set(POLYPHONE_QM_FILES "")
    set(qrc "<RCC>\n  <qresource prefix=\"/i18n\">\n")
    foreach(ts ${POLYPHONE_TS_FILES})
        get_filename_component(name ${ts} NAME_WE)
        set(qm ${CMAKE_CURRENT_BINARY_DIR}/i18n/${name}.qm)
        add_custom_command(OUTPUT ${qm}
            COMMAND ${POLYPHONE_LRELEASE} -silent -nounfinished -removeidentical ${ts} -qm ${qm}
            DEPENDS ${ts} VERBATIM)
        list(APPEND POLYPHONE_QM_FILES ${qm})
        string(APPEND qrc "    <file alias=\"${name}.qm\">${qm}</file>\n")
    endforeach()
    string(APPEND qrc "  </qresource>\n</RCC>\n")
    # Written if it changed only: otherwise each CMake run (Qt Creator runs it when opening the project) builds the
    # translation resource and links the application again. The .tmp file is kept (configure_file input).
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/i18n/translations.qrc.tmp "${qrc}")
    configure_file(${CMAKE_CURRENT_BINARY_DIR}/i18n/translations.qrc.tmp
                   ${CMAKE_CURRENT_BINARY_DIR}/i18n/translations.qrc COPYONLY)
    qt_add_resources(POLYPHONE_TRANSLATION_RESOURCES ${CMAKE_CURRENT_BINARY_DIR}/i18n/translations.qrc)
    set_source_files_properties(${POLYPHONE_TRANSLATION_RESOURCES} PROPERTIES SKIP_AUTOGEN ON)
    target_sources(polyphone PRIVATE ${POLYPHONE_QM_FILES} ${POLYPHONE_TRANSLATION_RESOURCES})

    # Target "update_translations": the .ts files updated by lupdate with the files built (relative locations, strings
    # of the files ignored by git not included)
    if(TARGET Qt${QT_VERSION_MAJOR}::lupdate)
        set(POLYPHONE_LUPDATE $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::lupdate>)
    else()
        find_program(POLYPHONE_LUPDATE NAMES lupdate lupdate-qt${QT_VERSION_MAJOR}
            HINTS ${QT${QT_VERSION_MAJOR}_INSTALL_PREFIX}/${QT${QT_VERSION_MAJOR}_INSTALL_BINS}
                  /usr/lib/qt${QT_VERSION_MAJOR}/bin)
    endif()
    if(POLYPHONE_LUPDATE)
        # The texts of the plugin are in the same files (contexts "Plugin...", see plugin/plugintranslator.h)
        set(POLYPHONE_PLUGIN_TRANSLATED_FILES
            plugin/plugineditor.cpp
            plugin/plugineditor.h
            plugin/pluginprocessor.cpp
            plugin/pluginprocessor.h
            plugin/plugintranslator.cpp
            plugin/plugintranslator.h)
        list(JOIN POLYPHONE_APP_FILES "\n" app_files)
        list(JOIN POLYPHONE_CORE_FILES "\n" core_files)
        list(JOIN POLYPHONE_PLUGIN_TRANSLATED_FILES "\n" plugin_files)
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/i18n/files.txt "${core_files}\n${app_files}\n${plugin_files}\n")
        add_custom_target(update_translations
            COMMAND ${POLYPHONE_LUPDATE} @${CMAKE_CURRENT_BINARY_DIR}/i18n/files.txt -ts ${POLYPHONE_TS_FILES}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Updating the translations (resources/translations)"
            VERBATIM)
    endif()

    # Installation (Linux)
    if(UNIX AND NOT APPLE)
        include(GNUInstallDirs)
        install(TARGETS polyphone RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
        install(FILES resources/polyphone.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/512x512/apps)
        install(FILES ../packaging/io.polyphone.polyphone.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
        install(FILES ../packaging/io.polyphone.polyphone.metainfo.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
        install(FILES ../packaging/polyphone.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/mime/packages)
        install(DIRECTORY ../packaging/man DESTINATION ${CMAKE_INSTALL_DATADIR})
        install(FILES ../README.md changelog DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/polyphone)
    endif()
endif()

if(POLYPHONE_BUILD_PLUGIN)
    add_subdirectory(plugin)
endif()

if(POLYPHONE_BUILD_TESTS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../tests/synth_core tests/synth_core)
endif()
