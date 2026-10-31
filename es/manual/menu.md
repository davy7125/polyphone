El menú, situado en la esquina superior derecha del programa, contiene varios elementos que dan acceso a distintas funciones.


![Menú](images/menu.png "Menú")


## Abrir un editor de soundfonts {#doc_open}


Nuevo
: Crea un nuevo soundfont vacío, en el que hay que hacerlo todo desde cero.
  También accesible con el atajo :kbd:`Ctrl`+:kbd:`N`.

Abrir…
: Abre un archivo [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [.dls](manual/annexes/the-different-soundfont-formats.md#doc_dls), [.sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) u [.organ](manual/annexes/the-different-soundfont-formats.md#doc_organ).
  También accesible con el atajo :kbd:`Ctrl`+:kbd:`O`.


## Guardar un soundfont {#doc_save}


Guardar
: Guarda el archivo actual.
  También accesible con el atajo :kbd:`Ctrl`+:kbd:`S`.

Guardar como…
: Guarda el archivo actual en otro archivo.
  También accesible con el atajo :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`S`.

Exportar soundfonts…
: Combina y exporta uno o varios conjuntos de los soundfonts abiertos en formato [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz) o [.dls](manual/annexes/the-different-soundfont-formats.md#doc_dls). El formato adicional .csv exporta todos los parámetros para un análisis detallado del contenido del soundfont.
  También accesible con el atajo :kbd:`Ctrl`+:kbd:`E`.


![Ventana de exportación](images/export.png "Ventana de exportación")


En la ventana de exportación se listan los conjuntos de todos los soundfonts abiertos, y hay que marcar los que se van a exportar (los botones :guilabel:`Seleccionar todo` y :guilabel:`Deseleccionar todo` ayudan a cambiar rápidamente la selección).
El campo situado encima de estos botones filtra la lista por los números y el nombre de los conjuntos (atajo :kbd:`Ctrl`+:kbd:`F`; las teclas :kbd:`↓` o :kbd:`Enter` llevan después al primer conjunto mostrado): los dos botones solo se aplican entonces a los conjuntos mostrados, y los conjuntos marcados que el filtro oculta se exportan igualmente.
Se pueden seleccionar varios conjuntos manteniendo pulsada la tecla :kbd:`Ctrl` y haciendo clic en ellos uno a uno, o con la tecla :kbd:`Shift` para seleccionar todos los conjuntos entre dos clics. La tecla :kbd:`Space` los marca o desmarca entonces a la vez.
Un doble clic en cualquier punto de la fila de un conjunto también lo marca o desmarca.
El número de conjuntos marcados se muestra debajo de la lista, junto con el número de los que oculta el filtro.
Los demás parámetros son:

Destino
: El directorio en el que se escriben los archivos.
  Nunca se sobrescribe un archivo: si ya existe un archivo con el mismo nombre, se añade un número al final del nombre del nuevo archivo.

Formato
: Uno de :guilabel:`.sf2`, :guilabel:`.sf3`, :guilabel:`.sfz`, :guilabel:`.csv` o :guilabel:`.dls`.
  Si los conjuntos proceden de varios soundfonts, se combinan en un único soundfont en el que cada soundfont de origen se convierte en un banco.
  Por eso se pueden exportar a la vez como máximo 127 soundfonts, cada uno con un máximo de 127 conjuntos seleccionados.

Calidad
: Solo para el formato .sf3, calidad de la compresión de las muestras.

Un archivo por conjunto
: Solo para los formatos .sf2, .sf3 y .dls, creación de un archivo por cada conjunto seleccionado en lugar de un único archivo.

Una carpeta por banco, número de conjunto como prefijo, Clasificación General MIDI
: Solo para el formato .sfz. La exportación crea un directorio que contiene los archivos sfz y un subdirectorio :file:`samples`.
  Los archivos sfz se pueden colocar en un directorio por banco, su nombre puede llevar como prefijo el número de conjunto y se pueden ordenar en directorios correspondientes a las categorías General MIDI.

Valores brutos
: Solo para el formato .csv. Los valores se escriben tal como se guardan en el formato sf2, en lugar de convertirse a unidades habituales (segundos, decibelios, centésimas, etc.).
  La exportación crea un directorio que contiene el archivo :file:`samples.csv` y un archivo por instrumento y por conjunto.


## Opciones de visualización {#doc_view}


Pantalla completa
: Activa o desactiva el modo de pantalla completa.
  También accesible con el atajo :kbd:`F11`.


## Accesos directos {#doc_shortcuts}


Preferencias
: Abre las [preferencias](manual/settings.md) del programa.
  También accesible con el atajo :kbd:`Ctrl`+:kbd:`,`.

Manual en línea
: Muestra este manual de usuario en un navegador web.
  También accesible con el atajo :kbd:`F1`.

Acerca de Polyphone…
: Muestra la versión del programa y el copyright. El botón :guilabel:`Créditos` muestra la lista de personas que han contribuido al proyecto.


## Cierre {#doc_close}


Cerrar pestaña
: Cierra la pestaña actual (un editor de soundfonts, un explorador de soundfonts, las preferencias, etc.).
  También accesible con el atajo :kbd:`Ctrl`+:kbd:`W`.

Salir
: Sale del programa.
  También accesible con el atajo :kbd:`Ctrl`+:kbd:`Q`.