Polyphone no es solo un editor de soundfonts: también es un sintetizador completo que te permite tocar con un teclado MIDI.
Este tutorial te muestra cómo iniciar Polyphone en modo sintetizador.


## Requisitos previos

Primero necesitas un **teclado maestro** u otro controlador MIDI, que se conecta al ordenador mediante un **adaptador MIDI-USB**.

Descarga la última versión de Polyphone [aquí](software), instálala y abre el programa. En las [preferencias](manual/settings.md#doc_general), deberías encontrar tu dispositivo MIDI entre las entradas MIDI. Si no es así, probablemente el controlador (driver) del adaptador MIDI no está instalado en el ordenador (resuélvelo antes de continuar).

En las mismas preferencias, comprueba que la salida de audio es correcta.

## Abrir Polyphone en modo sintetizador


### Windows

Crea un acceso directo a :file:`polyphone.exe` con el argumento :command:`-s`, como se explica en la sección [Opciones de inicio de Polyphone](#doc_launch) más abajo.

### Linux

Abre un terminal y escribe la siguiente línea de comandos:

```
polyphone -s
```

### Mac

Abre un terminal y escribe la siguiente línea de comandos:

```
/Applications/Polyphone.app/Contents/MacOS/Polyphone -s
```

**Nota:** si has movido la aplicación Polyphone a una carpeta distinta de :file:`Applications`, tendrás que adaptar la ruta anterior.


## Tocar un instrumento


Para tocar un instrumento, basta con abrir un soundfont, que puedes descargar del [repositorio](soundfonts), por ejemplo, y seleccionar un instrumento de ese soundfont. Los instrumentos se organizan por bancos: selecciona un banco para mostrar la lista de instrumentos debajo.

![Polyphone abierto con la opción del modo sintetizador](images/synth_mode.png "Polyphone abierto con la opción del modo sintetizador")

Una vez seleccionado un instrumento, su rango de notas se muestra en el teclado virtual y puedes tocarlo con las teclas correspondientes de tu controlador MIDI.
El botón :guilabel:`Pánico MIDI` detiene todos los sonidos que se están reproduciendo (útil si una nota se queda colgada), y el botón :guilabel:`Grabadora` abre la [grabadora](manual/soundfont-editor/toolbar.md#doc_recorder).


## Opciones


### Tocar varios instrumentos a la vez

Para tocar varios instrumentos al pulsar una tecla del teclado, basta con poner la opción :guilabel:`Selección múltiple` en :guilabel:`encendido`. Así se pueden seleccionar varios instrumentos en la lista de instrumentos.

### Tocar varios instrumentos por separado

Si tienes varios controladores MIDI conectados al ordenador, puedes abrir varios soundfonts y elegir un canal concreto para cada uno de los soundfonts abiertos. Así, por ejemplo, un primer teclado servirá para tocar un instrumento y un segundo teclado servirá para tocar otro. Se puede abrir el mismo soundfont varias veces para tocar varios de sus instrumentos a la vez.

La asignación de canales se hace modificando el valor de :guilabel:`Canal`. Por ejemplo, la primera pestaña abierta escuchará el canal 1, mientras que la segunda escuchará el canal 2.
El valor :guilabel:`todos` (por defecto) escucha todos los canales, y el valor :guilabel:`detectar...` selecciona el canal de la próxima tecla que pulses en tu teclado.

![Configuración del sintetizador con 2 canales](images/synth_2_channels.png "Configuración del sintetizador con 2 canales")

**Nota:** algunos teclados se pueden dividir en dos partes, de modo que un solo teclado puede enviar notas a 2 canales distintos.

### Cambiar de instrumento con una tecla

El cambio de instrumento suele hacerse mediante un mensaje MIDI de tipo «Program change», y Polyphone es capaz de leerlos.

Sin embargo, se puede usar otro mecanismo: teclas programadas para enviar mensajes «Note ON» / «Note OFF», con el número de nota correspondiente al número de instrumento. Este mecanismo puede ser útil cuando el soundfont tiene solo unos pocos instrumentos y cuando las teclas se pueden programar para enviar notas concretas (por ejemplo, los pads de un teclado maestro).

Si la opción :guilabel:`Selección por notas` está en :guilabel:`encendido`:

* un instrumento se activa cuando se recibe un mensaje MIDI «Note ON»,
* un instrumento se desactiva cuando se recibe un mensaje MIDI «Note OFF».

Esta opción se puede usar cuando el pad de un teclado maestro está configurado como conmutador (una pulsación activa la nota y otra pulsación la desactiva).

Si se selecciona :guilabel:`alternar`:

* un instrumento se activa cuando se recibe un mensaje MIDI «Note ON»,
* un instrumento se desactiva cuando se recibe otro mensaje MIDI «Note ON».

Esta vez, la conmutación se hace en Polyphone.


## Opciones de inicio de Polyphone {#doc_launch}


Todas las opciones vistas arriba se pueden indicar al abrir Polyphone añadiendo parámetros a la línea de comandos. También se puede indicar el soundfont que se debe abrir.

En **Linux** o **macOS**, basta con completar la línea de comandos. En **Windows**, crea un acceso directo haciendo clic derecho en el archivo ejecutable :file:`polyphone.exe` y selecciona :guilabel:`Enviar a` → :guilabel:`Escritorio (crear acceso directo)` (desde :guilabel:`Mostrar más opciones` del menú contextual en Windows 11). Después, mostrando las propiedades de este acceso directo, se pueden añadir parámetros al destino, que podría ser:

```
"C:\Program Files\Polyphone\polyphone.exe" -s -c "all|on|toggle"
```

Las distintas opciones se describen en esta [página](manual/annexes/command-line.md#doc_synth).


## Usos del sintetizador


Ahora que sabes cómo convertir Polyphone en un sintetizador, ¿por qué no [usar una Raspberry Pi para crear un instrumento musical](tutorials/create-a-synthesizer-with-raspberry-pi.md)?

El modo sintetizador de Polyphone también se puede usar para [leer archivos MIDI](tutorials/read-midi-file-with-soundfont.md).
