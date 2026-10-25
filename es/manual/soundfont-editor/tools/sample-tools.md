Cuando hay una o varias muestras seleccionadas en el [árbol](manual/soundfont-editor/tree.md), el [menú de herramientas](manual/soundfont-editor/tools/index.md) contiene varias herramientas ordenadas en 5 categorías:

* [procesamiento de audio](#doc_cat_processing),
* [edición rápida](#doc_cat_fast_editing),
* [archivos](#doc_cat_files),
* [bucle / reproducción](#doc_cat_loop_playback),
* [muestras estéreo](#doc_cat_stereo).

Se pueden usar las herramientas en varias muestras a la vez (con una selección múltiple).


## Procesamiento de audio {#doc_cat_processing}


### Cambiar volumen {#doc_volume}

Puedes cambiar el volumen de la muestra de tres formas distintas:

* sumando una cierta cantidad de dB (positiva o negativa),
* multiplicando la señal por un número,
* normalizando la señal para que su máximo represente un cierto porcentaje de la intensidad máxima.

![Herramienta de cambio de volumen](images/tool_change_volume.png "Herramienta de cambio de volumen")


### Comando externo {#doc_externalcommand}

Esta herramienta permite usar un comando o un programa externo para editar una muestra.
Ya no hace falta exportar y volver a importar una muestra en Polyphone: todo se hace automáticamente.

1. descarga un programa de edición de archivos .wav, como :program:`Audacity` o :program:`Sweep` (Linux),
2. usa la herramienta :guilabel:`Comando externo` en una o varias muestras,
3. escribe la línea de comandos en el campo :guilabel:`Comando` para llamar al programa externo (puede que tengas que leer la documentación de ese programa); el argumento :command:`{wav}` es obligatorio, ya que se sustituye por la ruta del archivo exportado (por ejemplo :command:`sweep {wav}`),
4. edita las muestras con la herramienta externa,
5. guarda la versión modificada de los archivos wav con la herramienta externa y ciérrala,
6. las muestras se cargarán automáticamente en el soundfont.

El botón situado a la derecha del campo ayuda a seleccionar un archivo ejecutable, y la lista :guilabel:`Historial` recuerda los últimos comandos usados.
Hay dos opciones disponibles:

Permitir la edición estéreo
: Cuando se procesa una muestra estéreo, se exporta con su muestra enlazada en un único archivo estéreo, y las dos muestras se actualizan al volver a importar el archivo.

Reemplazar información de la muestra si es posible
: El bucle, la nota raíz y la corrección guardados en el archivo devuelto por el programa externo reemplazan los de la muestra.
  En caso contrario, se conserva la información existente.

![Herramienta de comando externo](images/tool_external_command.png "Herramienta de comando externo")

**Nota:** durante el proceso, Polyphone exporta un archivo .wav y vuelve a importar ese mismo archivo después de ejecutar el comando externo. Si el comando externo crea otros archivos o guarda el resultado en otro archivo, esos archivos se ignorarán y Polyphone volverá a importar el archivo exportado inicialmente, que probablemente no se habrá modificado.


### Filtrar frecuencias {#doc_filter}

Al usar esta herramienta, se muestra una ventana con las frecuencias contenidas en las muestras seleccionadas.
Entonces puedes usar el ratón para dibujar una zona roja.
Todo lo que esté en esa zona se eliminará: esto significa que puedes reducir o eliminar manualmente algunas frecuencias.

![Herramienta de filtrado de frecuencias](images/tool_filter_frequencies.png "Herramienta de filtrado de frecuencias")

**Nota:** este filtro no es habitual, ya que corta las frecuencias de forma abrupta.
Esta herramienta puede ser útil para diseñar un filtro de tipo «brick-wall», pero en la mayoría de los casos probablemente querrás usar el [ecualizador](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq).


### Transportar {#doc_transpose}

La herramienta pide un desplazamiento en semitonos y luego remuestrea el sonido para cambiar la altura.

![Herramienta de transporte](images/tool_transpose_smpl.png "Herramienta de transporte")


## Edición rápida {#doc_cat_fast_editing}


### Cargar un parámetro de los instrumentos {#doc_load_from_instruments}

Los parámetros a nivel de instrumento pueden sustituir o desplazar los parámetros definidos a nivel de muestra.
Con esta herramienta, se pueden corregir o completar los parámetros de las muestras con lo que está definido en los instrumentos que usan esas muestras:

* modificar la nota raíz de la muestra con la sustitución definida a nivel de instrumento,
* desplazar la nota raíz de la muestra con el parámetro «Afinación (semitonos)»,
* desplazar la corrección de tono de la muestra con el parámetro «Afinación (centésimas)»,
* definir el inicio y el fin del bucle de la muestra con los parámetros de desfase del bucle.

![Herramienta para cargar un parámetro de los instrumentos](images/tool_load_from_instruments.png "Herramienta para cargar un parámetro de los instrumentos")


### Desplazar un parámetro {#doc_shift_parameter}

Esta herramienta permite editar a la vez, sumando un valor o multiplicando por un valor, uno de estos parámetros en todas las muestras seleccionadas:

* nota raíz,
* corrección,
* inicio del bucle,
* fin del bucle.

![Herramienta de edición rápida](images/tool_shift_parameter.png "Herramienta de edición rápida")


## Archivos {#doc_cat_files}


### Exportación WAV {#doc_wavexport}

Tras seleccionar un directorio de destino, todas las muestras seleccionadas se exportarán como archivos .wav.


## Bucle / reproducción {#doc_cat_loop_playback}


### Bucle automático {#doc_autoloop}

El programa busca en la muestra dos posiciones alejadas entre sí y adecuadas para crear un bucle.
Si es necesario, se copia un breve fragmento de sonido con un fundido cruzado para que la transición en el punto de unión sea lo más suave posible.
Con una muestra estéreo, se crea el mismo bucle en los dos lados.

Si falla, aparece un mensaje de advertencia que indica que no se ha podido encontrar ningún bucle (probablemente la muestra es demasiado corta o demasiado turbulenta).

**Atención:** esta herramienta modifica la muestra original.
Las modificaciones se pueden deshacer con la función «[deshacer](manual/soundfont-editor/toolbar.md#doc_edit)», pero si el archivo se guarda y se cierra Polyphone, no hay forma de volver atrás.

**Nota:** por defecto, el bucle está desactivado en un instrumento.
Para oírlo, hay que seleccionar ![bucle activado](images/loop_on.png "bucle activado"){.inline} en la fila del parámetro :guilabel:`Reproducción en bucle`, en la columna :guilabel:`Global` o en las columnas de las notas individuales de la [tabla](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).
![bucle activado](images/loop_on.png "bucle activado"){.inline} lo activa, ![bucle desactivado](images/loop_off.png "bucle desactivado"){.inline} lo desactiva.
![bucle activado y fin](images/loop_on_end.png "bucle activado y fin"){.inline} activa el bucle y la muestra se reproduce hasta el final después de soltar la tecla.


### Bucle de fundido cruzado {#doc_crossfadeloop}

La herramienta de fundido cruzado puede verse como la versión manual de la [herramienta de bucle automático](#doc_autoloop). Primero hay que colocar el inicio y el fin del bucle en el [gráfico](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph) alrededor de una zona estable. Después, esta herramienta crea un bucle a partir de esas posiciones aplicando un fundido cruzado cuya duración (en milisegundos) se puede configurar. Todo es, por tanto, configurable, pero hay que tener cuidado al buscar el inicio y el fin del bucle:

* en las dos posiciones, los armónicos del sonido deben coincidir para no oír un efecto de disco de vinilo rayado durante la transición,
* las fases de los distintos armónicos también deben coincidir para no oír una cancelación de frecuencias durante el fundido cruzado, que altera mucho el timbre de un instrumento,
* en la medida de lo posible, el fin del bucle debe estar lejos del inicio para evitar un sonido demasiado sintético.

Un truco es centrarse en las posiciones en las que la onda cruza el eje x (valor 0).

![Herramienta de bucle de fundido cruzado](images/tool_crossfade_loop.png "Herramienta de bucle de fundido cruzado")


### Eliminar silencio al inicio {#doc_removeblank}

Se elimina cualquier zona vacía antes del comienzo de la muestra, lo que reduce el tiempo hasta que se oye el sonido.
Con una muestra estéreo, se elimina la misma longitud en los dos lados.

![Eliminar silencio al inicio](images/remove_blank.png "Eliminar silencio al inicio")

**Nota:** esta herramienta se puede aplicar automáticamente cada vez que se importa una muestra; la opción está en las [preferencias](manual/settings.md#doc_general) del programa.


### Recortar hasta el final del bucle {#doc_trimloop}

Si hay un bucle definido para la muestra seleccionada, el programa corta el sonido después del final del bucle (dejando un pequeño margen al final).

![Recortar hasta el final del bucle](images/trim_to_loop.png "Recortar hasta el final del bucle")

**Nota:** esta herramienta se puede aplicar automáticamente cada vez que se importa una muestra, según las [preferencias](manual/settings.md#doc_general) del programa.


## Muestras estéreo {#doc_cat_stereo}


### Ajuste de balance {#doc_balance}

Si la muestra está enlazada a otra (estéreo), el programa puede igualar la intensidad media de las dos muestras para ajustar el balance.
Así resulta más fácil panoramizar con precisión una muestra en un [instrumento](manual/soundfont-editor/editing-pages/instrument-editor.md).
Esta herramienta no se puede usar con muestras mono.


### Encontrar un enlace {#doc_link}

Entre las muestras seleccionadas, las muestras cuyos nombres solo difieren en la última letra (L&nbsp;/&nbsp;R) se enlazan automáticamente (los espacios y los caracteres ``-``, ``_``, ``(``, ``)``, ``[``, ``]``, ``{``, ``}`` y ``|`` se ignoran en los nombres).
Por ejemplo, si en un soundfont dos muestras se llaman ``bassoon45L`` y ``bassoon45R``, ``bassoon45L`` se reconocerá como la parte izquierda de ``bassoon45R`` y ``bassoon45R`` se reconocerá como la parte derecha de ``bassoon45L``.

Con esta información, se usará el paneo correcto cuando se añadan estas muestras a un instrumento.
También puedes aprovechar la edición simultánea de las partes izquierda y derecha en la [tabla](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) del instrumento si la edición estéreo está activada en las [preferencias](manual/settings.md#doc_general).

**Atención:** esta herramienta puede romper enlaces existentes si los nombres no siguen la regla anterior.
Un mensaje enumera las muestras para las que no se ha encontrado ninguna correspondencia.


### Desenlazar {#doc_unlink}

Todas las muestras seleccionadas que están enlazadas (estéreo) se desenlazan.

Así, todas las muestras se tratarán como sonidos mono y tendrán todas el mismo paneo si se añaden a un instrumento.
La edición estéreo, si está activada en las [preferencias](manual/settings.md#doc_general), dejará de aplicarse.
