Cuando hay un conjunto o una de sus divisiones seleccionado en el [árbol](manual/soundfont-editor/tree.md), el [menú de herramientas](manual/soundfont-editor/tools/index.md) contiene varias herramientas ordenadas en 4 categorías:

* [análisis](#doc_cat_analyze),
* [edición rápida](#doc_cat_fast),
* [moduladores](#doc_cat_mod),
* [transformación](#doc_cat_transformation).

Se pueden usar las herramientas en varios conjuntos a la vez (con una selección múltiple), salvo para mostrar parámetros.
Todas las herramientas también están disponibles para los [instrumentos](manual/soundfont-editor/tools/instrument-tools.md).


## Análisis {#doc_cat_analyze}


### Mostrar parámetros {#doc_display}

La misma herramienta que «[Mostrar parámetros](manual/soundfont-editor/tools/instrument-tools.md#doc_display)» para los instrumentos.


## Edición rápida {#doc_cat_fast}


### Cambiar atenuaciones {#doc_attenuation}

La misma herramienta que «[Cambiar atenuaciones](manual/soundfont-editor/tools/instrument-tools.md#doc_attenuation)» para los instrumentos.


### Configuración basada en notas {#doc_global}

La misma herramienta que «[Configuración basada en notas](manual/soundfont-editor/tools/instrument-tools.md#doc_global)» para los instrumentos.


### Espacialización del sonido {#doc_spatialization}

La misma herramienta que «[Espacialización del sonido](manual/soundfont-editor/tools/instrument-tools.md#doc_spatialization)» para los instrumentos.


## Moduladores {#doc_cat_mod}


### Eliminar moduladores {#doc_remove_mod}

La misma herramienta que «[Eliminar moduladores](manual/soundfont-editor/tools/instrument-tools.md#doc_remove_mod)» para los instrumentos.


## Transformación {#doc_cat_transformation}


### Duplicación de las divisiones {#doc_division}

La misma herramienta que «[Duplicación de las divisiones](manual/soundfont-editor/tools/instrument-tools.md#doc_division)» para los instrumentos.


### Fusionar muestras para cada nota {#doc_mergesamples}

Cuando un conjunto llama a la vez a varios instrumentos y muestras al pulsar una tecla, esta herramienta fusiona, para cada nota, todas las muestras reproducidas. El resultado es un nuevo conjunto que reproduce una sola muestra por nota, lo que reduce la carga de CPU necesaria para ejecutar el conjunto.
La ventana pide el nombre del nuevo conjunto, la duración del sustain y la duración del release de los sonidos grabados, el intervalo entre las muestras creadas (cada nota, cada 3 notas o cada 6 notas), el bucle automático y el tipo de sonido (mono o estéreo).

Las opciones definidas en la imagen de abajo harán que la herramienta simule, cada 3 notas, una pulsación de tecla de 5 segundos. La grabación de audio será en estéreo y el release no se grabará. Las muestras así creadas recibirán después un bucle con la [herramienta de bucle automático](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop), y el instrumento que contiene todas estas muestras, así como el nuevo conjunto que lo usa, se llamarán «Cymbale III».

**Nota:** usar la opción de bucle automático elimina la parte del release y reduce ligeramente la duración sostenida de cada muestra.

![Fusionar muestras para cada nota](images/tool_merge_samples.png "Fusionar muestras para cada nota")
