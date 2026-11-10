Polyphone también existe como plugin para estaciones de trabajo de audio digital (DAW) y para los demás programas anfitriones de plugins de audio.
El plugin toca soundfonts en los formatos sf2, sf3, sfz y dls con el sintetizador de Polyphone: un soundfont suena igual en un anfitrión que en el editor.
Toca los soundfonts pero no los edita: los cambios se hacen en Polyphone, y después el soundfont se vuelve a abrir en el plugin.

El plugin está disponible en los formatos VST3, CLAP y LV2 en Linux, Windows y macOS, y también en el formato Audio Unit (AU) en macOS.


![Plugin de Polyphone](images/plugin.png "Plugin de Polyphone")


## Instalación {#doc_install}


El plugin se distribuye en un archivo comprimido para cada sistema operativo, disponible en la sección «[Software](software)».
Copia los formatos que usa tu anfitrión en las carpetas donde busca los plugins y luego vuelve a buscar los plugins desde el anfitrión.

Linux
: :file:`Polyphone.vst3` en :file:`~/.vst3/`,\
  :file:`Polyphone.clap` en :file:`~/.clap/`,\
  :file:`Polyphone.lv2` en :file:`~/.lv2/`.

macOS
: :file:`Polyphone.vst3` en :file:`~/Library/Audio/Plug-Ins/VST3/`,\
  :file:`Polyphone.clap` en :file:`~/Library/Audio/Plug-Ins/CLAP/`,\
  :file:`Polyphone.component` en :file:`~/Library/Audio/Plug-Ins/Components/` (formato Audio Unit, que usan Logic Pro y GarageBand),\
  :file:`Polyphone.lv2` en :file:`~/Library/Audio/Plug-Ins/LV2/`.

Windows
: :file:`Polyphone.vst3` en :file:`C:\Program Files\Common Files\VST3`,\
  :file:`Polyphone.clap` en :file:`C:\Program Files\Common Files\CLAP`,\
  :file:`Polyphone.lv2` en :file:`%APPDATA%\LV2`.

En macOS, los plugins no están notarizados: si el anfitrión se niega a cargarlos, quita el atributo de cuarentena de los archivos descargados, por ejemplo:

```
xattr -dr com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/Polyphone.vst3
```


## Soundfont {#doc_soundfont}


La parte superior de la ventana muestra el nombre del soundfont y su ruta.
Un soundfont se abre:

* con el botón :guilabel:`Abrir...`,
* con la flecha situada a su derecha, que muestra los 10 últimos soundfonts abiertos,
* soltando un archivo en la ventana.

Los archivos sfz y dls se convierten igual que cuando Polyphone los importa; las muestras de un archivo sfz son los archivos de audio a los que hace referencia.
Todo el soundfont se lee al abrirlo, y las muestras de un archivo sf3 se descomprimen: no se lee nada del disco mientras se toca.
Cuando varias instancias del plugin tocan el mismo archivo (una por pista, por ejemplo), el archivo se lee y se guarda en memoria una sola vez.
Si el archivo ha cambiado, por ejemplo tras volver a guardarlo en Polyphone, se vuelve a leer.


## Modos de reproducción {#doc_modes}


Tres botones encima de la lista de conjuntos eligen cómo se tocan los conjuntos:

:guilabel:`Un conjunto`
: El conjunto seleccionado en la lista se toca en los 16 canales MIDI.
  Los conjuntos se muestran con sus números de banco y de conjunto, por ejemplo «000:024 Guitar».

:guilabel:`16 canales`
: Cada canal MIDI toca su propio conjunto, elegido en su fila.
  La opción :guilabel:`Cambio de programa` toca el conjunto indicado por los mensajes de selección de banco y de cambio de programa que recibe el canal, como hace un sintetizador General MIDI: el banco 128 se usa entonces en el canal 10, para la percusión.
  El conjunto que se toca se muestra a la derecha de cada fila.
  Este modo es práctico para tocar un archivo MIDI arreglado para varios instrumentos.

