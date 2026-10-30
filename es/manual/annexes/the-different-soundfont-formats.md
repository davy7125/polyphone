Polyphone admite varios formatos de soundfont:

* [sf2](#doc_sf2),
* [sf3](#doc_sf3),
* [sfz](#doc_sfz),
* [dls](#doc_dls),
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


## Archivos DLS {#doc_dls}


El formato DLS (DownLoadable Sounds), publicado por la MIDI Manufacturers Association, tiene el mismo objetivo que el formato sf2: un único archivo contiene muestras y los parámetros necesarios para tocarlas como instrumentos musicales.
Existe en dos niveles: DLS nivel 1 (1997) y DLS nivel 2 (cuya última versión es la 2.2).
Los archivos DLS han sido utilizados por el sintetizador de software de Windows (el archivo :file:`gm.dls`), por DirectMusic, por videojuegos y por teléfonos móviles.

La estructura difiere de la del formato sf2: un archivo DLS contiene ondas (las muestras) e instrumentos, cada uno con un banco y un número de programa, como un conjunto.
Un instrumento está formado por regiones, cada una de las cuales reproduce una onda en un rango de notas y de velocidades.
Los parámetros (envolventes, LFO, filtro, modulaciones…) se escriben como conexiones entre una fuente (por ejemplo un controlador MIDI o un LFO), un control opcional y un destino.
Se pueden definir para todo el instrumento o para una sola región; los parámetros de una región sustituyen entonces a los del instrumento.

Polyphone importa archivos DLS de ambos niveles (solo archivos .dls; los bancos DLS integrados en archivos .rmi o .xmf no son compatibles) y [exporta](manual/menu.md#doc_save) soundfonts en formato DLS nivel 2.2.
Un archivo DLS importado no se modifica al guardar el soundfont: el soundfont se guarda en formato sf2.


### Limitaciones de la importación DLS


* Algunos parámetros no tienen equivalente en el formato sf2 y se ignoran: el tiempo de apagado de la envolvente de volumen, las salidas distintas del paneo estéreo (canales central, traseros y LFE), el LFO de vibrato aplicado al volumen o al filtro, y la envolvente de modulación aplicada al volumen.
* Una nota tocada de nuevo detiene la anterior en un sintetizador DLS, mientras que en un sintetizador sf2 se oyen ambas notas.
* Solo se conserva el MSB del banco (CC0), o el LSB del banco (CC32) si el MSB es 0. Si varios instrumentos tienen el mismo banco y el mismo número de programa, se cambia el número de programa de uno de ellos.


### Limitaciones de la exportación DLS


* Un sintetizador DLS de nivel 1 reproduce las muestras pero ignora las envolventes, los LFO, el filtro y las modulaciones.
* Las muestras de 24 bits se redondean a 16 bits.
* No se exportan:
  los parámetros :guilabel:`Nota fija` y :guilabel:`Velocidad fija`,
  las divisiones que usan la [reproducción en bucle](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) «release»,
  los moduladores con la transformación «valor absoluto» o vinculados a otros moduladores,
  los moduladores de un parámetro que no existe en el formato DLS (desfases de las muestras, nota a mantenimiento o caída de las envolventes…),
  los moduladores que cambian la profundidad de un LFO o de la envolvente de modulación con 2 fuentes.
* Dos moduladores con las mismas fuentes y el mismo destino pero curvas diferentes se fusionan.
* Solo se pueden exportar 15 clases exclusivas por conjunto.
* :guilabel:`Afinación (escala)` y las modulaciones de controladores MIDI distintos de CC1, CC7, CC10, CC11, CC91 y CC93 pueden ser ignorados por algunos sintetizadores DLS.


### En general


Las modulaciones por defecto de los formatos sf2 y DLS son diferentes (velocidad hacia la frecuencia de corte del filtro, vibrato con la rueda de modulación y la presión de canal, niveles de reverberación y de coro): se añaden moduladores durante las conversiones para que el sonido sea el mismo.

Las conversiones se han probado comparando los renderizados de los archivos originales y de los archivos convertidos, así como con <a href="https://github.com/spessasus/spessasynth_core" target="_blank">SpessaSynth</a>, que lee y escribe archivos DLS.
Como pocos sintetizadores admiten el formato DLS nivel 2, la forma en que se reproducen algunos parámetros puede variar de un sintetizador a otro.


## Archivos sfArk {#doc_sfark}


Los archivos sfArk, al igual que el formato sf3, son un formato comprimido.
Esto significa que la calidad de los sonidos se reduce un poco (aunque a menudo de forma imperceptible).
Este formato tiende a quedarse obsoleto, por eso Polyphone solo permite importar archivos sfArk (no exportarlos).


## Disposiciones de GrandOrgue {#doc_organ}


GrandOrgue es un programa cuyo objetivo principal es emular instrumentos como los órganos de tubos. Un órgano se define mediante una serie de archivos wav, cada uno con el sonido de un tubo, y un archivo con la extensión «.organ» que contiene todos los parámetros necesarios para crear un órgano a partir de todos los archivos wav. Básicamente, todas las muestras se asocian a notas concretas y se agrupan por registros, que se pueden activar o desactivar al tocar el órgano. Polyphone es capaz de crear soundfonts a partir de esta asignación, pero no admite la conversión de soundfonts al formato de GrandOrgue.

### Limitación de la importación

Actualmente no se admiten los transportes.


*[MIDI]: Musical Instrument Digital Interface
