Usa queste istruzioni per compilare il [plugin](manual/plugin.md) di Polyphone (VST3, CLAP, LV2 e AU su macOS) a partire dai suoi sorgenti.
Il plugin è realizzato con <a href="https://juce.com" target="_blank">JUCE</a> e <a href="https://github.com/free-audio/clap-juce-extensions" target="_blank">clap-juce-extensions</a>, scaricati automaticamente durante la configurazione del progetto: a differenza dell'editor, non ha bisogno di Qt.


## Prerequisiti


Sono necessari i seguenti elementi:

* :program:`CMake` 3.22 o superiore e un compilatore C++17,
* la libreria :program:`libvorbisfile`, per leggere i file sf3,
* la libreria :program:`libsndfile`, per leggere i campioni dei file sfz e le onde dei file dls.

Ti servono anche i sorgenti di Polyphone, disponibili <a href="software" target="_blank">qui</a> o su <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


### Linux

Sono necessari anche i pacchetti richiesti da JUCE: ALSA, freetype, fontconfig e le librerie X11.
Su Ubuntu o Debian:

```
sudo apt-get install build-essential cmake pkgconf libvorbis-dev libsndfile1-dev libasound2-dev libfreetype-dev libfontconfig1-dev libx11-dev libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev libxi-dev libxrandr-dev libxrender-dev libgl-dev
```

Su Fedora:

```
sudo dnf install @development-tools cmake pkgconf libvorbis-devel libsndfile-devel alsa-lib-devel freetype-devel fontconfig-devel libX11-devel libXcomposite-devel libXcursor-devel libXext-devel libXinerama-devel libXi-devel libXrandr-devel libXrender-devel mesa-libGL-devel
```


### Windows e macOS

Le librerie possono essere scaricate e compilate durante la configurazione, aggiungendo le opzioni ``-DPOLYPHONE_FETCH_VORBIS=ON -DPOLYPHONE_FETCH_SNDFILE=ON`` al primo dei comandi riportati più sotto.
Compilata in questo modo, :program:`libsndfile` non ha librerie esterne: i campioni dei file sfz possono essere letti nei formati wav e aiff, ma non flac, ogg o opus.

Le versioni ufficiali usano invece <a href="https://vcpkg.io" target="_blank">vcpkg</a>, che fornisce librerie statiche complete (è possibile leggere flac, ogg, opus e mp3).
Le librerie si installano con:

```
vcpkg install --triplet <triplet> --overlay-triplets=packaging/vcpkg-triplets libsndfile libvorbis
```

e le seguenti opzioni vengono poi aggiunte al primo dei comandi riportati più sotto:

```
-DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=<triplet> -DVCPKG_OVERLAY_TRIPLETS=packaging/vcpkg-triplets -DVCPKG_MANIFEST_MODE=OFF
```

dove il triplet è ``x64-windows-polyphone``, ``arm64-osx-polyphone`` o ``x64-osx-polyphone``.
Su Windows, il plugin viene compilato con :program:`clang-cl` di :program:`Visual Studio`: va aggiunta anche l'opzione ``-T ClangCL``.


## Compilazione


Nella radice del progetto (la cartella che contiene :file:`sources`), apri un terminale ed esegui i comandi seguenti:

```
cmake -S sources/plugin -B build/plugin -DCMAKE_BUILD_TYPE=Release
cmake --build build/plugin --config Release --parallel 4
```

I plugin si trovano allora nella cartella :file:`build/plugin/PolyphonePlugin_artefacts/Release`: :file:`VST3/Polyphone.vst3`, :file:`CLAP/Polyphone.clap`, :file:`LV2/Polyphone.lv2` e, su macOS, :file:`AU/Polyphone.component`.
Si installano copiandoli nelle cartelle degli host, come descritto nella pagina «[Plugin di Polyphone](manual/plugin.md#doc_install)».

La cartella :file:`Standalone` contiene anche un'applicazione autonoma, :program:`Polyphone Player`, che suona i soundfont senza host.

Il plugin può anche essere compilato insieme all'editor, aggiungendo l'opzione ``-DPOLYPHONE_BUILD_PLUGIN=ON`` alla [compilazione di Polyphone](development/building-polyphone-for-ubuntu.md).


## Traduzioni


I testi del plugin si trovano nei file di traduzione di Polyphone (contesti :guilabel:`PluginEditor`, :guilabel:`PluginProcessor` e :guilabel:`PluginJuce`): vengono tradotti insieme all'editor (vedi [Tradurre Polyphone](development/translate-polyphone.md)) e incorporati nel plugin durante la sua compilazione.


## Risoluzione dei problemi


Questa [categoria](forum/support-bug-reports) del forum ti darà aiuto, se necessario.
