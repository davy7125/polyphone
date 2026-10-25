Una Raspberry Pi es un ordenador del tamaño de una tarjeta de crédito en el que se puede instalar Polyphone. Integrándola en una caja con altavoces y asociándola a un teclado maestro, ¡se puede crear un auténtico sintetizador!

![Sintetizador casero: parte delantera](images/synth_front.jpg "Sintetizador casero: parte delantera")

![Sintetizador casero: parte trasera](images/synth_back.jpg "Sintetizador casero: parte trasera")

## Material necesario

Esta lista se puede adaptar a tu proyecto:

- una Raspberry Pi,
- una tarjeta micro SD de 32 GB o más,
- una fuente de alimentación para la Raspberry Pi,
- un convertidor digital/analógico (DAC) para transformar una salida USB en una salida de audio jack,
- altavoces de ordenador, que tienen la ventaja de incluir tanto los altavoces como la parte de amplificación de la señal,
- un interruptor general de encendido/apagado,
- un cable USB doble macho-hembra para empotrar,
- una caja para meterlo todo dentro,
- y, por supuesto, un teclado maestro para tocar.

Para configurar fácilmente la Raspberry Pi, también es recomendable tener:

- una pantalla con un cable micro-HDMI para conectarla a la Raspberry Pi,
- un teclado,
- un ratón.

En el ejemplo de esta página se han usado unos altavoces Edifier R1280T de segunda mano. Por un precio modesto, reproducen los graves relativamente bien y, como verás más abajo, será posible corregir las carencias de su respuesta en frecuencia.

El teclado maestro usado es un Akai MPK Mini Plus, que se conecta por USB. El adaptador USB / jack es un UGREEN de gama básica y la Raspberry Pi es la versión 5.

(estos detalles no pretenden hacer publicidad de ninguna marca, sino presentar una combinación que funciona)

## Preparar la caja

Este tutorial no se centrará en una forma concreta de fabricar una caja. Usar madera pondrá en valor tus dotes de carpintería, como en este ejemplo, pero el cartón puede funcionar igual de bien. ¡Te toca expresar tu talento creativo! En general, necesitarás una tapa para acceder a los componentes y aberturas para:

- los altavoces,
- el cable de alimentación,
- el interruptor de encendido/apagado,
- el volumen y, si es necesario, los mandos de ecualización,
- los resonadores de Helmholtz del bass-reflex, etc.

Este es un orden de montaje sugerido en la caja:

- empieza empotrando el botón de encendido/apagado, los conectores USB del panel trasero y cualquier otro botón conectado a los altavoces,
- coloca el cable de alimentación y conéctalo a la entrada del botón de encendido/apagado,
- coloca el transformador asociado a los altavoces y conéctalo a la salida del botón de encendido/apagado,
- coloca la placa electrónica de los altavoces y conéctala al transformador,
- fija los altavoces y conéctalos a la placa electrónica,
- fija la Raspberry Pi,
- conecta la fuente de alimentación de la Raspberry Pi a la salida del botón de encendido/apagado (la salida del botón alimenta tanto los altavoces como la Raspberry Pi),
- conecta el cable USB doble a la Raspberry Pi,
- conecta el convertidor USB/jack a otra toma USB,
- conecta la placa electrónica de los altavoces mediante un cable jack.

En la siguiente imagen, la fuente de alimentación y la placa electrónica de los altavoces están rodeadas en naranja. La Raspberry Pi está rodeada en azul.

![Interior del sintetizador](images/synth_inside.jpg "Interior del sintetizador")

Nota: los altavoces deben colocarse de forma que no haya ningún retorno posible del sonido entre la salida del altavoz y el interior de la caja; de lo contrario, aparecerá una resonancia. **¡Así que no como en este ejemplo!** A veces hay que experimentar para descubrir las trampas…

![La Raspberry Pi](images/synth_raspberry_pi.jpg "La Raspberry Pi")

Se usan 3 puertos USB:

- uno para obtener una salida jack,
- los otros 2 se llevan a la parte trasera de la caja y servirán para conectar el teclado maestro o, al configurar la Raspberry Pi, un teclado y un ratón.

## Preparar la Raspberry Pi

### Instalar Debian

Uno de los primeros pasos al recibir una Raspberry Pi es instalar **Debian** en una tarjeta micro SD. Es fácil encontrar instrucciones en internet, por ejemplo <a href="https://www.raspberrypi.com/documentation/computers/getting-started.html#installing-the-operating-system" target="_blank">aquí</a>.

Una vez preparada la tarjeta micro SD, puedes insertarla en la ranura de la Raspberry Pi. Después, conecta la fuente de alimentación, el monitor, el ratón y el teclado, ¡y tu ordenador estará listo!

### Instalar y configurar Polyphone

Primero, actualiza el sistema con:

```
sudo apt-get update
sudo apt-get upgrade
```

Después, instala **Polyphone** con el paquete .deb disponible en esta [página](software). Conecta el teclado MIDI por USB a la Raspberry Pi (desconecta el teclado del ordenador si no queda ningún puerto USB libre), abre Polyphone y selecciona el teclado MIDI en la lista desplegable de entradas MIDI de las [preferencias](manual/settings.md#doc_general). Descargando un soundfont, por ejemplo del [repositorio](soundfonts), ¡ya se puede tocar un instrumento con el teclado MIDI asociado a la Raspberry Pi!

### Iniciar Polyphone automáticamente

Este paso es necesario para prescindir de la pantalla. Polyphone se puede iniciar automáticamente al arrancar la Raspberry Pi creando un archivo :file:`synthesizer.desktop` con este texto:

```
[Desktop Entry]
Name=Synthesizer
Exec=polyphone -s /home/pi/Desktop/my-soundfont.sf2
Terminal=false
Type=Application
StartupNotify=true
Icon=/home/pi/Desktop/my-icon.png
Categories=AudioVideo;
```

Después, este archivo se colocará en la carpeta :file:`/home/pi/.config/autostart`.

La línea de comandos se puede completar con opciones para escuchar un canal MIDI concreto, tocar varios instrumentos a la vez o activar / desactivar instrumentos mediante teclas. Esta [página](tutorials/use-polyphone-as-a-synthesizer.md) detalla las opciones para usar Polyphone en modo sintetizador.

## Para ir más allá

### Ecualización del sonido

Cada altavoz, por su morfología, responde de forma más o menos distinta según las frecuencias que debe emitir. Para obtener una respuesta en frecuencia plana y reproducir así el sonido con fidelidad, se puede crear una ecualización que compense las carencias del altavoz. La herramienta <a href="https://github.com/erdewit/HiFiScan" target="_blank">HiFiScan</a> calcula automáticamente los parámetros de ecualización; el protocolo se describe en la página del proyecto.

Una vez calculada la curva de ecualización, **HiFiScan** puede exportar coeficientes para integrarlos en ecualizadores. Aquí nos interesa la ecualización de 12 bandas, que se puede usar con **Calf**.

### Conexión remota

Hasta ahora, la configuración de la Raspberry Pi ha necesitado una pantalla para mostrar el escritorio. Sin embargo, se puede configurar el sistema para permitir el acceso remoto mediante el **protocolo VNC**, y hay muchos recursos en internet para configurarlo.
