El objetivo de este tutorial es crear instrumentos expresivos con muestras en bucle cuyos releases se han grabado.
Aprenderás a aprovechar al máximo las posibilidades de una muestra así, usando el ataque y el sostenido de la muestra, pero también su release.


## Release tradicional frente a release personalizado


Cuando disparas una nota, primero oyes el principio de la muestra (el ataque) y, mientras no sueltes la tecla, el bucle de la muestra se reproduce una y otra vez (la fase sostenida).
Cuando sueltas la tecla, el bucle se sigue usando y el sonido se atenúa progresivamente hasta que deja de oírse.
Esta es la forma tradicional de hacer un release con los soundfonts: fácil de configurar, pero ignora los armónicos transitorios que podrían aparecer durante el release real de un instrumento acústico.
Este tipo de release puede verse como una simple prolongación de la fase sostenida.

Un release más realista consistiría en usar el release real que proporciona la muestra grabada.
El ataque y la fase sostenida se harían de la misma manera, pero al soltar la tecla la fase sostenida se cortaría de repente para dejar paso al release grabado, que empezaría a sonar.
Se usa un release personalizado en lugar de prolongar el sonido de la fase sostenida, lo que aporta detalles al sonido.

Para crear un instrumento así, el problema se divide en dos:

* primero, definir el ataque y la fase sostenida,
* después, disparar el release.

El ejemplo que se usará aquí es el sonido de una trompeta de órgano, por su gran riqueza armónica y su evolución audible.

**Nota:** desde la versión 2.4, Polyphone también puede disparar una muestra al soltar una tecla gracias al modo de reproducción en bucle ![release](images/sample_mode_release.png "release"){.inline} (ver el [editor de instrumentos](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)), lo que hace innecesario el bucle silencioso corto que se describe más abajo.
Sin embargo, este modo no está documentado en el formato sf2 y puede que otros reproductores no lo admitan, mientras que el método descrito en este tutorial funciona con cualquier reproductor.


## Preparación de las muestras


Pueden darse dos casos: tienes un archivo con la muestra completa, del ataque al release, o tienes un archivo adicional que contiene solo el release, ya extraído.


### Muestra completa con release


Carga la muestra dos veces en Polyphone y, en la segunda copia de la muestra, elimina todo salvo el release.
La eliminación se puede hacer manteniendo pulsada la tecla :kbd:`Alt` y definiendo una zona en el [gráfico de la muestra](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph).
Para encontrar el punto en el que empieza el release, reproduce la muestra varias veces e intenta determinar el punto en el que, en tu opinión, ya nada alimentaba el sonido.
Esta última parte será el release.
Después, sigue las instrucciones de abajo.


### Muestra y release separados


Muestras para el ejemplo: <a href="files/trumpet.wav" download>muestra 1</a>, <a href="files/trumpet-release.wav" download>muestra 2</a>.

Si tienes 2 muestras, la primera con el principio y la fase sostenida del sonido y la segunda con el release, solo tienes que asegurarte de que el release empiece con un silencio muy corto (unos pocos puntos) cercano a 0.
Con esta porción se hará un bucle corto e inaudible.

Afina y pon un bucle a la primera muestra como se explica en el tutorial «[cómo preparar una muestra](tutorials/how-to-prepare-a-sample.md)».

Pon un bucle a la segunda muestra en una parte muy corta del principio, por ejemplo del punto 1 al punto 2 o 3.
Indica la misma nota raíz y la misma corrección que en la primera muestra.


## Primer instrumento: ataque y fase sostenida


Crea un primer instrumento con las muestras que contienen el ataque y la fase sostenida del sonido.
Configura la división global así:

* selecciona ![bucle activado](images/loop_on.png "bucle activado"){.inline} en el parámetro :guilabel:`Reproducción en bucle`,
* indica 0.01 s en el parámetro :guilabel:`Vol env liberación (s)` (release muy corto).

Ahora tienes un instrumento sin release.


## Segundo instrumento: release


Crea un segundo instrumento para el release con las muestras de release que tienen un bucle silencioso muy corto al principio.
Configúralo así:

* selecciona ![bucle activado y release](images/loop_on_end.png "bucle activado y release"){.inline} (bucle activado + release) en el parámetro :guilabel:`Reproducción en bucle`,
* indica un número alto, como 100 segundos, en el parámetro :guilabel:`Vol env liberación (s)` para que el release no se atenúe,
* calcula aproximadamente la duración del ataque en la primera muestra y usa ese valor en el parámetro :guilabel:`Vol env ataque (s)`.

Ahora tienes un instrumento que permanece en silencio mientras no sueltes una tecla, y entonces se oye la fase de release.
También puedes notar que una pulsación muy rápida de una tecla dispara el release de forma atenuada.
Aquí apenas se nota con el ejemplo, ya que el ataque de una trompeta es muy rápido.
En el ejemplo se ha aumentado ligeramente la duración del ataque.


## Combinar los dos instrumentos


Crea un conjunto con estos dos instrumentos.
El primer instrumento se oye al pulsar una tecla y el segundo instrumento se dispara al soltarla.
Después puedes escuchar el resultado e intentar suavizar la transición entre la fase sostenida y el release:

* atenuando el primer o el segundo instrumento,
* aumentando la duración del release del primer instrumento.

El resultado con los archivos de ejemplo está <a href="files/custom%20release.sf2" download>aquí</a>.

Aquí puedes escuchar las diferencias entre:

* un <a href="files/classic-release.mp3" download>release clásico</a>

![](files/classic-release.mp3)

* y un <a href="files/custom-release.mp3" download>release personalizado</a>

![](files/custom-release.mp3)