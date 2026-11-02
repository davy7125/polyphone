Muchos soundfonts reproducen una sola muestra por nota: sea cual sea la fuerza con la que se pulse una tecla, se oye la misma grabación, solo que más fuerte o más suave.
Los instrumentos acústicos no se comportan así.
Una tecla de piano pulsada suavemente da un sonido redondo y aterciopelado, mientras que la misma tecla pulsada con fuerza da un sonido más brillante, más rico en armónicos y con un ataque más percusivo.

Las capas de velocidad aportan esta riqueza a un soundfont: cada nota se graba con varios matices, y la grabación que se reproduce se elige según la velocidad de la nota.
Este tutorial toma el piano como ejemplo, pero el método sirve para cualquier instrumento cuyo timbre cambia con la fuerza de la interpretación: guitarras, baterías, metales, cuerdas…
Termina con otros detalles que hacen que un instrumento sea todavía más realista.


## Por qué una sola muestra no basta {#doc_why}


Escucha la tecla Do4 pulsada cuatro veces, de suave a fuerte (velocidades 30, 60, 90 y 120):

* con una <a href="files/velocity-layers-one-layer.mp3" download>sola muestra</a>, grabada con un matiz medio,

![](files/velocity-layers-one-layer.mp3)

* con <a href="files/velocity-layers-four-layers.mp3" download>cuatro capas</a>, eligiéndose la grabación según la velocidad.

![](files/velocity-layers-four-layers.mp3)

Con una sola muestra, las notas suaves suenan como notas fuertes oídas desde lejos y a la nota más fuerte le falta brillo.
El sintetizador baja ligeramente la frecuencia de corte de un filtro para las notas suaves (es uno de los moduladores por defecto), pero esto queda muy lejos del cambio de timbre de un piano real.
Con cuatro capas, el timbre sigue la fuerza de la interpretación.

La diferencia también es evidente en un contexto musical, aquí con arpegios cada vez más fuertes:

* <a href="files/velocity-layers-phrase-one-layer.mp3" download>una sola muestra</a>,

![](files/velocity-layers-phrase-one-layer.mp3)

* <a href="files/velocity-layers-phrase-four-layers.mp3" download>cuatro capas</a>.

![](files/velocity-layers-phrase-four-layers.mp3)


## Preparar las muestras {#doc_samples}


### Grabar las capas


Cada nota debe grabarse con varios matices, por ejemplo *pp* (pianissimo), *mp* (mezzo-piano), *mf* (mezzo-forte) y *ff* (fortissimo).
Con 3 o 4 capas la mejora ya es evidente, mientras que las mejores bibliotecas de piano usan de 8 a 16 capas para lograr transiciones más suaves.
Todas las grabaciones deben hacerse en las mismas condiciones (micrófonos, distancia, ganancia): solo debe cambiar la fuerza de la interpretación.

El ejemplo de este tutorial usa 4 capas, con una muestra cada 3 teclas en toda la extensión del piano.


### Importación


Nombra los archivos con la nota y el matiz, por ejemplo :file:`C4 pp.wav`, :file:`C4 mp.wav`, :file:`C4 mf.wav` y :file:`C4 ff.wav`.
Cuando la nota raíz no está guardada en un archivo de audio, Polyphone la encuentra en el nombre del archivo si este contiene el nombre de la nota en notación inglesa (C4 para el Do4, tecla 60) o el número de la tecla.
El matiz escrito en el nombre será útil en el paso siguiente.

