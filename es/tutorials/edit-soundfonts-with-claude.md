Polyphone incluye un servidor que sigue el Model Context Protocol (MCP), un estándar que permite a los asistentes de IA usar otros programas.
Una vez activado, un asistente como Claude puede trabajar con los soundfonts abiertos en Polyphone: describes lo que quieres con tus propias palabras, y el asistente explora los soundfonts, los modifica, usa las herramientas de Polyphone y mide el resultado.

Cada cambio aparece inmediatamente en el editor y se puede deshacer, como si lo hubieras hecho tú.
Este tutorial muestra cómo conectar Claude a Polyphone y luego da ejemplos de lo que se le puede pedir, con archivos de audio para escuchar los resultados.


## Lo que puede hacer el asistente {#doc_principles}


El asistente tiene acceso a todo lo que está abierto en Polyphone:

* **exploración**: los soundfonts, sus muestras, instrumentos y presets, los parámetros de cada división y los elementos seleccionados en el [árbol](manual/soundfont-editor/tree.md) (de modo que entienda «esta muestra» o «los instrumentos seleccionados»),
* **edición**: crear, renombrar, duplicar o eliminar elementos, cambiar parámetros, rangos de notas y de velocidades, puntos de bucle, moduladores…
* **herramientas**: la mayoría de las herramientas del [menú de herramientas](manual/soundfont-editor/tools/index.md), como el bucle automático, el cambio de volumen, el posicionamiento automático de muestras o el release natural,
* **análisis del sonido**: medidas de sonoridad, altura, armónicos, ataque, calidad del bucle… y el renderizado de una nota tocada por el sintetizador.

Las funciones de análisis son esenciales: **el asistente no puede oír**.
En su lugar, mide el sonido antes y después de un cambio, como un ingeniero leería un analizador de espectro.
Por eso puede comprobar que un bucle no hace clic, que las notas están afinadas o que el volumen es uniforme en todo el teclado, pero tus oídos siguen teniendo la última palabra.

Cada cambio hecho por el asistente es un paso en el historial del soundfont: :kbd:`Ctrl`+:kbd:`Z` en Polyphone lo deshace.
No se escribe nada en el disco mientras no le pidas al asistente que guarde el soundfont o lo guardes tú mismo.


## Configuración {#doc_setup}


### Activar el servidor en Polyphone


