Sigue estas instrucciones si no puedes instalar Polyphone en tu distribución Fedora con los instaladores disponibles en la sección «[Software](software)».


## Requisitos previos


### Instalación de Qt

Tras una instalación básica de Fedora, instala los siguientes paquetes (incluido un compilador), necesarios para desarrollar con Qt:

```
sudo dnf update
sudo dnf install @development-tools
```

Después, instala el framework Qt 6 con el siguiente comando:

```
sudo dnf install qt-creator qt6-qtbase-devel qt6-qtsvg-devel qt6-qttools-devel
```


### Dependencias de Polyphone

Instala las siguientes dependencias (si tienes un gestor de paquetes gráfico instalado, también puedes usarlo para la instalación):

```
sudo dnf install alsa-lib-devel jack-audio-connection-kit-devel rtaudio-devel zlib-devel libogg-devel flac-devel libvorbis-devel glib2-devel openssl-devel rtmidi-devel stk-devel libsndfile-devel
```

**Nota:** en las versiones recientes de Fedora, el paquete :program:`jack-audio-connection-kit-devel` puede sustituirse por :program:`pipewire-jack-audio-connection-kit-devel`.


## Compilación


### Código fuente


Primero, obtén el código fuente de Polyphone <a href="software" target="_blank">aquí</a> o desde <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.

Si quieres continuar la compilación con Qt Creator, sigue ahora este tutorial: [compilar con Qt Creator](development/using-qt-creator-to-build-polyphone.md). Si no, sigue leyendo este documento.


### Obtener el ejecutable


Ve al directorio del código fuente (donde se encuentra el archivo :file:`polyphone.pro`), abre un terminal y compila Polyphone con este comando:

```
qmake6 && make
```

Si todo va bien, tendrás el archivo compilado :file:`polyphone` en el subdirectorio :file:`bin`.
Si la compilación se hace con Qt Creator, este archivo se encuentra en cambio en el directorio :file:`build-XXX` creado junto a :file:`sources`.


## Solución de problemas


Esta [categoría](forum/support-bug-reports) del foro te ayudará si lo necesitas.