Importa todos los archivos con :guilabel:`Agregar una muestra` en la [barra de herramientas](manual/soundfont-editor/toolbar.md#doc_edit), o [arrastrándolos y soltándolos](manual/soundfont-editor/tree.md#doc_dragdrop) en el árbol.
Un archivo estéreo da dos muestras, cuyos nombres terminan en «L» y «R».
Después comprueba la afinación de cada muestra como se explica en el tutorial «[Cómo preparar una muestra](tutorials/how-to-prepare-a-sample.md#doc_tuning)».
Las muestras de piano no necesitan ningún bucle: el sonido se apaga de forma natural.


## Un instrumento por capa {#doc_instruments}


Cada capa se convierte en un instrumento que contiene las muestras de un solo matiz.
Para cada matiz:

1. escribe su nombre («pp», por ejemplo) en la [barra de búsqueda](manual/soundfont-editor/tree.md#doc_search) del árbol, de modo que solo se muestren las muestras de esta capa,
2. selecciona todas esas muestras: haz clic en la primera y luego en la última manteniendo pulsada la tecla :kbd:`Shift`,
3. haz clic en :guilabel:`Agregar un instrumento` en la [barra de herramientas](manual/soundfont-editor/toolbar.md#doc_edit), ponle nombre al instrumento («Piano pp», por ejemplo) y deja marcada la opción :guilabel:`Enlazar muestras seleccionadas`.

Las muestras estéreo se colocan automáticamente a la izquierda y a la derecha, y los rangos de notas se reparten por el teclado según las notas raíz (ver la herramienta «[Posicionamiento automático de muestras](manual/soundfont-editor/tools/instrument-tools.md#doc_position)»).
Cada instrumento ya se puede tocar solo con el [teclado virtual](manual/soundfont-editor/toolbar.md#doc_keyboard).

Los parámetros comunes a todas las capas se pueden editar de una vez: cuando los cuatro instrumentos están seleccionados en el árbol, la [tabla](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) muestra sus divisiones globales una al lado de otra, y un valor escrito después de seleccionar las cuatro celdas de una fila se escribe en todas ellas.
En el ejemplo, el parámetro :guilabel:`Vol env liberación (s)` vale 1 s para que el sonido no se corte bruscamente al soltar una tecla.


## Apilar las capas en un conjunto {#doc_preset}


Selecciona los cuatro instrumentos en el árbol y haz clic en :guilabel:`Agregar un conjunto`, con la opción :guilabel:`Enlazar instrumentos seleccionados` marcada.
El conjunto contiene entonces cuatro divisiones, que cubren todas ellas el teclado completo.
Las capas se apilan dando a cada división del conjunto un :guilabel:`Rango de velocidad` diferente en el [editor de conjuntos](manual/soundfont-editor/editing-pages/preset-editor.md):

* 0-39 para la capa *pp*,
* 40-69 para la capa *mp*,
* 70-99 para la capa *mf*,
* 100-127 para la capa *ff*.

Los rangos deben cubrir todas las velocidades de 0 a 127, sin huecos (algunas velocidades quedarían en silencio) ni solapamientos (se oirían dos capas a la vez).
Los límites no tienen por qué ser regulares: se pueden mover según las grabaciones.
Los valores de la fila :guilabel:`Atenuación (dB)` se explican en la sección siguiente.


![Capas de velocidad en la tabla del conjunto](images/velocity_layers_table.png "Capas de velocidad en la tabla del conjunto")


En el [editor de rangos](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range), cada capa es una franja horizontal: la división seleccionada (aquí *mp*) aparece rayada.
Esta vista muestra rápidamente un hueco o un solapamiento.


![Capas de velocidad en el editor de rangos](images/velocity_layers_ranges.png "Capas de velocidad en el editor de rangos")


**Nota:** los rangos de velocidad también se pueden definir en un único instrumento que contenga todas las muestras, teniendo cada división su propio rango de velocidad.
En ese caso, la opción :guilabel:`por rango de velocidad` del ajuste :guilabel:`Ordenar divisiones` en las [preferencias](manual/settings.md#doc_interface) mantiene juntas las divisiones de una misma capa.


## Equilibrar las capas {#doc_balance}


Dentro de una capa, el volumen ya cambia con la velocidad: de ello se encarga otro modulador por defecto, definido por el formato sf2.
Pero en cada límite el volumen aumenta de golpe, porque las grabaciones de la capa superior son más fuertes: unos 5 a 7 dB en el ejemplo.
El objetivo es que al pasar de una capa a la siguiente solo cambie el timbre, aumentando el volumen de forma progresiva.

Usa el [teclado virtual](manual/soundfont-editor/toolbar.md#doc_keyboard) para comparar los dos lados de un límite: la velocidad de una nota depende de la posición vertical del clic, y las flechas :kbd:`↑` y :kbd:`↓` vuelven a tocar la última tecla con una velocidad aumentada o reducida en 1.
Después, para cada límite, de abajo arriba:

1. toca una tecla con la última velocidad de una capa (39, por ejemplo) y luego con la primera velocidad de la capa siguiente (40),
2. aumenta la atenuación de la capa superior en la tabla del conjunto hasta que las dos notas tengan más o menos el mismo volumen,
3. pasa al límite siguiente: las atenuaciones se suman, y cada capa queda al menos tan atenuada como la capa inferior.

En el ejemplo, las atenuaciones son de 0, 6, 11 y 16 dB, como se ve en la tabla anterior.
Comprueba varias teclas a lo largo del teclado: si un registro sigue desequilibrado, la atenuación de las divisiones afectadas se puede afinar en el instrumento de la capa, por ejemplo con la herramienta «[Configuración basada en notas](manual/soundfont-editor/tools/instrument-tools.md#doc_global)».

El rango dinámico global del instrumento, de la nota más suave a la más fuerte, lo da entonces el modulador por defecto.
Si las notas suaves son demasiado débiles o no lo bastante, este modulador se puede cambiar con la herramienta «[Anular el modulador por defecto](manual/soundfont-editor/tools/instrument-tools.md#doc_default_mod)».

Por último, nada mejor que una verdadera pieza musical para probar el resultado: toca el soundfont con un teclado MIDI o con el [reproductor MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player) (ver el tutorial «[Leer un archivo MIDI con un soundfont](tutorials/read-midi-file-with-soundfont.md)»).


## Más detalles para más realismo {#doc_details}


Las capas de velocidad son un gran paso, pero un instrumento real tiene otras características que un soundfont puede reproducir.
Aquí tienes dos de ellas para el piano.


### Ruidos al soltar las teclas


Cuando se suelta una tecla, el apagador vuelve a caer sobre las cuerdas y el mecanismo hace un pequeño ruido.
Algunas bibliotecas de muestras incluyen estos ruidos: la que se usa en el ejemplo tiene uno por tecla.

Se colocan en un quinto instrumento, «Piano release», añadido al conjunto en todo el rango de velocidad:

* selecciona el modo de reproducción ![release](images/sample_mode_release.png "release"){.inline} (release) en la fila :guilabel:`Reproducción en bucle`, para que una muestra se reproduzca al soltar la tecla,
* usa un valor largo de :guilabel:`Vol env liberación (s)` (5 s en el ejemplo) para que el ruido se reproduzca entero,
* atenúa mucho el instrumento (40 dB en el ejemplo): el ruido debe ser discreto.

Gracias al modulador por defecto que relaciona la velocidad con el volumen, el ruido es más fuerte cuando la nota se ha tocado con más fuerza.

**Nota:** el modo de reproducción «release» no está documentado en el formato sf2 y puede que otros reproductores no lo admitan.
El tutorial «[Usar releases personalizados en un instrumento](tutorials/using-custom-releases-in-an-instrument.md)» describe un método que funciona con cualquier reproductor.


### Cuerdas sin apagadores


En un piano, las cuerdas más agudas no tienen apagadores (aproximadamente la última octava y media): siguen sonando después de soltar la tecla.
Por eso, en cada instrumento de capa, las divisiones de estas teclas (a partir del Fa6, tecla 89, en el ejemplo) reciben un valor más largo de :guilabel:`Vol env liberación (s)`: 3 s en lugar de 1 s.

Escucha notas cortas en el centro del teclado, luego en la octava más aguda y por último un acorde en los graves:

* <a href="files/velocity-layers-without-details.mp3" download>sin estos detalles</a>,

![](files/velocity-layers-without-details.mp3)

* <a href="files/velocity-layers-with-details.mp3" download>con los ruidos al soltar las teclas y las cuerdas sin apagadores</a>.

![](files/velocity-layers-with-details.mp3)


### Otras ideas


Vale la pena probar otras mejoras:

* grabar cada tecla en lugar de una de cada tres: en un piano cada tecla tiene sus propias cuerdas, y una muestra transpuesta nunca es exactamente la nota vecina,
* usar grabaciones estéreo, que dan amplitud al sonido,
* calcular tiempos de release que cambian a lo largo del teclado con la herramienta «[Release natural](manual/soundfont-editor/tools/instrument-tools.md#doc_release)»,
* ajustar el volumen o el filtro por registro con la herramienta «[Configuración basada en notas](manual/soundfont-editor/tools/instrument-tools.md#doc_global)».

Y el mismo enfoque funciona con otros instrumentos: una cuerda de guitarra pulsada suave o fuerte, una caja entre notas fantasma y acentos, una trompeta que pasa de un sonido redondo a uno metálico…
Cada detalle tomado del instrumento real da más vida al soundfont.
