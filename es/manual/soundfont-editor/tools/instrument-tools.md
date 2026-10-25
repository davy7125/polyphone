Cuando hay un instrumento o una de sus divisiones seleccionado en el [árbol](manual/soundfont-editor/tree.md), el [menú de herramientas](manual/soundfont-editor/tools/index.md) contiene varias herramientas ordenadas en 4 categorías:

* [análisis](#doc_cat_analyze),
* [edición rápida](#doc_cat_fast),
* [moduladores](#doc_cat_mod),
* [transformación](#doc_cat_transformation).

Se pueden usar las herramientas en varios instrumentos a la vez (con una selección múltiple), salvo para crear acordes y mixturas y para mostrar parámetros.


## Análisis {#doc_cat_analyze} 


### Mostrar parámetros {#doc_display}


Esta herramienta muestra la evolución de un parámetro según la nota.
La leyenda distingue el valor definido en la división global, los valores definidos en las divisiones y el valor por defecto del parámetro.
Hay disponible una escala logarítmica (¡cuidado con los valores menores o iguales a 0!).


![Herramienta para mostrar parámetros](images/tool_display_parameters.png "Herramienta para mostrar parámetros")


## Edición rápida {#doc_cat_fast}


### Cambiar atenuaciones {#doc_attenuation}


La herramienta calcula las atenuaciones mínima y máxima en todos los instrumentos seleccionados y permite añadir un desplazamiento a todos los valores de atenuación sin superar los límites.

Por ejemplo, si la atenuación más baja de las divisiones de los instrumentos seleccionados es de 16 dB y la atenuación mínima deseada es de 5 dB (para aumentar el volumen general), la atenuación de todas las divisiones de todos los instrumentos se puede reducir en 11 dB.


![Herramienta de cambio de atenuación](images/tool_change_attenuation.png "Herramienta de cambio de atenuación")


### Desafinar {#doc_detune}


Esta función desafina ligeramente las notas del instrumento para crear un efecto ondulante junto con un instrumento afinado.
La intensidad del efecto se expresa en número de batimientos por segundo y se puede ajustar:

* número de batimientos por segundo en la nota 60, que es el «do» del centro del teclado (el signo de este número determina el sentido de la desafinación);
* evolución del número de batimientos según la nota, dividiendo o multiplicando el número de batimientos por un factor constante de una octava a otra (el valor «1» significa que el número de batimientos será constante en todo el teclado).


![Herramienta de desafinación](images/tool_detune.png "Herramienta de desafinación")


Este es un ejemplo de un instrumento <a href="files/celeste_tuning_off.mp3" download>afinado</a>:

![](files/celeste_tuning_off.mp3)

Y este instrumento se ha combinado consigo mismo en una versión <a href="files/celeste_tuning_on.mp3" download>desafinada</a>:

![](files/celeste_tuning_on.mp3)

El archivo .sf2 correspondiente se puede descargar <a href="files/celeste_tuning.sf2" download>aquí</a>.


### Configuración basada en notas {#doc_global}


Esta herramienta permite editar un parámetro en todas las divisiones de un instrumento a la vez, según el rango de notas de cada división.


![Herramienta de configuración basada en notas](images/tool_keybased_configuration.png "Herramienta de configuración basada en notas")


El gráfico representa la intensidad de la modificación: el primer valor a la izquierda corresponde al cambio aplicado a la división que contiene la nota 0, y el último valor a la derecha corresponde al cambio aplicado a la división que contiene la nota 127.
Los valores mínimo y máximo del gráfico se pueden ajustar.
Al cambiar el patrón, se puede editar la curva:

Manual
: La curva se dibuja con el ratón.

Lineal ascendente / Lineal descendente
: La curva es una recta.

Exponencial ascendente / Exponencial descendente
: La curva es una exponencial cuya curvatura se define con el parámetro :guilabel:`Curvatura`.

Aleatorio
: Los valores se generan al azar, según el parámetro :guilabel:`Distribución`.

Si se usa una ley lineal o exponencial, un clic izquierdo en el gráfico define el inicio de la zona lineal o exponencial y un clic derecho define el final.

El parámetro que se modifica se elige en :guilabel:`Parámetro`, y el tipo de modificación se elige en :guilabel:`Modificación`:

Suma
: El modificador se suma al valor del parámetro seleccionado.

Multiplicación
: El modificador se multiplica por el valor del parámetro seleccionado.

Reemplazo
: El modificador reemplaza el valor del parámetro seleccionado.

Se pueden aplicar los cambios solo a las divisiones cuyo rango de velocidad esté comprendido en un rango concreto, lo que resulta útil, por ejemplo, cuando un piano tiene varias capas de velocidad.


### Release natural {#doc_release}


Esta herramienta crea un release para cada división del instrumento.
Los tiempos de release se calculan teniendo en cuenta los dos primeros parámetros (la duración del release para la nota 36 y el factor entre dos octavas consecutivas, :guilabel:`División a la siguiente octava`), y se puede añadir una modificación de la altura con el tercero (:guilabel:`Desafinación inducida`).


![Herramienta de release natural](images/tool_natural_release.png "Herramienta de release natural")


### Posicionamiento automático de muestras {#doc_position}


El programa determina los rangos de notas de todas las divisiones de un instrumento para optimizar su uso en todo el teclado.
Esto también se hace automáticamente cuando se enlazan muestras con notas raíz distintas a un instrumento vacío.


### Espacialización del sonido {#doc_spatialization}


Esta función espacializa los sonidos según su altura, lo que puede ser útil para un piano, por ejemplo (graves y agudos de izquierda a derecha).


![Herramienta de espacialización del sonido](images/tool_sound_spatialization.png "Herramienta de espacialización del sonido")


Cada barra del gráfico representa una cuerda o un tubo, con una longitud relacionada con la altura.
La barra más corta corresponde a la nota más aguda y la barra más larga corresponde a la nota más grave.

Cada nota se sitúa en el espacio: el extremo izquierdo corresponde a un sonido que viene de la izquierda y el extremo derecho a un sonido que viene de la derecha.
Se pueden elegir varios patrones:

Ascendente
: Los agudos están a la izquierda y los graves a la derecha.

Descendente
: Los graves están a la izquierda y los agudos a la derecha.

Valle
: Los graves están en los dos extremos y los agudos en el centro.

Pico
: Los agudos están en los dos extremos y los graves en el centro.

Aleatorio
: Los agudos y los graves se sitúan al azar.

Hay otras opciones disponibles:

* número de repeticiones del patrón (:guilabel:`Número de divisiones`),
* intensidad del efecto (:guilabel:`Extensión`),
* llenado de una división (:guilabel:`Llenado`),
* desfase izquierda — derecha si la extensión no es del 100 % (:guilabel:`Desfase`),
* :guilabel:`Inversiones`.

El gráfico se actualiza cada vez que cambia un ajuste, para visualizarlo mejor.
Al pasar el ratón por encima, se muestra la correspondencia número de nota / paneo.


### Transportar {#doc_transpose}


La herramienta de transporte permite transportar un instrumento completo.
Los sonidos del instrumento siguen siendo los mismos, pero en otra posición del teclado.

Como entrada se necesita un desplazamiento en semitonos.
Para que la altura sea más aguda, el desplazamiento debe ser positivo.
A la inversa, para que la altura sea más grave, el desplazamiento debe ser negativo.
Las operaciones que realiza la herramienta, para cada división del instrumento, son entonces las siguientes:

* se ajusta la nota raíz,
* se modifica la afinación fina si el transporte se hace con un número de semitonos con decimales,
* se desplaza el rango de notas según el número de semitonos si :guilabel:`Adaptar rango de notas` está marcado.


![Herramienta de transporte](images/tool_transpose_inst.png "Herramienta de transporte")


## Moduladores {#doc_cat_mod}


### Anular el modulador por defecto {#doc_default_mod}


A cada instrumento se le asignan automáticamente moduladores por defecto.
Esta herramienta muestra la lista de esos moduladores y puede crear moduladores que anularán o desactivarán los seleccionados (botones :guilabel:`Anular` y :guilabel:`Deshabilitar`).


![Herramienta de anulación](images/tool_default_mod.png "Herramienta de anulación")


### Eliminar moduladores {#doc_remove_mod}


Se eliminarán todos los moduladores de todos los instrumentos seleccionados.
Un mensaje indica cuántos moduladores se han eliminado.

**Nota:** los moduladores por defecto seguirán aplicándose.
La única forma de desactivar un modulador por defecto es anularlo con la herramienta «[Anular el modulador por defecto](#doc_default_mod)».


## Transformación {#doc_cat_transformation}


### Crear acordes {#doc_chords}


A partir de un instrumento, esta herramienta permite crear todos los acordes posibles.
Puede ser interesante, por ejemplo, para crear un coro o pads.

El acorde se define eligiendo qué notas contiene (nota raíz, tercera, quinta, séptima y novena) y su calidad (mayor, menor, justa, disminuida, aumentada), el número de inversiones y la octava.
Las muestras que contienen los acordes se crean en cada nota, cada 3 notas o cada 6 notas, con un bucle automático opcional, en mono o en estéreo.


![Herramienta para crear acordes](images/tool_create_chords.png "Herramienta para crear acordes")


Este es un ejemplo, <a href="files/chord_base.mp3" download>antes</a>:

![](files/chord_base.mp3)

Y <a href="files/chord_done.mp3" download>después</a>:

![](files/chord_done.mp3)

Se puede descargar un archivo .sf2 de ejemplo, basado en una flauta, <a href="files/chords.sf2" download>aquí</a>.


### Duplicación de las divisiones {#doc_division}


Esta acción duplica todas las divisiones de un instrumento.
Por ejemplo, si una muestra enlazada a un instrumento tiene un rango de notas de 36-38, el programa duplica esta división para tener esa muestra enlazada en los rangos 36-36, 37-37 y 38-38.
Así se puede editar a fondo cada parámetro de la muestra para cada nota, en lugar de editar divisiones enteras con los mismos parámetros.

También se pueden duplicar las divisiones según los rangos de velocidad.
Un instrumento sin muestras no es compatible con esta herramienta.


![Herramienta de duplicación de divisiones](images/tool_division_duplication.png "Herramienta de duplicación de divisiones")


### Creación de mixturas {#doc_mixture}


Específica del órgano, esta herramienta permite crear una mixtura a partir de un instrumento existente.
Aparece una interfaz para indicar las distintas divisiones de la mixtura y, para cada una, la lista de hileras.
Los tipos de hileras posibles son muy variados, desde la octava hasta el 27.º armónico.


![Herramienta de creación de mixturas](images/tool_mixture.png "Herramienta de creación de mixturas")


Para funcionar, esta herramienta se basa en todas las muestras asociadas al instrumento seleccionado, en el rango definido para cada muestra y en la atenuación correspondiente.
Las muestras deben estar afinadas con independencia de la definición del instrumento (mediante el [editor de muestras](manual/soundfont-editor/editing-pages/sample-editor.md)), y la nota raíz indicada no debe tener en cuenta la pertenencia a un registro (sin transporte si el sonido es un principal de 4', por ejemplo).

Una opción permite el bucle automático de las muestras creadas, y otra define el intervalo de creación de las muestras (en cada nota, cada 3 notas o cada 6 notas).
Por último, se puede elegir el tipo de los sonidos creados: mono o estéreo.

Este es un ejemplo, <a href="files/mixture_base.mp3" download>antes</a>:

![](files/mixture_base.mp3)

Y <a href="files/mixture_done.mp3" download>después</a>:

![](files/mixture_done.mp3)

Se puede descargar un archivo .sf2 de ejemplo, basado en un registro de órgano, <a href="files/mixture.sf2" download>aquí</a>.
