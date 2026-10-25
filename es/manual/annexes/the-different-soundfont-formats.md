Polyphone admite varios formatos de soundfont:

* [sf2](#doc_sf2),
* [sf3](#doc_sf3),
* [sfz](#doc_sfz),
* [sfArk](#doc_sfark),
* [organ](#doc_organ).


## Soundfonts sf2 {#doc_sf2}


Los soundfonts sf2 son los principales archivos que edita Polyphone.
Cada archivo sf2 contiene uno o varios instrumentos musicales virtuales, compuestos de muestras de audio y de muchos parámetros.
Los parámetros definen cómo deben reproducirse las muestras a lo largo del teclado, opcionalmente moduladas por señales predefinidas ([moduladores](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator)).
Los archivos sf2 se construyen según una estructura de 3 niveles:

* [muestras](manual/soundfont-editor/editing-pages/sample-editor.md), procedentes de una trompeta o de un piano, por ejemplo,
* [instrumentos](manual/soundfont-editor/editing-pages/instrument-editor.md), compuestos de muestras,
* [conjuntos](manual/soundfont-editor/editing-pages/preset-editor.md), compuestos de instrumentos.

Además de la definición y la configuración de estos elementos, un archivo sf2 también contiene [información general](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md) (el autor, el copyright o la fecha de edición, por ejemplo).

Los soundfonts los usan sintetizadores de software, como <a href="http://www.fluidsynth.org/" target="_blank">FluidSynth</a>, controlados por señales MIDI.
También se pueden usar soundfonts sf2 para escuchar archivos MIDI con <a href="https://sourceforge.net/projects/timidity" target="_blank">TiMidity</a>, <a href="https://www.mindwerks.net/projects/wildmidi" target="_blank">WildMIDI</a> o QuickTime, y para escuchar partituras con <a href="https://musescore.org/" target="_blank">MuseScore</a>.
Algunos equipos, como samplers, tarjetas de sonido o incluso sintetizadores, también pueden admitir el formato .sf2.

Se puede encontrar más información sobre los soundfonts, por ejemplo, en <a href="https://es.wikipedia.org/wiki/SoundFont" target="_blank">Wikipedia</a>.
Las especificaciones sf2 están disponibles [aquí](files/sf_specifications_v2.01.pdf) para la versión 2.01 y [allí](files/sf_specifications_v2.04.pdf) para la versión 2.04.


## Soundfonts sf3 {#doc_sf3}


El formato sf3, desarrollado por <a href="https://musescore.org/" target="_blank">MuseScore</a>, es en todo similar al formato sf2, salvo que las [muestras](manual/soundfont-editor/editing-pages/sample-editor.md) se pueden almacenar en formato OGG (como el formato MP3, pero de código abierto) en lugar de almacenarse como datos sin procesar.
La consecuencia es que el formato sf3 es unas 10 veces más ligero que el formato sf2, con una calidad comparable.
En Polyphone, la compresión se activa en la [información general](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md#doc_edit) del soundfont, y luego cada muestra se puede comprimir en el [editor de muestras](manual/soundfont-editor/editing-pages/sample-editor.md#doc_info) (se puede elegir la calidad de la compresión).

Las muestras comprimidas de un soundfont exportado en este formato no están pensadas para editarse después, porque las compresiones sucesivas darían como resultado una calidad de sonido inferior.
Este formato debe considerarse un producto final y resulta muy interesante para todos los usuarios finales de soundfonts, ya que:

* facilita las descargas por internet,
* ahorra espacio en los ordenadores,
* la combinación MIDI + sf3 ofrece una solución ligera y potente para reproducir bandas sonoras (videojuegos, aplicaciones móviles, …).

A diferencia del formato <a href="http://www.fmjsoft.com/fmt/sf2pack.htm" target="_blank">sf2pack</a>, que responde a las mismas necesidades, el formato sf3 es completamente de código abierto.
El código fuente está disponible, lo que garantiza su continuidad (¡evitemos los errores cometidos con sfArk o sfPack!).


## Soundfonts sfz {#doc_sfz}


El formato sfz tiene el mismo objetivo que el formato sf2: crear un instrumento musical organizando y configurando muestras a lo largo del teclado.
La principal diferencia es que, mientras que el formato sf2 es un único archivo que lo contiene todo, el formato sfz es un archivo de texto que se distribuye con un conjunto de muestras .wav.
Como estaba pensado para que una persona pudiera editarlo, su principal ventaja era poder editar el archivo sin necesidad de un editor complejo.
Pero un editor sigue siendo muy recomendable para instrumentos grandes, ya que la cantidad de parámetros puede resultar rápidamente desalentadora.
El formato sfz tampoco está definido de forma tan estricta como el formato sf2: puede haber diferencias en la forma de editar y en la forma de reproducir un instrumento sfz. Pero, al mismo tiempo, las capacidades de los instrumentos sfz se han ampliado con respecto al estándar sf2.


### Limitaciones de la importación sfz


* Solo se importan los opcodes que tienen un equivalente en las [tablas](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).
* Como la envolvente aplicada a la altura y al filtro es la misma en los archivos sf2 (Mod env), no se pueden importar envolventes distintas para la altura y para el filtro (opcodes ``fileg_*`` y ``pitcheg_*``).
* Como el LFO aplicado al volumen y al filtro es el mismo en los archivos sf2 (Mod LFO), no se pueden importar LFO distintos para el volumen y para el filtro (opcodes ``amplfo_*`` y ``fillfo_*``).
* El único filtro que se puede importar es el filtro paso bajo de segundo orden (``fil_type=lpf_2p``).
* Para importarse correctamente, los opcodes ``group`` y ``off_by`` deben contener el mismo valor dentro de un mismo grupo.

**Atención:** los opcodes ``width`` y ``position`` podrían no interpretarse correctamente.


### Limitaciones de la exportación sfz


* Actualmente los [moduladores](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) no se exportan.
* Como el formato sfz (versión 1) solo permite una modulación de altura, los dos parámetros :guilabel:`Vib LFO → tono (c)` y :guilabel:`Mod LFO → tono (c)` no se pueden exportar a la vez.
* Un bucle 0-1 se puede exportar, pero la mayoría de los reproductores sfz lo ignoran.
* Algunos sintetizadores pueden no admitir muestras de 24 bits; consulta su manual de especificaciones.


### En general


La correspondencia de los niveles sonoros (en dB) durante el proceso de conversión sfz / sf2 se ha probado con el programa Sforzando.
Aun así, pueden quedar algunas diferencias.

Además, la forma en que los parámetros varían según la nota (mediante :guilabel:`Nota → Vol env mantenimiento / decaimiento` y :guilabel:`Nota → Mod env mantenimiento / decaimiento`) no tiene una correspondencia exacta.
El formato sf2 usa una ley exponencial para definir las duraciones de decaimiento y de mantenimiento según la nota, mientras que el formato sfz usa una ley lineal.


## Archivos sfArk {#doc_sfark}


Los archivos sfArk, al igual que el formato sf3, son un formato comprimido.
Esto significa que la calidad de los sonidos se reduce un poco (aunque a menudo de forma imperceptible).
Este formato tiende a quedarse obsoleto, por eso Polyphone solo permite importar archivos sfArk (no exportarlos).


## Disposiciones de GrandOrgue {#doc_organ}


GrandOrgue es un programa cuyo objetivo principal es emular instrumentos como los órganos de tubos. Un órgano se define mediante una serie de archivos wav, cada uno con el sonido de un tubo, y un archivo con la extensión «.organ» que contiene todos los parámetros necesarios para crear un órgano a partir de todos los archivos wav. Básicamente, todas las muestras se asocian a notas concretas y se agrupan por registros, que se pueden activar o desactivar al tocar el órgano. Polyphone es capaz de crear soundfonts a partir de esta asignación, pero no admite la conversión de soundfonts al formato de GrandOrgue.

### Limitación de la importación

Actualmente no se admiten los transportes.


*[MIDI]: Musical Instrument Digital Interface
