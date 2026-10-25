Un instrumento construido en un soundfont se compone de muestras, procedentes, por ejemplo, de grabaciones de un instrumento real.
Este tutorial describe cómo añadir y preparar una muestra en un soundfont.


## Importación {#doc_import}


Primero necesitas tener muestras en tu ordenador.
En internet se pueden encontrar muchas muestras gratuitas, ya sea grabadas a partir de instrumentos reales (flauta, guitarra, violonchelo, …) o creadas con distintos tipos de síntesis sonora.

Para este tutorial se proporciona esta muestra de flauta: <a href="files/flute.wav" download>flute.wav</a>.

Cuando la tengas, crea un nuevo soundfont con Polyphone (:menuselection:`Menu --> Nuevo`) e importa la muestra (:guilabel:`Agregar una muestra` en la [barra de herramientas](manual/soundfont-editor/toolbar.md#doc_edit)).


![Importar una muestra](images/import-sample.png "Importar una muestra")


## Bucle {#doc_loop}


### Introducción


Un bucle es una zona de una muestra que se puede repetir para prolongar la duración de la muestra.
Supongamos que una muestra solo dura 0,5 segundos: con un buen bucle, el sonido se puede prolongar hasta 10 segundos o más.
Esto solo es útil para muestras de instrumentos no percusivos, como flautas, trompetas, órganos o violines.
En cambio, una batería no necesita bucles.
También puede ser el caso de un piano o de una guitarra, según cómo se sinteticen estos sonidos.

Así pues, esta sección solo es necesaria si sientes la necesidad de prolongar la duración original de una muestra.
Si no, pasa directamente a la siguiente sección, «Afinación».


### Configurar un bucle


En el árbol, haz clic en la muestra a la que quieres poner un bucle y fíjate en los parámetros del bucle.


![Bucle de la muestra: parámetros del bucle](images/loop-sample-1.png "Bucle de la muestra: parámetros del bucle")


Si tienes suerte, estos valores venían incluidos en la muestra .wav y los campos ya están rellenos.
Si no, o si está mal configurado (puede pasar…), haz clic de forma aproximada a la izquierda y a la derecha en el gráfico (zona de visualización del WAV) para definir el principio y el final del bucle.
Esto suele hacerse en la fase estable del sonido, no durante el ataque ni durante el release.
Después, haz zoom alrededor del principio del bucle e intenta superponer las curvas verde (o roja) y azul.


![Bucle de la muestra: alineación de las ondas](images/loop-sample-2.png "Bucle de la muestra: alineación de las ondas")


### Escuchar el resultado


Cuando el bucle esté configurado, pulsa :guilabel:`Reproducir` en el reproductor con la opción :guilabel:`bucle` activada para comprobar el resultado.


![Bucle de la muestra: comprobar el resultado](images/loop-sample-3.png "Bucle de la muestra: comprobar el resultado")


Si no oyes la transición cuando se lee el final del bucle, el bucle es bueno.
Si no, deja el reproductor en marcha y mueve ligeramente el final o el principio del bucle, o ambos, hasta encontrar puntos con una transición lo más suave posible.
Escucha las siguientes grabaciones para oír la diferencia entre un bucle malo y uno bueno:

* <a href="files/bad-loop.mp3" download>bucle malo</a>

![](files/bad-loop.mp3)

* <a href="files/good-loop.mp3" download>bucle bueno</a>

![](files/good-loop.mp3)


**Nota:** existe una herramienta para «hacer trampa» si no consigues encontrar un bucle fácilmente: la herramienta de [bucle automático](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop).
Crea automáticamente un bucle cortando la muestra y superponiendo dos secciones, de modo que el final de la sección coincida exactamente con su principio.
Consulta la documentación correspondiente para más información.


## Afinación {#doc_tuning}


Todas las muestras deben afinarse si no quieres un instrumento desafinado.
Al indicar en el soundfont la altura exacta de todas las muestras importadas, el soundfont podrá adaptarlas a las notas.
Algunas muestras pueden incluir ya esta información; en ese caso, no deberías tener que cambiar los valores de :guilabel:`Nota raíz` y :guilabel:`Corrección`.
Pero, por precaución, siempre conviene comprobarlo.


![Afinar la muestra: información de altura](images/tune-sample-1.png "Afinar la muestra: información de altura")


Activa el bucle si la muestra tiene uno y pulsa :guilabel:`Reproducir` en el reproductor.
Después, activa la calibración :guilabel:`seno` y ajusta el volumen para oír los dos sonidos lo más claramente posible.


![Afinar la muestra: reproducir la muestra con la señal de calibración](images/tune-sample-2.png "Afinar la muestra: reproducir la muestra con la señal de calibración")


Empieza la afinación cambiando la nota raíz hasta que los dos sonidos se parezcan lo más posible.
Continúa ajustando la corrección.
En esta etapa, presta atención a los batimientos que aparezcan y haz que sean lo más lentos posible.
Estos son ejemplos con el sonido de una flauta y un seno de calibración:

* <a href="files/very-bad-tune.mp3" download>afinación muy mala</a> (la nota raíz no coincide)

![](files/very-bad-tune.mp3)

* <a href="files/bad-tune.mp3" download>afinación mala</a> (la corrección no es correcta y se oyen batimientos)

![](files/bad-tune.mp3)

* <a href="files/good-tune.mp3" download>afinación buena</a> (los dos valores son correctos)

![](files/good-tune.mp3)


**Nota:** el [análisis de frecuencias](manual/soundfont-editor/editing-pages/sample-editor.md#doc_freq) puede ser un buen indicador para empezar la afinación. Pero ten en cuenta que ningún algoritmo es perfecto para esta tarea y que solo tus oídos encontrarán la afinación correcta.


![Afinar la muestra: análisis de frecuencias](images/tune-sample-3.png "Afinar la muestra: análisis de frecuencias")


## Para ir más allá {#doc_further}


Hay varias herramientas disponibles en el menú :guilabel:`Caja de herramientas` de la [barra de herramientas](manual/soundfont-editor/toolbar.md#doc_edit).
Puedes editar las muestras:

* [eliminando el silencio](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank) al inicio,
* [ecualizando](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq) el sonido,
* [transportando](manual/soundfont-editor/tools/sample-tools.md#doc_transpose),
* [normalizando](manual/soundfont-editor/tools/sample-tools.md#doc_volume) el volumen,
* [ajustando el balance](manual/soundfont-editor/tools/sample-tools.md#doc_balance) de las muestras estéreo.


## Nota sobre las muestras estéreo {#doc_stereo}


Las muestras estéreo se componen de dos muestras mono.
El enlace se guarda en la información de una muestra.


![Muestra estéreo](images/stereo-sample.png "Muestra estéreo")


Gestionar dos muestras mono puede complicar la creación del bucle, porque un bucle puede sonar bien en un lado y no en el otro.
En ese caso, no dudes en hacer trampa configurando dos bucles distintos en los lados derecho e izquierdo.
La calidad del sonido no se verá afectada.
