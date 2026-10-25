El árbol es el primer elemento del editor en el que debes fijarte.
Representa la arquitectura del soundfont abierto y controla la parte derecha del programa cuando seleccionas uno o varios elementos.

El árbol se muestra siempre con la misma [estructura](#doc_structure), ofrece un [menú contextual](#doc_menu) y dispone de funciones de [arrastrar y soltar](#doc_dragdrop), [copiar / pegar](#doc_copypaste) y [búsqueda](#doc_search).


## Estructura {#doc_structure}


La estructura del árbol se compone de 4 encabezados y del contenido de los encabezados :guilabel:`Muestras`, :guilabel:`Instrumentos` y :guilabel:`Conjuntos`.


![Árbol: estructura](images/tree_1.png "Árbol: estructura")


### Encabezado «General»


Un clic en el encabezado :guilabel:`General` abre el editor de la [información general](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md).


### Sección «Muestras»


Esta sección enumera todas las muestras contenidas en el soundfont, que son la materia prima sobre la que se construyen todos los instrumentos.
Seleccionar una o varias muestras lleva al [editor de muestras](manual/soundfont-editor/editing-pages/sample-editor.md).
Un clic en el encabezado :guilabel:`Muestras` muestra el [resumen de configuración](manual/soundfont-editor/configuration-summaries.md#doc_sample) de todas las muestras.


### Sección «Instrumentos»


Esta sección enumera todos los instrumentos contenidos en el soundfont; un instrumento es un elemento intermedio compuesto de muestras y utilizado por los conjuntos.
Los instrumentos contienen enlaces a muestras llamados «divisiones», que se muestran en forma de lista debajo de cada instrumento.
Seleccionar un instrumento o una de sus divisiones lleva al [editor de instrumentos](manual/soundfont-editor/editing-pages/instrument-editor.md).
Un clic en el encabezado :guilabel:`Instrumentos` muestra el [resumen de configuración](manual/soundfont-editor/configuration-summaries.md#doc_instrument) de todos los instrumentos.


### Sección «Conjuntos»


Esta sección enumera todos los conjuntos contenidos en el soundfont; un conjunto es un elemento final, compuesto de instrumentos, al que se puede acceder desde fuera del soundfont.
Los conjuntos contienen enlaces a instrumentos llamados «divisiones», que se muestran en forma de lista debajo de cada conjunto.
Seleccionar un conjunto o una de sus divisiones muestra el [editor de conjuntos](manual/soundfont-editor/editing-pages/preset-editor.md).
Un clic en el encabezado :guilabel:`Conjuntos` muestra el [resumen de configuración](manual/soundfont-editor/configuration-summaries.md#doc_preset) de todos los conjuntos.


## Menú contextual {#doc_menu}


Un clic derecho sobre un elemento muestra un menú con las siguientes acciones:

Enlazar a…
: Enlaza las muestras seleccionadas a un instrumento, o los instrumentos seleccionados a un conjunto.
  El destino se elige en una lista que se puede filtrar.
  Cuando se enlazan muestras con notas raíz distintas a un instrumento vacío, sus rangos de notas se definen automáticamente (como con la herramienta [Posicionamiento automático de muestras](manual/soundfont-editor/tools/instrument-tools.md#doc_position)).

Reemplazar por…
: Reemplaza una muestra por otra muestra en una división de instrumento, o un instrumento por otro instrumento en una división de conjunto.
  La configuración de la división se conserva.

Copiar
: Copia los elementos seleccionados; también accesible con :kbd:`Ctrl`+:kbd:`C`.

Pegar
: Pega los elementos copiados previamente; también accesible con :kbd:`Ctrl`+:kbd:`V`.

Duplicar
: Duplica los elementos seleccionados, que pueden ser muestras, instrumentos, conjuntos o divisiones.
  También accesible con :kbd:`Ctrl`+:kbd:`D`.

Eliminar
: Elimina los elementos seleccionados; también accesible con :kbd:`Del`.
  No se puede eliminar una muestra utilizada por un instrumento ni un instrumento utilizado por un conjunto.

Silenciar
: Para hacer pruebas, se puede silenciar una división dentro de un instrumento o de un conjunto. Esta opción también se encuentra en el menú contextual del [encabezado de la tabla](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_solo).

Reproducir siempre
: A diferencia de la función «Silenciar», se pueden añadir uno o varios instrumentos o conjuntos durante la reproducción. Así es posible encontrar un buen equilibrio entre distintos instrumentos o conjuntos que suenan juntos.

Restaurar reproducción
: Anula los efectos de «Silenciar» y «Reproducir siempre» en todo el soundfont.

Renombrar o renombrar en lote
: Renombra los elementos seleccionados; también accesible con :kbd:`F2`.
  Renombrar una división renombra la muestra o el instrumento al que está enlazada.
  Cuando hay varios elementos seleccionados, un cuadro de diálogo propone distintas opciones:
  
  * reemplazar el nombre actual con el nombre de la nota como sufijo (solo para las muestras y las divisiones de instrumento),
  * reemplazar el nombre actual con el nombre de la nota y velocidad como sufijo (solo para las divisiones de instrumento),
  * reemplazar el nombre actual con un sufijo numérico ascendente,
  * reemplazar caracteres,
  * insertar texto en una posición específica,
  * eliminar un intervalo de caracteres.


![Árbol: menú contextual](images/tree_2.png "Árbol: menú contextual")


## Arrastrar y soltar {#doc_dragdrop}


La operación de «arrastrar y soltar» en el árbol permite hacer asociaciones o copias rápidas.
Antes de arrastrar y soltar, hay que seleccionar elementos.
Puede ser un único elemento o una selección múltiple, posible con las teclas :kbd:`Ctrl` y :kbd:`Shift`.

El resultado de arrastrar y soltar depende entonces del tipo de elementos arrastrados y del tipo de elemento de destino.

Muestra → instrumento
: Asociación de la muestra al instrumento, creación de una división de instrumento.

Muestra → encabezado :guilabel:`Instrumentos`
: Creación de un instrumento que contiene todas las muestras arrastradas, o creación de un instrumento por cada muestra arrastrada.

Instrumento → conjunto
: Asociación del instrumento al conjunto, creación de una división de conjunto.

Instrumento → instrumento
: Copia de todas las divisiones del instrumento de origen en el instrumento de destino (puede ser el mismo instrumento).

División de instrumento → instrumento o división de instrumento
: Copia de las divisiones seleccionadas del instrumento de origen en el instrumento de destino (puede ser el mismo instrumento).

Instrumento → encabezado :guilabel:`Conjuntos`
: Creación de un conjunto que contiene todos los instrumentos arrastrados, o creación de un conjunto por cada instrumento arrastrado.

Conjunto → conjunto
: Copia de todas las divisiones del conjunto de origen en el conjunto de destino (puede ser el mismo conjunto).

División de conjunto → conjunto o división de conjunto
: Copia de las divisiones seleccionadas del conjunto de origen en el conjunto de destino (puede ser el mismo conjunto).

Archivo de audio (desde el explorador de archivos del ordenador) → árbol
: Importación del archivo como una nueva muestra, si su formato es compatible (ver el [editor de muestras](manual/soundfont-editor/editing-pages/sample-editor.md)).


## Copiar / pegar {#doc_copypaste}


Todo lo que es posible con arrastrar y soltar, como se ha visto antes, también es posible con copiar / pegar, accesible desde el menú contextual o con los atajos :kbd:`Ctrl`+:kbd:`C` y :kbd:`Ctrl`+:kbd:`V`.

También se puede copiar / pegar entre soundfonts distintos.
Es posible copiar un elemento en un primer soundfont y pegarlo en un segundo soundfont.
Se admite la selección múltiple.

Muestras seleccionadas
: Copia de las muestras.

Instrumentos seleccionados
: Copia de los instrumentos, incluidas todas las muestras enlazadas.

Conjuntos seleccionados
: Copia de los conjuntos, incluidos todos los instrumentos y muestras enlazados.

Al copiar elementos entre soundfonts distintos, si ya existe un elemento con el mismo nombre, se ofrecen varias opciones:

* ignorar la operación, en cuyo caso se cancela la copia del elemento con el mismo nombre;
* reemplazar el elemento existente, en cuyo caso el elemento existente se sobrescribe con el elemento copiado;
* duplicar el elemento, en cuyo caso el elemento se copia junto al elemento existente con otro nombre.


## Búsqueda {#doc_search}


Para facilitar la búsqueda de elementos en el árbol, hay una barra de búsqueda para filtrarlos (el atajo :kbd:`Ctrl`+:kbd:`F` pone el foco en ella).


![Árbol: barra de búsqueda](images/tree_3.png "Árbol: barra de búsqueda")


Cuando se escribe un texto en la barra de búsqueda, se muestran los elementos que lo contienen.
Además:

* si el texto está incluido en el nombre de una muestra, se muestran los instrumentos que usan la muestra y los conjuntos que usan esos instrumentos;
* si el texto está incluido en el nombre de un instrumento, se muestran las muestras que usa el instrumento y los conjuntos que usan el instrumento;
* si el texto está incluido en el nombre de un conjunto, se muestran los instrumentos que usa el conjunto y las muestras que usan esos instrumentos.

Un clic en la cruz situada a la derecha del campo editable anula el filtro.