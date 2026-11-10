Utilisez ces instructions pour compiler le [plugin](manual/plugin.md) de Polyphone (VST3, CLAP et LV2, plus AU sous macOS) à partir de ses sources.
Le plugin est réalisé avec <a href="https://juce.com" target="_blank">JUCE</a> et <a href="https://github.com/free-audio/clap-juce-extensions" target="_blank">clap-juce-extensions</a>, téléchargés automatiquement pendant la configuration du projet&nbsp;: contrairement à l'éditeur, il n'a pas besoin de Qt.


## Prérequis


Les éléments suivants sont nécessaires&nbsp;:

* :program:`CMake` 3.22 ou ultérieur et un compilateur C++17,
* la bibliothèque :program:`libvorbisfile`, pour lire les fichiers sf3,
* la bibliothèque :program:`libsndfile`, pour lire les échantillons des fichiers sfz et les ondes des fichiers dls.

Vous aurez également besoin des sources de Polyphone, disponibles <a href="software" target="_blank">ici</a> ou sur <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


### Linux

Les paquets nécessaires à JUCE sont également requis&nbsp;: ALSA, freetype, fontconfig et les bibliothèques X11.
Sous Ubuntu ou Debian&nbsp;:

```
sudo apt-get install build-essential cmake pkgconf libvorbis-dev libsndfile1-dev libasound2-dev libfreetype-dev libfontconfig1-dev libx11-dev libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev libxi-dev libxrandr-dev libxrender-dev libgl-dev
```

Sous Fedora&nbsp;:

```
sudo dnf install @development-tools cmake pkgconf libvorbis-devel libsndfile-devel alsa-lib-devel freetype-devel fontconfig-devel libX11-devel libXcomposite-devel libXcursor-devel libXext-devel libXinerama-devel libXi-devel libXrandr-devel libXrender-devel mesa-libGL-devel
```


### Windows et macOS

Les bibliothèques peuvent être téléchargées et compilées pendant la configuration, en ajoutant les options ``-DPOLYPHONE_FETCH_VORBIS=ON -DPOLYPHONE_FETCH_SNDFILE=ON`` à la première commande ci-dessous.
Compilée de cette manière, la bibliothèque :program:`libsndfile` ne dispose d'aucune bibliothèque externe&nbsp;: les échantillons des fichiers sfz peuvent être lus aux formats wav et aiff, mais pas flac, ogg ou opus.

Les versions officielles utilisent plutôt <a href="https://vcpkg.io" target="_blank">vcpkg</a>, qui fournit des bibliothèques statiques complètes (les formats flac, ogg, opus et mp3 peuvent être lus).
Les bibliothèques sont installées avec&nbsp;:

```
vcpkg install --triplet <triplet> --overlay-triplets=packaging/vcpkg-triplets libsndfile libvorbis
```

et les options suivantes sont alors ajoutées à la première commande ci-dessous&nbsp;:

```
-DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=<triplet> -DVCPKG_OVERLAY_TRIPLETS=packaging/vcpkg-triplets -DVCPKG_MANIFEST_MODE=OFF
```

le triplet étant ``x64-windows-polyphone``, ``arm64-osx-polyphone`` ou ``x64-osx-polyphone``.
Sous Windows, le plugin est compilé avec :program:`clang-cl`, fourni avec :program:`Visual Studio`&nbsp;: l'option ``-T ClangCL`` est également à ajouter.


## Compilation


À la racine du projet (le répertoire contenant :file:`sources`), ouvrez un terminal et exécutez les commandes suivantes&nbsp;:

```
cmake -S sources/plugin -B build/plugin -DCMAKE_BUILD_TYPE=Release
cmake --build build/plugin --config Release --parallel 4
```

Les plugins se trouvent alors dans le répertoire :file:`build/plugin/PolyphonePlugin_artefacts/Release`&nbsp;: :file:`VST3/Polyphone.vst3`, :file:`CLAP/Polyphone.clap`, :file:`LV2/Polyphone.lv2`, et :file:`AU/Polyphone.component` sous macOS.
Pour les installer, copiez-les dans les dossiers des hôtes, comme décrit sur la page «&nbsp;[Plugin Polyphone](manual/plugin.md#doc_install)&nbsp;».

Le répertoire :file:`Standalone` contient également une application autonome, :program:`Polyphone Player`, qui joue les banques de sons sans hôte.

Le plugin peut aussi être compilé avec l'éditeur, en ajoutant l'option ``-DPOLYPHONE_BUILD_PLUGIN=ON`` à la [compilation de Polyphone](development/building-polyphone-for-ubuntu.md).


## Traductions


Les textes du plugin se trouvent dans les fichiers de traduction de Polyphone (contextes :guilabel:`PluginEditor`, :guilabel:`PluginProcessor` et :guilabel:`PluginJuce`)&nbsp;: ils sont traduits en même temps que l'éditeur (voir [Traduire Polyphone](development/translate-polyphone.md)) et intégrés au plugin lors de sa compilation.


## Dépannage


Cette [section](forum/support-bug-reports) dans le forum vous permettra d'obtenir de l'aide.
