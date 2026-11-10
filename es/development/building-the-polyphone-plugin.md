Sigue estas instrucciones para compilar el [plugin](manual/plugin.md) de Polyphone (VST3, CLAP, LV2, y AU en macOS) a partir de su código fuente.
El plugin está hecho con <a href="https://juce.com" target="_blank">JUCE</a> y <a href="https://github.com/free-audio/clap-juce-extensions" target="_blank">clap-juce-extensions</a>, que se descargan automáticamente durante la configuración del proyecto: a diferencia del editor, no necesita Qt.


## Requisitos previos


Se necesitan los siguientes elementos:

* :program:`CMake` 3.22 o posterior y un compilador de C++17,
* la biblioteca :program:`libvorbisfile`, para leer los archivos sf3,
* la biblioteca :program:`libsndfile`, para leer las muestras de los archivos sfz y las ondas de los archivos dls.

También necesitas el código fuente de Polyphone, disponible <a href="software" target="_blank">aquí</a> o en <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


### Linux

También se necesitan los paquetes que requiere JUCE: ALSA, freetype, fontconfig y las bibliotecas X11.
En Ubuntu o Debian:

```
sudo apt-get install build-essential cmake pkgconf libvorbis-dev libsndfile1-dev libasound2-dev libfreetype-dev libfontconfig1-dev libx11-dev libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev libxi-dev libxrandr-dev libxrender-dev libgl-dev
```

En Fedora:

```
sudo dnf install @development-tools cmake pkgconf libvorbis-devel libsndfile-devel alsa-lib-devel freetype-devel fontconfig-devel libX11-devel libXcomposite-devel libXcursor-devel libXext-devel libXinerama-devel libXi-devel libXrandr-devel libXrender-devel mesa-libGL-devel
```


### Windows y macOS

Las bibliotecas se pueden descargar y compilar durante la configuración, añadiendo las opciones ``-DPOLYPHONE_FETCH_VORBIS=ON -DPOLYPHONE_FETCH_SNDFILE=ON`` al primer comando de más abajo.
Compilada de esta forma, :program:`libsndfile` no tiene ninguna biblioteca externa: las muestras de los archivos sfz se pueden leer en los formatos wav y aiff, pero no en flac, ogg u opus.

Las versiones oficiales usan en su lugar <a href="https://vcpkg.io" target="_blank">vcpkg</a>, que proporciona bibliotecas estáticas completas (se pueden leer flac, ogg, opus y mp3).
Las bibliotecas se instalan con:

```
vcpkg install --triplet <triplet> --overlay-triplets=packaging/vcpkg-triplets libsndfile libvorbis
```

y luego se añaden las siguientes opciones al primer comando de más abajo:

```
-DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=<triplet> -DVCPKG_OVERLAY_TRIPLETS=packaging/vcpkg-triplets -DVCPKG_MANIFEST_MODE=OFF
```

donde el triplete es ``x64-windows-polyphone``, ``arm64-osx-polyphone`` o ``x64-osx-polyphone``.
En Windows, el plugin se compila con :program:`clang-cl`, incluido en :program:`Visual Studio`: también hay que añadir la opción ``-T ClangCL``.


## Compilación


En la raíz del proyecto (el directorio que contiene :file:`sources`), abre un terminal y ejecuta los siguientes comandos:

```
cmake -S sources/plugin -B build/plugin -DCMAKE_BUILD_TYPE=Release
cmake --build build/plugin --config Release --parallel 4
```

Los plugins se encuentran entonces en el directorio :file:`build/plugin/PolyphonePlugin_artefacts/Release`: :file:`VST3/Polyphone.vst3`, :file:`CLAP/Polyphone.clap`, :file:`LV2/Polyphone.lv2`, y :file:`AU/Polyphone.component` en macOS.
Para instalarlos, cópialos en las carpetas de los anfitriones, como se describe en la página «[Plugin de Polyphone](manual/plugin.md#doc_install)».

El directorio :file:`Standalone` también contiene una aplicación independiente, :program:`Polyphone Player`, que toca los soundfonts sin anfitrión.

El plugin también se puede compilar junto con el editor, añadiendo la opción ``-DPOLYPHONE_BUILD_PLUGIN=ON`` a la [compilación de Polyphone](development/building-polyphone-for-ubuntu.md).


## Traducciones


Los textos del plugin están en los archivos de traducción de Polyphone (contextos :guilabel:`PluginEditor`, :guilabel:`PluginProcessor` y :guilabel:`PluginJuce`): se traducen al mismo tiempo que el editor (consulta [Traducir Polyphone](development/translate-polyphone.md)) y se integran en el plugin durante su compilación.


## Solución de problemas


Esta [categoría](forum/support-bug-reports) del foro te ayudará si lo necesitas.
