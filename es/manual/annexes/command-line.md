Polyphone se puede ejecutar desde la línea de comandos para convertir soundfonts al formato [sf2](#doc_sf2), [sf3](#doc_sf3), [sfz](#doc_sfz), [csv](#doc_csv) o [dls](#doc_dls). Los formatos de archivo admitidos para la conversión son [sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [dls](manual/annexes/the-different-soundfont-formats.md#doc_dls), [sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) y [organ](manual/annexes/the-different-soundfont-formats.md#doc_organ).

Un [archivo MIDI se puede reproducir](#doc_render) con un soundfont y guardar en un archivo wav.
También se puede usar una opción para abrir Polyphone en [modo sintetizador](#doc_synth).

También se pueden pasar archivos y directorios como argumentos para abrirlos en la interfaz gráfica, por ejemplo :command:`polyphone file1.sf2 file2.sf3 /path/to/directory` (un directorio se abre en el [explorador de soundfonts](manual/soundfont-explorer.md)).
Hay algunas [otras opciones](#doc_other) disponibles para restablecer la configuración o mostrar la ayuda.

En una conversión, la carga y la escritura se registran en la consola.
El código de retorno es 0 si la conversión se ha realizado correctamente, y otro valor en caso de error (el archivo de entrada no existe, el archivo no se puede leer ni escribir, ya existe un archivo de salida para los formatos sf2, sf3 y dls, etc.).


## Conversión a sf2 {#doc_sf2}


### Línea de comandos


```
polyphone -1 -i <file/to/convert> -d <output/directory> -o <output_file_name>
```

**Nota:** en Windows hay que escribir la ruta completa de Polyphone en lugar de solo :command:`polyphone`, por ejemplo :command:`C:/Program files/Polyphone/polyphone.exe`.


### Argumentos


-1
: El modo «1» es «convertir a sf2».

-d
: Directorio de salida en el que se convertirá el archivo de entrada.
  Este argumento es opcional; por defecto es el mismo directorio que el del archivo de entrada.

-o
: Nombre de salida del archivo convertido, sin la extensión (la extensión «.sf2» se añadirá automáticamente).
  Este argumento es opcional; por defecto es el mismo nombre que el del archivo de entrada.


### Ejemplo


```
polyphone -1 -i /path/to/file.sfArk
```


## Conversión a sf3 {#doc_sf3}


### Línea de comandos


```
polyphone -2 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**Nota:** en Windows hay que escribir la ruta completa de Polyphone en lugar de solo :command:`polyphone`, por ejemplo :command:`C:/Program files/Polyphone/polyphone.exe`.


### Argumentos


-2
: El modo «2» es «convertir a sf3».

-d
: Directorio de salida en el que se convertirá el archivo de entrada.
  Este argumento es opcional; por defecto es el mismo directorio que el del archivo de entrada.

-o
: Nombre de salida del archivo convertido, sin la extensión (la extensión «.sf3» se añadirá automáticamente).
  Este argumento es opcional; por defecto es el mismo nombre que el del archivo de entrada.

-c
: Configuración de la conversión.
  Se puede indicar la calidad de la compresión: «0» es baja, «1» es media y «2» es alta.
  Este argumento es opcional; por defecto es «1» (calidad media).


### Ejemplo


```
polyphone -2 -i /path/to/file.sf2 -c 2
```


## Conversión a sfz {#doc_sfz}


### Línea de comandos


```
polyphone -3 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**Nota:** en Windows hay que escribir la ruta completa de Polyphone en lugar de solo :command:`polyphone`, por ejemplo :command:`C:/Program files/Polyphone/polyphone.exe`.


### Argumentos


-3
: El modo «3» es «convertir a sfz».

-d
: Directorio de salida en el que se convertirá el archivo de entrada.
  Este argumento es opcional; por defecto es el mismo directorio que el del archivo de entrada.

-o
: Nombre de salida del archivo convertido, sin la extensión (la extensión «.sfz» se añadirá automáticamente).
  Este argumento es opcional; por defecto es el mismo nombre que el del archivo de entrada.

-c
: Configuración de la conversión, compuesta de 3 valores separados por el carácter «|». Este argumento es opcional; por defecto es «0|0|0».
  
  * El primer valor es «1» si cada conjunto debe llevar como prefijo su número de conjunto, y «0» en caso contrario.
  * El segundo valor es «1» si se debe crear un directorio por banco, y «0» en caso contrario.
  * El tercer valor es «1» si se debe usar la clasificación General MIDI para ordenar los conjuntos, y «0» en caso contrario.


### Ejemplo


```
polyphone -3 -i /path/to/file.sf3 -c '0|1|1'
```

La conversión crea un directorio (con el nombre de salida) que contiene los archivos sfz y un subdirectorio :file:`samples`. Si ese directorio ya existe, se añade un número a su nombre.


## Conversión a .csv {#doc_csv}


### Línea de comandos


```
polyphone -4 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**Nota:** en Windows hay que escribir la ruta completa de Polyphone en lugar de solo :command:`polyphone`, por ejemplo :command:`C:/Program files/Polyphone/polyphone.exe`.


### Argumentos


-4
: El modo «4» es «convertir a .csv».

-d
: Directorio de salida en el que se convertirá el archivo de entrada.
  Este argumento es opcional; por defecto es el mismo directorio que el del archivo de entrada.

-o
: Nombre de salida del archivo convertido, sin la extensión (la extensión «.csv» se añadirá automáticamente).
  Este argumento es opcional; por defecto es el mismo nombre que el del archivo de entrada.

-c
: Configuración de la conversión.
  La opción «raw» se puede usar para exportar los valores brutos (tal como se guardan en el archivo de soundfont) en lugar de valores con unidades físicas medibles. Por ejemplo, el tiempo de liberación exportado será 0.491 (segundos), o -1229 en su forma bruta. Por defecto, se exportan los valores con unidades físicas.


### Ejemplo


```
polyphone -4 -i /path/to/file.sf2 -c 'raw'
```

La conversión crea un directorio (con el nombre de salida) que contiene el archivo :file:`samples.csv` y un archivo por instrumento y por conjunto. Si ese directorio ya existe, se añade un número a su nombre.


## Renderizado de un archivo MIDI {#doc_render}


### Línea de comandos


```
polyphone -5 -i <soundfont> -c <MIDI/file> -d <output/directory> -o <output_file_name>
```

**Nota:** en Windows hay que escribir la ruta completa de Polyphone en lugar de solo :command:`polyphone`, por ejemplo :command:`C:/Program files/Polyphone/polyphone.exe`.


### Argumentos


-5
: El modo «5» es «renderizar un archivo MIDI»: el archivo MIDI se reproduce con el soundfont y el resultado se escribe en un archivo wav.

-d
: Directorio de salida en el que se escribirá el archivo wav.
  Este argumento es opcional; por defecto es el mismo directorio que el del archivo MIDI.

-o
: Nombre de salida del archivo wav, sin la extensión (la extensión «.wav» se añadirá automáticamente).
  Este argumento es opcional; por defecto es el mismo nombre que el del archivo MIDI.

-c
: Ruta del archivo MIDI que se reproducirá (extensión .mid, .midi, .kar o .rmi).


### Ejemplo


```
polyphone -5 -i /path/to/file.sf2 -c /path/to/song.mid
```

Todos los canales del archivo MIDI tocan el soundfont, y los conjuntos se eligen mediante los cambios de programa del archivo (banco 128 en el canal 10, para la percusión).
Se usan los ajustes del sintetizador: [sonido](manual/settings.md#doc_sound) (reverberación, coro, volumen general), [polifonía](manual/settings.md#doc_general) y [afinación](manual/settings.md#doc_keyboard).
El resultado es un archivo wav estéreo de 32 bits en coma flotante a 48 000 Hz, que termina cuando el sonido de las últimas notas se ha extinguido. Un archivo wav existente se reemplaza.
Las notas de un conjunto que el soundfont no tiene no se tocan, y se muestra un aviso para ese conjunto.
En la interfaz gráfica, un archivo MIDI también se puede exportar desde el [reproductor MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player_settings), donde cada canal puede tocar un soundfont o un conjunto diferente.


## Conversión a dls {#doc_dls}


### Línea de comandos


```
polyphone -6 -i <file/to/convert> -d <output/directory> -o <output_file_name>
```

**Nota:** en Windows hay que escribir la ruta completa de Polyphone en lugar de solo :command:`polyphone`, por ejemplo :command:`C:/Program files/Polyphone/polyphone.exe`.


### Argumentos


-6
: El modo «6» es «convertir a dls».

-d
: Directorio de salida en el que se convertirá el archivo de entrada.
  Este argumento es opcional; por defecto es el mismo directorio que el del archivo de entrada.

-o
: Nombre de salida del archivo convertido, sin la extensión (la extensión «.dls» se añadirá automáticamente).
  Este argumento es opcional; por defecto es el mismo nombre que el del archivo de entrada.


### Ejemplo


```
polyphone -6 -i /path/to/file.sf2
```

El archivo se escribe en formato DLS nivel 2; las limitaciones de la conversión se describen [aquí](manual/annexes/the-different-soundfont-formats.md#doc_dls).


## Modo sintetizador {#doc_synth}


### Línea de comandos


```
polyphone -s -i <file/to/read> -c <configuration>
```

**Nota:** en Windows hay que escribir la ruta completa de Polyphone en lugar de solo :command:`polyphone`, por ejemplo :command:`C:/Program files/Polyphone/polyphone.exe`.

![Polyphone abierto con la opción del modo sintetizador](images/synth_mode.png "Polyphone abierto con la opción del modo sintetizador")


### Argumentos


-s
: El modo «s» es «abrir Polyphone en modo sintetizador».

-c
: Configuración del sintetizador, compuesta de 3 valores separados por el carácter «|». Este argumento es opcional; por defecto es «0|off|off». Estos parámetros se pueden cambiar después desde la interfaz gráfica.
  
  * El primer valor es el número del canal MIDI (entre 1 y 16) que se escucha para tocar. Un valor de 0 o «all» indica que se usarán todos los canales y se tratarán como uno solo. Un valor de -1 o «each» escucha cada canal por separado, lo que es un requisito previo, por ejemplo, para tocar la salida MIDI de un secuenciador (un archivo MIDI también se puede reproducir directamente con el [reproductor MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player)).
  * El segundo valor es «off» u «on», y desactiva o activa la selección de varios conjuntos para tocar.
  * El tercer valor es «off», «on» o «toggle», y desactiva o activa la selección de los conjuntos mediante las teclas correspondientes al número de conjunto (la tecla 0 activa el conjunto 0, por ejemplo).


### Ejemplo


```
polyphone -s -i /path/to/file.sf2 -c '1|on|toggle'
```


## Otras opciones {#doc_other}


-h
: Muestra un breve mensaje de ayuda.

-r
: Restablece la configuración de Polyphone (preferencias, geometría de las ventanas, historial, etc.).
  Puede ser útil en caso de problemas.

```
polyphone -r
```
