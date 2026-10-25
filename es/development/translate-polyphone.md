Polyphone se ha diseñado para admitir traducciones y así ser más accesible.
Cualquiera puede ayudar en este proceso: se puede crear una traducción desde cero o completar o mejorar una traducción existente.
Desde la versión 2.6, todas las traducciones que estaban incompletas se han completado con ayuda de una IA, y conviene que las revisen hablantes nativos.


## Requisitos previos


### Archivo de traducción


Descarga el paquete de traducción correspondiente al idioma que quieres traducir desde esta [página](software/translations).
Si el idioma aún no está en la lista, usa el formulario de [contacto](contact) para solicitarlo (basta con indicar el nuevo idioma que quieres).
Recibirás rápidamente un nuevo paquete de traducción.

Los archivos de traducción también están disponibles en el repositorio de <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>, en el directorio :file:`sources` (un archivo :file:`polyphone_XX.ts` por idioma).


### Qt Linguist


El único programa que necesitas para traducir es :program:`Qt Linguist`: una herramienta para generar archivos de traducción.
Este programa se instala con Qt (ver las [páginas de desarrollo](development/index.md)) y también se puede encontrar <a href="https://github.com/thurask/Qt-Linguist/releases" target="_blank">aquí</a> para los usuarios de Windows, o en los repositorios de las distribuciones Linux (paquete :program:`linguist-qt6` en Debian y Ubuntu, :program:`qt6-linguist` en Fedora).


## Abrir el archivo de traducción


Descomprime el paquete de traducción que acabas de descargar.
Encontrarás un archivo .ts :file:`polyphone_XX.ts`, donde :file:`XX` corresponde a tu idioma.
Con :program:`Qt Linguist`, abre :file:`polyphone_XX.ts` y podrás empezar a traducir.


## Traducir


Hay que traducir cada elemento de cada contexto.
Basta con revisar los elementos uno por uno y escribir tu traducción.
Pueden darse tres casos según tu grado de seguridad.

* Si no sabes cómo traducir algo, no dudes en saltarte el elemento.
  No pasa nada: otras personas podrán completarlo.
* Crees que tienes la traducción correcta, pero no estás seguro: escribe tu propuesta y luego usa :kbd:`Ctrl`+:kbd:`J`.
  El elemento no se validará, pero la traducción se usará de todos modos en la interfaz traducida.
  Solo indica a las demás personas que mejoran la traducción que no estabas seguro.
* Tienes la traducción correcta: escríbela y usa :kbd:`Ctrl`+:kbd:`Enter`.
  El elemento se validará.

Una traducción requiere atención (en un mundo en el que todo está hecho para <a href="https://medium.com/thrive-global/how-technology-hijacks-peoples-minds-from-a-magician-and-google-s-design-ethicist-56d62ef5edf3" target="_blank">acapararla</a>).
Presta atención a:

* la puntuación,
* los espacios iniciales y finales,
* las mayúsculas y minúsculas,
* las formas del singular y del plural.

Un último consejo: **¡tómate tu tiempo!**
Y no olvides guardar tu trabajo con :kbd:`Ctrl`+:kbd:`S`.


## Probar tu trabajo


Puedes comprobar tu trabajo en cualquier momento.
En :program:`Qt Linguist`, ve a :menuselection:`File --> Release As…` y crea un archivo .qm correspondiente a la traducción que estás aportando.
Sigue las instrucciones del paquete de traducción para instalarlo (archivo :file:`readme.txt`) y abre Polyphone.
Ve a las [preferencias](manual/settings.md#doc_interface), selecciona tu traducción en la lista de idiomas y reinicia Polyphone.
Ahora podrás revisar tu traducción.

**Nota:** la operación :guilabel:`Release As…` puede traducir solo los textos que no contienen acentos. Si se produce este error, abre un terminal y usa el siguiente comando: :command:`lrelease polyphone_XX.ts` (adáptalo a tu archivo).
Un archivo de traducción :file:`polyphone_XX.qm` también se puede cargar desde un directorio llamado :file:`translations` (en el directorio actual o en el directorio de configuración de Polyphone) en lugar de estar integrado en el programa.


## Enviar la traducción


Cuando hayas terminado (¡o te hayas cansado!), envía el archivo .ts (¡no el .qm!) que estabas editando a <contact@polyphone.io>.
El paquete de traducción de esta [página](software/translations) se actualizará.

Si estás acostumbrado a GitHub, también puedes proponer tu archivo .ts mediante una pull request en el <a href="https://github.com/davy7125/polyphone" target="_blank">repositorio</a> (consulta la guía de contribución).