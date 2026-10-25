Sigue estas instrucciones para saber cómo compilar Polyphone con Qt Creator.
Se supone que ya tienes instalado en tu ordenador Qt Creator con el framework Qt, un compilador y las dependencias necesarias para compilar Polyphone (consulta las páginas correspondientes).


## Requisitos previos


Se necesitan los siguientes elementos:

* un compilador de C++ (VC++, Clang, GCC, …),
* Qt Creator con su framework (Qt 6),
* las dependencias de Polyphone.

Las instrucciones para instalarlos dependen del sistema operativo que uses; consulta las páginas de documentación correspondientes.


## Compilar Polyphone


### Copiar el código fuente


Obtén el código fuente de Polyphone <a href="software" target="_blank">aquí</a> o desde <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


### Abrir y configurar el proyecto


Primero, abre el archivo :file:`polyphone.pro` y edítalo para usar, si es necesario, las versiones distribuidas de :program:`rtmidi` y de :program:`stk` si tu sistema operativo no las proporciona (la versión distribuida de :program:`rtaudio` se usa por defecto).
Quita el comentario de las siguientes líneas eliminando el ‘#’ inicial:

```
DEFINES += USE_LOCAL_RTMIDI
DEFINES += USE_LOCAL_STK
```

Puedes saltarte estas modificaciones si quieres usar las bibliotecas ya instaladas en tu ordenador (¡asegúrate de tenerlas!).
Este paso no es necesario en Windows ni en macOS, ya que siempre se usan las versiones distribuidas.

Inicia Qt Creator y abre el archivo :file:`polyphone.pro`.


![Abrir Qt Creator](images/open-Qt-Creator.png "Abrir Qt Creator")


Después, configura el proyecto.

![Configurar el proyecto](images/configure-project.png "Configurar el proyecto")


### Ejecutar qmake y compilar el proyecto


Ve al menú :guilabel:`Build` y elige la opción :guilabel:`Run qmake`.


![Ejecutar qmake](images/run-qmake.png "Ejecutar qmake")


La ejecución de :command:`qmake` termina cuando la barra de progreso de la esquina inferior derecha llega al 100 % en verde.


![qmake terminado](images/qmake-finished.png "qmake terminado")


En la sección :guilabel:`Project` de la columna izquierda y luego en la sección :guilabel:`Build&Run`, selecciona la configuración de compilación (es preferible :guilabel:`Release`).

Por último, haz clic en la acción :guilabel:`Build project Polyphone` del menú :guilabel:`Build`. Durante la compilación del proyecto pueden aparecer advertencias, pero no debería haber errores.

Ahora deberías tener el ejecutable en el directorio de compilación creado por Qt Creator (:file:`build-XXX` junto al código fuente, por ejemplo), según lo que hayas elegido en la configuración de compilación de Qt Creator:

* en Linux, el archivo :file:`polyphone` está en el subdirectorio :file:`bin`,
* en Windows, el archivo :file:`polyphone.exe` está en el subdirectorio :file:`release` o :file:`debug`,
* en macOS, el paquete :file:`Polyphone.app` está en el directorio :file:`macos`, situado junto al directorio :file:`sources`.


## Solución de problemas


Consulta esta [categoría](forum/support-bug-reports) si necesitas más información o ayuda.


<p class="endpage">Gracias especialmente a <a href="dashboard/820-tomotello">Tomotello</a> por este artículo.</p>