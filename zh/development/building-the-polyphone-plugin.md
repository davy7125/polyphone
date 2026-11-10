按照以下说明，可以从源代码编译 Polyphone 的[插件](manual/plugin.md)（VST3、CLAP、LV2，在 macOS 上还有 AU）。
该插件基于 <a href="https://juce.com" target="_blank">JUCE</a> 和 <a href="https://github.com/free-audio/clap-juce-extensions" target="_blank">clap-juce-extensions</a> 开发，它们会在配置项目时自动下载：与编辑器不同，插件不需要 Qt。


## 前提条件


需要以下组件：

* :program:`CMake` 3.22 或更高版本，以及 C++17 编译器，
* 用于读取 sf3 文件的 :program:`libvorbisfile` 库，
* 用于读取 sfz 文件的采样和 dls 文件的波形的 :program:`libsndfile` 库。

还需要 Polyphone 的源代码，可以在<a href="software" target="_blank">这里</a>或 <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> 上获取。


### Linux

还需要 JUCE 所需的软件包：ALSA、freetype、fontconfig 以及 X11 库。
在 Ubuntu 或 Debian 上：

```
sudo apt-get install build-essential cmake pkgconf libvorbis-dev libsndfile1-dev libasound2-dev libfreetype-dev libfontconfig1-dev libx11-dev libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev libxi-dev libxrandr-dev libxrender-dev libgl-dev
```

在 Fedora 上：

```
sudo dnf install @development-tools cmake pkgconf libvorbis-devel libsndfile-devel alsa-lib-devel freetype-devel fontconfig-devel libX11-devel libXcomposite-devel libXcursor-devel libXext-devel libXinerama-devel libXi-devel libXrandr-devel libXrender-devel mesa-libGL-devel
```


### Windows 和 macOS

在下面的第一条命令中添加选项 ``-DPOLYPHONE_FETCH_VORBIS=ON -DPOLYPHONE_FETCH_SNDFILE=ON``，即可在配置时下载并编译这些库。
以这种方式编译的 :program:`libsndfile` 不带任何外部库：sfz 文件的采样可以读取 wav 和 aiff 格式，但不能读取 flac、ogg 或 opus 格式。

官方版本则使用 <a href="https://vcpkg.io" target="_blank">vcpkg</a>，它提供完整的静态库（可以读取 flac、ogg、opus 和 mp3）。
用以下命令安装这些库：

```
vcpkg install --triplet <triplet> --overlay-triplets=packaging/vcpkg-triplets libsndfile libvorbis
```

然后在下面的第一条命令中添加以下选项：

```
-DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=<triplet> -DVCPKG_OVERLAY_TRIPLETS=packaging/vcpkg-triplets -DVCPKG_MANIFEST_MODE=OFF
```

其中 triplet 为 ``x64-windows-polyphone``、``arm64-osx-polyphone`` 或 ``x64-osx-polyphone``。
在 Windows 上，插件使用 :program:`Visual Studio` 的 :program:`clang-cl` 编译：还需要添加选项 ``-T ClangCL``。


## 编译


在项目的根目录（包含 :file:`sources` 的目录）中打开终端，运行以下命令：

```
cmake -S sources/plugin -B build/plugin -DCMAKE_BUILD_TYPE=Release
cmake --build build/plugin --config Release --parallel 4
```

编译好的插件位于 :file:`build/plugin/PolyphonePlugin_artefacts/Release` 目录中：:file:`VST3/Polyphone.vst3`、:file:`CLAP/Polyphone.clap`、:file:`LV2/Polyphone.lv2`，在 macOS 上还有 :file:`AU/Polyphone.component`。
按照“[Polyphone 插件](manual/plugin.md#doc_install)”页面中的说明，将它们复制到宿主的文件夹中即可完成安装。

:file:`Standalone` 目录中还包含一个独立应用程序 :program:`Polyphone Player`，无需宿主即可演奏音色库。

也可以在[编译 Polyphone](development/building-polyphone-for-ubuntu.md) 时添加选项 ``-DPOLYPHONE_BUILD_PLUGIN=ON``，将插件与编辑器一起编译。


## 翻译


插件的文本位于 Polyphone 的翻译文件中（:guilabel:`PluginEditor`、:guilabel:`PluginProcessor` 和 :guilabel:`PluginJuce` 上下文）：它们与编辑器同时翻译（参见[翻译 Polyphone](development/translate-polyphone.md)），并在编译时嵌入插件中。


## 故障排除


如有需要，可以在论坛的这个[分类](forum/support-bug-reports)中获得帮助。
