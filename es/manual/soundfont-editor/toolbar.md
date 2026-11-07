La barra de herramientas, situada en la parte superior del editor, contiene iconos para:

* las [funciones de edición](#doc_edit) habituales,
* cambiar la [vista](#doc_view) en los editores,
* mostrar la [grabadora](#doc_recorder),
* mostrar el [teclado virtual](#doc_keyboard),
* mostrar el [reproductor MIDI](#doc_midi_player).


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


Al hacer clic en el icono ![grabadora](images/toolbar_recorder.png "grabadora"){.inline}, se abre una ventana con la grabadora, que puede grabar en un archivo lo que reproduce Polyphone, la entrada de audio o las señales MIDI. El tiempo transcurrido se muestra en la ventana.


![Grabadora](images/recorder.png "Grabadora")


La lista del banner selecciona el modo de grabación:

Polyphone → wav
: Graba la salida de Polyphone en un archivo :file:`.wav`.

Micrófono → wav
: Graba la entrada de audio elegida en las [preferencias](manual/settings.md#doc_general) en un archivo :file:`.wav`, en estéreo o en mono según el dispositivo.
  La entrada no se envía a la salida de audio.

Señales MIDI
: Graba en un archivo :file:`.mid` lo que se toca con un teclado MIDI o el [teclado virtual](#doc_keyboard).

El botón izquierdo puede tener dos estados (atajo :kbd:`Ctrl`+:kbd:`R`, disponible en todas las ventanas, la grabadora se abre si es necesario):

Grabar
: Inicia una nueva grabación si la captura está detenida.
  El archivo se crea en el directorio de la última grabación, con un número añadido a su nombre si ya existe un archivo con ese nombre.

Detener
: Detiene la captura en curso.
  La captura también se detiene al cerrar la ventana, tras una confirmación.

El botón derecho también puede tener dos estados (solo accesible si la captura está en marcha, atajo :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`R`):

Pausa
: Pone en pausa la captura en curso.

Reanudar
: Reanuda la captura en curso.

Debajo de estos botones, el medidor muestra el nivel de los canales, incluso cuando no se graba nada: así se puede ajustar el volumen antes de una grabación. El valor a su derecha es el nivel más alto alcanzado desde el inicio de la grabación, mostrado sobre fondo rojo si se ha alcanzado el nivel máximo, en cuyo caso el sonido está recortado. Un clic en el medidor restablece este valor. En el modo :guilabel:`Señales MIDI`, el medidor muestra la velocidad de las notas tocadas, lo que confirma que las señales se reciben.

La parte inferior de la ventana lista las 5 últimas grabaciones, de audio o MIDI según el modo, con su tamaño o su número de notas. Un archivo que ya no existe se retira de la lista al abrir la ventana. Un clic selecciona una línea, que entonces muestra cuatro botones:

Escuchar
: Reproduce el archivo de audio en Polyphone, por la salida de audio (un segundo clic lo detiene, y seleccionar otra línea durante la reproducción reproduce su archivo en su lugar), o abre el archivo MIDI en el [reproductor MIDI](#doc_midi_player).

Directorio
: Abre el directorio del archivo.

Eliminar
: Mueve el archivo a la papelera, tras una confirmación.

Importar
: Importa el archivo de audio como muestra en el soundfont mostrado.
  Este botón solo está disponible si se muestra un editor de soundfont.

El menú a la derecha del banner contiene las siguientes opciones:

Iniciar automáticamente
: El botón de grabación espera el primer sonido o señal MIDI antes de iniciar la grabación.
  Tras una parada automática, la siguiente grabación se espera de la misma manera, hasta un clic en el botón de parada.

Detener tras un silencio
: Número de segundos sin sonido ni señal MIDI que detiene una grabación, de 1 a 10 (:guilabel:`desactivado` para no detenerla automáticamente).
  El silencio se retira del archivo, y una tecla aún pulsada no es un silencio.

Nivel de sonido
: Solo para la entrada de audio: nivel a partir del cual la entrada se considera un sonido, por encima de su ruido de fondo.
  Este nivel está fijado en -90 dB para la salida de Polyphone.

Pedir un nombre de archivo
: Un cuadro de diálogo pide el nombre del archivo al inicio de una grabación.

Cambiar el directorio...
: Selecciona el directorio de las próximas grabaciones, común a los archivos de audio y MIDI.

Se muestra un mensaje si una grabación no puede continuar (disco lleno o demasiado lento, archivo que alcanza 4 GB, cambio de la entrada de audio o de la frecuencia de muestreo de la salida de audio): lo grabado hasta entonces permanece en el archivo.


## Teclado virtual {#doc_keyboard}


Al hacer clic en el icono ![piano](images/toolbar_piano.png "piano"){.inline}, se abre una ventana con el teclado virtual, pensado para probar muestras, instrumentos y conjuntos.
Esta ventana emite señales MIDI:

* **note on** / **note off** / **aftertouch** con el teclado de piano,
* otras señales MIDI si la **parte de controladores MIDI** está desplegada.

La nota, la velocidad y la postpulsación (aftertouch) de la nota que se toca se muestran en la parte inferior de la ventana, junto con una lista desplegable para elegir el tamaño del teclado (5 octavas, 6 octavas, 88 teclas o 128 teclas).
Dos botones en la esquina inferior derecha muestran u ocultan los controladores MIDI encima del teclado y los efectos de audio debajo de él.

La ventana conserva las proporciones del teclado: al cambiar su anchura cambia su altura, y viceversa.
Mostrar u ocultar una zona solo cambia la altura de la ventana.

El teclado virtual también se puede mostrar desde el [explorador de soundfonts](manual/soundfont-explorer.md#doc_play), para tocar los elementos de los archivos de un directorio sin abrirlos.


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


## Reproductor MIDI {#doc_midi_player}


Al hacer clic en el icono ![MIDI](images/toolbar_midi.png "MIDI"){.inline}, se abre una ventana con el reproductor MIDI, que reproduce un archivo MIDI (:file:`.mid`, :file:`.midi`, :file:`.kar` o :file:`.rmi`) con los soundfonts abiertos o con los archivos seleccionados en un [explorador de soundfonts](manual/soundfont-explorer.md#doc_play), para probarlos en una pieza musical.
Los soundfonts se tocan tal como están en el editor, con sus cambios sin guardar.
El reproductor MIDI también está disponible en [modo sintetizador](tutorials/use-polyphone-as-a-synthesizer.md).


![Reproductor MIDI](images/midi_player.png "Reproductor MIDI")


La franja de la parte superior de la ventana contiene, de izquierda a derecha:

* un botón para abrir un archivo MIDI (al abrir la ventana, el último archivo abierto se vuelve a cargar si todavía existe),
* el nombre del archivo, el tiempo transcurrido y la duración total, encima de un deslizador para ir a otra posición de la pieza,
* un botón para iniciar o pausar la reproducción, también accesible con :kbd:`Space`,
* un botón para detener la reproducción y volver al principio,
* un botón que abre las [opciones de reproducción](#doc_midi_player_settings).

Si el archivo no se puede leer, el motivo se muestra en lugar de los canales.
La reproducción se detiene al cerrar la ventana.

Durante la reproducción, un teclado MIDI externo o el [teclado virtual](#doc_keyboard) siguen pudiendo tocar el elemento seleccionado en el editor junto con el archivo (las notas del archivo no se muestran en el teclado virtual), y la [grabadora](#doc_recorder) puede grabar el resultado.


### Canales


La tabla muestra los canales del archivo que contienen notas, cada uno con sus propios ajustes:

Canal
: El número del canal; el nombre de su pista se muestra en una información emergente.
  Los botones :guilabel:`M` (silenciar) y :guilabel:`S` (solo) silencian el canal o hacen que solo se oigan los canales en solo.
  El botón MIDI muestra los valores MIDI actuales del canal que difieren de su valor por defecto: controladores, rueda de tono (en semitonos), su sensibilidad y presión de canal.

Soundfont
: El soundfont que toca el canal: :guilabel:`Selección`, uno de los soundfonts abiertos en pestañas, o :guilabel:`Ninguno` para el silencio.
  :guilabel:`Selección`, elegida para todos los canales al abrir un archivo, toca lo que está seleccionado en la ventana principal: el soundfont de la pestaña actual, o el archivo seleccionado en un [explorador de soundfonts](manual/soundfont-explorer.md#doc_play), cuyo nombre se muestra entre paréntesis.
  Las demás pestañas (pantalla de inicio, preferencias, repositorio en línea…) conservan el soundfont que se toca.
  Cuando la selección cambia, un indicador de carga gira delante del nombre mientras se carga el soundfont, y el anterior sigue sonando mientras tanto; después, los presets de estos canales vuelven a :guilabel:`Auto`.

Conjunto
: Con :guilabel:`Auto`, los conjuntos se eligen mediante los cambios de programa del archivo (banco 128 en el canal 10, para la percusión) y el conjunto actual se muestra entre paréntesis.
  En caso contrario, se toca el conjunto elegido en la lista, pida lo que pida el archivo.
  Si el soundfont no tiene el conjunto pedido por el archivo, se muestra un icono de aviso y las notas del canal no se tocan.

Volumen
: El LED se enciende cuando empiezan notas, y el deslizador cambia el volumen del canal de -40 a +10 dB (un doble clic lo devuelve a 0 dB).

La lista de soundfonts o de conjuntos se abre con un doble clic en una celda, o con un clic en una celda ya seleccionada.
Estos ajustes se restablecen al abrir otro archivo.


### Opciones de reproducción {#doc_midi_player_settings}


El botón de la derecha de la franja abre un menú con los siguientes ajustes, que se conservan al abrir otro archivo:

Reproducción en bucle
: La pieza vuelve a empezar desde el principio cuando termina.

Tempo
: Velocidad de reproducción, del 25 % al 200 % del tempo del archivo (se conservan sus cambios de tempo).
  El tempo del archivo se muestra en una información emergente, y el tiempo transcurrido y la duración mostrados en la franja siguen la velocidad de reproducción.

Tono
: Transposición de -12 a +12 semitonos, salvo para la percusión (banco 128).

Un doble clic en un deslizador lo devuelve a su valor por defecto (100 % y 0).


![Opciones de reproducción](images/midi_player_settings.png "Opciones de reproducción")


El menú termina con :guilabel:`Exportar audio...`, que escribe la pieza en un archivo :file:`.wav` tal como se oye en el reproductor: mismos soundfonts, conjuntos, volúmenes, silencio y solo, tempo y tono.
La reproducción se pausa durante la exportación, que termina cuando el sonido de las últimas notas se ha extinguido.
Si se cancela la exportación, el archivo incompleto se elimina.
Un archivo MIDI también se puede convertir en un archivo wav desde la [línea de comandos](manual/annexes/command-line.md#doc_render).

*[MIDI]: Musical Instrument Digital Interface