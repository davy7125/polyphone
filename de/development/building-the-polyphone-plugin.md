Mit dieser Anleitung kompilieren Sie das [Plugin](manual/plugin.md) von Polyphone (VST3, CLAP, LV2 und unter macOS AU) aus seinen Quellen.
Das Plugin wird mit <a href="https://juce.com" target="_blank">JUCE</a> und <a href="https://github.com/free-audio/clap-juce-extensions" target="_blank">clap-juce-extensions</a> erstellt, die bei der Konfiguration des Projekts automatisch heruntergeladen werden: Anders als der Editor benötigt es kein Qt.


## Voraussetzungen


Folgende Elemente werden benötigt:

* :program:`CMake` 3.22 oder höher und ein C++17-Compiler,
* die Bibliothek :program:`libvorbisfile` zum Lesen der sf3-Dateien,
* die Bibliothek :program:`libsndfile` zum Lesen der Samples der sfz-Dateien und der Waves der dls-Dateien.

Außerdem benötigen Sie die Quellen von Polyphone, die <a href="software" target="_blank">hier</a> oder auf <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> verfügbar sind.


### Linux

Außerdem werden die von JUCE benötigten Pakete gebraucht: ALSA, freetype, fontconfig und die X11-Bibliotheken.
Unter Ubuntu oder Debian:

```
sudo apt-get install build-essential cmake pkgconf libvorbis-dev libsndfile1-dev libasound2-dev libfreetype-dev libfontconfig1-dev libx11-dev libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev libxi-dev libxrandr-dev libxrender-dev libgl-dev
```

Unter Fedora:

```
sudo dnf install @development-tools cmake pkgconf libvorbis-devel libsndfile-devel alsa-lib-devel freetype-devel fontconfig-devel libX11-devel libXcomposite-devel libXcursor-devel libXext-devel libXinerama-devel libXi-devel libXrandr-devel libXrender-devel mesa-libGL-devel
```


### Windows und macOS

Die Bibliotheken können während der Konfiguration heruntergeladen und kompiliert werden, indem dem ersten der unten stehenden Befehle die Optionen ``-DPOLYPHONE_FETCH_VORBIS=ON -DPOLYPHONE_FETCH_SNDFILE=ON`` hinzugefügt werden.
Auf diese Weise kompiliert, besitzt :program:`libsndfile` keine externe Bibliothek: Die Samples der sfz-Dateien können in den Formaten wav und aiff gelesen werden, aber nicht in flac, ogg oder opus.

Die offiziellen Versionen verwenden stattdessen <a href="https://vcpkg.io" target="_blank">vcpkg</a>, das vollständige statische Bibliotheken bereitstellt (flac, ogg, opus und mp3 können gelesen werden).
Die Bibliotheken werden installiert mit:

```
vcpkg install --triplet <triplet> --overlay-triplets=packaging/vcpkg-triplets libsndfile libvorbis
```

Anschließend werden dem ersten der unten stehenden Befehle die folgenden Optionen hinzugefügt:

```
-DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=<triplet> -DVCPKG_OVERLAY_TRIPLETS=packaging/vcpkg-triplets -DVCPKG_MANIFEST_MODE=OFF
```

wobei das Triplet ``x64-windows-polyphone``, ``arm64-osx-polyphone`` oder ``x64-osx-polyphone`` ist.
Unter Windows wird das Plugin mit :program:`clang-cl` von :program:`Visual Studio` kompiliert: Zusätzlich ist die Option ``-T ClangCL`` hinzuzufügen.


## Kompilieren


Öffnen Sie im Stammverzeichnis des Projekts (dem Verzeichnis, das :file:`sources` enthält) ein Terminal und führen Sie die folgenden Befehle aus:

```
cmake -S sources/plugin -B build/plugin -DCMAKE_BUILD_TYPE=Release
cmake --build build/plugin --config Release --parallel 4
```

Die Plugins befinden sich dann im Verzeichnis :file:`build/plugin/PolyphonePlugin_artefacts/Release`: :file:`VST3/Polyphone.vst3`, :file:`CLAP/Polyphone.clap`, :file:`LV2/Polyphone.lv2` und unter macOS :file:`AU/Polyphone.component`.
Sie werden installiert, indem sie in die Ordner der Hosts kopiert werden, wie auf der Seite „[Polyphone-Plugin](manual/plugin.md#doc_install)“ beschrieben.

Das Verzeichnis :file:`Standalone` enthält außerdem eine eigenständige Anwendung, :program:`Polyphone Player`, die die Soundfonts ohne Host abspielt.

Das Plugin kann auch zusammen mit dem Editor kompiliert werden, indem beim [Kompilieren von Polyphone](development/building-polyphone-for-ubuntu.md) die Option ``-DPOLYPHONE_BUILD_PLUGIN=ON`` hinzugefügt wird.


## Übersetzungen


Die Texte des Plugins befinden sich in den Übersetzungsdateien von Polyphone (Kontexte :guilabel:`PluginEditor`, :guilabel:`PluginProcessor` und :guilabel:`PluginJuce`): Sie werden zusammen mit dem Editor übersetzt (siehe [Polyphone übersetzen](development/translate-polyphone.md)) und beim Kompilieren in das Plugin eingebettet.


## Fehlerbehebung


Diese [Kategorie](forum/support-bug-reports) des Forums hilft Ihnen bei Bedarf weiter.
