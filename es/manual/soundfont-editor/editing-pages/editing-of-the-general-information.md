La página de edición de la información general se muestra cuando se selecciona el encabezado :guilabel:`General` en el [árbol](manual/soundfont-editor/tree.md).
Esta página se compone de tres tipos de elementos:

* [campos editables](#doc_edit),
* [información general](#doc_info) no editable,
* [estadísticas](#doc_stats) sobre el soundfont.

El botón :guilabel:`Moduladores por defecto` de la [barra de herramientas](manual/soundfont-editor/toolbar.md#doc_view) muestra los [moduladores por defecto](#doc_default_mod) del soundfont, y el botón :guilabel:`General` vuelve a mostrar esta página.


![Página de información general](images/edit_general.png "Página de información general")


## Campos editables {#doc_edit}


La página de edición de la información general ofrece campos para modificar los siguientes atributos:

Título
: Es el nombre interno del soundfont, que no tiene por qué coincidir con el nombre del archivo.

Autor
: Corresponde al nombre del autor del soundfont.

Fecha
: Fecha de creación del archivo (un calendario ayuda a elegirla).

Producto
: Producto para el que está diseñado el soundfont.

Copyright
: Mención que se debe rellenar si el archivo está sujeto a derechos de autor.

Comentarios
: Información no incluida en ninguna de las categorías anteriores (el texto puede contener cualquier carácter UTF-8).

Profundidad de bits
: Indica si las muestras se almacenan con una resolución de 16 o de 24 bits (en este último caso, la compresión no está permitida).
  Ten en cuenta que algunos sintetizadores pueden no admitir muestras de 24 bits.

Compresión
: Activa la compresión de las muestras (en ese caso, la profundidad de bits se fija en 16 bits). Si esta opción está activada, se muestra un botón de compresión en el [editor de muestras](manual/soundfont-editor/editing-pages/sample-editor.md).
  La compresión es una función que ofrece el [formato sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3) y puede que no todos los sintetizadores la admitan.
  Activar o desactivar esta opción determina el formato del archivo al guardar, y Polyphone cambia la extensión del archivo .sf2 / .sf3 si es necesario.


## Información general {#doc_info}


La página de edición de la información general ofrece la siguiente información no editable:

Formato SoundFont
: Versión del [formato sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2) que usa el archivo.

Nombre y versión de la ROM
: Nombre y versión de una ROM de muestras, si se usa.

Motor de sonido
: Nombre de un motor de sonido.

Software de edición
: Software de edición utilizado para crear el soundfont.

Estos datos se muestran en una información emergente cuando el ratón pasa sobre el icono de información, al final de la línea que muestra el autor y la fecha.

Nombre y tamaño del archivo
: Ruta y nombre del archivo, y su tamaño en disco, que se muestran en la parte inferior de la ventana (el tamaño es el del archivo tal como se guardó por última vez).


## Estadísticas {#doc_stats}


Se hace un recuento de:

* el número de [muestras](manual/soundfont-editor/editing-pages/sample-editor.md), de muestras sin usar y de muestras duplicadas,
* el número de [instrumentos](manual/soundfont-editor/editing-pages/instrument-editor.md) y de instrumentos sin usar,
* el número de [conjuntos](manual/soundfont-editor/editing-pages/preset-editor.md),
* el número de parámetros y de moduladores editados en los instrumentos,
* el número de parámetros y de moduladores editados en los conjuntos.

El botón :guilabel:`Detalles` junto a las muestras, los instrumentos y los conjuntos muestra el [resumen de configuración](manual/soundfont-editor/configuration-summaries.md) correspondiente.

Las muestras y los instrumentos sin usar se pueden eliminar fácilmente con la herramienta «[Borrar elementos sin uso](manual/soundfont-editor/tools/global-tools.md#doc_unused)».
Las muestras duplicadas se pueden eliminar con la herramienta «[Limpiar duplicados](manual/soundfont-editor/tools/global-tools.md#doc_duplicates)».

**Nota:** no se debe superar el número de 65536 parámetros en los instrumentos y los conjuntos.
Algunos sintetizadores podrían no ser capaces de leerlos todos.



## Moduladores por defecto {#doc_default_mod}


Los moduladores por defecto se aplican a cada instrumento del soundfont, antes de los [moduladores](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) del instrumento: un modulador de un instrumento con las mismas fuentes, el mismo destino y la misma transformación reemplaza a un modulador por defecto (indicación :guilabel:`reemplaza mod. por defecto`).

Sin lista propia, un soundfont usa los moduladores por defecto del formato SoundFont, que se muestran sin poder editarse: velocidad hacia atenuación y hacia frecuencia de corte, presión de canal y rueda de modulación hacia vibrato, volumen, panorámica, expresión, reverberación, chorus y rueda de tono.
El modulador velocidad hacia frecuencia de corte depende de los [ajustes de sonido](manual/settings.md#doc_sound).

El botón :guilabel:`Personalizar` da al soundfont su propia lista, que empieza con estos moduladores y luego se edita como los moduladores de un instrumento (añadir, eliminar, copiar, pegar, modificar).
Esta lista reemplaza los moduladores por defecto del formato SoundFont: un modulador eliminado de la lista ya no se aplica a ningún instrumento, y un modulador añadido se aplica a todos.
El botón :guilabel:`Restablecer los valores por defecto de SoundFont` elimina la lista y restablece los moduladores por defecto del formato.

La lista se guarda en el bloque DMOD de los archivos sf2 y sf3, una [extensión del formato](manual/annexes/the-different-soundfont-formats.md#doc_sf2) que leen FluidSynth 2.5 y posteriores, SpessaSynth y Polyphone 2.7 y posteriores (incluido el [plugin](manual/plugin.md)).
Los demás sintetizadores la ignoran y aplican los moduladores por defecto del formato SoundFont.

Cuando un instrumento se copia en otro soundfont cuyos moduladores por defecto son distintos, se añaden moduladores globales al instrumento para que suene igual.
La [exportación DLS](manual/annexes/the-different-soundfont-formats.md#doc_dls) convierte los moduladores por defecto del soundfont; la exportación sfz solo conserva su efecto de la velocidad sobre el volumen y sobre la frecuencia de corte.