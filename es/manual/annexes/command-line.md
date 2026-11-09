Polyphone se puede ejecutar desde la línea de comandos para convertir soundfonts al formato [sf2](#doc_sf2), [sf3](#doc_sf3), [sfz](#doc_sfz), [csv](#doc_csv) o [dls](#doc_dls). Los formatos de archivo admitidos para la conversión son [sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [dls](manual/annexes/the-different-soundfont-formats.md#doc_dls), [sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) y [organ](manual/annexes/the-different-soundfont-formats.md#doc_organ).

Un [archivo MIDI se puede reproducir](#doc_render) con un soundfont y guardar en un archivo wav.
También se pueden usar opciones para abrir Polyphone en [modo sintetizador](#doc_synth) o con el [servidor MCP](#doc_mcp) iniciado.

También se pueden pasar archivos y directorios como argumentos para abrirlos en la interfaz gráfica, por ejemplo :command:`polyphone file1.sf2 file2.sf3 /path/to/directory` (un directorio se abre en el [explorador de soundfonts](manual/soundfont-explorer.md)).
Si Polyphone ya está abierto, los archivos se abren en la instancia en ejecución.
Hay algunas [otras opciones](#doc_other) disponibles para restablecer la configuración o mostrar la ayuda y la versión.

La lista de todas las opciones se muestra con :command:`polyphone --help`.
El orden de los argumentos no importa.
Las opciones de las [versiones anteriores](#doc_legacy) se siguen aceptando.

**Nota:** en Windows hay que escribir la ruta completa de Polyphone en lugar de solo :command:`polyphone`, por ejemplo :command:`C:/Program files/Polyphone/polyphone.exe`.


## Conversión {#doc_conversion}


### Línea de comandos


```
polyphone --convert <formato> <archivos/a/convertir> -d <directorio/de/salida> -o <archivo/de/salida> -f
```


### Argumentos


--convert
: Formato de la conversión: «sf2», «sf3», «sfz», «csv» o «dls»; las opciones propias de cada formato se describen más abajo.
  Este argumento es opcional si se indica un archivo de salida con su extensión, por ejemplo :command:`polyphone file.sf2 -o file.sf3`.

-d, --output-dir
: Directorio de salida, que debe existir.
  Este argumento es opcional; por defecto es el directorio de cada archivo de entrada.

-o, --output
: Archivo de salida, con o sin su extensión (la extensión del formato se añade automáticamente).
  Este argumento es opcional; por defecto el archivo de salida tiene el nombre del archivo de entrada.
  Un nombre solo se escribe en el directorio de salida, una ruta relativa parte del directorio actual (o del directorio de salida si se usa -d).
  Este argumento no se puede usar si se convierten varios archivos.

-f, --force
: Reemplaza un archivo de salida existente.
  Sin este argumento, la conversión falla si el archivo de salida ya existe.
  El archivo de entrada se puede reemplazar por su conversión, por ejemplo :command:`polyphone --convert sf2 file.sf2 -f` reescribe :file:`file.sf2`.

Se pueden convertir varios archivos con un solo comando, y cada archivo de salida tiene el nombre de su archivo de entrada.
En caso de error, los demás archivos se convierten igualmente.


### Ejemplos


```
polyphone --convert sf2 /path/to/file.sfArk
polyphone /path/to/file.sf2 -o /path/to/converted.sf3
polyphone --convert sf3 *.sf2 -d /path/to/directory
```


## Conversión a sf2 {#doc_sf2}


No se usa ninguna opción particular para esta conversión.

```
polyphone --convert sf2 /path/to/file.dls
```


## Conversión a sf3 {#doc_sf3}


--quality
: Calidad de compresión: «low» (baja), «medium» (media) o «high» (alta).
  Este argumento es opcional; por defecto es «medium».

```
polyphone --convert sf3 /path/to/file.sf2 --quality high
```


## Conversión a sfz {#doc_sfz}


--preset-prefix
: Cada conjunto lleva como prefijo su número de conjunto.

--bank-dir
: Se crea un directorio por banco.

--gm-sort
: Se usa la clasificación General MIDI para ordenar los conjuntos.

```
polyphone --convert sfz /path/to/file.sf3 --bank-dir --gm-sort
```

La conversión crea un directorio (con el nombre del archivo de salida) que contiene los archivos sfz y un subdirectorio :file:`samples`.
Si este directorio ya existe, se añade un número a su nombre: no se reemplaza nada, ni siquiera con -f.


## Conversión a .csv {#doc_csv}


--raw
: Se exportan los valores brutos (tal como se almacenan en el archivo soundfont) en lugar de valores con unidades físicas y medibles.
  Por ejemplo, el tiempo de liberación exportado será 0,491 (segundos) o -1229 en su forma bruta.
  Por defecto, se exportan los valores con unidades físicas.

```
polyphone --convert csv /path/to/file.sf2 --raw
```

La conversión crea un directorio (con el nombre del archivo de salida) que contiene el archivo :file:`samples.csv` y un archivo por instrumento y por conjunto.
Si este directorio ya existe, se añade un número a su nombre: no se reemplaza nada, ni siquiera con -f.


## Conversión a dls {#doc_dls}


No se usa ninguna opción particular para esta conversión.

```
polyphone --convert dls /path/to/file.sf2
```

El archivo se escribe en formato DLS nivel 2; las limitaciones de la conversión se describen [aquí](manual/annexes/the-different-soundfont-formats.md#doc_dls).


## Renderizado de un archivo MIDI {#doc_render}


### Línea de comandos


```
polyphone --render <archivo/MIDI> <soundfont> -d <directorio/de/salida> -o <archivo/de/salida> -f
```


### Argumentos


--render
: Ruta del archivo MIDI que se reproducirá (extensión .mid, .midi, .kar o .rmi): el archivo MIDI se reproduce con el soundfont y el resultado se escribe en un archivo wav.

-d, --output-dir
: Directorio de salida en el que se escribirá el archivo wav.
  Este argumento es opcional; por defecto es el directorio del archivo MIDI.

-o, --output
: Archivo wav de salida, con o sin su extensión.
  Este argumento es opcional; por defecto el archivo wav tiene el nombre del archivo MIDI.

-f, --force
: Reemplaza un archivo wav existente.


### Ejemplo


```
polyphone --render /path/to/song.mid /path/to/file.sf2
```

Todos los canales del archivo MIDI tocan el soundfont, y los conjuntos se eligen mediante los cambios de programa del archivo (banco 128 en el canal 10, para la percusión).
Se usan los ajustes del sintetizador: [sonido](manual/settings.md#doc_sound) (reverberación, coro, volumen general), [polifonía](manual/settings.md#doc_general) y [afinación](manual/settings.md#doc_keyboard).
El resultado es un archivo wav estéreo de 32 bits en coma flotante a 48 000 Hz, que termina cuando el sonido de las últimas notas se ha extinguido.
Las notas de un conjunto que el soundfont no tiene no se tocan, y se muestra un aviso para ese conjunto.
En la interfaz gráfica, un archivo MIDI también se puede exportar desde el [reproductor MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player_settings), donde cada canal puede tocar un soundfont o un conjunto diferente.


## Modo sintetizador {#doc_synth}


### Línea de comandos


```
polyphone --synth <archivo/a/leer> --channel <canal> --multiple-selection --key-selection <modo>
```

![Polyphone abierto con la opción del modo sintetizador](images/synth_mode.png "Polyphone abierto con la opción del modo sintetizador")


### Argumentos


-s, --synth
: Polyphone se abre en modo sintetizador.

--channel
: Número del canal MIDI (entre 1 y 16) que se escucha para tocar.
  El valor «all» indica que se usarán todos los canales y se tratarán como uno solo (valor por defecto).
  El valor «each» escucha cada canal por separado, lo que es un requisito previo, por ejemplo, para tocar la salida MIDI de un secuenciador (un archivo MIDI también se puede reproducir directamente con el [reproductor MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player)).

--multiple-selection
: Se pueden seleccionar varios conjuntos para tocar.

--key-selection
: Con el valor «on», los conjuntos se seleccionan mediante las teclas correspondientes al número de conjunto (la tecla 0 activa el conjunto 0, por ejemplo).
  Con el valor «toggle», una tecla también deselecciona su conjunto.

Estos argumentos son opcionales, y los ajustes se pueden cambiar después desde la interfaz gráfica.


### Ejemplo


```
polyphone --synth /path/to/file.sf2 --channel 1 --multiple-selection --key-selection toggle
```


## Servidor MCP {#doc_mcp}


### Línea de comandos


```
polyphone --mcp <archivos/a/abrir> --mcp-port <puerto>
```


### Argumentos


--mcp
: El servidor MCP, que permite a un asistente de IA explorar, editar y analizar los soundfonts abiertos, se inicia para esta sesión aunque esté desactivado en las [preferencias](manual/settings.md).
  La dirección del servidor se muestra en la consola.
  Se abre una nueva instancia de Polyphone, aunque Polyphone ya esté abierto.

--mcp-port
: Puerto del servidor MCP (este argumento implica --mcp).
  Este argumento es opcional; por defecto es el puerto de las preferencias (8642).

Cambiar los ajustes del servidor MCP en la interfaz gráfica reemplaza estas opciones.


### Ejemplo


```
polyphone --mcp-port 8700 /path/to/file.sf2
```


## Otras opciones {#doc_other}


-h, --help
: Muestra la lista de todas las opciones.

-v, --version
: Muestra la versión de Polyphone.

-r, --reset-config
: Restablece la configuración de Polyphone (preferencias, geometría de las ventanas, historial, etc.).
  Puede ser útil en caso de problemas.

```
polyphone --reset-config
```


## Mensajes y códigos de retorno {#doc_exit}


La carga y la escritura se registran en la consola, y los errores se escriben en la salida de errores.
Los mensajes y la ayuda están en el idioma elegido en las [preferencias](manual/settings.md).

El código de retorno es:

0
: Éxito.

1
: Un archivo de entrada o el directorio de salida no existe, o el archivo de salida ya existe (sin -f).

2
: Los argumentos no son válidos: opción desconocida, valor incorrecto, opción no usada con el modo elegido, formato de archivo de entrada no compatible, etc.

3
: Un archivo de entrada no se puede leer (soundfont, archivo MIDI).

4
: La salida no se puede escribir.

Cuando se convierten varios archivos, el código de retorno es el del primer error.


## Opciones de las versiones anteriores {#doc_legacy}


Las opciones de las versiones anteriores de Polyphone se siguen aceptando:

-1, -2, -3, -4, -6
: Conversión a sf2, sf3, sfz, csv y dls, igual que con --convert.

-5
: Renderizado de un archivo MIDI, cuya ruta se indica con -c.

-i
: Archivo de entrada, que también se puede escribir sin esta opción.

-c
: Configuración, compuesta de valores separados por el carácter «|»:
  
  * conversión a sf3: calidad de compresión, «0» para baja, «1» para media, «2» para alta;
  * conversión a sfz: 3 valores, «1» o «0», para --preset-prefix, --bank-dir y --gm-sort, por ejemplo «0|1|1»;
  * conversión a csv: «raw» para --raw;
  * renderizado de un archivo MIDI (-5): ruta del archivo MIDI;
  * modo sintetizador: canal («all», «each» o de 1 a 16, donde 0 equivale a «all» y -1 a «each»), selección múltiple («on» u «off») y selección por teclas («on», «off» o «toggle»), por ejemplo «all|on|toggle».

```
polyphone -2 -i /path/to/file.sf2 -c 2
polyphone -s -i /path/to/file.sf2 -c 'all|on|toggle'
```
