Sigue estas instrucciones si no puedes instalar Polyphone en tu distribución Linux con los instaladores disponibles en la sección «[Software](software)».
Este es el método que se usa para compilar las versiones Linux de Polyphone, en Ubuntu 24.04.


## Requisitos previos


Se necesitan las siguientes herramientas y bibliotecas:

* un compilador (``build-essential``)
* qt 6 (``qt6-base-dev``, ``qt6-svg-dev``, ``qt6-tools-dev``, ``qt6-tools-dev-tools`` y ``qt6-l10n-tools``)
* alsa (``libasound2-dev``)
* jack (``libjack-jackd2-dev``)
* rtaudio (``librtaudio-dev``)
* rtmidi (``librtmidi-dev``)
* stk (``libstk-dev``)
* vorbis (``libvorbis-dev``)
* ogg (``libogg-dev``)
* flac (``libflac-dev``)
* zlib (``zlib1g-dev``)
* ssl (``libssl-dev``)
* sndfile (``libsndfile1-dev``)

Se pueden instalar con el siguiente comando:

```
sudo apt-get install build-essential qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools libasound2-dev libjack-jackd2-dev librtaudio-dev librtmidi-dev libstk-dev libvorbis-dev libogg-dev libflac-dev zlib1g-dev libssl-dev libsndfile1-dev
```

También necesitas el código fuente de Polyphone, disponible <a href="software" target="_blank">aquí</a> o en <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


## Compilación


En el directorio :file:`sources` del proyecto (donde se encuentra el archivo :file:`polyphone.pro`), abre un terminal y compila Polyphone ejecutando el siguiente comando:

```
qmake6 polyphone.pro PREFIX=/usr && make
```

Si todo ha ido bien, debería aparecer un archivo ejecutable :file:`polyphone` en el directorio :file:`bin`.
Se puede instalar en el sistema con el comando :command:`sudo make install`.

Por defecto, se usa una copia local de la biblioteca :program:`RtAudio`.
Si en tu distribución faltan bibliotecas como :program:`RtMidi` o :program:`Stk`, o no están en una versión compatible, puedes editar el archivo :file:`.pro` para usar copias locales de ellas.
Para ello, quita el comentario de las líneas correspondientes. Por ejemplo,

```
#DEFINES += USE_LOCAL_RTMIDI
```

pasará a ser

```
DEFINES += USE_LOCAL_RTMIDI
```

(sin el «#» inicial).

**Nota:** si usas :program:`Qt Creator`, el proyecto se puede abrir mediante su archivo :file:`.pro`, situado en el directorio :file:`sources`.
