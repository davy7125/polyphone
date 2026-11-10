Sigue estas instrucciones para saber cómo compilar Polyphone con Qt Creator.
Se supone que ya tienes instalado en tu ordenador Qt Creator con el framework Qt, un compilador y las dependencias necesarias para compilar Polyphone (consulta las páginas correspondientes).


## Requisitos previos


Se necesitan los siguientes elementos:

* un compilador de C++ (GCC, Clang, MinGW…),
* Qt Creator con su framework (Qt 6, o Qt 5),
* :program:`CMake` (versión 3.16 o posterior), que se puede instalar con Qt Creator,
* las dependencias de Polyphone.

Las instrucciones para instalarlos dependen del sistema operativo que uses; consulta las páginas de documentación correspondientes.


## Compilar Polyphone


### Copiar el código fuente


Obtén el código fuente de Polyphone <a href="software" target="_blank">aquí</a> o desde <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


### Abrir y configurar el proyecto


Inicia Qt Creator y abre el archivo :file:`CMakeLists.txt` situado en el directorio :file:`sources`.


![Abrir Qt Creator](images/open-Qt-Creator.png "Abrir Qt Creator")


Después, configura el proyecto eligiendo un kit, es decir, una versión de Qt y un compilador.

![Configurar el proyecto](images/configure-project.png "Configurar el proyecto")


Qt Creator ejecuta entonces :program:`CMake`, que comprueba que se encuentran las dependencias.
Los mensajes se muestran en el panel :guilabel:`General Messages`: si falta una biblioteca, instálala y vuelve a ejecutar :program:`CMake` con la opción :guilabel:`Run CMake` del menú :guilabel:`Build`.

Las opciones del proyecto están en la sección :guilabel:`Projects` de la columna izquierda, en :guilabel:`Build` → :guilabel:`CMake`.
En Linux, se pueden usar las copias de :program:`RtAudio` y :program:`RtMidi` incluidas en el código fuente en lugar de las bibliotecas del sistema: activa las opciones ``POLYPHONE_LOCAL_RTAUDIO`` y ``POLYPHONE_LOCAL_RTMIDI`` y luego haz clic en :guilabel:`Run CMake`.
Esto no es necesario en Windows ni en macOS, ya que siempre se usan las versiones distribuidas.
En la misma sección, selecciona la configuración de compilación (es preferible :guilabel:`Release`).


### Compilar el proyecto


Por último, haz clic en la acción :guilabel:`Build Project "Polyphone"` del menú :guilabel:`Build`. Durante la compilación del proyecto pueden aparecer advertencias, pero no debería haber errores.

Ahora deberías tener el ejecutable en el subdirectorio :file:`bin` del directorio de compilación creado por Qt Creator (:file:`build/Desktop_Qt_6_…-Release` en el directorio :file:`sources`, por ejemplo):

* en Linux, el archivo :file:`polyphone`,
* en Windows, el archivo :file:`polyphone.exe`,
* en macOS, el paquete :file:`Polyphone.app`.


## Solución de problemas


Consulta esta [categoría](forum/support-bug-reports) si necesitas más información o ayuda.


<p class="endpage">Gracias especialmente a <a href="dashboard/820-tomotello">Tomotello</a> por este artículo.</p>