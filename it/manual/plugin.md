Polyphone esiste anche come plugin per le workstation audio digitali (DAW) e gli altri host di plugin audio.
Il plugin suona i soundfont nei formati sf2, sf3, sfz e dls con il sintetizzatore di Polyphone: un soundfont suona allo stesso modo in un host e nell'editor.
Suona i soundfont ma non li modifica: le modifiche si fanno in Polyphone, dopodiché il soundfont viene riaperto nel plugin.

Il plugin è disponibile nei formati VST3, CLAP e LV2 su Linux, Windows e macOS, e anche Audio Unit (AU) su macOS.


![Plugin di Polyphone](images/plugin.png "Plugin di Polyphone")


## Installazione {#doc_install}


Il plugin è distribuito in un archivio per ciascun sistema operativo, disponibile nella sezione «[Software](software)».
Copia i formati usati dal tuo host nelle cartelle in cui cerca i plugin, poi avvia nell'host una nuova scansione dei plugin.

Linux
: :file:`Polyphone.vst3` in :file:`~/.vst3/`,\
  :file:`Polyphone.clap` in :file:`~/.clap/`,\
  :file:`Polyphone.lv2` in :file:`~/.lv2/`.

macOS
: :file:`Polyphone.vst3` in :file:`~/Library/Audio/Plug-Ins/VST3/`,\
  :file:`Polyphone.clap` in :file:`~/Library/Audio/Plug-Ins/CLAP/`,\
  :file:`Polyphone.component` in :file:`~/Library/Audio/Plug-Ins/Components/` (formato Audio Unit, usato da Logic Pro e GarageBand),\
  :file:`Polyphone.lv2` in :file:`~/Library/Audio/Plug-Ins/LV2/`.

Windows
: :file:`Polyphone.vst3` in :file:`C:\Program Files\Common Files\VST3`,\
  :file:`Polyphone.clap` in :file:`C:\Program Files\Common Files\CLAP`,\
  :file:`Polyphone.lv2` in :file:`%APPDATA%\LV2`.

Su macOS i plugin non sono notarizzati: se l'host si rifiuta di caricarli, rimuovi l'attributo di quarantena dei file scaricati, ad esempio:

```
xattr -dr com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/Polyphone.vst3
```


## Soundfont {#doc_soundfont}


La parte superiore della finestra mostra il nome del soundfont e il suo percorso.
Un soundfont si apre:

* con il pulsante :guilabel:`Apri...`,
* con la freccia alla sua destra, che elenca gli ultimi 10 soundfont aperti,
* trascinando un file sulla finestra.

