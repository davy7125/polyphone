Al hacer clic en uno o varios conjuntos en el [árbol](manual/soundfont-editor/tree.md), aparece la página de edición de los conjuntos.
Un conjunto es una combinación de uno o varios instrumentos y es el elemento visible desde fuera de un [soundfont](manual/annexes/the-different-soundfont-formats.md#doc_sf2).

Al igual que en el [editor de instrumentos](manual/soundfont-editor/editing-pages/instrument-editor.md), hay varias partes:

* la [tabla](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table),
* el [editor de rangos](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range), cuando se pulsa el botón :guilabel:`Rangos` de la [barra de herramientas](manual/soundfont-editor/toolbar.md#doc_view),
* la [sección de moduladores](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator).


![Página del conjunto](images/edit_preset.png "Página del conjunto")


Si solo hay un conjunto seleccionado, la página permite editar los parámetros globales del conjunto y los parámetros de todas sus divisiones.
Si hay varios conjuntos seleccionados, se pueden editar los parámetros globales de cada uno de ellos.


## Diferencias con los instrumentos {#doc_diff}


Los parámetros que se editan aquí se llaman relativos, a diferencia de los parámetros de un instrumento, que, salvo casos especiales, se llaman absolutos.
Los valores de los parámetros de un conjunto se suman a los parámetros de los instrumentos que lo componen o se multiplican por ellos.
Por eso, una atenuación (o cualquier otro parámetro) puede ser negativa a nivel de conjunto, lo que no ocurre a nivel de instrumento.
Algunos parámetros no se pueden editar a nivel de conjunto:

* la reproducción en bucle,
* la nota raíz,
* la clase exclusiva,
* la nota fija,
* la velocidad fija,
* los cuatro desfases.

Un conjunto se identifica con un número de conjunto y un número de banco; esta combinación es única y se puede ajustar debajo de la tabla.
Cada banco contiene 128 conjuntos (numerados de 0 a 127).

El último banco (128) se usa normalmente para los kits de percusión.