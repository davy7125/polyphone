Para crear un [soundfont](manual/annexes/the-different-soundfont-formats.md#doc_sf2), haz clic en :guilabel:`Nuevo soundfont` en la [pantalla de inicio](manual/index.md#doc_left).
Los soundfonts tienen una estructura de 3 niveles, por lo que la creación de un nuevo soundfont consta de tres etapas fundamentales:

* la [preparación de las muestras](#doc_sample),
* la [creación de los instrumentos](#doc_instrument), compuestos de muestras,
* la [creación de los conjuntos](#doc_preset), compuestos de instrumentos.


## Preparación de las muestras {#doc_sample}


### Carga de las muestras


La preparación de las muestras empieza por la **carga** de archivos de audio (.wav, por ejemplo).
Para ello, haz clic en :guilabel:`Agregar una muestra` en la [barra de herramientas](manual/soundfont-editor/toolbar.md#doc_edit) y selecciona uno o varios archivos de audio (los archivos también se pueden arrastrar y soltar en el [árbol](manual/soundfont-editor/tree.md#doc_dragdrop)).
Las muestras se pueden obtener:

* por internet,
* grabando un instrumento musical real,
* por síntesis, con un programa especializado.


### Bucle de las muestras


A continuación, puede hacer falta un **bucle** en la muestra si quieres que suene más tiempo que su duración normal.
Un ejemplo sería tener solo una muestra de flauta de 1 segundo y querer que suene indefinidamente.
Esto se consigue asignando, de forma manual o automática, puntos de bucle (inicio + fin) dentro de los límites de la muestra, de modo que la zona del bucle se pueda repetir y la muestra suene más tiempo.
Aunque puedes asignar los puntos de bucle manualmente, usar la función «[Bucle automático](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop)» para asignarlos automáticamente suele dar mejores resultados y es mucho más rápido.

Para asignar los puntos de bucle manualmente:

1. haz clic en el [árbol](manual/soundfont-editor/tree.md) en la muestra a la que quieres poner un bucle,
2. en el [editor de muestras](manual/soundfont-editor/editing-pages/sample-editor.md), coloca de forma aproximada el inicio y el fin del bucle en el [gráfico](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph) (zona de visualización del WAV), salvo que ya haya un bucle definido.
  Usa el clic izquierdo para el inicio y el clic derecho para el fin.
  El punto derecho debe colocarse primero (ya que el punto derecho está por defecto en la posición 0 y no se puede colocar un punto izquierdo después del punto derecho).
3. pulsa el botón :guilabel:`Reproducir` después de activar la opción :guilabel:`bucle`,
4. ajusta el inicio o el fin del bucle, o ambos, mientras se reproduce la muestra, hasta que la transición entre las dos posiciones sea lo más suave posible.


**Nota:** para oír el bucle en un instrumento, hay que seleccionar ![bucle activado](images/loop_on.png "bucle activado"){.inline} en la fila del parámetro :guilabel:`Reproducción en bucle`, en la columna :guilabel:`Global` o en las columnas de las notas individuales de la [tabla de parámetros](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table). ![bucle activado](images/loop_on.png "bucle activado"){.inline} lo activa; ![bucle desactivado](images/loop_off.png "bucle desactivado"){.inline} o una celda vacía lo desactivan.


![Ilustración del bucle](images/loop_illustration.png "Ilustración del bucle")


### Afinación de las muestras


Por último, hay que hacer la **afinación**.
Para ello, hay una herramienta de calibración (seno) disponible en la página de edición de las [muestras](manual/soundfont-editor/editing-pages/sample-editor.md#doc_player).
Para cada muestra, el método es el siguiente:

1. inicia la reproducción (pulsa el botón :guilabel:`Reproducir`), si es posible con la opción :guilabel:`bucle` activada,
2. activa la opción :guilabel:`seno`,
3. ajusta el deslizador de volumen para oír los dos sonidos lo más claramente posible,
4. cambia la :guilabel:`Nota raíz` hasta que los dos sonidos se parezcan lo más posible,
5. ajusta la :guilabel:`Corrección` (en centésimas) para afinar la muestra en la nota raíz.
   Para ello, presta atención a los batimientos que aparezcan y haz que sean lo más lentos posible.

El análisis de [frecuencias](manual/soundfont-editor/editing-pages/sample-editor.md#doc_freq) puede ser un buen indicador para empezar la afinación.


### Para ir más allá


Hay varias herramientas disponibles para editar las muestras, como:

* [eliminar el silencio](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank) al inicio,
* [ecualizar](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq) el sonido,
* [transportar](manual/soundfont-editor/tools/sample-tools.md#doc_transpose),
* [normalizar](manual/soundfont-editor/tools/sample-tools.md#doc_volume) el volumen,
* [ajustar el balance](manual/soundfont-editor/tools/sample-tools.md#doc_balance) de las muestras estéreo.


## Creación de los instrumentos {#doc_instrument}


En esta etapa, se supone que hay una o varias muestras disponibles para crear un instrumento.


### Crear un instrumento y añadir muestras


Primero, haz clic en :guilabel:`Agregar un instrumento` en la [barra de herramientas](manual/soundfont-editor/toolbar.md#doc_edit).
Ahora hay que escribir un nombre (si hay muestras seleccionadas en el árbol, se pueden enlazar directamente al nuevo instrumento).

Si no, añade muestras al instrumento con un «[arrastrar y soltar](manual/soundfont-editor/tree.md#doc_dragdrop)».
En el árbol verás que aparecen divisiones en el instrumento.

**Nota:** cuando se crean las divisiones, las muestras no se copian, sino que se enlazan. No hay límite en el número de divisiones.


### Distribución de las divisiones


Cuando se añaden muestras a los instrumentos, aparecen como divisiones (columnas) en la [tabla del editor de instrumentos](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).
Después hay que colocar cada división en el teclado cambiando el :guilabel:`Rango de notas` en la tabla.
En general, es buena práctica que el rango de una división incluya la nota raíz de la muestra que representa.
Hay que cubrir toda la superficie del teclado (por ejemplo, de la nota 36 a la nota 96 para un teclado de sintetizador clásico).

La herramienta «[Posicionamiento automático de muestras](manual/soundfont-editor/tools/instrument-tools.md#doc_position)» distribuye automáticamente las muestras por el teclado.


### Configuración de las divisiones


Para activar el modo de reproducción en bucle en un instrumento que contiene muestras con bucle, selecciona ![bucle activado](images/loop_on.png "bucle activado"){.inline} en la fila del parámetro :guilabel:`Reproducción en bucle`, en la columna :guilabel:`Global` o en las columnas de las notas individuales de la tabla de parámetros.

* ![bucle activado](images/loop_on.png "bucle activado"){.inline} lo activa,
* ![bucle desactivado](images/loop_off.png "bucle desactivado"){.inline} o una celda vacía lo desactivan.
* ![bucle activado y fin](images/loop_on_end.png "bucle activado y fin"){.inline} activa el bucle, y el resto de la muestra se lee después de soltar la tecla.

La división global, como su nombre indica, permite introducir un parámetro para todo el instrumento.
Un parámetro global solo se aplica a las divisiones en las que no está indicado el mismo parámetro.
Esto significa que los parámetros de las divisiones normales tienen prioridad sobre los ajustes de la división global.

En el marco de este sencillo tutorial, no hace falta editar ningún otro parámetro para que el instrumento funcione.
Ya debería poder tocarse con el [teclado virtual](manual/soundfont-editor/toolbar.md#doc_keyboard).

Sin embargo, para mejorar el instrumento, conviene introducir un valor en la fila :guilabel:`Vol env liberación (s)`, lo que evita que el sonido se corte bruscamente en cuanto se suelta una tecla.
Los demás parámetros se explican en la descripción de las [tablas](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).


## Creación de los conjuntos {#doc_preset}


En esta etapa, se supone que hay uno o varios instrumentos disponibles para crear un conjunto.


### ¿Qué es un conjunto?


Un conjunto es el equivalente de un nombre de sonido (patch) de los sintetizadores físicos.
Es visible desde fuera de un soundfont y se identifica con un número de banco y un número de conjunto.
Un soundfont puede tener uno o muchos conjuntos.
Cada conjunto contiene uno o varios instrumentos, igual que los instrumentos contienen una o varias muestras.


### Crear un conjunto y añadir instrumentos


Primero, haz clic en :guilabel:`Agregar un conjunto` en la [barra de herramientas](manual/soundfont-editor/toolbar.md#doc_edit).
Ahora hay que escribir un nombre (si hay instrumentos seleccionados en el árbol, se pueden enlazar directamente al nuevo conjunto).

Si no, añade instrumentos al conjunto con un «[arrastrar y soltar](manual/soundfont-editor/tree.md#doc_dragdrop)».
En el árbol verás que aparecen una o varias divisiones en el conjunto.
A menudo solo habrá una, como en el caso de un conjunto que contiene un único instrumento.

Se pueden crear tantos conjuntos como instrumentos (o incluso más), y cada conjunto puede contener uno o varios instrumentos.


### Configurar un conjunto


Puede que no haga falta cambiar los parámetros del conjunto, ya que:

* al crear un conjunto, el número de banco y el número de conjunto se asignan automáticamente,
* al añadir un instrumento a un conjunto, el rango de notas se calcula automáticamente según el instrumento.

En el marco de este tutorial, ¡la creación del soundfont ha terminado!
