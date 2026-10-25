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

- acceso a internet,
- un navegador web,
- un archivo MIDI, como <a href="files/ff7choco.mid" download>este</a>,
- opcionalmente, un soundfont que quieras probar y que cumpla el estándar <a href="https://es.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> (hay un filtro en el [repositorio](soundfonts/midi-standard/gm)),
- opcionalmente, la última versión de Polyphone, disponible en esta [página](software).


## Leer un archivo MIDI


Un método sencillo es usar el sintetizador <a href="https://spessasus.github.io/SpessaSynth/" target="_blank">SpessaSynth</a>, que es a la vez un lector de archivos MIDI y un sintetizador de soundfonts. Tiene la ventaja de estar disponible en línea y de no necesitar ninguna instalación especial.

Haz clic en el botón :guilabel:`Load MIDI file`, selecciona tu archivo MIDI y listo.

![El reproductor MIDI y sintetizador SpessaSynth](images/SpessaSynth.jpg "El reproductor MIDI y sintetizador SpessaSynth")


## Elegir un soundfont


Para usar el soundfont que prefieras, haz clic en el botón :guilabel:`Upload soundfont` y selecciona tu archivo.


## Conectar la salida MIDI a Polyphone


Como se ha dicho antes, **SpessaSynth** es a la vez un lector de archivos MIDI y un sintetizador. Se puede usar solo la función de lector de archivos MIDI y enviar las señales a Polyphone, que se encargará de la síntesis sonora. Esto puede ser útil si el soundfont es demasiado grande para enviarlo por internet.

Para conectar SpessaSynth a Polyphone (método probado en Linux):

- inicia Polyphone en [modo sintetizador](tutorials/use-polyphone-as-a-synthesizer.md) y abre las [preferencias](manual/settings.md#doc_general),
- selecciona «MIDI through» como entrada MIDI (recuerda el nombre),
- abre SpessaSynth en tu navegador web y abre sus ajustes,
- selecciona la salida MIDI «MIDI through» (el mismo nombre que la entrada MIDI de Polyphone).

Como un archivo MIDI puede enviar mensajes a 16 canales, tendrás que abrir el soundfont 16 veces e indicar un canal distinto, del 1 al 16, en cada pestaña. Para mayor comodidad, al abrir Polyphone puedes indicar el soundfont que se debe abrir y la opción «each» para abrir las 16 pestañas de una vez. El comando sería, por ejemplo:

```
polyphone -s -i "/home/davy/Downloads/gm.sf2" -c "each|off|off"
```

Consulta esta [página](manual/annexes/command-line.md#doc_synth) para más información sobre las líneas de comandos.