:guilabel:`MPE`
: El conjunto seleccionado lo toca un controlador MPE (MIDI Polyphonic Expression): cada nota tiene su propio canal y, por tanto, su propio pitch bend, su propia presión y su propio timbre (CC 74).
  Se usa una zona inferior de 15 canales hasta que el controlador envía su configuración (RPN 6).
  El canal maestro de una zona actúa sobre todas sus notas: su pitch bend se suma al de cada nota y sus controladores se aplican a todas ellas.
  El rango del pitch bend es de 48 semitonos para las notas y de 2 semitonos para el canal maestro, y el controlador puede cambiarlo (RPN 0).


![Plugin en el modo «16 canales»](images/plugin_channels.png "Plugin en el modo «16 canales»")


## Parámetros {#doc_parameters}


Los parámetros de la derecha son los del sintetizador de Polyphone, que también se encuentran en sus preferencias de [sonido](manual/settings.md#doc_sound) y de [teclado](manual/settings.md#doc_keyboard):

:guilabel:`Sintetizador`
: :guilabel:`Ganancia` (de -50 a +50 dB), :guilabel:`Transposición` (de -24 a +24 semitonos; la percusión no se transpone), :guilabel:`Afinación` (frecuencia del la situado por encima del do central, de 400 a 480 Hz) y :guilabel:`Temperamento`, cuyos ajustes finos son relativos a la tonalidad elegida debajo de la lista.

:guilabel:`Reverberación`
: :guilabel:`Nivel`, :guilabel:`Tamaño`, :guilabel:`Anchura` y :guilabel:`Amortiguación`.

:guilabel:`Coro`
: :guilabel:`Nivel`, :guilabel:`Profundidad` y :guilabel:`Frecuencia`.

El anfitrión puede automatizar todos estos parámetros.


## Teclado y estado {#doc_keyboard}


El teclado de la parte inferior de la ventana muestra las notas recibidas y se puede tocar con el ratón.
Debajo se muestran el número de voces que suenan y la proporción del procesador que usa el plugin.

El botón :guilabel:`Pánico` detiene todos los sonidos y restablece los controladores, lo que resulta útil si una nota se queda colgada.


## Preferencias {#doc_settings}


El botón :guilabel:`Preferencias` abre un menú con las siguientes opciones:

:guilabel:`Polifonía`
: Número máximo de voces, de 64 a 4096 (1024 por defecto).
  Por encima de este número, las notas más suaves se desvanecen.

:guilabel:`Teclado`
: Canal MIDI de las notas tocadas con el teclado de la ventana.

:guilabel:`Idioma`
: Idioma del plugin; por defecto, el del sistema.
  Esta elección es común a todas las instancias del plugin.
  Los nombres de los parámetros y de las salidas que se dan al anfitrión son los del idioma usado al cargar el plugin (siempre en inglés con LV2).

:guilabel:`Guardar el soundfont en el proyecto`
: El soundfont se guarda en el proyecto del anfitrión, que luego se puede abrir en otro ordenador sin el archivo.
  No es posible con un archivo sfz, cuyas muestras son otros archivos.

:guilabel:`Canales y efectos en sus propias salidas`
: Además de la salida principal, las salidas «Canal 1» a «Canal 16» reciben las notas de su canal sin efectos, y la salida «Efectos» recibe la parte enviada a la reverberación y al coro.
  Un canal cuya salida no está activada en el anfitrión se queda en la salida principal.
  Cuando esta opción está desactivada (por defecto), todo se mezcla en la salida principal.


## Uso en un proyecto {#doc_project}


El proyecto del anfitrión guarda los parámetros, los conjuntos que se tocan, la polifonía y la ruta del soundfont (o su contenido, si está guardado en el proyecto).
Al abrir un proyecto, el soundfont se lee antes de restaurar el resto, para que una exportación offline pueda empezar de inmediato.

Una nota empieza exactamente en la posición de su mensaje MIDI, sea cual sea el tamaño del búfer del anfitrión.
Durante una exportación offline, las voces nunca se limitan por la carga del procesador.
