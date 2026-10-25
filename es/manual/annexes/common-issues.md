Esta página enumera los defectos habituales que se encuentran en los soundfonts (que pueden señalarse en las valoraciones) y, para cada uno de ellos, ofrece técnicas y enlaces a la documentación para corregirlos:

* [muestras de mala calidad o mal preparadas](#doc_sample),
* [afinación incorrecta](#doc_tuning),
* [envolvente ADSR inadecuada](#doc_adsr),
* [bucles ausentes o imprecisos](#doc_loop),
* [estándar MIDI no respetado](#doc_standard).


## Muestras de mala calidad o mal preparadas {#doc_sample}


Puede ocurrir que un soundfont no ofrezca instrumentos convincentes; si la causa está en muestras de mala calidad, la única solución es reemplazarlas:

* buscar otras muestras en internet,
* grabar con un micrófono de mejor calidad (o usar herramientas externas de procesamiento de la señal),
* crear muestras por otros medios.

A veces el problema se puede resolver preparando o usando mejor las muestras existentes.

### Mala calidad de sonido, sobre todo en los agudos

Normalmente, una sola muestra de audio no basta para crear un instrumento de buena calidad. Una muestra tiene su propia altura (salvo en la percusión no melódica), y usarla en un rango amplio de notas deforma el sonido, sobre todo en las notas más agudas. Para corregir este problema, hay que usar varias muestras o, si solo se dispone de una, se puede:

* duplicar la muestra,
* transportar la muestra duplicada una octava, por ejemplo, con esta [herramienta](manual/soundfont-editor/tools/sample-tools.md#doc_transpose),
* filtrar los armónicos agudos con esta [herramienta](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq) o con [esta otra](manual/soundfont-editor/tools/sample-tools.md#doc_filter).

Después hay que repartir las muestras en los rangos de notas en los que su reproducción no altera el timbre original.

### El sonido contiene «clics» regulares

El problema viene probablemente de un punto de bucle mal colocado, tema que se trata en otra [sección](#doc_loop).

### Muestras mal recortadas

A menudo las muestras solo se usan en parte, y eliminar lo sobrante puede reducir el tamaño del archivo:

* la parte útil de una muestra a veces se encuentra antes del punto final del bucle: entonces se puede eliminar la parte de la muestra posterior al final del bucle, y esta [herramienta](manual/soundfont-editor/tools/sample-tools.md#doc_trimloop) permite hacerlo en unos pocos clics;
* las muestras pueden empezar con un silencio, que conviene eliminar para reducir la latencia al disparar una nota (también hay una [herramienta](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank) para hacerlo rápidamente);
* por último, puede que algunas muestras simplemente no se usen: este [resumen de configuración](manual/soundfont-editor/configuration-summaries.md#doc_sample) las enumera y esta [herramienta](manual/soundfont-editor/tools/global-tools.md#doc_unused) las elimina automáticamente.


## Afinación incorrecta {#doc_tuning}


Afinar un instrumento virtual consta de varias etapas, y es importante pasar por todas ellas, en orden.

### Afinación a nivel de muestra

Es la etapa de afinación más importante y se describe en esta [página](tutorials/how-to-prepare-a-sample.md#doc_tuning).

### Afinación a nivel de instrumento

4 parámetros, descritos en esta [página](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table), sirven para definir el transporte con el que se reproduce una muestra: **nota raíz**, **afinación (semitonos)**, **afinación (centésimas)** y **afinación (escala)**. Estos valores pueden modificarse durante la interpretación del instrumento mediante [moduladores](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator), que también conviene revisar con atención.

Si no hay ningún parámetro ni modulador definido para la división del instrumento enlazada a una muestra, es importante comprobar si hay parámetros o moduladores definidos en la división global del instrumento (esta división fija la configuración por defecto de todas las demás divisiones).

### Afinación a nivel de conjunto

A nivel de conjunto, 3 parámetros afectan al transporte de una muestra: **afinación (semitonos)**, **afinación (centésimas)** y **afinación (escala)**. Estos valores se suman a la configuración de los instrumentos usados en las divisiones del conjunto.

Al igual que los instrumentos, los conjuntos pueden incluir moduladores, y su división global actúa como una configuración por defecto que no hay que pasar por alto.


## Envolvente ADSR inadecuada {#doc_adsr}


La envolvente ADSR representa cómo evoluciona el volumen cuando se toca una nota, pero también cuando se suelta esa misma nota. Cada una de las cuatro letras del término ADSR corresponde a una fase concreta, y la envolvente de volumen que se usa en los soundfonts tiene en realidad 6 fases:

* retraso (delay),
* ataque (A),
* mantenimiento (hold),
* decaimiento (D),
* sostenido (S),
* liberación (R).

Estas fases se describen con más detalle en esta [página](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope), y a continuación se enumeran algunos problemas posibles relacionados con la envolvente.

### Un sonido empieza de forma demasiado brusca

Para suavizar el inicio de una nota, indica o aumenta la **duración del ataque** de la nota a nivel de [instrumento](manual/soundfont-editor/editing-pages/instrument-editor.md) o de [conjunto](manual/soundfont-editor/editing-pages/preset-editor.md).

### Un sonido empieza de forma demasiado suave

Al contrario que en el caso anterior, reduce o elimina la **duración del ataque** de la nota.

### Se nota un retraso antes de que empiece el sonido

Tres soluciones posibles y complementarias:

* eliminar la **duración del retraso** si está indicada a nivel de [instrumento](manual/soundfont-editor/editing-pages/instrument-editor.md) o de [conjunto](manual/soundfont-editor/editing-pages/preset-editor.md),
* eliminar el silencio al principio de la muestra de audio (ver el recorte de muestras en esta [página](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph) o la herramienta de eliminación de silencio [aquí](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)),
* por último, la latencia percibida puede deberse al servidor de audio que usa tu ordenador (en ese caso no es un defecto del propio soundfont, y puedes cambiar la salida de audio en las [preferencias](manual/settings.md#doc_general)).

### El sonido se corta de repente

Si el sonido se corta de repente cuando debería continuar mientras se mantiene la nota, consulta la sección sobre los [bucles](#doc_loop).

Si el sonido se corta bruscamente al soltar una nota, indica o aumenta la **duración de la liberación** a nivel de [instrumento](manual/soundfont-editor/editing-pages/instrument-editor.md) o de [conjunto](manual/soundfont-editor/editing-pages/preset-editor.md). Esta [herramienta](manual/soundfont-editor/tools/instrument-tools.md#doc_release) permite configurar esta duración para que suene natural en toda la tesitura, de los graves a los agudos.


## Bucles ausentes o imprecisos {#doc_loop}


Los bucles permiten producir un sonido de duración infinita a partir de una muestra de longitud fija, sin que el oído perciba las repeticiones. Los bucles son imprescindibles para algunos tipos de instrumentos, como las flautas, pero no para otros, como la percusión. También se pueden usar bucles en muestras de piano para simular un pedal de resonancia con una muestra corta.

### El sonido se detiene antes de soltar la nota

Si el sonido se detiene cuando debería seguir sonando, las causas más probables son:

* el bucle de la muestra no está definido; en ese caso, puedes consultar esta [página](tutorials/how-to-prepare-a-sample.md#doc_loop) para configurar un bucle,
* la reproducción en bucle no está activada; en ese caso, hay que cambiar el parámetro **reproducción en bucle** a nivel de [instrumento](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).

### El sonido contiene «clics» regulares

Lo más probable es que el bucle sea incorrecto; esta [página](tutorials/how-to-prepare-a-sample.md#doc_loop) explica cómo crear bucles correctos.


## Estándar MIDI no respetado {#doc_standard}


Los estándares MIDI imponen una numeración concreta para los instrumentos contenidos en los soundfonts. Así, distintos soundfonts que siguen el mismo estándar son intercambiables sin provocar diferencias importantes en los timbres reproducidos: por ejemplo, un piano y una flauta se llamarán de la misma manera. Si una valoración indica que no se respeta un estándar, hay dos soluciones posibles:

* eliminar la información que indica que el soundfont cumple un estándar,
* o comprobar que todos los conjuntos del soundfont están asignados correctamente al tipo de instrumento adecuado (consulta esta [página](manual/annexes/midi-standards.md) para ver la lista de instrumentos) y que están todos los instrumentos que exige el estándar.
