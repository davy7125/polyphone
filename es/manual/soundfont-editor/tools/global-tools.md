Cuando el encabezado :guilabel:`General` está seleccionado en el [árbol](manual/soundfont-editor/tree.md), el [menú de herramientas](manual/soundfont-editor/tools/index.md) ofrece herramientas ordenadas en 3 categorías:

* [limpiar](#doc_cat_clean),
* [moduladores](#doc_cat_mod),
* [utilidad](#doc_cat_util).


## Limpiar {#doc_cat_clean}


### Borrar elementos sin uso {#doc_unused}


Se eliminan automáticamente todas las muestras que no usa ningún instrumento y todos los instrumentos que no usa ningún conjunto.
Un mensaje indica cuántos elementos se han eliminado.


### Limpiar duplicados {#doc_duplicates}


Las muestras idénticas se fusionan: los datos, la frecuencia de muestreo, el bucle, la nota raíz y la corrección deben ser iguales, y las muestras estéreo se comparan junto con su muestra enlazada.
En cada grupo de muestras idénticas, se conserva la primera (en el orden del archivo), que sustituye a las demás en los instrumentos, y las demás se eliminan.
Un mensaje indica cuántas muestras se han eliminado.

Los duplicados pueden verse antes en el [resumen de configuración de las muestras](manual/soundfont-editor/configuration-summaries.md#doc_sample).


## Moduladores {#doc_cat_mod}


### Eliminar todos los moduladores {#doc_removemods}


Se eliminarán todos los [moduladores](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) presentes en todos los [instrumentos](manual/soundfont-editor/editing-pages/instrument-editor.md) y [conjuntos](manual/soundfont-editor/editing-pages/preset-editor.md).


## Utilidad {#doc_cat_util}


### Exportar la lista de conjuntos {#doc_export}


Con esta herramienta, la lista de todos los [conjuntos](manual/soundfont-editor/editing-pages/preset-editor.md) se escribe en forma de texto, con sus números de banco y de conjunto.
Este texto puede tener formato csv, para integrarlo en una hoja de cálculo, o de tabla html, para integrarlo en un sitio web.
Un botón copia el texto en el portapapeles.


![Lista de conjuntos](images/en_preset_list.png "Lista de conjuntos")