I file sfz e dls vengono convertiti nello stesso modo in cui Polyphone li importa, e i campioni di un file sfz sono i file audio a cui fa riferimento.
All'apertura di un soundfont, la sua struttura viene letta subito: gli strumenti vengono elencati e possono essere suonati.
I suoi campioni vengono poi letti in background, con un indicatore di caricamento davanti al suo nome (il suo tooltip mostra l'avanzamento): una nota i cui campioni non sono ancora stati letti resta muta.
I campioni delle note suonate e degli strumenti scelti vengono letti per primi, e i campioni di un file sf3 vengono decompressi.
Una volta letto tutto, durante l'esecuzione non viene letto nulla dal disco; un'esportazione offline attende che tutti i campioni siano letti.
Quando più istanze del plugin suonano lo stesso file (una per traccia, ad esempio), il file viene letto e tenuto in memoria una sola volta.
Se il file è cambiato, ad esempio dopo essere stato salvato di nuovo in Polyphone, viene riletto.


## Modalità di esecuzione {#doc_modes}


Tre pulsanti sopra l'elenco degli strumenti (i preset del soundfont) scelgono il modo in cui vengono suonati gli strumenti:

:guilabel:`Uno strumento`
: Lo strumento selezionato nell'elenco viene suonato sui 16 canali MIDI.
  Gli strumenti sono mostrati con il loro numero di banco e di programma, ad esempio «000:024 Guitar».

:guilabel:`16 canali`
: Ogni canale MIDI suona il proprio strumento, scelto nella sua riga.
  La scelta :guilabel:`Cambio programma` suona lo strumento scelto dai messaggi di bank select e di program change ricevuti dal canale, come fa un sintetizzatore General MIDI: il banco 128 viene allora usato sul canale 10, per la batteria.
  Lo strumento suonato è mostrato a destra di ogni riga.
  Questa modalità è comoda per suonare un file MIDI arrangiato per più strumenti.

:guilabel:`MPE`
: Lo strumento selezionato viene suonato da un controller MPE (MIDI Polyphonic Expression): ogni nota ha il proprio canale, e quindi il proprio pitch bend, la propria pressione e il proprio timbro (CC 74).
  Viene usata una zona inferiore di 15 canali finché il controller non invia la sua configurazione (RPN 6).
  Il canale master di una zona agisce su tutte le sue note: il suo pitch bend si somma a quello di ciascuna nota e i suoi controller si applicano a tutte le note.
  L'estensione del pitch bend è di 48 semitoni per le note e di 2 semitoni per il canale master, e può essere modificata dal controller (RPN 0).


![Plugin nella modalità «16 canali»](images/plugin_channels.png "Plugin nella modalità «16 canali»")


## Parametri {#doc_parameters}


I parametri sono quelli del sintetizzatore di Polyphone, che si ritrovano nelle sue impostazioni [Suono](manual/settings.md#doc_sound) e [Tastiera](manual/settings.md#doc_keyboard).
:guilabel:`Guadagno` (da -24 a +24 dB) si trova nell'intestazione, gli altri parametri sono raggruppati a destra:

:guilabel:`Accordatura`
: :guilabel:`Intonazione di riferimento`: una nota da La#3 a La4 e la sua frequenza (da 400 a 480 Hz per il La4, 440 Hz per impostazione predefinita); questa nota suona a questa frequenza qualunque sia il temperamento.
  :guilabel:`Trasposizione`: da -24 a +24 semitoni, la batteria non viene trasposta.

:guilabel:`Velocità`
: La risposta alla velocità delle note; ogni impostazione è rappresentata da un grafico (velocità da 0 a 127 da sinistra a destra):
  
  * :guilabel:`Taglio del filtro`: modulatore predefinito che abbassa la frequenza di taglio del filtro in base alla velocità, fino a 2 ottave, come nelle impostazioni [Suono](manual/settings.md#doc_sound) di Polyphone. La versione 2.04 del formato SoundFont (predefinita) la abbassa tanto più quanto la velocità è bassa. La versione 2.01 aveva un salto alla velocità 64, un errore corretto dalla versione 2.04: è una modalità di compatibilità per i vecchi soundfont realizzati compensando questo salto. Con :guilabel:`Disattivato`, la frequenza di taglio non dipende dalla velocità.
  * :guilabel:`Curva`: curva applicata alla velocità delle note ricevute, lineare al centro (predefinita, ripristinata con un doppio clic), concava verso sinistra (velocità più morbida: le velocità alte richiedono un tocco più deciso) e convessa verso destra (la velocità sale rapidamente).

:guilabel:`Temperamenti e scale`
: I temperamenti e le scale di Polyphone nelle loro categorie, poi il temperamento personalizzato, trasposti nella :guilabel:`Tonalità` scelta.
  La tastiera a destra mostra l'intonazione fine di ogni nota in cent e si modifica come nelle [impostazioni della tastiera](manual/settings.md#doc_keyboard) di Polyphone: rotella del mouse o trascinamento verticale, doppio clic per tornare a 0, menu contestuale per cambiare l'utilizzo di una nota o spostarvi la tonalità. Un temperamento modificato diventa il temperamento personalizzato.
  Le note non utilizzate da una scala non vengono suonate, a meno che :guilabel:`Suona tutto` sia selezionato.

:guilabel:`Riverbero`
: :guilabel:`Livello`, :guilabel:`Dimensione`, :guilabel:`Ampiezza` e :guilabel:`Smorzamento`.

:guilabel:`Chorus`
: :guilabel:`Livello`, :guilabel:`Profondità` e :guilabel:`Frequenza`.

I gruppi :guilabel:`Temperamenti e scale`, :guilabel:`Riverbero` e :guilabel:`Chorus` hanno un interruttore a destra del titolo: spento, il gruppo è compresso e il suo temperamento o il suo effetto non viene applicato (temperamento equabile con tutte le note, nessun effetto); le sue impostazioni vengono conservate.
L'altezza della finestra segue i gruppi espansi.

L'host può automatizzare tutti questi parametri, tranne la nota dell'intonazione di riferimento (viene automatizzata la frequenza del La4) e le intonazioni fini del temperamento personalizzato, che vengono salvate con il progetto.


## Tastiera e stato {#doc_keyboard}


La tastiera nella parte inferiore della finestra mostra le note ricevute e può essere suonata con il mouse.
Sotto di essa sono indicati il numero di voci in esecuzione e la quota del processore occupata dal plugin.

Il pulsante :guilabel:`Panico` ferma tutti i suoni e reimposta i controller, il che è utile se una nota resta bloccata.


## Impostazioni {#doc_settings}


Il pulsante :guilabel:`Impostazioni` apre un menu con le seguenti opzioni:

:guilabel:`Polifonia`
: Numero massimo di voci, da 64 a 4096 (1024 per impostazione predefinita).
  Oltre questo numero, le note più deboli si dissolvono.

:guilabel:`Tastiera`
: Canale MIDI delle note suonate con la tastiera della finestra.

:guilabel:`Lingua`
: Lingua del plugin, per impostazione predefinita quella del sistema.
  Questa scelta è condivisa da tutte le istanze del plugin.
  I nomi dei parametri e delle uscite forniti all'host sono quelli della lingua usata al caricamento del plugin (sempre l'inglese con LV2).

:guilabel:`Latenza fissa (note suonate nella loro posizione esatta)`
: Quando questa opzione è disattivata (impostazione predefinita), i messaggi MIDI di un blocco audio vengono anticipati insieme in modo che la prima nota del blocco cominci all'inizio del blocco: le note suonate dal vivo suonano prima, in media di mezza durata del buffer, ma la loro latenza varia secondo la loro posizione nel buffer dell'host.
  Quando è attivata, ogni messaggio viene elaborato nella sua posizione esatta: la latenza delle note suonate dal vivo è costante.
  Durante un'esportazione offline, i messaggi vengono sempre elaborati nella loro posizione esatta.

:guilabel:`Clipping a [-1; 1]`
: Le uscite sono limitate all'intervallo [-1; 1] (impostazione predefinita), come in Polyphone.
  Senza clipping, i valori più grandi vengono passati all'host, che li mixa in virgola mobile: il guadagno può allora essere aumentato senza saturazione, regolando il livello prima dell'uscita dell'host.

:guilabel:`Salva il soundfont nel progetto`
: Il soundfont viene salvato nel progetto dell'host, che può quindi essere aperto su un altro computer senza il file.
  Non è possibile con un file sfz, i cui campioni sono altri file.

:guilabel:`Canali ed effetti sulle proprie uscite`
: Oltre all'uscita principale, le uscite da «Canale 1» a «Canale 16» ricevono le note del loro canale senza effetti, e l'uscita «Effetti» riceve la parte inviata al riverbero e al chorus.
  Un canale la cui uscita non è attivata dall'host resta nell'uscita principale.
  Quando questa opzione è disattivata (impostazione predefinita), tutto viene mixato nell'uscita principale.


## Uso in un progetto {#doc_project}


Il progetto dell'host salva i parametri, gli strumenti suonati, la polifonia e il percorso del soundfont (o il suo contenuto, se è salvato nel progetto).
All'apertura di un progetto, viene letta solo la struttura del soundfont prima che venga ripristinato il resto: l'host non attende i campioni, e un'esportazione offline può iniziare subito (attende i campioni non ancora letti).

Una nota inizia esattamente nella posizione del suo messaggio MIDI, qualunque sia la dimensione del buffer dell'host, durante un'esportazione offline e in tempo reale con una [latenza fissa](manual/plugin.md#doc_settings).
Durante un'esportazione offline, le voci non sono mai limitate dal carico del processore.
