La barra degli strumenti, situata nella parte superiore dell'editor, contiene icone per:

* le [funzioni di modifica](#doc_edit) più comuni,
* il cambio di [vista](#doc_view) negli editor,
* la visualizzazione del [registratore](#doc_recorder),
* la visualizzazione della [tastiera virtuale](#doc_keyboard).


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


Facendo clic sull'icona ![registratore](images/toolbar_recorder.png "registratore"){.inline} si apre una finestra con il registratore, che può registrare l'uscita di Polyphone in un file :file:`.wav`. Il tempo trascorso viene mostrato nella finestra.


![Registratore](images/recorder.png "Registratore")


Il pulsante sinistro può avere due stati:

Registra
: Avvia una nuova registrazione se non è in corso alcuna acquisizione.
  Una finestra di dialogo chiede il nome del file che conterrà il segnale audio registrato.

Stop
: Interrompe l'acquisizione in corso.
  L'acquisizione si interrompe anche alla chiusura della finestra.

Anche il pulsante destro può avere due stati (disponibile solo durante un'acquisizione):

Pausa
: Mette in pausa l'acquisizione in corso.

Riprendi
: Riprende l'acquisizione in corso.


## Tastiera virtuale {#doc_keyboard}


Facendo clic sull'icona ![pianoforte](images/toolbar_piano.png "pianoforte"){.inline} si apre una finestra con la tastiera virtuale, pensata per provare campioni, strumenti e preset.
Questa finestra genera segnali MIDI:

* **note on** / **note off** / **aftertouch** con la tastiera del pianoforte,
* altri segnali MIDI se la **sezione dei controller MIDI** è espansa.

La nota, la velocità e l'aftertouch della nota suonata vengono mostrati nella parte inferiore della finestra, insieme a un elenco a discesa per scegliere la dimensione della tastiera (5 ottave, 6 ottave, 88 tasti o 128 tasti).


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

:kbd:`→` / :kbd:`←`
: La freccia destra o sinistra suona la nota immediatamente sopra o sotto l'ultimo tasto suonato, con la stessa velocità.
  In combinazione con il tasto :kbd:`Shift`, si saltano 6 tasti invece di 1.

:kbd:`↑` / :kbd:`↓`
: La freccia su o giù suona la stessa nota dell'ultimo tasto suonato, ma con una velocità maggiore o minore.
  In combinazione con il tasto :kbd:`Shift`, la velocità cambia di 10 invece che di 1.


### Controller MIDI


Facendo clic sulla freccia in basso a destra si può mostrare o nascondere l'area dei controller MIDI.
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

*[MIDI]: Musical Instrument Digital Interface
