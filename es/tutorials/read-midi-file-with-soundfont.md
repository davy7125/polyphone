Los archivos MIDI contienen mensajes MIDI que, juntos, forman una melodía. Se pueden combinar con un sintetizador que use un soundfont, y este tutorial muestra cómo usar un archivo MIDI para probar el resultado sonoro de un soundfont.


## Un poco de contexto


<div class="too_complicated">
<p>Al tocar en un teclado de sintetizador, se emiten señales <a href="https://es.wikipedia.org/wiki/MIDI" target="_blank">MIDI</a> que contienen información como las notas tocadas y su velocidad (término que indica la fuerza con la que se pulsa la tecla). Un archivo MIDI, en pocas palabras, contiene un registro cronológico de las señales emitidas por un teclado así. Hasta aquí, todavía no se ha creado ningún sonido.</p>

<p>Para que el altavoz emita un sonido, hace falta una etapa de conversión, y aquí es donde entra el sintetizador: dispara sonidos a partir de las señales MIDI recibidas. Existen varios tipos de sintetizador; aquí hablaremos de los sintetizadores basados en muestras y, más concretamente, de los sintetizadores que usan bancos de sonidos en formato SoundFont.</p>

<p>Un archivo MIDI puede controlar hasta 16 canales distintos, lo que permite usar 16 instrumentos distintos en paralelo. Además de indicar qué notas tocar, el archivo MIDI contiene instrucciones que dicen al sintetizador que use el instrumento X del banco Y para las próximas notas del canal Z. Para garantizar una selección correcta de los instrumentos, el banco de sonidos que usa el sintetizador debe cumplir el estándar <a href="https://es.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a>, que asigna un instrumento concreto a cada número X e Y. Por ejemplo, el instrumento <strong>001</strong> debe corresponder a un piano en el soundfont.</p>

<p>En resumen, aquí vamos a hablar de la reproducción de un archivo MIDI que controla un sintetizador, el cual dispara sonidos contenidos en un soundfont que cumple el estándar General MIDI. ¡Lo que viene a continuación es más sencillo!</p>
</div>


## Requisitos previos


Para reproducir un archivo MIDI con el método que se describe a continuación, necesitarás:

- la última versión de Polyphone, disponible en esta [página](software),
- un archivo MIDI, como <a href="files/ff7choco.mid" download>este</a>,
- un soundfont que quieras probar y que cumpla el estándar <a href="https://es.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> (hay un filtro en el [repositorio](soundfonts/midi-standard/gm)).

También se puede probar un soundfont que no cumpla este estándar, [eligiendo tú mismo sus conjuntos](#doc_presets).


## Leer un archivo MIDI


Abre tu soundfont en Polyphone y haz clic en el icono ![MIDI](images/toolbar_midi.png "MIDI"){.inline} a la derecha de la [barra de herramientas](manual/soundfont-editor/toolbar.md) para mostrar el [reproductor MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player).

Haz clic en el botón de la carpeta, arriba a la izquierda de la ventana, selecciona tu archivo MIDI y haz clic en el botón de reproducción (o pulsa :kbd:`Space`), y listo.

![El reproductor MIDI de Polyphone](images/midi_player.png "El reproductor MIDI de Polyphone")

Cada canal del archivo que contiene notas tiene su propia fila en la tabla. La columna :guilabel:`Conjunto` muestra el conjunto pedido por el archivo, que puede cambiar a lo largo de la pieza, y el LED de la columna :guilabel:`Volumen` se enciende cada vez que empieza una nota. El deslizador situado bajo el nombre del archivo permite ir a otro pasaje de la pieza.

El reproductor MIDI también está disponible cuando Polyphone se abre en [modo sintetizador](tutorials/use-polyphone-as-a-synthesizer.md).


## Probar el soundfont


El soundfont se toca tal como está en el editor, con los cambios sin guardar incluidos: puedes modificar un instrumento mientras suena la pieza y oír el resultado de inmediato, por ejemplo al ajustar su volumen, su envolvente o su filtro.

El reproductor también te ayuda a escuchar los detalles:

- los botones :guilabel:`S` (solo) y :guilabel:`M` (silenciar) de un canal aíslan un instrumento o lo quitan de la mezcla, y el deslizador de la columna :guilabel:`Volumen` lo equilibra con los demás;
- un icono de aviso en la columna :guilabel:`Conjunto` significa que el soundfont no tiene el conjunto pedido por el archivo: las notas de ese canal no se tocan, lo que revela un soundfont que no cumple del todo el estándar General MIDI;
- el botón MIDI de un canal muestra los controladores que el archivo usa en ese momento (rueda de modulación, expresión, reverberación…), algo útil para comprobar los [moduladores](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) del soundfont;
- las [opciones de reproducción](manual/soundfont-editor/toolbar.md#doc_midi_player_settings), a la derecha de la franja superior, permiten reproducir la pieza en bucle, ralentizarla para escuchar un pasaje rápido o transponerla para probar otras notas.


## Elegir los soundfonts y los conjuntos {#doc_presets}


Cada canal puede tocar un soundfont diferente: abre varios soundfonts en Polyphone y haz doble clic en una celda de la columna :guilabel:`Soundfont` para elegir el que toca el canal. Puedes, por ejemplo, comparar los pianos de dos soundfonts en la misma parte, o mantener un soundfont General MIDI para el acompañamiento mientras tu propio soundfont toca la melodía.

De forma predeterminada, los canales están en :guilabel:`Selección`: tocan el soundfont de la pestaña actual, o el archivo seleccionado en un [explorador de soundfonts](manual/soundfont-explorer.md#doc_play). Para comparar todos los soundfonts de un directorio, ábrelo en Polyphone y selecciona sus archivos uno tras otro mientras suena la pieza: cada soundfont se carga en segundo plano y se escucha en cuanto está listo, sin abrir ninguna pestaña.

![Elegir el soundfont de un canal](images/midi_player_soundfonts.png "Elegir el soundfont de un canal")

Del mismo modo, un doble clic en una celda de la columna :guilabel:`Conjunto` impone un conjunto del soundfont en lugar del pedido por el archivo. Así se prueba un soundfont que no cumple el estándar General MIDI, por ejemplo un soundfont con un solo instrumento: elige su conjunto para el canal de la melodía, y los cambios de programa del archivo se ignoran entonces en ese canal.


## Exportar el resultado


Cuando el resultado te convenza, :guilabel:`Exportar audio...` en las opciones de reproducción escribe la pieza en un archivo wav tal como se oye en el reproductor, con los mismos soundfonts, conjuntos, volúmenes, tempo y tono.
Un archivo MIDI también se puede convertir en un archivo wav desde la [línea de comandos](manual/annexes/command-line.md#doc_render), por ejemplo en un script.
