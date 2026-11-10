Sigue estas instrucciones si no puedes instalar Polyphone en tu distribución Fedora con los instaladores disponibles en la sección «[Software](software)».


## Requisitos previos


### Instalación de Qt

Tras una instalación básica de Fedora, instala los siguientes paquetes (incluidos un compilador y :program:`CMake`), necesarios para desarrollar con Qt:

```
sudo dnf update
sudo dnf install @development-tools cmake pkgconf
```

Después, instala el framework Qt 6 con el siguiente comando:

```
sudo dnf install qt-creator qt6-qtbase-devel qt6-qtsvg-devel qt6-qttools-devel
```


### Dependencias de Polyphone

Instala las siguientes dependencias (si tienes un gestor de paquetes gráfico instalado, también puedes usarlo para la instalación):

```
sudo dnf install alsa-lib-devel jack-audio-connection-kit-devel rtaudio-devel zlib-devel libogg-devel flac-devel libvorbis-devel glib2-devel openssl-devel rtmidi-devel libsndfile-devel
```

**Nota:** en las versiones recientes de Fedora, el paquete :program:`jack-audio-connection-kit-devel` puede sustituirse por :program:`pipewire-jack-audio-connection-kit-devel`.


## Compilación


### Código fuente


Primero, obtén el código fuente de Polyphone <a href="software" target="_blank">aquí</a> o desde <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.

Si quieres continuar la compilación con Qt Creator, sigue ahora este tutorial: [compilar con Qt Creator](development/using-qt-creator-to-build-polyphone.md). Si no, sigue leyendo este documento.


### Obtener el ejecutable


Polyphone se compila con :program:`CMake`, y el proyecto se describe en el archivo :file:`CMakeLists.txt` del directorio :file:`sources`.
En la raíz del proyecto (el directorio que contiene :file:`sources`), abre un terminal y compila Polyphone con estos comandos:

```
cmake -S sources -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 4
```

Si todo va bien, tendrás el archivo compilado :file:`polyphone` en el directorio :file:`build/bin`.
Si la compilación se hace con Qt Creator, este archivo se encuentra en cambio en el subdirectorio :file:`bin` del directorio de compilación elegido en Qt Creator.

Si :program:`RtAudio` o :program:`RtMidi` no están disponibles en una versión compatible, se pueden usar las copias incluidas en el código fuente de Polyphone añadiendo las opciones ``-DPOLYPHONE_LOCAL_RTAUDIO=ON -DPOLYPHONE_LOCAL_RTMIDI=ON`` al primer comando.
Las demás opciones se describen en la página [Compilar Polyphone para Ubuntu](development/building-polyphone-for-ubuntu.md).


## Solución de problemas


Esta [categoría](forum/support-bug-reports) del foro te ayudará si lo necesitas.
