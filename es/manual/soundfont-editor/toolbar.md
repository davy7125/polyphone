La barra de herramientas, situada en la parte superior del editor, contiene iconos para:

* las [funciones de edición](#doc_edit) habituales,
* cambiar la [vista](#doc_view) en los editores,
* mostrar la [grabadora](#doc_recorder),
* mostrar el [teclado virtual](#doc_keyboard).


![Barra de herramientas](images/toolbar.png "Barra de herramientas")


## Funciones de edición {#doc_edit}


Los elementos de la parte izquierda de la barra de herramientas sirven para las funciones de edición habituales:

![nueva muestra](images/toolbar_sample.png "nueva muestra"){.inline} Agregar una muestra
: Añade nuevas [muestras](manual/soundfont-editor/editing-pages/sample-editor.md) al soundfont.
  Se muestra un cuadro de diálogo para seleccionar archivos de audio (se pueden importar archivos :file:`.wav`, :file:`.flac`, :file:`.mp3`, :file:`.ogg`, :file:`.aif`, :file:`.aiff`, :file:`.aifc` y :file:`.snd`), y se pueden seleccionar varios archivos a la vez.

![nuevo instrumento](images/toolbar_instrument.png "nuevo instrumento"){.inline} Agregar un instrumento
: Crea un nuevo [instrumento](manual/soundfont-editor/editing-pages/instrument-editor.md).
  Un cuadro de diálogo pide el nombre del instrumento y, si hay muestras seleccionadas en el [árbol](manual/soundfont-editor/tree.md), propone enlazarlas al nuevo instrumento.
  En ese caso, las muestras estéreo se colocan a la izquierda y a la derecha, el bucle se activa si todas las muestras tienen bucle y, cuando las muestras tienen notas raíz distintas, los rangos de notas se definen automáticamente (ver la herramienta [Posicionamiento automático de muestras](manual/soundfont-editor/tools/instrument-tools.md#doc_position)).

![nuevo conjunto](images/toolbar_preset.png "nuevo conjunto"){.inline} Agregar un conjunto
: Crea un nuevo [conjunto](manual/soundfont-editor/editing-pages/preset-editor.md).
  Un cuadro de diálogo pide el nombre del conjunto y, si hay instrumentos seleccionados en el árbol, propone enlazarlos al nuevo conjunto.
  Se usan los primeros números de banco y de conjunto disponibles.

![herramientas](images/toolbar_toolbox.png "herramientas"){.inline} Caja de herramientas
: Al hacer clic en este botón se muestra un menú con todas las [herramientas](manual/soundfont-editor/tools/index.md) disponibles para los elementos seleccionados en el [árbol](manual/soundfont-editor/tree.md).

![deshacer](images/toolbar_undo.png "deshacer"){.inline} Deshacer
: Deshace la última modificación; también accesible con :kbd:`Ctrl`+:kbd:`Z`.

![rehacer](images/toolbar_redo.png "rehacer"){.inline} Rehacer
: Rehace la última modificación deshecha; también accesible con :kbd:`Ctrl`+:kbd:`Y` o :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`Z`.

![guardar](images/toolbar_save.png "guardar"){.inline} Guardar
: Guarda el soundfont; también accesible con :kbd:`Ctrl`+:kbd:`S`.


## Cambiar de vista {#doc_view}


En la parte derecha de la barra de herramientas pueden aparecer algunos iconos según el [editor](manual/soundfont-editor/editing-pages/index.md) que se muestre:

* al editar un instrumento, se puede cambiar entre la ![tabla](images/toolbar_table.png "tabla"){.inline} [vista de tabla](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table), el ![rangos](images/toolbar_range.png "rangos"){.inline} [editor de rangos](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range) y el ![envolvente](images/toolbar_adsr.png "envolvente"){.inline} [editor de envolventes](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope),
* al editar un conjunto, se puede cambiar entre la ![tabla](images/toolbar_table.png "tabla"){.inline} vista de tabla y el ![rangos](images/toolbar_range.png "rangos"){.inline} editor de rangos,
* en los demás casos no se muestra nada.

La vista elegida se mantiene al seleccionar otro instrumento o conjunto. La vista que se muestra para el primer instrumento o conjunto al abrir un soundfont se puede elegir con la opción :guilabel:`Vista de inicio` de las [preferencias](manual/settings.md#doc_interface).


## Grabadora {#doc_recorder}


Al hacer clic en el icono ![grabadora](images/toolbar_recorder.png "grabadora"){.inline}, se abre una ventana con la grabadora, que puede grabar la salida de Polyphone en un archivo :file:`.wav`. El tiempo transcurrido se muestra en la ventana.


![Grabadora](images/recorder.png "Grabadora")


El botón izquierdo puede tener dos estados:

Grabar
: Inicia una nueva grabación si la captura está detenida.
  Un cuadro de diálogo pide el nombre del archivo que contendrá la señal de audio grabada.

Detener
: Detiene la captura en curso.
  La captura también se detiene al cerrar la ventana.

El botón derecho también puede tener dos estados (solo accesible si la captura está en marcha):

Pausa
: Pone en pausa la captura en curso.

Reanudar
: Reanuda la captura en curso.


## Teclado virtual {#doc_keyboard}


Al hacer clic en el icono ![piano](images/toolbar_piano.png "piano"){.inline}, se abre una ventana con el teclado virtual, pensado para probar muestras, instrumentos y conjuntos.
Esta ventana emite señales MIDI:

* **note on** / **note off** / **aftertouch** con el teclado de piano,
* otras señales MIDI si la **parte de controladores MIDI** está desplegada.

La nota, la velocidad y la postpulsación (aftertouch) de la nota que se toca se muestran en la parte inferior de la ventana, junto con una lista desplegable para elegir el tamaño del teclado (5 octavas, 6 octavas, 88 teclas o 128 teclas).
Dos botones en la esquina inferior derecha muestran u ocultan los controladores MIDI encima del teclado y los efectos de audio debajo de él.

La ventana conserva las proporciones del teclado: al cambiar su anchura cambia su altura, y viceversa.
Mostrar u ocultar una zona solo cambia la altura de la ventana.


![Teclado virtual](images/virtual_keyboard.png "Teclado virtual")


### Teclado de piano


El teclado de piano emite señales que contienen números de nota y de velocidad.
Se puede controlar de tres formas:

con el ratón
: Se puede hacer clic con el ratón en la tecla deseada.
  La velocidad se define según la posición vertical del clic.
  Tras un clic, los movimientos posteriores dentro de la misma tecla emiten señales de aftertouch.

con el teclado del ordenador
: Las teclas utilizadas se definen en las [preferencias](manual/settings.md#doc_keyboard).
  Suenan en cuanto la ventana del teclado virtual está activa, aunque otro elemento de esta ventana tenga el foco (un potenciómetro, la lista desplegable…).

con un teclado MIDI externo
: La entrada MIDI se define en las [preferencias](manual/settings.md#doc_general).
  Si el teclado de piano está visible, mostrará lo que estás tocando.


#### Marcadores y colores


* Cuando se selecciona un [instrumento](manual/soundfont-editor/editing-pages/instrument-editor.md) o un [conjunto](manual/soundfont-editor/editing-pages/preset-editor.md), la zona que no cubren las divisiones aparece **en gris**.
  Así se puede visualizar la extensión completa de un instrumento o de un conjunto.
* Cuando se selecciona una división de un instrumento o de un conjunto, su rango se muestra de la misma forma.
  Además, su nota raíz se identifica con un **marcador naranja**.
* Cuando se selecciona una tecla en el teclado, aparece en **azul** (o en el color de selección elegido en las [preferencias](manual/settings.md#doc_interface) del programa).
  Además, el rango de la división correspondiente aparece en **azul claro**.
  Esto solo se aplica a nivel de instrumento.
* El do central (nota 60) se identifica con un **marcador negro**.


#### Atajos útiles


:kbd:`Ctrl`+:kbd:`K`
: Este atajo pone el foco en el teclado virtual en cualquier momento (el teclado se muestra si estaba oculto).
  Esto permite, por ejemplo mientras se edita una tabla, usar el teclado del ordenador para tocar en lugar de editar una celda.
  Un efecto de resplandor en el teclado virtual confirma que el foco está en él.

:kbd:`Ctrl`+:kbd:`1`, :kbd:`Ctrl`+:kbd:`2`, … :kbd:`Ctrl`+:kbd:`8`
: Estos atajos cambian las octavas que se tocan con el teclado del ordenador.
  Tras usar :kbd:`Ctrl`+:kbd:`1`, se pueden tocar las octavas más graves.
  Las octavas más agudas se alcanzan con :kbd:`Ctrl`+:kbd:`8`.
  También funcionan en cuanto la ventana del teclado virtual está activa.

:kbd:`→` / :kbd:`←`
: La flecha derecha o izquierda toca la altura inmediatamente superior o inferior a la última tecla tocada, con la misma velocidad.
  Combinada con la tecla :kbd:`Shift`, se saltan 6 teclas en lugar de 1.

:kbd:`↑` / :kbd:`↓`
: La flecha arriba o abajo toca la misma altura que la última tecla tocada, pero con una velocidad mayor o menor.
  Combinada con la tecla :kbd:`Shift`, la velocidad cambia en 10 en lugar de 1.

Las flechas solo tocan notas si el teclado de piano tiene el foco: si no, actúan sobre el elemento que tiene el foco, por ejemplo un potenciómetro.


### Controladores MIDI


Al hacer clic en el botón MIDI de la esquina inferior derecha, la zona de controladores MIDI se muestra encima del teclado o se oculta.
La ventana se desplaza entonces para que el teclado se quede en el mismo lugar de la pantalla (no es posible con Wayland en Linux).
Si trabajas con moduladores, es recomendable usar estas funciones para probar tu soundfont.


![Teclado virtual con controladores MIDI](images/virtual_keyboard_2.png "Teclado virtual con controladores MIDI")


Esta zona se compone de distintos botones:

Rueda de tono
: La rueda de tono aumenta o disminuye la altura.
  Vuelve automáticamente a su posición original 0.

Sensibilidad
: La sensibilidad define el rango de modificación de la rueda de tono, en semitonos.
  Por defecto es 2 (un tono entero).

Presión
: La presión es la cantidad de presión ejercida sobre el teclado.
  Se puede comparar con el aftertouch, pero mientras que el aftertouch es propio de cada tecla, la presión es una señal global para todas las teclas.

Potenciómetros
: Hay 3 potenciómetros para cambiar valores de controladores MIDI.
  Para cada potenciómetro se puede indicar un número de controlador y luego su valor.

LED de encendido / apagado
: Este LED sirve para poner el valor de un controlador MIDI a 0 o a 127 (útil, por ejemplo, para el pedal de resonancia).
  Es como un potenciómetro, pero con solo dos valores posibles.

Todos estos elementos se pueden sincronizar con un teclado MIDI conectado al ordenador si la entrada MIDI está bien configurada en las [preferencias](manual/settings.md#doc_general). Se admiten valores relativos de controladores MIDI para manejar los potenciómetros.


### Efectos de audio {#doc_keyboard_effects}


Al hacer clic en el botón del altavoz de la esquina inferior derecha, se muestra debajo del teclado una zona para ajustar los principales parámetros del sintetizador mientras se toca:

Reverberación
: Un LED activa o desactiva la reverberación y luego 4 potenciómetros cambian su nivel, el tamaño de la sala, la anchura y la amortiguación.
  El nombre de un potenciómetro se muestra al pasar el ratón por encima.

Coro
: Un LED activa o desactiva el coro y luego 3 potenciómetros cambian su nivel, su profundidad y su frecuencia.

Volumen
: El deslizador cambia el volumen general.

Estos ajustes son los mismos que en las [preferencias de sonido](manual/settings.md#doc_sound): un cambio hecho en un lugar se muestra en el otro.
Un cambio es progresivo y no interrumpe las notas que se están tocando.

*[MIDI]: Musical Instrument Digital Interface