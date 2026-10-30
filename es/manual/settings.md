Las preferencias del programa son accesibles desde la [pantalla de inicio](manual/index.md) o desde el [menú](manual/menu.md#doc_shortcuts) (atajo :kbd:`Ctrl`+:kbd:`,`).
Se dividen en cinco secciones:

* [General](#doc_general)
* [Interfaz](#doc_interface)
* [Sonido](#doc_sound)
* [Teclado](#doc_keyboard)
* [Repositorio en línea](#doc_repository)

Todas las modificaciones se guardan inmediatamente.


## General {#doc_general}


La sección :guilabel:`General` permite modificar los siguientes parámetros:

Salida de audio
: Indica el sistema de audio y el dispositivo que se usan para reproducir los sonidos.
  Los sistemas disponibles dependen del sistema operativo (por ejemplo WASAPI y ASIO en Windows, ALSA y JACK en Linux, Core Audio en macOS).
  Elegir «-» desactiva la salida de audio.

Tamaño del búfer
: Indica el tamaño del búfer de audio.
  Un búfer pequeño da una latencia baja, pero el sonido puede volverse entrecortado.
  Con el valor :guilabel:`Por defecto`, el tamaño depende del sistema de audio.

Polifonía
: Número máximo de voces reproducidas a la vez, de 64 a 4096 (1024 por defecto).
  Una voz es una muestra en reproducción: una nota puede usar varias voces, por ejemplo dos con una muestra estéreo.
  Por encima de este número, algunas notas se desvanecen: primero las que se han soltado, luego las más suaves.
  Las notas que acaban de empezar, así como la nota más aguda y la más grave de las que se mantienen pulsadas, se conservan el mayor tiempo posible.

Carga del procesador
: Proporción máxima del tiempo de procesador utilizada para calcular las voces, del 20 al 100 % (60 % por defecto).
  Por encima de esta proporción, algunas notas se desvanecen de la misma manera.
  Un valor alto permite tocar más notas a la vez, pero el sonido puede volverse entrecortado.

Entrada MIDI
: Indica la entrada MIDI que controla el sintetizador y el teclado virtual.

Además, se pueden marcar las siguientes opciones:

Edición estéreo: cambiar muestra enlazada
: Al editar un lado de una muestra estéreo, el otro lado se edita automáticamente de la misma manera.
  Lo mismo ocurre al editar una división estéreo de un instrumento: la división correspondiente al otro canal (con los mismos rangos de notas y de velocidad) se edita de la misma manera.

Importar muestras: recortar hasta el final del bucle
: Al importar una muestra, los datos situados después del final del bucle se eliminan automáticamente (mismo efecto que la herramienta «[Recortar hasta el final del bucle](manual/soundfont-editor/tools/sample-tools.md#doc_trimloop)»).

Importar muestras: eliminar silencio al inicio
: Al importar una muestra, cualquier zona vacía situada antes del comienzo del ataque se elimina automáticamente (mismo efecto que la herramienta «[Eliminar silencio al inicio](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)»).


![Preferencias, general](images/settings_general.png "Preferencias, general")


## Interfaz {#doc_interface}


Esta sección muestra opciones relacionadas con la interfaz del programa:

Idioma
: Aquí se puede cambiar el idioma.

Nombres de las notas
: Las notas se pueden nombrar por su número (según la especificación MIDI) o según el nombre del do central (C3, C4 o C5).
  Los nombres de las notas pueden incluir sostenidos o bemoles.

Instancia única
: Abre todos los archivos en una sola ventana.

Vista de inicio
: Permite elegir la vista que se muestra al abrir un soundfont entre las siguientes: :guilabel:`General` (por defecto), :guilabel:`Muestras`, :guilabel:`Muestras - primera muestra`, :guilabel:`Instrumentos`, :guilabel:`Instrumentos - primer instrumento - Tabla`, :guilabel:`Instrumentos - primer instrumento - Rangos`, :guilabel:`Instrumentos - primer instrumento - Envolventes`, :guilabel:`Conjuntos`, :guilabel:`Conjuntos - primer conjunto - Tabla`, :guilabel:`Conjuntos - primer conjunto - Rangos`. Para el primer instrumento o conjunto, la última parte indica la página mostrada: [tabla](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table), [editor de rangos](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range) o [editor de envolventes](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope).

Ordenar divisiones
: Aquí se puede cambiar el orden de las divisiones dentro de un instrumento o de un conjunto: :guilabel:`por rango de notas`, :guilabel:`por rango de velocidad`, :guilabel:`en orden alfabético` o :guilabel:`ninguna` (el orden de creación).

Estilo
: El estilo de los elementos de la interfaz.
  El estilo :guilabel:`Fusion` es el predeterminado, y se pueden ofrecer otros según el sistema operativo (Adwaita, Windows, macOS…).

Colores
: Hay distintos juegos de colores predefinidos para personalizar el aspecto de Polyphone.
  También se pueden elegir manualmente los colores del texto, de la ventana, de los botones, de la selección y de las listas.
  Los colores solo se pueden cambiar con los estilos :guilabel:`Fusion` y :guilabel:`Windows`.

Bordes de la ventana
: Muestra u oculta los bordes de la ventana, para ganar espacio en la pantalla (no disponible en macOS).

Decoraciones
: Las decoraciones de fondo de algunas listas se pueden desactivar para mejorar la legibilidad.

Las modificaciones del idioma, de los nombres de las notas, del estilo, de los colores, de los bordes de la ventana o de las decoraciones se aplican la próxima vez que se inicia el programa.


![Preferencias, interfaz](images/settings_interface.png "Preferencias, interfaz")


## Sonido {#doc_sound}


La sección :guilabel:`Sonido` permite modificar la reverberación (nivel, tamaño de la sala, amortiguación y anchura) y el coro (nivel, profundidad y frecuencia) del sintetizador, así como su volumen general.
El volumen general se puede cambiar con el controlador MIDI de volumen si hay un teclado externo conectado.

Como el modulador por defecto que modifica la frecuencia de corte del filtro cambió entre las versiones 2.01 y 2.04 de las especificaciones sf2, aquí se puede elegir qué versión usar:

* :guilabel:`SoundFont 2.01`: usa la regla «bajar la frecuencia del filtro teniendo en cuenta la velocidad SI la velocidad es inferior a 64» (se oye un salto claro y molesto entre las velocidades 63 y 64);
* :guilabel:`SoundFont 2.04` (por defecto): usa la regla «bajar la frecuencia del filtro teniendo en cuenta la velocidad» (ya sin salto);
* :guilabel:`deshabilitado`: este modulador por defecto no se define.


![Preferencias, sonido](images/settings_sound.png "Preferencias, sonido")


## Teclado {#doc_keyboard}


Las teclas del teclado del ordenador que se usan para controlar el [teclado virtual](manual/soundfont-editor/toolbar.md#doc_keyboard) se definen en la sección :guilabel:`Teclado`.
La altura se puede subir o bajar octava a octava con el ajuste :guilabel:`Primer do`.

Para los músicos más avanzados, aquí se puede cambiar la afinación:

Diapasón de referencia
: La frecuencia del la situado por encima del do central (nota 69), entre 390 y 470 Hz (440 Hz por defecto).

Temperamento
: Al tocar se puede aplicar un temperamento distinto del temperamento igual.
  Cuando se selecciona un temperamento, también se puede elegir la nota que sirve de referencia a ese temperamento (su tónica).

El diapasón de referencia y el temperamento también se pueden cambiar con mensajes MIDI RPN (master fine tuning, tuning program select y tuning bank select).


![Preferencias, teclado](images/settings_keyboard.png "Preferencias, teclado")


## Repositorio en línea {#doc_repository}


Aquí se pueden introducir la dirección de correo electrónico y la contraseña de una cuenta para vincular Polyphone con ella; una [cuenta Premium](premium) desbloquea todo el [repositorio en línea](manual/online-repository.md).
Tras una conexión correcta, Polyphone se conecta automáticamente a la cuenta en cada inicio, hasta que se pulsa el botón :guilabel:`Cerrar sesión`.

Aquí también se puede cambiar el directorio que se usa al descargar soundfonts del [repositorio en línea](manual/online-repository.md).


![Preferencias, repositorio](images/settings_repository.png "Preferencias, repositorio")



*[JACK]: JACK Audio Connection Kit
*[ASIO]: Audio Stream Input/Output
*[MIDI]: Musical Instrument Digital Interface