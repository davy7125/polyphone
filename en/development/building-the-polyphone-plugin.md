Use these instructions for building the [plugin](manual/plugin.md) of Polyphone (VST3, CLAP, LV2, and AU on macOS) from its sources.
The plugin is made with <a href="https://juce.com" target="_blank">JUCE</a> and <a href="https://github.com/free-audio/clap-juce-extensions" target="_blank">clap-juce-extensions</a>, downloaded automatically during the configuration of the project: unlike the editor, it doesn't need Qt.


## Prerequisite


The following elements are required:

* :program:`CMake` 3.22 or more and a C++17 compiler,
* the library :program:`libvorbisfile`, for reading the sf3 files,
* the library :program:`libsndfile`, for reading the samples of the sfz files and the waves of the dls files.

You also need the sources of Polyphone, available <a href="software" target="_blank">here</a> or on <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


### Linux

The packages needed by JUCE are also required: ALSA, freetype, fontconfig and the X11 libraries.
On Ubuntu or Debian:

```
sudo apt-get install build-essential cmake pkgconf libvorbis-dev libsndfile1-dev libasound2-dev libfreetype-dev libfontconfig1-dev libx11-dev libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev libxi-dev libxrandr-dev libxrender-dev libgl-dev
```

On Fedora:

```
sudo dnf install @development-tools cmake pkgconf libvorbis-devel libsndfile-devel alsa-lib-devel freetype-devel fontconfig-devel libX11-devel libXcomposite-devel libXcursor-devel libXext-devel libXinerama-devel libXi-devel libXrandr-devel libXrender-devel mesa-libGL-devel
```


### Windows and macOS

The libraries can be downloaded and built during the configuration, by adding the options ``-DPOLYPHONE_FETCH_VORBIS=ON -DPOLYPHONE_FETCH_SNDFILE=ON`` to the first command below.
Built this way, :program:`libsndfile` has no external library: the samples of the sfz files can be read in the formats wav and aiff, but not flac, ogg or opus.

The official versions use <a href="https://vcpkg.io" target="_blank">vcpkg</a> instead, which provides complete static libraries (flac, ogg, opus and mp3 can be read).
The libraries are installed with:

```
vcpkg install --triplet <triplet> --overlay-triplets=packaging/vcpkg-triplets libsndfile libvorbis
```

and the following options are then added to the first command below:

```
-DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=<triplet> -DVCPKG_OVERLAY_TRIPLETS=packaging/vcpkg-triplets -DVCPKG_MANIFEST_MODE=OFF
```

the triplet being ``x64-windows-polyphone``, ``arm64-osx-polyphone`` or ``x64-osx-polyphone``.
On Windows, the plugin is built with :program:`clang-cl`, of :program:`Visual Studio`: the option ``-T ClangCL`` is also to be added.


## Build


At the root of the project (the directory containing :file:`sources`), open a terminal and run the following commands:

```
cmake -S sources/plugin -B build/plugin -DCMAKE_BUILD_TYPE=Release
cmake --build build/plugin --config Release --parallel 4
```

The plugins are then in the directory :file:`build/plugin/PolyphonePlugin_artefacts/Release`: :file:`VST3/Polyphone.vst3`, :file:`CLAP/Polyphone.clap`, :file:`LV2/Polyphone.lv2`, and :file:`AU/Polyphone.component` on macOS.
They are installed by copying them in the folders of the hosts, as described in the page “[Polyphone plugin](manual/plugin.md#doc_install)”.

The directory :file:`Standalone` also contains a standalone application, :program:`Polyphone Player`, which plays the soundfonts without a host.

The plugin can also be built with the editor, by adding the option ``-DPOLYPHONE_BUILD_PLUGIN=ON`` to the [build of Polyphone](development/building-polyphone-for-ubuntu.md).


## Translations


The texts of the plugin are in the translation files of Polyphone (contexts :guilabel:`PluginEditor`, :guilabel:`PluginProcessor` and :guilabel:`PluginJuce`): they are translated at the same time as the editor (see [Translate Polyphone](development/translate-polyphone.md)), and embedded in the plugin during its build.


## Troubleshooting


This forum [category](forum/support-bug-reports) will give you some help if needed.
