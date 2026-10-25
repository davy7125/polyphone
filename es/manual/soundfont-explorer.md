Con Polyphone, se puede abrir un directorio desde la [pantalla de inicio](manual/index.md#doc_left) haciendo clic en el botón :guilabel:`Abrir directorio`. El explorador de soundfonts se abre entonces en una nueva pestaña para mostrar los archivos de ese directorio (sus subdirectorios no se analizan), con una vista previa del contenido de cada soundfont.


![Explorador de soundfonts](images/soundfont_explorer.png "Explorador de soundfonts")


## Vista previa de los soundfonts {#doc_preview}


La lista principal, situada en el lado izquierdo de la interfaz, contiene la lista de archivos, y Polyphone los distingue de la siguiente manera:

* los archivos compatibles que se pueden analizar rápidamente (formatos [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3) y [.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), por ejemplo) se muestran con detalles sobre su contenido,
* los archivos compatibles que no se pueden analizar rápidamente (formato [.sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark), en particular) se muestran solo con información básica,
* los archivos no compatibles aparecen en gris y no se pueden abrir.

Al hacer clic en un archivo analizable se muestran, en las listas del lado derecho de la interfaz, las muestras, los instrumentos y los conjuntos que contiene. Cada elemento de estas listas incluye además información que ayuda a valorar el nivel de detalle del soundfont y la cantidad de trabajo invertida en él.

### Detalles a nivel de archivo

* para cada archivo se muestran el tamaño y la fecha de la última modificación,
* para los soundfonts que se pueden analizar, se añade la siguiente información:
  * número de muestras, instrumentos y conjuntos,
  * nombres de los primeros conjuntos.

### Información de cada muestra

* tipo (mono / estéreo),
* duración total,
* duración del bucle, si lo hay,
* frecuencia de muestreo.

### Información de cada instrumento o conjunto

* número total de divisiones, parámetros y generadores,
* número de rangos de notas distintos,
* número de rangos de velocidad distintos.


## Funciones de ordenación y búsqueda {#doc_filter}


La sección superior incluye un campo de texto (el atajo :kbd:`Ctrl`+:kbd:`F` pone el foco en él). Cuando se rellena, el explorador solo muestra los archivos que cumplen al menos uno de los siguientes criterios:

* el nombre contiene el texto buscado,
* el nombre de una de las muestras, instrumentos o conjuntos contiene el texto buscado.

Las listas de muestras, instrumentos y conjuntos también se filtran para resaltar los elementos coincidentes.

A la derecha del campo de texto hay una lista desplegable que permite ordenar los archivos por nombre (A→Z), por fecha de última modificación (más recientes primero) o por tamaño de archivo (más grandes o más pequeños primero).


## Operaciones con archivos {#doc_file_operations}


Al hacer clic en un archivo de la lista principal, aparecen 2 iconos en los que se puede hacer clic antes del nombre del archivo:

* un icono de lápiz para renombrar el archivo (también accesible con :kbd:`F2`),
* un icono de papelera para mover el archivo a la papelera de reciclaje, tras confirmarlo (también accesible con :kbd:`Del`).

Estas operaciones permiten gestionar y ordenar rápidamente una colección local de soundfonts.


## Abrir un soundfont {#doc_open_soundfont}


Un doble clic en un elemento de la lista principal abre el archivo —siempre que su formato sea compatible con Polyphone— en la ubicación configurada en las [preferencias](manual/settings.md#doc_interface) con la opción :guilabel:`Vista de inicio` (por defecto se muestra la [información general](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md)).

Un doble clic en una [muestra](manual/soundfont-editor/editing-pages/sample-editor.md), en un [instrumento](manual/soundfont-editor/editing-pages/instrument-editor.md) o en un [conjunto](manual/soundfont-editor/editing-pages/preset-editor.md) abre el archivo en el elemento seleccionado.