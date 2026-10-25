Lo scopo di questo tutorial è creare strumenti espressivi usando campioni in loop di cui è stato registrato anche il release.
Imparerai a sfruttare appieno le possibilità di un campione di questo tipo, usandone non solo l'attacco e il sustain ma anche il release.


## Release tradizionale e release personalizzato


Quando suoni una nota, senti prima l'inizio del campione (l'attacco) e poi, finché non rilasci il tasto, il loop all'interno del campione viene riprodotto più e più volte (la fase sostenuta).
Quando rilasci il tasto, il loop continua a essere usato e il suono si attenua progressivamente finché non si sente più.
Questo è il modo tradizionale di realizzare un release nei soundfont: facile da impostare, ma ignora le armoniche transitorie che potrebbero comparire durante il release reale di uno strumento acustico.
Questo tipo di release può essere visto come un semplice prolungamento della fase sostenuta.

Un release più realistico consisterebbe nell'usare il release reale contenuto nel campione registrato.
L'attacco e la fase sostenuta vengono realizzati allo stesso modo, ma quando il tasto viene rilasciato la fase sostenuta si interrompe di colpo, lasciando spazio al release registrato, che inizia a suonare.
Invece di prolungare il suono della fase sostenuta si usa un release personalizzato, che aggiunge dettagli al suono.

Per creare uno strumento di questo tipo si divide il problema in due:

* prima si definiscono l'attacco e la fase sostenuta,
* poi si attiva il release.

L'esempio che verrà usato qui è il suono di una tromba d'organo, per il suo elevato contenuto armonico e la sua evoluzione udibile.

**Nota:** dalla versione 2.4, Polyphone può anche attivare un campione al rilascio di un tasto grazie alla modalità di riproduzione in loop ![release](images/sample_mode_release.png "release"){.inline} (vedi l'[editor degli strumenti](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)), che rende superfluo il breve loop silenzioso descritto qui sotto.
Questa modalità, però, non è documentata dal formato sf2 e potrebbe non essere supportata da altri lettori, mentre il metodo descritto in questo tutorial funziona con qualsiasi lettore.


## Preparazione dei campioni


Si possono presentare due casi: hai un file che comprende il campione completo dall'attacco al release, oppure hai un file aggiuntivo che contiene solo il release già estratto.


### Campione completo con release


Carica il campione due volte in Polyphone e poi, nella seconda istanza del campione, rimuovi tutto tranne il release.
La rimozione si può fare tenendo premuto il tasto :kbd:`Alt` e definendo un'area nel [grafico del campione](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph).
Per trovare il punto in cui inizia il release, riproduci il campione più volte e cerca di individuare il punto in cui ritieni che il suono non fosse più alimentato da alcuna energia.
Quest'ultima parte sarà il release.
Poi segui le istruzioni qui sotto.


### Campione e release separati


Campioni per l'esempio: <a href="files/trumpet.wav" download>campione 1</a>, <a href="files/trumpet-release.wav" download>campione 2</a>.

Se hai 2 campioni, il primo con l'inizio e la fase sostenuta del suono e il secondo con il release, devi solo assicurarti che il release inizi con un brevissimo tratto vuoto (un paio di punti) vicino a 0.
Con questa porzione verrà creato un breve loop impercettibile.

Accorda e metti in loop il primo campione come spiegato nel tutorial «[Come preparare un campione](tutorials/how-to-prepare-a-sample.md)».

Metti in loop il secondo campione su una parte molto breve all'inizio, ad esempio dal punto 1 al punto 2 o 3.
Indica la stessa nota di base e la stessa correzione del primo campione.


## Primo strumento: attacco e fase sostenuta


Crea un primo strumento con i campioni che contengono l'attacco e la fase sostenuta del suono.
Configura la divisione globale in questo modo:

* seleziona ![loop acceso](images/loop_on.png "loop acceso"){.inline} nel parametro :guilabel:`Riproduzione in loop`,
* indica 0,01 s nel parametro :guilabel:`Vol env release (s)` (release molto breve).

Ora hai uno strumento senza release.


## Secondo strumento: release


Crea un secondo strumento per il release, con i campioni di release che hanno all'inizio un brevissimo loop vuoto.
Configuralo in questo modo:

* seleziona ![loop acceso e fine](images/loop_on_end.png "loop acceso e release"){.inline} (loop acceso + release) nel parametro :guilabel:`Riproduzione in loop`,
* scegli un valore elevato, ad esempio 100 secondi, nel parametro :guilabel:`Vol env release (s)`, in modo che il release non venga attenuato,
* stabilisci approssimativamente la durata dell'attacco nel primo campione e usa questo valore nel parametro :guilabel:`Vol env attack (s)`.

Ora hai uno strumento che resta silenzioso finché non rilasci un tasto, dopodiché senti la fase di release.
Potresti anche notare che un tocco molto rapido su un tasto attiva il release in modo attenuato.
Nell'esempio questo si nota appena, perché l'attacco di una tromba è molto rapido.
Nell'esempio la durata dell'attacco è stata leggermente aumentata.


## Unire i due strumenti


Crea un preset con questi due strumenti.
Il primo strumento si sente quando si preme un tasto e il secondo viene attivato quando il tasto viene rilasciato.
Puoi quindi ascoltare il risultato e cercare di rendere più fluida la transizione tra la fase sostenuta e il release:

* attenuando il primo o il secondo strumento,
* aumentando la durata del release del primo strumento.

Il risultato con i file di esempio è disponibile <a href="files/custom%20release.sf2" download>qui</a>.

Qui puoi ascoltare le differenze tra:

* un <a href="files/classic-release.mp3" download>release classico</a>

![](files/classic-release.mp3)

* e un <a href="files/custom-release.mp3" download>release personalizzato</a>

![](files/custom-release.mp3)
