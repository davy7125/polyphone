La barra degli strumenti, situata nella parte superiore dell'editor, contiene icone per:

* le [funzioni di modifica](#doc_edit) più comuni,
* il cambio di [vista](#doc_view) negli editor,
* la visualizzazione del [registratore](#doc_recorder),
* la visualizzazione della [tastiera virtuale](#doc_keyboard),
* la visualizzazione del [lettore MIDI](#doc_midi_player).


![Barra degli strumenti](images/toolbar.png "Barra degli strumenti")


## Funzioni di modifica {#doc_edit}


Gli elementi nella parte sinistra della barra degli strumenti servono per le funzioni di modifica più comuni:

![nuovo campione](images/toolbar_sample.png "nuovo campione"){.inline} Aggiungi un campione
: Aggiunge nuovi [campioni](manual/soundfont-editor/editing-pages/sample-editor.md) al soundfont.
  Viene mostrata una finestra di dialogo per selezionare i file audio (si possono importare file :file:`.wav`, :file:`.flac`, :file:`.mp3`, :file:`.ogg`, :file:`.aif`, :file:`.aiff`, :file:`.aifc` e :file:`.snd`), con la possibilità di selezionare più file contemporaneamente.

![nuovo strumento](images/toolbar_instrument.png "nuovo strumento"){.inline} Aggiungi uno strumento
: Crea un nuovo [strumento](manual/soundfont-editor/editing-pages/instrument-editor.md).
  Una finestra di dialogo chiede il nome dello strumento e, se nell'[albero](manual/soundfont-editor/tree.md) sono selezionati dei campioni, propone di collegarli al nuovo strumento.
  In questo caso, i campioni stereo vengono posizionati a sinistra e a destra, il loop viene attivato se tutti i campioni hanno un loop e, quando i campioni hanno note di base diverse, gli intervalli di note vengono definiti automaticamente (vedi la funzione [Auto-posizionamento campione](manual/soundfont-editor/tools/instrument-tools.md#doc_position)).

![nuovo preset](images/toolbar_preset.png "nuovo preset"){.inline} Aggiungi un preset
: Crea un nuovo [preset](manual/soundfont-editor/editing-pages/preset-editor.md).
  Una finestra di dialogo chiede il nome del preset e, se nell'albero sono selezionati degli strumenti, propone di collegarli al nuovo preset.
  Vengono usati i primi numeri di banco e di preset disponibili.

![funzioni](images/toolbar_toolbox.png "funzioni"){.inline} Toolbox
: Un clic su questo pulsante mostra un menu con tutte le [funzioni](manual/soundfont-editor/tools/index.md) disponibili per gli elementi selezionati nell'[albero](manual/soundfont-editor/tree.md).

![annulla](images/toolbar_undo.png "annulla"){.inline} Annulla
: Annulla l'ultima modifica, accessibile anche con :kbd:`Ctrl`+:kbd:`Z`.

![rifai](images/toolbar_redo.png "rifai"){.inline} Rifai
: Ripete l'ultima modifica annullata, accessibile anche con :kbd:`Ctrl`+:kbd:`Y` o :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`Z`.

![salva](images/toolbar_save.png "salva"){.inline} Salva
: Salva il soundfont, accessibile anche con :kbd:`Ctrl`+:kbd:`S`.


## Cambiare vista {#doc_view}


Nella parte destra della barra degli strumenti possono comparire alcune icone, a seconda dell'[editor](manual/soundfont-editor/editing-pages/index.md) mostrato:

* durante la modifica di uno strumento è possibile passare dalla ![tabella](images/toolbar_table.png "tabella"){.inline} [vista a tabella](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) all'![intervalli](images/toolbar_range.png "intervalli"){.inline} [editor degli intervalli](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range) e all'![inviluppi](images/toolbar_adsr.png "inviluppi"){.inline} [editor degli inviluppi](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope) e viceversa,
* durante la modifica di un preset è possibile passare dalla ![tabella](images/toolbar_table.png "tabella"){.inline} vista a tabella all'![intervalli](images/toolbar_range.png "intervalli"){.inline} editor degli intervalli e viceversa,
* negli altri casi non viene mostrato nulla.

La vista scelta viene mantenuta quando si seleziona un altro strumento o preset. La vista mostrata per il primo strumento o preset all'apertura di un soundfont può essere scelta con l'opzione :guilabel:`Vista all'avvio` delle [impostazioni](manual/settings.md#doc_interface).


## Registratore {#doc_recorder}


Facendo clic sull'icona ![registratore](images/toolbar_recorder.png "registratore"){.inline} si apre una finestra con il registratore, che può registrare in un file ciò che suona Polyphone, l'ingresso audio o i segnali MIDI. Il tempo trascorso viene mostrato nella finestra.


![Registratore](images/recorder.png "Registratore")


L'elenco nel banner seleziona la modalità di registrazione:

Polyphone → wav
: Registra l'uscita di Polyphone in un file :file:`.wav`.

Microfono → wav
: Registra l'ingresso audio scelto nelle [impostazioni](manual/settings.md#doc_general) in un file :file:`.wav`, in stereo o in mono a seconda del dispositivo.
  L'ingresso non viene inviato all'uscita audio.

Segnali MIDI
: Registra in un file :file:`.mid` ciò che viene suonato con una tastiera MIDI o la [tastiera virtuale](#doc_keyboard).

Il pulsante sinistro può avere due stati (scorciatoia :kbd:`Ctrl`+:kbd:`R`, disponibile in tutte le finestre, il registratore viene aperto se necessario):

Registra
: Avvia una nuova registrazione se non è in corso alcuna acquisizione.
  Il file viene creato nella cartella dell'ultima registrazione, con un numero aggiunto al nome se esiste già un file con quel nome.

Stop
: Interrompe l'acquisizione in corso.
  L'acquisizione si interrompe anche alla chiusura della finestra, dopo una conferma.

Anche il pulsante destro può avere due stati (disponibile solo durante un'acquisizione, scorciatoia :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`R`):

Pausa
: Mette in pausa l'acquisizione in corso.

Riprendi
: Riprende l'acquisizione in corso.

Sotto questi pulsanti, l'indicatore mostra il livello dei canali, anche quando non si sta registrando: il volume può così essere regolato prima di una registrazione. Il valore alla sua destra è il livello più alto raggiunto dall'inizio della registrazione, mostrato su sfondo rosso se è stato raggiunto il livello massimo, nel qual caso il suono è saturato. Un clic sull'indicatore azzera questo valore. Nella modalità :guilabel:`Segnali MIDI`, l'indicatore mostra la velocità delle note suonate, il che conferma che i segnali vengono ricevuti.

La parte inferiore della finestra elenca le ultime 5 registrazioni, audio o MIDI a seconda della modalità, con la loro dimensione o il numero di note. Un file che non esiste più viene tolto dall'elenco all'apertura della finestra. Un clic seleziona una riga, che mostra allora quattro pulsanti:

Ascolta
: Riproduce il file audio in Polyphone, sull'uscita audio (un secondo clic lo ferma, e selezionando un'altra riga durante la riproduzione viene riprodotto invece il suo file), oppure apre il file MIDI nel [lettore MIDI](#doc_midi_player).

Cartella
: Apre la cartella del file.

Elimina
: Sposta il file nel cestino, dopo una conferma.

Importa
: Importa il file audio come campione nel soundfont visualizzato.
  Questo pulsante è disponibile solo se è visualizzato un editor di soundfont.

Il menu a destra del banner contiene le seguenti opzioni:

Avvia automaticamente
: Il pulsante di registrazione attende il primo suono o segnale MIDI prima di avviare la registrazione.
  Dopo un arresto automatico, la registrazione successiva viene attesa allo stesso modo, fino a un clic sul pulsante di arresto.

Ferma dopo un silenzio
: Numero di secondi senza suono né segnale MIDI che ferma una registrazione, da 1 a 10 (:guilabel:`disattivato` per nessun arresto automatico).
  Il silenzio viene rimosso dal file, e un tasto ancora premuto non è un silenzio.

Livello del suono
: Solo per l'ingresso audio: livello a partire dal quale l'ingresso è considerato un suono, al di sopra del rumore di fondo.
  Questo livello è fissato a -90 dB per l'uscita di Polyphone.

Chiedi un nome di file
: Una finestra di dialogo chiede il nome del file all'inizio di una registrazione.

Cambia cartella...
: Seleziona la cartella delle prossime registrazioni, comune ai file audio e MIDI.

Viene mostrato un messaggio se una registrazione non può continuare (disco pieno o troppo lento, file che raggiunge 4 GB, cambio dell'ingresso audio o della frequenza di campionamento dell'uscita audio): ciò che è stato registrato fino a quel momento resta nel file.


## Tastiera virtuale {#doc_keyboard}


Facendo clic sull'icona ![pianoforte](images/toolbar_piano.png "pianoforte"){.inline} si apre una finestra con la tastiera virtuale, pensata per provare campioni, strumenti e preset.
Questa finestra genera segnali MIDI:

* **note on** / **note off** / **aftertouch** con la tastiera del pianoforte,
* altri segnali MIDI se la **sezione dei controller MIDI** è espansa.

Il banner nella parte superiore della finestra contiene un elenco per scegliere il tipo di tastiera (5 ottave, 6 ottave, 88 tasti o 128 tasti) e mostra la nota, la velocità e l'aftertouch della nota suonata.
Il menu a destra del banner mostra o nasconde i [controller MIDI](#doc_keyboard_controllers) e gli [effetti audio](#doc_keyboard_effects), visualizzati sopra la tastiera.

La finestra mantiene le proporzioni della tastiera: modificandone la larghezza cambia l'altezza.
Mostrare o nascondere un'area cambia solo l'altezza della finestra, che si allunga o si accorcia dal lato superiore in modo che la tastiera resti nello stesso punto dello schermo (non è possibile con Wayland su Linux).

La tastiera virtuale può essere mostrata anche dall'[esploratore di soundfont](manual/soundfont-explorer.md#doc_play), per suonare gli elementi dei file di una cartella senza aprirli.


![Tastiera virtuale](images/virtual_keyboard.png "Tastiera virtuale")


### Tastiera del pianoforte


La tastiera del pianoforte genera segnali che contengono il numero di nota e la velocità.
Può essere controllata in tre modi:

con il mouse
: Si può fare clic con il mouse sul tasto desiderato.
  La velocità dipende dalla posizione verticale del clic.
  Dopo un clic, i movimenti successivi all'interno dello stesso tasto generano segnali di aftertouch.

con la tastiera del computer
: I tasti usati sono definiti nelle [impostazioni](manual/settings.md#doc_keyboard).
  Suonano non appena la finestra della tastiera virtuale è attiva, anche se un altro elemento di questa finestra ha il focus (una manopola, l'elenco a discesa…).

con una tastiera MIDI esterna
: L'ingresso MIDI è definito nelle [impostazioni](manual/settings.md#doc_general).
  Se la tastiera del pianoforte è visibile, mostra ciò che stai suonando.


#### Indicatori e colori


* Quando è selezionato uno [strumento](manual/soundfont-editor/editing-pages/instrument-editor.md) o un [preset](manual/soundfont-editor/editing-pages/preset-editor.md), l'area non coperta dalle divisioni è **in grigio**.
  È così possibile visualizzare l'estensione completa di uno strumento o di un preset.
* Quando è selezionata una divisione di uno strumento o di un preset, il suo intervallo viene mostrato nello stesso modo.
  Inoltre, la sua nota di base è contrassegnata da un **indicatore arancione**.
* Quando un tasto è selezionato sulla tastiera, appare in **blu** (o nel colore di selezione scelto nelle [impostazioni](manual/settings.md#doc_interface) del software).
  Inoltre, l'intervallo della divisione corrispondente appare in **azzurro**.
  Questo vale solo a livello di strumento.
* Il Do centrale (nota 60) è contrassegnato da un **indicatore nero**.


#### Scorciatoie utili


:kbd:`Ctrl`+:kbd:`K`
: Questa scorciatoia porta in qualsiasi momento il focus sulla tastiera virtuale (la tastiera viene mostrata se era nascosta).
  Ciò permette, ad esempio durante la modifica di una tabella, di usare la tastiera del computer per suonare invece di modificare una cella.
  Un effetto luminoso sulla tastiera virtuale conferma che il focus è su di essa.

:kbd:`Ctrl`+:kbd:`1`, :kbd:`Ctrl`+:kbd:`2`, … :kbd:`Ctrl`+:kbd:`8`
: Queste scorciatoie cambiano le ottave suonate con la tastiera del computer.
  Dopo :kbd:`Ctrl`+:kbd:`1` si possono suonare le ottave più gravi.
  Le ottave più acute si raggiungono con :kbd:`Ctrl`+:kbd:`8`.
  Funzionano anch'esse non appena la finestra della tastiera virtuale è attiva.

:kbd:`→` / :kbd:`←`
: La freccia destra o sinistra suona la nota immediatamente sopra o sotto l'ultimo tasto suonato, con la stessa velocità.
  In combinazione con il tasto :kbd:`Shift`, si saltano 6 tasti invece di 1.

:kbd:`↑` / :kbd:`↓`
: La freccia su o giù suona la stessa nota dell'ultimo tasto suonato, ma con una velocità maggiore o minore.
  In combinazione con il tasto :kbd:`Shift`, la velocità cambia di 10 invece che di 1.

Le frecce suonano le note solo se la tastiera del pianoforte ha il focus: altrimenti agiscono sull'elemento che ha il focus, ad esempio una manopola.


### Controller MIDI {#doc_keyboard_controllers}


Quando l'opzione :guilabel:`Controller MIDI` è spuntata nel menu a destra del banner, l'area dei controller MIDI viene mostrata sopra la tastiera.
Se lavori con i modulatori, è consigliabile usare queste funzionalità per provare il tuo soundfont.


![Tastiera virtuale con controller MIDI](images/virtual_keyboard_2.png "Tastiera virtuale con controller MIDI")


Quest'area è composta da diversi comandi:

Ruota del pitch
: La ruota del pitch alza o abbassa l'intonazione.
  Torna automaticamente alla posizione iniziale 0.

Sensibilità
: La sensibilità definisce l'escursione, in semitoni, della ruota del pitch.
  Il valore predefinito è 2 (un tono intero).

Pressione
: La pressione è la forza esercitata sulla tastiera.
  Può essere paragonata all'aftertouch, ma mentre l'aftertouch è specifico di ciascun tasto, la pressione è un segnale globale per tutti i tasti.

Manopole
: 3 manopole permettono di modificare i valori dei controller MIDI.
  Per ogni manopola puoi specificare un numero di controller e poi il suo valore.

LED acceso / spento
: Questo LED imposta il valore di un controller MIDI a 0 o 127 (utile ad esempio per il pedale di sustain).
  Funziona come una manopola, ma con solo due valori possibili.

Tutti questi elementi possono essere sincronizzati con una tastiera MIDI collegata al computer, se l'ingresso MIDI è impostato correttamente nelle [impostazioni](manual/settings.md#doc_general). Per il controllo delle manopole sono supportati i valori relativi dei controller MIDI.


### Effetti audio {#doc_keyboard_effects}


Quando l'opzione :guilabel:`Effetti` è spuntata nel menu a destra del banner, nella parte superiore della finestra compare un'area per regolare le principali impostazioni del sintetizzatore mentre si suona:

Riverbero
: Un LED attiva o disattiva il riverbero, poi 4 manopole ne modificano il livello, la dimensione della stanza, l'ampiezza e lo smorzamento.
  Il nome di una manopola compare passandoci sopra con il mouse.

Chorus
: Un LED attiva o disattiva il chorus, poi 3 manopole ne modificano il livello, la profondità e la frequenza.

Volume
: Il cursore modifica il volume generale.

Queste impostazioni sono le stesse delle [impostazioni del suono](manual/settings.md#doc_sound): una modifica fatta in un punto viene mostrata anche nell'altro.
Una modifica è progressiva e non interrompe le note in corso.


## Lettore MIDI {#doc_midi_player}


Facendo clic sull'icona ![MIDI](images/toolbar_midi.png "MIDI"){.inline} si apre una finestra con il lettore MIDI, che riproduce un file MIDI (:file:`.mid`, :file:`.midi`, :file:`.kar` o :file:`.rmi`) con i soundfont aperti o con i file selezionati in un [esploratore di soundfont](manual/soundfont-explorer.md#doc_play), per provarli in un brano musicale.
I soundfont vengono suonati così come sono nell'editor, con le modifiche non salvate.
Il lettore MIDI è disponibile anche in [modalità sintetizzatore](tutorials/use-polyphone-as-a-synthesizer.md).


![Lettore MIDI](images/midi_player.png "Lettore MIDI")


La fascia in alto nella finestra contiene, da sinistra a destra:

* un pulsante per aprire un file MIDI (all'apertura della finestra, l'ultimo file aperto viene ricaricato se esiste ancora),
* il nome del file, il tempo trascorso e la durata totale, sopra un cursore per spostarsi in un altro punto del brano,
* un pulsante per avviare o mettere in pausa la riproduzione, accessibile anche con :kbd:`Space`,
* un pulsante per fermare la riproduzione e tornare all'inizio,
* un pulsante che apre le [impostazioni di riproduzione](#doc_midi_player_settings).

Se il file non può essere letto, il motivo viene mostrato al posto dei canali.
La riproduzione si ferma quando la finestra viene chiusa.

Durante la riproduzione, una tastiera MIDI esterna o la [tastiera virtuale](#doc_keyboard) possono ancora suonare l'elemento selezionato nell'editor insieme al file (le note del file non vengono mostrate sulla tastiera virtuale), e il [registratore](#doc_recorder) può registrare il risultato.


### Canali


La tabella elenca i canali del file che contengono note, ognuno con le proprie impostazioni:

Canale
: Il numero del canale; il nome della sua traccia viene mostrato in un suggerimento.
  I pulsanti :guilabel:`M` (silenzia) e :guilabel:`S` (solo) silenziano il canale o fanno sentire solo i canali in solo.
  Il pulsante MIDI mostra i valori MIDI attuali del canale che differiscono dal loro valore predefinito: controller, ruota del pitch (in semitoni), la sua sensibilità e pressione del canale.

Soundfont
: Il soundfont suonato dal canale: :guilabel:`Selezione`, uno dei soundfont aperti nelle schede, oppure :guilabel:`Nessuno` per il silenzio.
  :guilabel:`Selezione`, scelta per tutti i canali all'apertura di un file, suona ciò che è selezionato nella finestra principale: il soundfont della scheda corrente, o il file selezionato in un [esploratore di soundfont](manual/soundfont-explorer.md#doc_play), il cui nome è indicato tra parentesi.
  Le altre schede (schermata iniziale, impostazioni, archivio online…) mantengono il soundfont suonato.
  Quando la selezione cambia, un indicatore di caricamento gira davanti al nome mentre il soundfont viene caricato, e nel frattempo continua a suonare il precedente; poi i preset di questi canali tornano su :guilabel:`Auto`.

Preset
: Con :guilabel:`Auto`, i preset vengono scelti dai cambi di programma del file (banco 128 sul canale 10, per le percussioni) e il preset corrente viene mostrato tra parentesi.
  Altrimenti viene suonato il preset scelto nell'elenco, qualunque cosa richieda il file.
  Se il soundfont non contiene il preset richiesto dal file, viene mostrata un'icona di avviso e le note del canale non vengono suonate.

Volume
: Il LED si accende quando iniziano delle note, e il cursore modifica il volume del canale da -40 a +10 dB (un doppio clic lo riporta a 0 dB).

L'elenco dei soundfont o dei preset si apre con un doppio clic su una cella, o con un clic su una cella già selezionata.
Queste impostazioni vengono reimpostate quando si apre un altro file.


### Impostazioni di riproduzione {#doc_midi_player_settings}


Il pulsante a destra della fascia apre un menu con le seguenti impostazioni, che vengono mantenute quando si apre un altro file:

Riproduzione in loop
: Il brano ricomincia dall'inizio quando finisce.

Tempo
: Velocità di riproduzione, dal 25% al 200% del tempo del file (i suoi cambi di tempo vengono mantenuti).
  Il tempo del file viene mostrato in un suggerimento, e il tempo trascorso e la durata mostrati nella fascia seguono la velocità di riproduzione.

Intonazione
: Trasposizione da -12 a +12 semitoni, tranne che per le percussioni (banco 128).

Un doppio clic su un cursore lo riporta al suo valore predefinito (100% e 0).


![Impostazioni di riproduzione](images/midi_player_settings.png "Impostazioni di riproduzione")


Il menu termina con :guilabel:`Esporta audio...`, che scrive il brano in un file :file:`.wav` così come si sente nel lettore: stessi soundfont, preset, volumi, silenziamento e solo, tempo e intonazione.
La riproduzione viene messa in pausa durante l'esportazione, che termina quando il suono delle ultime note si è spento.
Se l'esportazione viene annullata, il file incompleto viene eliminato.
Un file MIDI può anche essere convertito in un file wav dalla [riga di comando](manual/annexes/command-line.md#doc_render).

*[MIDI]: Musical Instrument Digital Interface
