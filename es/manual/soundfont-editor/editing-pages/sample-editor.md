Al hacer clic en una o varias muestras en el [árbol](manual/soundfont-editor/tree.md), aparece la página de edición de las muestras.

Esta página se divide en varias partes:

* un [gráfico](#doc_graph) que representa la muestra,
* una zona de [información](#doc_info),
* un análisis de las [frecuencias](#doc_freq),
* un [ecualizador](#doc_eq),
* un [reproductor](#doc_player) de muestras.


![Página de la muestra](images/edit_sample.png "Página de la muestra")


## El gráfico {#doc_graph}


### Zoom y desplazamiento


El gráfico permite visualizar la forma de onda.
Se puede hacer zoom vertical u horizontal manteniendo pulsado el botón derecho del ratón y arrastrando:

* arrastrar hacia la derecha amplía el eje horizontal,
* arrastrar hacia la izquierda reduce el eje horizontal,
* arrastrar hacia arriba amplía el eje vertical,
* arrastrar hacia abajo reduce el eje vertical.

Mantener pulsado el botón izquierdo del ratón permite desplazar el gráfico horizontalmente, si se ha ampliado el eje horizontal.
La rueda del ratón también desplaza el gráfico horizontalmente.


### Posiciones del bucle y de la reproducción


Una barra vertical verde indica la posición de inicio del bucle (se cambia con un clic izquierdo en el gráfico) y una barra roja indica la posición de fin del bucle (se cambia con un clic derecho en el gráfico).
A cada barra vertical se asocia una línea de puntos que representa una porción de la onda, útil para visualizar la superposición del principio y del final del bucle.

Durante la reproducción, una barra vertical blanca recorre el gráfico e indica la posición de reproducción en tiempo real.


### Cortar la muestra


Manteniendo pulsada la tecla :kbd:`Alt`, se puede definir una zona que cortar:

* al pulsar el botón izquierdo del ratón se define el inicio de la zona,
* al soltarlo se define el final de la zona.

Una vez definida la zona, un cuadro de diálogo pide confirmación antes de cortar la muestra.
La respuesta :guilabel:`Sí y crear otra muestra` corta la muestra y además crea una nueva muestra a partir de la parte eliminada.


![Cortar la muestra](images/cutting_sample.png "Cortar la muestra")


### Colores del gráfico


Los colores del gráfico se pueden modificar en las [preferencias](manual/settings.md#doc_interface) del programa.


## Sección «Información» {#doc_info}


La sección :guilabel:`Información` permite ver —y, en la mayoría de los casos, editar— la siguiente información:

Tamaño
: Se muestran el número de valores y la duración correspondiente en segundos.

Bucle
: Aquí se pueden modificar los puntos de inicio y de fin del bucle.
  Un botón ajusta las posiciones de inicio y de fin para que el bucle abarque toda la muestra.

Afinación
: Permite ajustar la nota raíz de una muestra y su corrección en centésimas para que el sonido quede afinado en el temperamento igual.
  Un botón ajusta la nota raíz y la corrección para que los valores coincidan con la [frecuencia](#doc_freq) estimada.

Frecuencia de muestreo
: Cualquier modificación provoca un remuestreo para que la altura del sonido no se altere.

Canal
: Permite elegir el tipo entre :guilabel:`mono`, :guilabel:`izquierda`, :guilabel:`derecha` o :guilabel:`enlace`.
  Si el tipo es :guilabel:`izquierda`, :guilabel:`derecha` o :guilabel:`enlace`, se puede anular el estéreo seleccionando :guilabel:`mono`.
  Cuando :guilabel:`mono` está seleccionado, para volver a un estado estéreo hay que seleccionar una muestra enlazada.

Enlace
: Selección de la muestra que se enlaza para que el sonido sea estéreo.
  Si se selecciona una muestra, su tipo pasará a :guilabel:`enlace` (este tipo se puede cambiar luego a :guilabel:`izquierda` o :guilabel:`derecha`).

Si la [compresión](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md#doc_edit) está activada para el soundfont ([formato sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3)), se muestra un botón de compresión para las muestras que aún no están comprimidas.
Al comprimir, se pide en un cuadro de diálogo elegir la calidad de compresión (:guilabel:`Alta`, :guilabel:`Media` o :guilabel:`Baja`).
La compresión puede alterar el bucle de una muestra.\
**Nota:** la descompresión se hace automáticamente al editar la muestra (aplicar un filtro, recortar, amplificar, etc.).

![Selección de la calidad de compresión](images/sample_compress_dialog.png "Selección de la calidad de compresión")

Cualquier cambio en una muestra estéreo se puede aplicar a la muestra enlazada si la opción :guilabel:`Edición estéreo` está marcada en las [preferencias](manual/settings.md#doc_general) (o con el botón situado en la esquina inferior derecha de la ventana).
Mediante una selección múltiple en el [árbol](manual/soundfont-editor/tree.md), se pueden editar varias muestras a la vez.


## Análisis de frecuencias {#doc_freq}


El análisis de frecuencias incluye una transformada de Fourier (gráfico que muestra la intensidad de las frecuencias contenidas en la señal).

![Gráfico de Fourier](images/fourier_graph.png "Gráfico de Fourier")

A la derecha se muestra la lista de los picos dominantes con la siguiente información:

* relación entre la intensidad del pico y la intensidad del pico máximo,
* frecuencia,
* nota correspondiente más cercana,
* corrección (distancia a la nota correspondiente más cercana).

A partir de estos datos se hace una estimación de la altura del sonido.
Esta estimación se puede usar para la afinación.

Un clic derecho en el gráfico abre un menú contextual que permite:

* exportar el gráfico a una imagen .png,
* mostrar la lista de todos los picos y un botón para exportar esa lista a un archivo .csv.

![Mostrar picos de frecuencia](images/tool_show_peak_frequencies.png "Mostrar picos de frecuencia")


## Ecualizador {#doc_eq}


El ecualizador permite aumentar o reducir algunas frecuencias, de -15 a 15 dB.
Se puede escuchar el efecto con la opción :guilabel:`vista previa` activada y luego aplicarlo definitivamente pulsando el botón :guilabel:`Aplicar`.
Al pulsar :guilabel:`Restablecer`, todos los deslizadores vuelven a 0 dB (pero no se elimina un efecto ya aplicado al sonido: para ello, usa el botón :guilabel:`Deshacer` de la [barra de herramientas](manual/soundfont-editor/toolbar.md#doc_edit)).

Cualquier cambio en una muestra estéreo se puede aplicar automáticamente a la muestra enlazada si la opción está marcada en las [preferencias](manual/settings.md#doc_general).


## Reproductor {#doc_player}


El reproductor reproduce el sonido.
Durante la reproducción, un cursor blanco recorre el gráfico para indicar la posición de reproducción.
Las opciones de reproducción son:

Bucle
: Reproduce el sonido en bucle si el principio y el final del bucle están definidos.

Estéreo
: Reproduce la muestra en estéreo, con su muestra enlazada.
  Si también se pide el bucle, se usan los dos bucles respectivos (pueden ser distintos).

Seno
: Se añade al sonido una señal de calibración sinusoidal, que permite afinar una muestra con una precisión de una centésima de semitono.
  Primero hay que indicar la nota raíz.
  En ese punto, se pueden oír batimientos si la frecuencia del seno difiere ligeramente de la frecuencia fundamental del sonido.
  Solo queda ajustar con precisión la corrección para que los batimientos sean lo más lentos posible, de modo que la muestra quede lo mejor afinada posible.
  Consulta el tutorial «[Cómo preparar una muestra](tutorials/how-to-prepare-a-sample.md)» para más información.

Un deslizador ajusta el volumen de la reproducción (este volumen no se guarda en el soundfont).

Si hay una reproducción en curso al pasar de una muestra a otra mediante el [árbol](manual/soundfont-editor/tree.md), se reinicia automáticamente.
Además, pulsar la tecla :kbd:`Space` inicia o detiene la reproducción si el foco está en el árbol.