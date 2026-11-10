Следуйте этим инструкциям, чтобы собрать [плагин](manual/plugin.md) Polyphone (VST3, CLAP, LV2, а в macOS — AU) из исходного кода.
Плагин создан с помощью <a href="https://juce.com" target="_blank">JUCE</a> и <a href="https://github.com/free-audio/clap-juce-extensions" target="_blank">clap-juce-extensions</a>, которые загружаются автоматически при настройке проекта: в отличие от редактора, ему не нужен Qt.


## Что понадобится


Необходимы следующие компоненты:

* :program:`CMake` версии 3.22 или новее и компилятор C++17,
* библиотека :program:`libvorbisfile` — для чтения файлов sf3,
* библиотека :program:`libsndfile` — для чтения семплов файлов sfz и волн файлов dls.

Также нужен исходный код Polyphone, доступный <a href="software" target="_blank">здесь</a> или на <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


### Linux

Также необходимы пакеты, которые нужны JUCE: ALSA, freetype, fontconfig и библиотеки X11.
В Ubuntu или Debian:

```
sudo apt-get install build-essential cmake pkgconf libvorbis-dev libsndfile1-dev libasound2-dev libfreetype-dev libfontconfig1-dev libx11-dev libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev libxi-dev libxrandr-dev libxrender-dev libgl-dev
```

В Fedora:

```
sudo dnf install @development-tools cmake pkgconf libvorbis-devel libsndfile-devel alsa-lib-devel freetype-devel fontconfig-devel libX11-devel libXcomposite-devel libXcursor-devel libXext-devel libXinerama-devel libXi-devel libXrandr-devel libXrender-devel mesa-libGL-devel
```


### Windows и macOS

Библиотеки можно скачать и собрать во время настройки, добавив к первой из приведённых ниже команд параметры ``-DPOLYPHONE_FETCH_VORBIS=ON -DPOLYPHONE_FETCH_SNDFILE=ON``.
Собранная таким образом библиотека :program:`libsndfile` не использует внешних библиотек: семплы файлов sfz можно читать в форматах wav и aiff, но не flac, ogg или opus.

Официальные версии вместо этого используют <a href="https://vcpkg.io" target="_blank">vcpkg</a>, который предоставляет полные статические библиотеки (поддерживается чтение flac, ogg, opus и mp3).
Библиотеки устанавливаются командой:

```
vcpkg install --triplet <triplet> --overlay-triplets=packaging/vcpkg-triplets libsndfile libvorbis
```

после чего к первой из приведённых ниже команд добавляются следующие параметры:

```
-DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=<triplet> -DVCPKG_OVERLAY_TRIPLETS=packaging/vcpkg-triplets -DVCPKG_MANIFEST_MODE=OFF
```

где триплет — ``x64-windows-polyphone``, ``arm64-osx-polyphone`` или ``x64-osx-polyphone``.
В Windows плагин собирается с помощью :program:`clang-cl` из :program:`Visual Studio`: нужно также добавить параметр ``-T ClangCL``.


## Сборка


В корне проекта (в каталоге, содержащем :file:`sources`) откройте терминал и выполните следующие команды:

```
cmake -S sources/plugin -B build/plugin -DCMAKE_BUILD_TYPE=Release
cmake --build build/plugin --config Release --parallel 4
```

Плагины появятся в каталоге :file:`build/plugin/PolyphonePlugin_artefacts/Release`: :file:`VST3/Polyphone.vst3`, :file:`CLAP/Polyphone.clap`, :file:`LV2/Polyphone.lv2`, а в macOS — :file:`AU/Polyphone.component`.
Они устанавливаются копированием в папки хостов, как описано на странице «[Плагин Polyphone](manual/plugin.md#doc_install)».

В каталоге :file:`Standalone` также находится автономное приложение :program:`Polyphone Player`, которое воспроизводит SoundFont без хоста.

Плагин можно также собрать вместе с редактором, добавив параметр ``-DPOLYPHONE_BUILD_PLUGIN=ON`` при [сборке Polyphone](development/building-polyphone-for-ubuntu.md).


## Переводы


Тексты плагина находятся в файлах перевода Polyphone (контексты :guilabel:`PluginEditor`, :guilabel:`PluginProcessor` и :guilabel:`PluginJuce`): они переводятся одновременно с редактором (см. [Перевод Polyphone](development/translate-polyphone.md)) и встраиваются в плагин при его сборке.


## Устранение неполадок


При необходимости помощь можно получить в этой [категории](forum/support-bug-reports) форума.
