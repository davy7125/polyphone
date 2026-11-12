Al hacer clic en uno o varios instrumentos en el [árbol](manual/soundfont-editor/tree.md), aparece la página de edición de los instrumentos.

Esta página consta de varias partes:

* la [tabla](#doc_table),
* el [editor de rangos](#doc_range), cuando se pulsa el botón :guilabel:`Rangos` de la [barra de herramientas](manual/soundfont-editor/toolbar.md#doc_view),
* el [editor de envolventes](#doc_envelope), cuando se pulsa el botón :guilabel:`Envolventes` de la [barra de herramientas](manual/soundfont-editor/toolbar.md#doc_view),
* la [vista de timbre](#doc_timbre), cuando se pulsa el botón :guilabel:`Timbre` de la [barra de herramientas](manual/soundfont-editor/toolbar.md#doc_view),
* la [sección de moduladores](#doc_modulator).


![Página del instrumento](images/edit_instrument.png "Página del instrumento")


Si solo hay un instrumento seleccionado, la página permite editar los parámetros globales del instrumento y los parámetros de todas sus divisiones.

Si hay varios instrumentos seleccionados, se pueden editar los parámetros globales de cada uno de ellos.


![Página del instrumento con varios instrumentos seleccionados](images/edit_multiple_instruments.png "Página del instrumento con varios instrumentos seleccionados")


## Tabla {#doc_table}


### Presentación


La tabla ofrece una vista de conjunto y permite editar la configuración de un instrumento.
Un cambio de un parámetro en la división :guilabel:`Global` se aplica a todas las divisiones, salvo a aquellas en las que el mismo parámetro ya está especificado (sustitución local).

Las filas escritas en cursiva (:guilabel:`Nota raíz de la muestra`, :guilabel:`Longitud de la muestra` y :guilabel:`Bucle`) no se pueden editar: solo recuerdan, a título informativo, las propiedades de la muestra enlazada a cada división.

Los parámetros editables son:

Rango de notas
: Rango de notas en el que se oirá la muestra.

Rango de velocidad
: Rango de velocidad en el que se oirá la muestra.
  Se pueden definir sonidos distintos para una misma nota, con rangos de velocidad distintos, para distinguir diferentes matices de una tecla de piano, por ejemplo.

Atenuación (dB)
: Atenuación en decibelios aplicada a la muestra.\
  Nota: el formato sf2 no permite que este valor sea negativo a nivel de instrumento, por lo que no es posible amplificar.

Paneo
: Balance aplicado al sonido, para oírlo a la izquierda o a la derecha en estéreo.

Reproducción en bucle
: ![Bucle desactivado](images/loop_off.png "bucle desactivado"){.inline} indica un sonido reproducido sin bucle.
  ![Bucle activado](images/loop_on.png "bucle activado"){.inline} indica un sonido que se repite continuamente.
  ![Release](images/sample_mode_release.png "release"){.inline} sirve para disparar la reproducción de una muestra al soltar una tecla. La envolvente de volumen se sigue usando para definir el volumen, y se añade un ataque corto (0,2 ms) para suavizar el inicio de la muestra en el release.
  ![Bucle activado + release](images/loop_on_end.png "bucle activado y release"){.inline} indica un sonido que se repite mientras la tecla está pulsada y que luego sigue reproduciendo el resto de la muestra.\
  Nota: el modo de reproducción «release» NO está documentado en el formato sf2, por lo que otros reproductores, como FluidSynth, pueden decidir no implementarlo.
  La otra forma de obtener este comportamiento es duplicar cada muestra con una muestra que contenga un bucle silencioso corto seguido del sonido que debe emitirse en el release.
  La muestra añadida se leerá con el parámetro de reproducción «bucle activado + release».
  Consulta el tutorial «[Usar releases personalizados en un instrumento](tutorials/using-custom-releases-in-an-instrument.md)».

Nota raíz
: Permite cambiar la nota raíz normal de una muestra por una nota distinta de la que se usa habitualmente.
  Por ejemplo, si una muestra tiene la nota raíz 50 y se reproduce en el rango de notas [49 — 51], se transportará inicialmente -1, 0 y +1 semitono en las notas 49, 50 y 51 respectivamente.
  Si luego se sustituye la nota raíz por 51 a nivel de instrumento, la misma muestra se transportará inicialmente -2, -1 y 0 semitonos en las notas 49, 50 y 51 respectivamente.

Afinación (semitonos y centésimas)
: Ajusta la afinación en semitonos y en centésimas de semitono.
  Este transporte se suma al transporte inicial ligado a la nota disparada (por ejemplo, el transporte inicial de una muestra con nota raíz 60 será de +2 semitonos si la tecla pulsada en el teclado es la 62).

Afinación (escala)
: Este parámetro modifica el intervalo entre dos notas consecutivas y, por tanto, afecta al transporte inicial ligado a la nota disparada.
  Un valor de 100 representa la escala del temperamento igual: se usan 100 centésimas de semitono para transportar una muestra de una nota a la siguiente.
  Un valor de 0 desactiva este transporte inicial, lo que significa que la muestra sonará igual en todas las teclas del teclado.

Filtro
: Aplica un filtro paso bajo, indicando la frecuencia de corte y la resonancia.

Envolvente de volumen
: Cambia la envolvente de volumen y, opcionalmente, cómo varía según la nota.

Modulación
: Define una modulación (envolvente, retraso, frecuencia, variaciones según la nota) y su efecto sobre el tono, el filtro y el volumen.

Vibrato
: Define un vibrato (retraso, frecuencia) y su efecto sobre el tono.

Clase exclusiva
: Este parámetro permite definir una o varias clases exclusivas, asignando a un conjunto de divisiones de una clase el mismo valor de parámetro distinto de 0.
  Cuando se define una clase exclusiva, cualquier nota disparada desde una de las divisiones de la clase exclusiva termina todos los demás sonidos de la misma clase.
  El alcance de una clase exclusiva es el conjunto en el que se define la clase (un sonido disparado en un instrumento terminará el sonido de otro instrumento si los dos instrumentos están en el mismo conjunto).
  Este efecto se puede usar en instrumentos de percusión como un platillo.
  Básicamente, tocar una nota termina automáticamente otra del mismo grupo.

Coro y reverberación
: Indica la intensidad de los efectos de coro y de reverberación.
  Los efectos se configuran en las [preferencias](manual/settings.md#doc_sound) del programa.

Nota y velocidad fijas
: Fuerza el valor de la nota y de la velocidad en toda la extensión de la división.

Desfases
: Cambia las posiciones de inicio y de fin de un sonido durante la reproducción, así como el inicio y el fin del bucle.
  La unidad de un desfase son las muestras (número de valores).


### Facilidades de edición en la tabla


Para facilitar y agilizar la edición en la tabla, se han implementado las siguientes funciones:

* se puede usar la rueda del ratón para cambiar un valor (antes la celda debe estar en modo de edición),
* se puede usar un teclado MIDI externo (o el [teclado virtual](manual/soundfont-editor/toolbar.md#doc_keyboard)) para definir la nota raíz, el rango de notas y el rango de velocidad (antes la celda debe estar en modo de edición),
* una selección múltiple seguida de la edición de la última celda edita todas las celdas a la vez,
* las teclas :kbd:`Del` y :kbd:`Backspace` borran el contenido de todas las celdas seleccionadas,
* las divisiones estéreo se pueden editar a la vez si la opción está activada en las [preferencias](manual/settings.md#doc_general) del programa,
* los nombres de las notas pueden ser numéricos o textuales (con C3, C4 o C5 como do central), según la opción elegida en las [preferencias](manual/settings.md#doc_interface),
* se puede copiar / pegar / cortar un conjunto de celdas (no necesariamente contiguas) con :kbd:`Ctrl`+:kbd:`C` / :kbd:`Ctrl`+:kbd:`V` / :kbd:`Ctrl`+:kbd:`X`.

¡No te olvides de las [herramientas](manual/soundfont-editor/tools/instrument-tools.md) disponibles!
En particular, la herramienta que permite una [configuración basada en notas](manual/soundfont-editor/tools/instrument-tools.md#doc_global).


### Posibilidad de silenciar divisiones {#doc_solo}


Para hacer pruebas, en las tablas se puede silenciar una división o silenciar todas las divisiones menos una.
Las opciones están accesibles con un clic derecho en el encabezado de una división, y también en el menú contextual del [árbol](manual/soundfont-editor/tree.md#doc_menu).


![Silenciar divisiones en una tabla](images/table_mute_divisions.png "Silenciar divisiones en una tabla")


## Editor de rangos {#doc_range}


El editor de rangos permite organizar las muestras de forma rápida y gráfica según sus rangos de notas y de velocidad.
Este gráfico también pone de manifiesto problemas de configuración, como zonas sin muestras o superposiciones no deseadas.


![Editor de rangos](images/edit_range.png "Editor de rangos")


Cada muestra se representa con un rectángulo semitransparente, de modo que se puede identificar la superposición de varios elementos.


### Selección de rectángulos


Un simple clic izquierdo en un rectángulo lo selecciona y anula todas las demás selecciones.
Si hay varios rectángulos superpuestos, los clics sucesivos cambian la selección bajo el cursor del ratón.
Un doble clic en un rectángulo abre la [muestra](manual/soundfont-editor/editing-pages/sample-editor.md) correspondiente (o el instrumento correspondiente en el [editor de conjuntos](manual/soundfont-editor/editing-pages/preset-editor.md)).

Se puede hacer una selección múltiple:

* manteniendo pulsada la tecla :kbd:`Ctrl` y haciendo clic en los distintos rectángulos para seleccionarlos uno a uno,
* usando la tecla :kbd:`Shift`: se seleccionan todos los rectángulos entre el primer rectángulo pulsado y el siguiente.


### Modificar los rectángulos


Tras seleccionar uno o varios rectángulos, con el botón izquierdo se puede:

* cambiar el tamaño, agarrando uno de los bordes de un rectángulo;
* cambiar la posición, apuntando al centro de un rectángulo.

Si hay varios rectángulos seleccionados, todos los cambios se hacen a la vez.
La tecla :kbd:`Esc` cancela un cambio en curso.


### Desplazamiento y zoom


Mantener pulsado el clic derecho permite cambiar el zoom alrededor del punto seleccionado.

Mantener pulsado el clic izquierdo permite desplazar el gráfico si el punto inicial no está sobre un rectángulo.

Un clic con el botón central toca una nota: la nota la da la posición horizontal del ratón y la velocidad, su posición vertical.


## Editor de envolventes {#doc_envelope}


El editor de envolventes ofrece una vista gráfica de los 8 parámetros que definen la envolvente de volumen y de los 8 parámetros que definen la envolvente de modulación.
Es, por tanto, una herramienta para entender mejor el efecto de cada uno de estos parámetros.


![Editor de envolventes](images/edit_envelope.png "Editor de envolventes")


Los botones situados encima del gráfico seleccionan alternativamente la envolvente de volumen (mostrada en verde) o la envolvente de modulación (mostrada en rojo) para editarla.
Mientras se edita la envolvente de modulación, la envolvente de volumen se muestra con una línea fina.

La parte izquierda del gráfico representa la evolución de la envolvente cuando se **pulsa** una tecla.
En esta representación intervienen 7 de los 8 parámetros:

Retraso
: Duración durante la que la envolvente se mantiene en 0 justo después de pulsar una tecla. En la mayoría de los casos queremos oír el sonido inmediatamente después de pulsar las teclas, por lo que este parámetro se deja a cero. Sin embargo, el **retraso** resulta útil, por ejemplo, para crear sonidos de pad más ricos construidos con varias capas. Supongamos que queremos introducir una segunda capa de sonido 0,5 segundos después de pulsar la tecla: basta con escribir «0.5» en este campo. Un retraso no aplaza la reproducción del sonido, sino que silencia el audio. Si el retraso es de 0,5 segundos, empezaremos a oír la muestra tal como suena tras 0,5 segundos de reproducción, ya que los primeros 0,5 segundos se han reproducido en silencio.

Ataque
: Duración, tras el **retraso**, durante la que la envolvente crece de 0 al valor máximo. Por ejemplo, un sonido de cuerda frotada como el de un violín tiene un ataque bastante largo, mientras que el de una cuerda pulsada como la de una guitarra es muy corto. Incluso cuando queremos un ataque lo más corto posible, es aconsejable indicar al menos un valor de 0.001; de lo contrario, se podría oír un «clic» al pulsar una tecla.

Mantenimiento
: Duración, tras el **ataque**, durante la que la envolvente mantiene su valor máximo.

Decaimiento
: Duración, tras el **mantenimiento**, durante la que la envolvente pasa progresivamente del valor máximo al valor indicado por el **sostenido**. Cuando no se indica ninguna reducción en el **sostenido** (0 dB, el valor por defecto), el volumen durante el **decaimiento** se mantiene al máximo, sea cual sea el valor indicado.

Sostenido
: Valor, tras el **decaimiento**, que se mantiene mientras no se suelte la tecla pulsada. Al igual que la **atenuación**, el valor de sostenido es una reducción expresada en decibelios. Si queremos reducir el volumen a la mitad tras el tiempo de decaimiento, hay que indicar un valor de 6 dB. Se puede indicar el número «144» si queremos silenciar el sonido por completo.

Nota → Mantenimiento
: Variación de la duración del **mantenimiento** según la nota (ver más abajo).

Nota → Decaimiento
: Variación de la duración del **decaimiento** según la nota (ver más abajo).

Un valor positivo en **Nota → XXX** reduce progresivamente la duración de XXX hacia los agudos, mientras que un valor negativo hace lo contrario.
Por ejemplo:

* el valor 100 divide por dos la duración de XXX por cada octava por encima del do central (60) y la duplica por cada octava por debajo del do central,
* el valor -100 duplica la duración de XXX por cada octava por encima del do central (60) y la divide por dos por cada octava por debajo del do central.

Estos parámetros pueden ser útiles para diseñar una guitarra o un piano con el pedal de resonancia pisado: cuanto más grave es la nota, más tiempo dura hasta que deja de oírse.

La parte derecha representa la evolución de la envolvente cuando se **suelta** una tecla.
En esta representación solo interviene la duración de la **liberación**.
La envolvente vuelve progresivamente a 0 durante esta duración.
Los sonidos de violines o de pads de sintetizador tienen un tiempo de liberación relativamente largo, mientras que el de otros sonidos, como los de órgano, es muy corto. En la práctica, incluso para tiempos de liberación cortos, es aconsejable indicar una duración de al menos 0,2 segundos, porque un tiempo de liberación demasiado corto no resulta agradable ni natural (aunque también depende de lo que tengamos en mente para el instrumento que queremos crear).

Cuando solo hay una división de un instrumento seleccionada en el árbol, la [muestra](manual/soundfont-editor/editing-pages/sample-editor.md) sonora correspondiente se muestra de fondo.
Así se puede visualizar mejor la evolución de la envolvente con respecto a la muestra reproducida.


### Mover los puntos


Cada punto de la curva se puede mover con el botón izquierdo:

* los finales del **retraso**, del **ataque**, del **mantenimiento** y de la **liberación** se mueven horizontalmente;
* el final del **decaimiento** se mueve en ambas direcciones: horizontalmente para el **decaimiento**, verticalmente para el **sostenido**;
* el punto en el que se suelta la tecla se mueve verticalmente, para el **sostenido**.

Los parámetros del punto bajo el cursor del ratón y sus valores se muestran en una esquina superior del gráfico.
Cuando varios puntos están en el mismo lugar, el círculo se divide en partes, desde la primera etapa a la izquierda hasta la última a la derecha.

Los parámetros **Nota → Mantenimiento** y **Nota → Decaimiento** se editan con una pequeña regla que atraviesa el final del **mantenimiento** o del **decaimiento**: su extremo superior indica la duración para la nota más aguda de la división, su extremo inferior la duración para la nota más grave.
Mover un extremo solo cambia la duración de su nota, y el otro extremo permanece en su sitio; manteniendo pulsada la tecla :kbd:`Ctrl`, ambos extremos se mueven alrededor del centro de la regla.
La regla aparece cuando el ratón se acerca al punto y sigue visible cuando el parámetro no es 0.

La tecla :kbd:`Shift` permite un ajuste más fino.
Empujar un punto contra el borde derecho del gráfico desplaza la vista o reduce el zoom, para alcanzar duraciones más largas.
La tecla :kbd:`Esc` cancela un cambio en curso.
Un doble clic en un punto restablece sus parámetros (solo **Nota → Mantenimiento** o **Nota → Decaimiento** para un extremo de la regla): vuelve a aplicarse el valor de la división :guilabel:`Global` o el valor por defecto.

Si hay varias divisiones o instrumentos seleccionados, todas sus envolventes se modifican a la vez: las duraciones en la misma proporción, los demás valores en la misma cantidad.


### Desplazamiento y zoom


Mantener pulsado el clic derecho permite cambiar el zoom alrededor del punto seleccionado.

Mantener pulsado el clic izquierdo permite desplazar el gráfico si el clic no empieza sobre un punto de la curva.


## Vista de timbre {#doc_timbre}


La vista de timbre muestra, a lo largo de todo el teclado, el timbre de un instrumento **tal como se oye**: el nivel de cada armónico, una vez transpuesta cada muestra a las notas que reproduce y una vez aplicados la atenuación, el filtro y los moduladores de su división.
Las irregularidades se hacen visibles, en particular las rupturas en las uniones entre muestras.
Desde esta vista se pueden crear notas calculadas, eliminar muestras redundantes y corregir el timbre o el nivel de una muestra.
La vista mide y propone; no se aplica nada automáticamente.


![Vista de timbre](images/edit_timbre.png "Vista de timbre")


### Divisiones tenidas en cuenta


La vista muestra lo que suena a la velocidad elegida arriba a la izquierda (100 por defecto, y luego la última utilizada).
La barra contigua muestra las capas de velocidad del instrumento: un clic en una capa selecciona la velocidad situada en su centro.

Las divisiones tenidas en cuenta son las que suenan a esta velocidad, que no están [silenciadas](#doc_solo) y que no se reproducen solo al soltar la tecla (modo de reproducción «release»).
Silenciar una división es, por tanto, la forma de excluirla de la vista, por ejemplo un ruido o un sonido de release.
Cuando el [solo en la selección](manual/soundfont-editor/tree.md#doc_menu) está activado, el silenciado sigue a la selección y no se tiene en cuenta.
Las divisiones que reproducen una misma nota (par estéreo, capas) se combinan: sus potencias se suman.

Una **nota raíz** agrupa todas las divisiones que tienen esa nota raíz, posiblemente con varios rangos de notas.
Las muestras se analizan en segundo plano (el número de muestras restantes se muestra arriba a la derecha): los armónicos se miden en el bucle, o en la parte estable del sonido si no hay bucle.
Estos análisis se conservan mientras no cambien los datos, el bucle, la nota raíz o la corrección de afinación de una muestra.


### Mapa


Cada columna es una nota, alineada con el teclado mostrado debajo, y cada fila un armónico (H1: fundamental, H2: segundo armónico...), con el armónico 1 abajo, como en un espectrograma.
Los primeros armónicos se muestran uno a uno, y los siguientes se agrupan en bandas (ver [Parciales](#doc_timbre_partials)).

Encima del mapa, unas filas de resumen muestran para cada nota:

* :guilabel:`Nivel`: nivel total,
* :guilabel:`Timbre`: desviación de timbre de la nota raíz respecto a su referencia (rayada cuando la referencia es menos fiable, en los extremos del teclado),
* :guilabel:`Transposición`: azul cuando la muestra se transpone hacia abajo, rojo cuando se transpone hacia arriba,
* y, a elegir en el menú :guilabel:`Opciones` > :guilabel:`Resúmenes`: :guilabel:`Brillo` (centro de gravedad de los armónicos), :guilabel:`Impares / pares` (armónicos impares comparados con los pares), :guilabel:`Ruido` (relación armónicos / ruido), :guilabel:`Afinación` (centésimas respecto a la nota más cercana del temperamento igual) y :guilabel:`Bucle` (calidad del bucle).

En el mapa, unas líneas verticales marcan las uniones entre divisiones (más gruesas cuando cambia la muestra), un punto marca cada nota raíz, un rayado muestra los armónicos ausentes (fuera de la banda de la muestra o de la salida, por encima del límite superior) o medidos de forma poco fiable, y las notas que no reproducen nada aparecen en gris.
Una información emergente da los valores de la celda bajo el cursor del ratón: nota, armónico, nivel, desviación, muestras, transposición y factor de duración (el ataque, el trémolo y los batimientos son más rápidos cuando una muestra se transpone hacia arriba).

La lista :guilabel:`Visualización` elige lo que representan los colores:

Niveles absolutos
: Nivel de cada armónico en dB.

Niveles relativos
: Nivel de cada armónico respecto al más fuerte de la nota: el timbre independientemente del nivel.

Desviación respecto a la referencia
: Diferencia entre la nota raíz y su referencia para cada armónico, en todas las notas que reproduce la nota raíz: demasiado fuerte (rojo) o demasiado débil (azul).

Saltos
: Diferencia con la nota anterior: las rupturas en las uniones.

Transposición
: Semitonos entre la nota y la nota raíz de su muestra.

Confianza
: Parte de la potencia que se mide de forma fiable.

El menú :guilabel:`Opciones` también permite mostrar todo el teclado o una sola columna por nota raíz, colorear por escalones según los umbrales (las desviaciones dentro de la tolerancia quedan neutras) y reproducir las notas en las que se hace clic.
El teclado bajo el mapa reproduce el instrumento a la velocidad de la vista.


### Referencia y decisiones


La **referencia** de una nota raíz es lo que predicen sus vecinas: las notas raíz vecinas se reproducen en su nota (con los parámetros de sus divisiones) y sus niveles se interpolan armónico por armónico, en dB.
En los dos extremos del teclado, la referencia se extrapola a partir de las dos vecinas del mismo lado, lo que es menos fiable.
Con :guilabel:`Regresión local`, se usa en su lugar una regresión robusta sobre varias vecinas a cada lado, para que una vecina defectuosa pese menos.

La desviación de una nota raíz respecto a su referencia se resume con dos números:

* la desviación de **nivel**, diferencia de los niveles totales, que se puede corregir con la atenuación,
* la desviación de **timbre**, media de las diferencias de las filas una vez eliminada la desviación de nivel, donde cada fila pesa según su sonoridad (un armónico débil cuenta, sin dominar).

La franja :guilabel:`Decisiones` da una etiqueta a cada nota raíz y a cada unión, con una forma y un color:

Coherente ✓ (verde)
: La desviación de timbre está dentro de la tolerancia.

Redundante − (azul)
: La nota raíz es coherente, y eliminarla cambiaría sus notas menos que el umbral :guilabel:`Redundancia`, sin ruptura en la nueva unión: se puede eliminar.

Por corregir ~ (naranja)
: La desviación de timbre está entre la tolerancia y el máximo: se puede corregir el timbre.

Demasiado diferente × (rojo)
: La desviación de timbre supera el máximo: la nota raíz se puede eliminar o reemplazar por una nota calculada.

Ruptura + (morado)
: En un cambio de muestra, el salto de timbre supera el umbral :guilabel:`Ruptura` y existe una nota sin muestra entre las dos notas raíz: se puede crear allí una nota calculada.

Nivel ▲ ▼ (amarillo)
: La desviación de nivel supera el umbral :guilabel:`Nivel` (▲ demasiado fuerte, ▼ demasiado débil): se puede corregir la atenuación.

Los umbrales se ajustan en el menú :guilabel:`Umbrales`.
Sus valores por defecto (:guilabel:`Tolerancia` 4,5 dB, :guilabel:`Máximo` 8 dB, :guilabel:`Redundancia` 2,5 dB, :guilabel:`Ruptura` 6 dB, :guilabel:`Nivel` 2,5 dB) proceden de bancos de muestras de órgano, en los que el 70 % de las notas raíz se desvían menos de 4,5 dB de sus vecinas: se pueden adaptar al instrumento.
Una unión o una desviación puede ser intencionada (un instrumento que se convierte en otro, la repetición de una mixtura): las etiquetas solo son propuestas.


### Parciales {#doc_timbre_partials}


El menú :guilabel:`Parciales` define las filas del mapa, compartidas por la visualización, las desviaciones y las correcciones:

Armónicos mostrados uno a uno
: Número de armónicos mostrados y comparados uno a uno (10 por defecto). El oído distingue individualmente los primeros armónicos, mientras que los siguientes se perciben en conjunto como brillo.

Armónicos siguientes
: Los armónicos siguientes se agrupan en bandas de un tercio de octava (11-12, 13-16, 17-20...) o de una octava, para que muchos armónicos agudos y débiles no pesen más que los primeros.

Límite superior
: Los parciales por encima de esta frecuencia se ignoran (16 kHz por defecto, o la mitad de la frecuencia de muestreo).

Rango dinámico
: Los armónicos situados más de este valor por debajo del más fuerte de la nota no se comparan (60 dB por defecto).


### Detalles y acciones


Un clic en una nota muestra bajo el mapa lo que reproduce: las muestras con su transposición, atenuación y filtro, el factor de duración, el tono, las desviaciones de la nota raíz y lo que cambiaría su eliminación.
El gráfico de barras compara los niveles oídos con la referencia, o con la nota calculada que podría recibir una nota sin muestra (una columna con trazo discontinuo en el mapa).

Un clic en una nota raíz selecciona sus divisiones en el [árbol](manual/soundfont-editor/tree.md), y seleccionar divisiones en el árbol selecciona sus notas raíz.
Se pueden seleccionar varias notas raíz con :kbd:`Ctrl` o :kbd:`Shift`, o arrastrando el ratón sobre las notas.

Los botones de la derecha y el menú contextual del mapa se aplican a las notas raíz seleccionadas:

Crear una nota aquí...
: En una nota sin muestra: se transpone la nota raíz más cercana y los niveles de sus armónicos se interpolan (o se extrapolan) a partir de una segunda nota raíz. Se pueden elegir las notas raíz utilizadas, y se muestra una advertencia cuando la nota está lejos de ellas. Los rangos de notas de alrededor se ajustan; las divisiones contenidas por completo en el rango de la nueva nota se eliminan.

Eliminar...
: Las divisiones de la nota raíz se eliminan y sus vecinas pasan a reproducir sus notas. Las muestras que ya no se usan también se pueden eliminar.

Reemplazar por una nota calculada...
: La nota raíz se elimina y luego se vuelve a crear a partir de sus vecinas.

Corregir el timbre...
: Abre la ventana de [corrección](#doc_timbre_correction).

Corregir el nivel
: La atenuación de las divisiones se modifica para que la desviación de nivel sea nula (una atenuación no puede ser negativa).

Verificar mediante renderizado
: El motor de sonido reproduce la nota y se miden los armónicos del resultado: se muestran en el gráfico de barras para verificar el cálculo de la vista.

Silenciar estas divisiones
: En el menú contextual: excluye las divisiones de la vista.

El menú :guilabel:`Proponer` selecciona todas las notas raíz (o uniones) que tienen una etiqueta y les aplica la acción correspondiente, tras una confirmación: corregir las notas raíz por corregir, reemplazar las demasiado diferentes, eliminar las redundantes, crear notas calculadas en las rupturas, corregir los niveles que se desvían.
Se procesan de la peor a la menos mala, y las referencias se vuelven a calcular tras cada acción: una nota raíz defectuosa también falsea la referencia de sus vecinas, que parecen medio defectuosas hasta que se corrige.

Cada acción, o serie de acciones, se puede deshacer en un solo paso (:kbd:`Ctrl`+:kbd:`Z`).


### Corrección del timbre {#doc_timbre_correction}


![Corrección del timbre](images/timbre_correction.png "Corrección del timbre")


La ventana muestra para cada fila los niveles oídos, el objetivo y el resultado esperado, y ofrece los siguientes parámetros:

Referencia
: Objetivo de la corrección: :guilabel:`Interpolación entre las vecinas`, :guilabel:`Regresión local` sobre varias vecinas a cada lado, o :guilabel:`Alineación con una nota raíz` (otra nota raíz reproducida en la nota).

Corregir el timbre (datos de la muestra)
: La ganancia de cada fila se aplica a los datos de las muestras con una curva de fase mínima que sigue el tono: la corrección sigue siendo válida en todas las notas de la división, sea cual sea la transposición, y el bucle se conserva. La misma corrección se aplica a las dos muestras de un par estéreo.

Corregir el nivel (atenuación)
: La desviación de nivel se corrige mediante la atenuación de las divisiones, lo que no modifica los datos. Si la atenuación pasara a ser negativa, el resto se aplica a los datos, dentro del límite de la escala completa.

Tolerancia
: Una desviación menor en una fila no se corrige; solo se corrige el exceso.

Intensidad
: Proporción de la desviación que se corrige.

Corrección máxima
: Mayor ganancia de una fila, en ambos sentidos.

Armónicos corregidos
: Todas las filas, o solo los armónicos del 1 a un armónico dado.

Brillo, Armónicos impares
: Pendiente añadida al objetivo (positiva para un sonido más brillante) y nivel añadido a los armónicos impares 3, 5, 7... del objetivo.

La sección :guilabel:`Escucha` compara el sonido original (A) y el corregido (B) con el botón :guilabel:`Reproducir`.
Si la corrección superara la escala completa, la muestra se baja y la atenuación lo compensa.
Cuando una muestra también se usa en otras divisiones o instrumentos, Polyphone pregunta si hay que duplicarla para las notas raíz corregidas o modificarla en todas partes (la respuesta se puede recordar hasta cerrar Polyphone).


## Moduladores {#doc_modulator}


La sección inferior de la página de edición de los instrumentos está destinada a crear moduladores, de forma global para un instrumento o para una división concreta.
Cuando tocas un instrumento, un modulador escucha una o dos señales MIDI y modifica uno de los parámetros de la tabla según la regla matemática configurada.
La salida de un modulador también se puede usar como entrada de otro modulador (esta función apareció en la versión 2.04 del [formato sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2) y puede que algunos sintetizadores no la admitan).


![Editor de moduladores](images/modulator_editor_2.png "Editor de moduladores")


Los botones de la izquierda sirven para:

* expandir / contraer la zona de moduladores,
* añadir / eliminar un modulador,
* copiar / pegar / duplicar uno o varios moduladores.

Cuando se añade un modulador, el borde de la celda correspondiente de la tabla se vuelve más grueso (ver la afinación en la columna :guilabel:`Global` más arriba).

Los moduladores de una división (o de la división global) solo se muestran si hay una única división seleccionada.
Cuando la sección está contraída, un resumen indica el número de moduladores y los parámetros que modifican.
Al seleccionar celdas en la tabla se seleccionan los moduladores que actúan sobre los parámetros correspondientes, y viceversa.

Cada modulador se compone de los siguientes elementos, de izquierda a derecha:

Fuentes
: La primera fuente es una señal MIDI: la velocidad o la nota tocada, la presión polifónica, la presión de canal, la rueda de tono, la sensibilidad de la rueda de tono o un controlador MIDI (:guilabel:`CC`).
  También puede ser la salida de uno o varios moduladores (:guilabel:`Enlace`), los que tienen este modulador como destino.
  La segunda fuente es opcional y modifica el efecto de la primera.

Curvas
: Junto a cada fuente, un botón da acceso a una cuadrícula de curvas para elegir cómo se convierte la señal: lineal, cóncava, convexa o en escalón, ascendente o descendente, unipolar (de 0 a 1) o bipolar (de -1 a 1).

Cantidad
: Amplitud máxima de la modulación.

Transformación
: :guilabel:`ninguna operación` o :guilabel:`valor absoluto` (se usa el valor absoluto del resultado de la modulación).

Destino
: El parámetro que modifica el modulador, u otro modulador (:guilabel:`Modulador`).
  El texto bajo el destino indica el rango de la modulación: los valores que se suman al parámetro (:guilabel:`Sumar desde` … :guilabel:`Hasta`) o los factores que se le aplican (:guilabel:`Multiplicar desde` … :guilabel:`Hasta`).

Cuando un modulador es idéntico a otro situado más arriba en la lista, se muestra la indicación :guilabel:`sobrescrito por`.
A nivel de instrumento, la indicación :guilabel:`reemplaza mod. por defecto` señala un modulador que reemplaza a uno de los [moduladores por defecto](manual/soundfont-editor/tools/instrument-tools.md#doc_default_mod) del formato sf2, y :guilabel:`desactiva mod. por defecto` indica que su cantidad es 0 (el modulador por defecto queda entonces desactivado).
