Sigue estas instrucciones si no puedes instalar Polyphone en tu Mac con los instaladores disponibles en la sección «[Software](software)».


## Requisitos previos


Se necesitan las siguientes aplicaciones:

* <a href="https://www.qt.io/download-open-source" target="_blank">Qt Creator con su framework</a> (versión 6),
* Xcode o solo las "Command Line Tools" (la versión que hay que usar se indica en el sitio web de Qt, <a href="https://doc.qt.io/qt-6/supported-platforms.html" target="_blank">aquí</a> por ejemplo).

También necesitas el código fuente de Polyphone, disponible <a href="software" target="_blank">aquí</a> o en <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.

Para abrir correctamente un proyecto con Qt Creator, primero hay que darle permiso para leer todos los archivos del ordenador (en «Privacidad y seguridad» => «Acceso total al disco» => añadir «Qt Creator.app», que está en su directorio de instalación).

Las bibliotecas que usa Polyphone se instalan fácilmente con <a href="https://brew.sh/" target="_blank">Homebrew</a>. Después de instalar esta herramienta, ejecuta este comando:

```
brew install libsndfile libogg libvorbis flac lame mpg123 opus openssl
```

El archivo de proyecto :file:`polyphone.pro` busca los encabezados y las bibliotecas en :file:`/opt/homebrew` (Apple Silicon), en :file:`/usr/local` (Intel) y en el directorio :file:`macos`, situado junto al directorio :file:`sources`.
También se puede descomprimir el archivo de bibliotecas estáticas disponible [aquí](files/macos.zip) para obtener el directorio :file:`macos`, pero su contenido puede estar desactualizado.

Si algunas bibliotecas no están actualizadas o no son compatibles con la versión de tu Mac, como SSL por ejemplo, habrá que buscar su código fuente en internet y volver a compilarlas activando el enlace estático:

```
configure --enable-static --disable-shared --prefix=...
make
make install
```

Después, copia todos los .h (encabezados) en :file:`macos/include` y todos los .a (bibliotecas estáticas) en :file:`macos/lib`.


## Compilación


Abre el archivo :file:`polyphone.pro` con :program:`Qt Creator` (ver [Usar Qt Creator para compilar Polyphone](development/using-qt-creator-to-build-polyphone.md)) y compila el proyecto.
Debería aparecer un paquete :file:`Polyphone.app` en el directorio :file:`macos`, situado junto al directorio :file:`sources`.

La compilación también se puede hacer desde la línea de comandos:

```
cd sources
qmake polyphone.pro CONFIG+=release
make -j4
```

Si el compilador se queja de que algunas funciones no están disponibles con la versión de macOS de destino indicada, puedes aumentar esta variable en el archivo :file:`polyphone.pro` (el valor por defecto es 12.0):

```
QMAKE_MACOSX_DEPLOYMENT_TARGET = 12.0
```

Este valor también hay que escribirlo en el archivo :file:`packaging/mac/polyphone.plist`, debajo de «LSMinimumSystemVersion».


## Completar el paquete


Ejecuta el siguiente comando para incluir las bibliotecas y los frameworks dentro del paquete (¡adapta antes la ruta!):

```
/Path/To/Qt/6.7.3/macos/bin/macdeployqt /Path/To/macos/Polyphone.app
```

En un Mac con procesador Apple Silicon, el paquete también debe firmarse (aunque sea con una firma ad hoc) para poder ejecutarse:

```
xattr -cr /Path/To/macos/Polyphone.app
codesign --force --deep --sign - /Path/To/macos/Polyphone.app
```

Después puedes ejecutar el programa o comprimirlo en un archivo .zip o .dmg para compartirlo.


## Solución de problemas


Consulta este [tema](forum/support-bug-reports/8-success-build-polyphone-on-osx-10-11-6-qt-5-7) si necesitas más información o ayuda.
