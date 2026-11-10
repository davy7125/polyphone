Sigue estas instrucciones si no puedes instalar Polyphone en tu distribución Linux con los instaladores disponibles en la sección «[Software](software)».
Este es el método que se usa para compilar las versiones Linux de Polyphone, en Ubuntu 24.04.


## Requisitos previos


Se necesitan las siguientes herramientas y bibliotecas:

* un compilador (``build-essential``)
* cmake (``cmake``) y pkg-config (``pkgconf``)
* qt 6 (``qt6-base-dev``, ``qt6-svg-dev``, ``qt6-tools-dev``, ``qt6-tools-dev-tools`` y ``qt6-l10n-tools``)
* alsa (``libasound2-dev``)
* jack (``libjack-jackd2-dev``)
* rtaudio (``librtaudio-dev``)
* rtmidi (``librtmidi-dev``)
* vorbis (``libvorbis-dev``)
* ogg (``libogg-dev``)
* flac (``libflac-dev``)
* zlib (``zlib1g-dev``)
* ssl (``libssl-dev``)
* sndfile (``libsndfile1-dev``)

Se pueden instalar con el siguiente comando:

```
sudo apt-get install build-essential cmake pkgconf qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools libasound2-dev libjack-jackd2-dev librtaudio-dev librtmidi-dev libvorbis-dev libogg-dev libflac-dev zlib1g-dev libssl-dev libsndfile1-dev
```

También necesitas el código fuente de Polyphone, disponible <a href="software" target="_blank">aquí</a> o en <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


## Compilación


Polyphone se compila con :program:`CMake`, y el proyecto se describe en el archivo :file:`CMakeLists.txt` del directorio :file:`sources`.
En la raíz del proyecto (el directorio que contiene :file:`sources`), abre un terminal y ejecuta los siguientes comandos:

```
cmake -S sources -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build --parallel 4
```

Si todo ha ido bien, debería aparecer un archivo ejecutable :file:`polyphone` en el directorio :file:`build/bin`.
Se puede instalar en el sistema con el comando :command:`sudo cmake --install build`.

Si en tu distribución faltan bibliotecas como :program:`RtAudio` o :program:`RtMidi`, o no están en una versión compatible, puedes usar en su lugar las copias incluidas en el código fuente de Polyphone, añadiendo estas opciones al primer comando:

```
-DPOLYPHONE_LOCAL_RTAUDIO=ON -DPOLYPHONE_LOCAL_RTMIDI=ON
```

Así ya no hacen falta los paquetes ``librtaudio-dev`` y ``librtmidi-dev``.

Se pueden añadir otras opciones al primer comando:

-DPOLYPHONE_QT_VERSION=5
: Compilar con Qt 5 en lugar de Qt 6 (por defecto, se usa la versión más reciente que se encuentre).
  Los paquetes de Qt 5 son entonces ``qtbase5-dev``, ``libqt5svg5-dev``, ``qttools5-dev`` y ``qttools5-dev-tools``.

-DPOLYPHONE_BUILD_PLUGIN=ON
: Compilar también el [plugin](development/building-the-polyphone-plugin.md).

-DPOLYPHONE_WOLFSSL=ON
: Usar :program:`wolfSSL` en lugar de :program:`OpenSSL`.

**Nota:** si usas :program:`Qt Creator`, el proyecto se abre mediante el archivo :file:`CMakeLists.txt`, situado en el directorio :file:`sources` (ver [Usar Qt Creator para compilar Polyphone](development/using-qt-creator-to-build-polyphone.md)).
