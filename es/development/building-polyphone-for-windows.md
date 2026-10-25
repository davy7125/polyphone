Sigue estas instrucciones para compilar Polyphone para Windows con la última versión del código fuente disponible en <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>. Tendrás el programa en su estado de desarrollo más avanzado, con las funciones más recientes, pero quizá también con alguna inestabilidad.


## Requisitos previos


### Instalación de Qt

Polyphone está escrito en C++ con el entorno de desarrollo Qt (versión 6). Para instalar Qt, ve a esta <a href="https://www.qt.io/download-qt-installer-oss" target="_blank">página</a> y descarga el instalador. Primero tendrás que crear una cuenta.

Después, ejecuta el instalador y selecciona los siguientes componentes:

![Componentes que hay que seleccionar](images/select-qt-components.png "Componentes que hay que seleccionar")

Elige la última versión disponible si estas opciones no son exactamente las mismas.


### Instalación del compilador y de las dependencias

Qt necesita un compilador para compilar el código fuente de Polyphone. Hay que usar MinGW por los siguientes motivos:

* ante todo, por un motivo práctico, ya que las dependencias que se indican abajo son compatibles con MinGW,
* pero también para que la importación / exportación de archivos en formato sfArk funcione correctamente (se necesita la opción de compilación ``-ffloat-store``).

El compilador y las bibliotecas que usa Polyphone se instalan con <a href="https://www.msys2.org/" target="_blank">MSYS2</a>.
Instálalo en el directorio por defecto :file:`C:/msys64` (el archivo de proyecto :file:`polyphone.pro` busca las bibliotecas en :file:`C:/msys64/mingw64`), abre el terminal :program:`MSYS2 MINGW64` y ejecuta este comando:

```
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-zlib mingw-w64-x86_64-libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-openssl mingw-w64-x86_64-flac mingw-w64-x86_64-libsndfile
```

Después, ve a la configuración del sistema de Windows para añadir la carpeta :file:`C:/msys64/mingw64/bin` al Path:

* abre el menú de Windows,
* escribe «Variables de entorno»,
* abre las propiedades del sistema y haz clic en «Variables de entorno»,
* haz doble clic en «Path» en las variables del sistema,
* y añade :file:`C:/msys64/mingw64/bin` a los directorios de la variable «Path».

Así, Qt podrá encontrar los ejecutables y las bibliotecas de MinGW.


## Compilación


### Código fuente

Empieza por descargar el código fuente de Polyphone <a href="software" target="_blank">aquí</a> o desde <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> y descomprime los archivos en tu escritorio de Windows, por ejemplo.

### Compilar con Qt Creator

Continúa ahora con este tutorial para compilar Polyphone con Qt Creator: [Usar Qt Creator para compilar Polyphone](development/using-qt-creator-to-build-polyphone.md). No será necesario modificar el archivo :file:`polyphone.pro`.

### Compilar desde la línea de comandos

Abre el terminal :program:`MSYS2 MINGW64`, añade Qt al Path y compila el proyecto en un nuevo directorio (adapta antes las rutas):

```
export PATH="/c/Qt/6.6.2/mingw_64/bin:$PATH"
mkdir build
cd build
qmake ../sources/polyphone.pro CONFIG+=release
mingw32-make -j4
```

Si la compilación ha ido bien, deberías ver el archivo ejecutable :file:`polyphone.exe` en el directorio :file:`build/release`.


### Iniciar Polyphone

Para que el ejecutable pueda funcionar de forma independiente, hay que completar su carpeta con las bibliotecas de Qt.
Ejecuta el siguiente comando en un terminal (adapta antes las rutas):

```
C:\Qt\6.6.2\mingw_64\bin\windeployqt.exe "C:\Users\User\Desktop\build\release\polyphone.exe"
```

Aparecerán las bibliotecas de Qt (:file:`Qt6Core`, por ejemplo), y un doble clic en el archivo :file:`polyphone.exe` iniciará el programa.
Para que Polyphone se pueda iniciar en otro ordenador con Windows, también hacen falta estos archivos de :file:`C:\msys64\mingw64\bin` para completar la carpeta:

* libgcc_s_seh-1.dll
* libstdc++-6.dll
* libwinpthread-1.dll
* libssp-0.dll
* libsndfile-1.dll
* libFLAC.dll
* libogg-0.dll
* libvorbis-0.dll
* libvorbisenc-2.dll
* libvorbisfile-3.dll
* libopus-0.dll
* libmp3lame-0.dll
* libmpg123-0.dll
* libcrypto-3-x64.dll
* libssl-3-x64.dll
* zlib1.dll


## Solución de problemas


Esta [categoría](forum/support-bug-reports) del foro te ayudará si lo necesitas.
