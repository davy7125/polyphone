La página de edición de la información general se muestra cuando se selecciona el encabezado :guilabel:`General` en el [árbol](manual/soundfont-editor/tree.md).
Esta página se compone de tres tipos de elementos:

* [campos editables](#doc_edit),
* [información general](#doc_info) no editable,
* [estadísticas](#doc_stats) sobre el soundfont.


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

* el número de [muestras](manual/soundfont-editor/editing-pages/sample-editor.md) y de muestras sin usar,
* el número de [instrumentos](manual/soundfont-editor/editing-pages/instrument-editor.md) y de instrumentos sin usar,
* el número de [conjuntos](manual/soundfont-editor/editing-pages/preset-editor.md),
* el número de parámetros y de moduladores editados en los instrumentos,
* el número de parámetros y de moduladores editados en los conjuntos.

El botón :guilabel:`Detalles` junto a las muestras, los instrumentos y los conjuntos muestra el [resumen de configuración](manual/soundfont-editor/configuration-summaries.md) correspondiente.

Las muestras y los instrumentos sin usar se pueden eliminar fácilmente con la herramienta «[Borrar elementos sin uso](manual/soundfont-editor/tools/global-tools.md#doc_unused)».

**Nota:** no se debe superar el número de 65536 parámetros en los instrumentos y los conjuntos.
Algunos sintetizadores podrían no ser capaces de leerlos todos.