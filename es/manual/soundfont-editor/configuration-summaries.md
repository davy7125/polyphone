Los resúmenes de configuración ofrecen una vista de conjunto de todos los elementos que contiene un soundfont.
Son accesibles desde los encabezados :guilabel:`Muestras`, :guilabel:`Instrumentos` y :guilabel:`Conjuntos` del [árbol](manual/soundfont-editor/tree.md) y permiten visualizar los elementos correspondientes.

La información reunida en estas páginas puede servir de apoyo para revisar soundfonts grandes.

Cada elemento se muestra en una fila de una tabla.
Un clic en el encabezado de una columna ordena la tabla según esa columna, y un doble clic en una fila abre el elemento correspondiente en su [página de edición](manual/soundfont-editor/editing-pages/index.md).
Los valores que merecen atención se escriben en negrita, en amarillo (advertencia) o en rojo (error).


## Muestras {#doc_sample}


Al hacer clic en el encabezado :guilabel:`Muestras` de un soundfont en el [árbol](manual/soundfont-editor/tree.md), aparece el resumen de configuración de las muestras.


![Resumen de configuración de las muestras](images/conf_summary_sample.png "Resumen de configuración de las muestras")


Para cada muestra, las columnas son: :guilabel:`Utilizado`, :guilabel:`Duración total`, :guilabel:`Duración del bucle`, :guilabel:`Nota raíz`, :guilabel:`Corrección`, :guilabel:`Tipo de muestra`, :guilabel:`Muestra enlazada`, :guilabel:`Frecuencia de muestreo` y :guilabel:`Compresión`.
Esta página permite, por ejemplo, hacer las siguientes comprobaciones:

* ¿Se usa la muestra?
* ¿Su duración total (y, en su caso, la duración del bucle) es suficiente?
* ¿El bucle es correcto? (se resalta un bucle que puede producir un clic)
* ¿Se ha hecho la afinación?
* ¿Está bien enlazada en estéreo? (las dos muestras de un par estéreo deben tener la misma nota raíz)
* ¿La frecuencia de muestreo es lo bastante alta para una buena calidad de audio?
* ¿Se ha comprimido?


## Instrumentos {#doc_instrument}


Al hacer clic en el encabezado :guilabel:`Instrumentos` de un soundfont en el [árbol](manual/soundfont-editor/tree.md), aparece el resumen de configuración de los instrumentos.


![Resumen de configuración de los instrumentos](images/conf_summary_instrument.png "Resumen de configuración de los instrumentos")


Para cada instrumento, las columnas son: :guilabel:`Utilizado`, :guilabel:`Número de muestras`, :guilabel:`Número de parámetros`, :guilabel:`Número de moduladores`, :guilabel:`Rango máximo de notas`, :guilabel:`Rango máximo de velocidad`, :guilabel:`Atenuación`, :guilabel:`Reproducción en bucle`, :guilabel:`Coro` y :guilabel:`Reverberación`.
Esta página permite, por ejemplo, hacer las siguientes comprobaciones:

* ¿Se usa el instrumento?
* ¿Son coherentes los números de muestras enlazadas, de parámetros y de moduladores?
* ¿Son correctos los rangos de notas y de velocidad?
* ¿Se han atenuado bien las muestras?
* ¿Está activada la reproducción en bucle?
* ¿Se han olvidado los ajustes de coro y de reverberación?


## Conjuntos {#doc_preset}


Al hacer clic en el encabezado :guilabel:`Conjuntos` de un soundfont en el [árbol](manual/soundfont-editor/tree.md), aparece el resumen de configuración de los conjuntos.


![Resumen de configuración de los conjuntos](images/conf_summary_preset.png "Resumen de configuración de los conjuntos")


Para cada conjunto, las columnas son: :guilabel:`Banco - Conjunto`, :guilabel:`Número de instrumentos`, :guilabel:`Número de parámetros`, :guilabel:`Número de moduladores`, :guilabel:`Rango máximo de notas`, :guilabel:`Rango máximo de velocidad`, :guilabel:`Atenuación`, :guilabel:`Coro` y :guilabel:`Reverberación`.
Esta página permite, por ejemplo, hacer las siguientes comprobaciones:

* ¿Se han rellenado bien los números de banco y de conjunto?
* ¿Son coherentes los números de instrumentos enlazados, de parámetros y de moduladores?
* ¿Son correctos los rangos de notas y de velocidad?
* ¿Se han atenuado con precisión los instrumentos?
* ¿Se han olvidado los ajustes de coro y de reverberación (como desplazamientos)?