Abre las [preferencias](manual/settings.md#doc_ai_assistant) y ve a la sección :guilabel:`Asistente de IA`.
Marca :guilabel:`Permitir que los asistentes de IA accedan a los soundfonts abiertos`: la línea de abajo muestra entonces la dirección del servidor, http://127.0.0.1:8642/mcp por defecto.
Si el puerto 8642 ya lo usa otro programa, elige otro con :guilabel:`Puerto`.

El servidor solo acepta conexiones desde tu propio ordenador.
La misma página enumera todas las funciones que se ofrecen al asistente, agrupadas en cuatro categorías: :guilabel:`Exploración`, :guilabel:`Edición`, :guilabel:`Herramientas` y :guilabel:`Análisis del sonido`.


### Conectar Claude Code


<a href="https://claude.com/claude-code" target="_blank">Claude Code</a> es un asistente creado por Anthropic que funciona en un terminal (también existe como extensión de los editores VS Code y JetBrains).
Una vez instalado, copia el comando que aparece en las preferencias de Polyphone con el botón :guilabel:`Copiar` y ejecútalo en un terminal:

```
claude mcp add --transport http polyphone http://127.0.0.1:8642/mcp
```

Por defecto, Claude Code solo usa el servidor cuando se inicia desde la carpeta en la que se ejecutó este comando.
Para usar Polyphone desde cualquier carpeta, añade la opción `--scope user`:

```
claude mcp add --scope user --transport http polyphone http://127.0.0.1:8642/mcp
```

Después inicia una nueva sesión de Claude Code: las funciones de Polyphone se cargan al iniciar una sesión, y Polyphone ya debe estar abierto con el servidor activado.
El comando `/mcp` de Claude Code indica si el servidor «polyphone» está conectado.
La primera vez que el asistente quiere usar una función de Polyphone, Claude Code te pide permiso, y puedes elegir que no te lo vuelva a preguntar para esa función.

**Nota:** otros asistentes compatibles con MCP mediante el transporte HTTP pueden conectarse a la misma dirección.


### Primeras preguntas


Empieza con preguntas sencillas para ver cómo el asistente explora los soundfonts:

* *«¿Qué soundfonts están abiertos en Polyphone?»*
* *«¿Qué contiene este soundfont? ¿Hay muestras que no use ningún instrumento?»*
* *«Dame la lista de los presets con sus números de banco y de preset.»*

Cuando seleccionas un elemento en el árbol de Polyphone, el asistente puede referirse a él: *«Baja 6 dB el volumen de la muestra seleccionada»* o *«Renombra estos instrumentos con el prefijo “Órgano”»*.
En los cambios pequeños, el resultado es inmediato y visible en el editor.


## Ejemplo: revisar y corregir un instrumento {#doc_check}


Es fácil cometer errores al construir un instrumento con muchas muestras, y algunos son difíciles de notar: una nota ligeramente desafinada, una muestra un poco más fuerte que sus vecinas, un bucle que solo hace clic al cabo de unos segundos…
El asistente puede revisar un instrumento entero en unos segundos.

Para probarlo tú mismo, descarga <a href="files/flute-with-mistakes.sf2" download>flute-with-mistakes.sf2</a> y ábrelo en Polyphone.
Contiene un instrumento de flauta en el que se han introducido algunos errores a propósito.
Escucha una escala de Do4 a Fa6 seguida de un Sol6 largo:

![](files/ai-assistant-flute-before.mp3)

Luego pide:

*«Revisa el instrumento “Flute”: ¿hay notas desafinadas, demasiado fuertes o demasiado suaves, teclas sin sonido o bucles defectuosos?»*

El asistente analiza el instrumento (una sola función mide la sonoridad, la altura, el timbre y el bucle de cada división) y luego examina en detalle las muestras sospechosas.
Su respuesta enumera los problemas:

* la tecla Mi4 no toca nada: la división de «Flute 63» termina en Re♯4 y la siguiente empieza en Fa4,
* «Flute 69» suena un semitono demasiado alta: toca un La4 (440 Hz) pero su nota raíz es Sol♯4,
* «Flute 78» está 30 cents demasiado alta: su corrección es de +30 cents, mientras que la grabación está afinada,
* la división de «Flute 84» es 6 dB más fuerte que las demás (teclas Si5 a Do♯6),
* el bucle de «Flute 90» hace clic: las formas de onda no coinciden en el punto de bucle y la longitud del bucle no es un número entero de periodos.

Pide entonces: *«Corrige estos problemas.»*

El asistente amplía el rango de notas de «Flute 63» hasta Mi4, pone la nota raíz de «Flute 69» en La4 y la corrección de «Flute 78» en 0, da una atenuación de 6 dB a la división de «Flute 84» (la muestra en sí no se modifica) y usa la herramienta [Bucle automático](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop) en «Flute 90».
Un nuevo análisis confirma el resultado: ninguna tecla sin sonido, ninguna nota desafinada, el mismo volumen en todas partes y un bucle calificado de «bueno».
La misma escala después de las correcciones:

![](files/ai-assistant-flute-after.mp3)

El asistente explica lo que hace, y a menudo tiene que tomar decisiones: aquí ha atenuado una división en lugar de modificar los datos de audio de la muestra.
Si prefieres otra solución, basta con decirlo, y puedes revisar cada cambio en el editor o deshacerlo.


## Ejemplo: crear un instrumento desde cero {#doc_create}


Un asistente como Claude Code también puede ejecutar programas en tu ordenador, por ejemplo para calcular muestras.
Combinado con las funciones de Polyphone, esto permite construir un instrumento entero a partir de una descripción.
La flauta del ejemplo anterior se hizo así, a partir de dos peticiones:

*«¿Puedes hacerme un sonido de flauta para la tecla 60?»*

*«Ahora haz una flauta completa con varios niveles de intensidad, en un soundfont nuevo.»*

Esto es lo que hizo el asistente, paso a paso:

1. midió una flauta grabada en un soundfont General MIDI para conocer el timbre del instrumento en cada registro (armónicos, ruido de soplo, ataque, vibrato),
2. escribió un programa que sintetiza 39 muestras: 13 notas de Do4 a Do7, una cada 3 teclas, cada una en tres matices; *p* es más oscuro y con más soplo, con un ataque suave, y *f* es más rico en armónicos, con un ataque más marcado,
3. importó las muestras en un soundfont nuevo, ajustó sus notas raíz y sus bucles, y creó un instrumento por matiz con un release de 0,4 s,
4. apiló los tres instrumentos en un preset (banco 0, preset 73, la flauta del estándar General MIDI) con los rangos de velocidades 0-49, 50-89 y 90-127, como se describe en el tutorial «[Instrumentos realistas con varias capas de velocidad](tutorials/realistic-instruments-with-multiple-velocity-layers.md)»,
5. renderizó notas a ambos lados de cada límite de velocidad: el volumen solo cambia entre 0,1 y 0,4 dB, de modo que de una capa a la siguiente solo cambia el timbre.

El último paso reveló un punto débil: en la octava más aguda, las tres capas tenían casi el mismo timbre.
El asistente modificó su programa para que las notas fuertes también sean más brillantes en el registro agudo, y luego volvió a generar las muestras y a construir el instrumento.

Escucha la tecla Do4 tocada con las velocidades 35, 70 y 110, una nota por capa:

![](files/ai-assistant-flute-layers.mp3)

Y una frase cada vez más fuerte, en la que el timbre sigue al matiz:

![](files/ai-assistant-flute-phrase.mp3)

Una flauta sintetizada nunca igualará una buena grabación: este ejemplo muestra el método más que el sonido.
Con tus propias grabaciones, sirve el mismo tipo de petición: *«Importa los archivos wav de esta carpeta, encuentra su nota raíz, ponles un bucle y construye un instrumento con una capa por matiz.»*


## Otras ideas {#doc_ideas}


Estas son otras peticiones que funcionan bien:

* *«Pon un release natural en todos los instrumentos, más largo en los graves.»*
* *«Las notas agudas de este instrumento son demasiado brillantes: suavízalas progresivamente a partir de Do6.»*
* *«Crea un registro de voz celeste a partir de este instrumento de órgano, con 3 batimientos por segundo en Do4.»*
* *«Convierte todas las muestras estéreo a mono y actualiza los instrumentos.»*
* *«Compara estas dos muestras: ¿por qué la segunda suena más apagada?»*
* *«Explica los moduladores de este preset con palabras sencillas.»*
* *«Elimina las muestras sin usar y los duplicados, y luego guarda el soundfont en formato sf3.»*


## Consejos {#doc_tips}


* **Precisa el alcance**: «solo el instrumento “Piano”», «no cambies las muestras, solo los instrumentos»…
* **Pide medidas**: «comprueba el resultado» o «compara antes y después» hace que el asistente verifique su trabajo con las funciones de análisis.
* **Avanza paso a paso** en los cambios grandes, y escucha entre un paso y otro.
* **Guarda una copia** de tu soundfont y guarda solo cuando estés satisfecho: pide al asistente que guarde, o usa :kbd:`Ctrl`+:kbd:`S`.
* **Los soundfonts grandes** tardan más en analizarse: apunta a un instrumento o a un preset en lugar del soundfont entero.
* **Privacidad**: el asistente recibe los nombres, los parámetros, las medidas e imágenes de formas de onda, no el audio en sí; pero estos datos se envían al servicio del asistente, como el resto de la conversación.
