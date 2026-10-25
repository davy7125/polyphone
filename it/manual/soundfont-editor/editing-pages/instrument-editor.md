Facendo clic su uno o più strumenti nell'[albero](manual/soundfont-editor/tree.md), compare la pagina di modifica degli strumenti.

Questa pagina comprende diverse parti:

* la [tabella](#doc_table),
* l'[editor degli intervalli](#doc_range), quando si fa clic sul pulsante :guilabel:`Intervalli` della [barra degli strumenti](manual/soundfont-editor/toolbar.md#doc_view),
* l'[editor degli inviluppi](#doc_envelope), quando si fa clic sul pulsante :guilabel:`Inviluppi` della [barra degli strumenti](manual/soundfont-editor/toolbar.md#doc_view),
* la [sezione dei modulatori](#doc_modulator).


![Pagina degli strumenti](images/edit_instrument.png "Pagina degli strumenti")


Se è selezionato un solo strumento, la pagina permette di modificare i parametri globali dello strumento e i parametri di tutte le sue divisioni.

Se sono selezionati più strumenti, si possono modificare i parametri globali di ciascuno di essi.


![Pagina degli strumenti con più strumenti selezionati](images/edit_multiple_instruments.png "Pagina degli strumenti con più strumenti selezionati")


## Tabella {#doc_table}


### Presentazione


La tabella offre una vista d'insieme della configurazione di uno strumento e ne permette la modifica.
Una modifica di un parametro nella divisione :guilabel:`Globale` si applica a tutte le divisioni, tranne a quelle in cui lo stesso parametro è già specificato (sovrascrittura locale).

Le righe scritte in corsivo (:guilabel:`Nota di base del campione`, :guilabel:`Lunghezza del campione` e :guilabel:`Loop`) non sono modificabili: ricordano soltanto, a titolo informativo, le proprietà del campione collegato a ciascuna divisione.

I parametri modificabili sono:

Intervallo di note
: Intervallo di note in cui si sente il campione.

Intervallo di velocità
: Intervallo di velocità in cui si sente il campione.
  È possibile impostare suoni diversi per la stessa nota, con intervalli di velocità diversi, ad esempio per distinguere diverse sfumature di un tasto di pianoforte.

Attenuazione (dB)
: Attenuazione in decibel applicata al campione.\
  Nota: il formato sf2 non consente valori negativi a livello di strumento, quindi non è possibile alcuna amplificazione.

Pan
: Bilanciamento applicato al suono, per sentirlo a sinistra o a destra in stereo.

Riproduzione in loop
: ![Loop spento](images/loop_off.png "loop spento"){.inline} indica un suono riprodotto senza loop.
  ![Loop acceso](images/loop_on.png "loop acceso"){.inline} indica un suono che va in loop continuamente.
  ![Release](images/sample_mode_release.png "release"){.inline} serve ad avviare la riproduzione di un campione quando si rilascia un tasto. Il volume continua a essere definito dall'inviluppo del volume, e viene aggiunto un breve attacco (0,2 ms) per ammorbidire l'inizio del campione al rilascio.
  ![Loop acceso + release](images/loop_on_end.png "loop acceso e release"){.inline} indica un suono che va in loop per tutto il tempo in cui il tasto è premuto e poi prosegue riproducendo il resto del campione.\
  Nota: la modalità di riproduzione «release» NON è documentata dal formato sf2, quindi altri lettori come FluidSynth potrebbero decidere di non implementarla.
  Un altro modo per ottenere questo comportamento è affiancare a ogni campione un campione che contiene un breve loop silenzioso seguito dal suono da emettere al rilascio.
  Il campione aggiunto verrà riprodotto con il parametro di riproduzione «loop acceso + release».
  Vedi il tutorial «[Usare release personalizzati in uno strumento](tutorials/using-custom-releases-in-an-instrument.md)».

Nota di base
: Permette di cambiare la nota di base normale di un campione con una nota diversa da quella usata abitualmente.
  Ad esempio, se un campione ha nota di base 50 ed è suonato sull'intervallo di note [49 — 51], verrà inizialmente trasposto rispettivamente di -1, 0 e +1 semitono sulle note 49, 50 e 51.
  Se poi la nota di base viene sovrascritta con 51 a livello di strumento, lo stesso campione verrà inizialmente trasposto rispettivamente di -2, -1 e 0 semitoni sulle note 49, 50 e 51.

Accordatura (semitoni e centesimi)
: Regola l'accordatura in semitoni e centesimi di semitono.
  Questa trasposizione si aggiunge alla trasposizione iniziale legata alla nota suonata (ad esempio, la trasposizione iniziale di un campione con nota di base 60 sarà di +2 semitoni se il tasto premuto sulla tastiera è il 62).

Accordatura (scala)
: Questo parametro modifica l'intervallo tra due note consecutive e influisce quindi sulla trasposizione iniziale legata alla nota suonata.
  Un valore di 100 corrisponde alla scala temperata equabile: si usano 100 centesimi di semitono per trasporre un campione da un tasto al successivo.
  Un valore di 0 disattiva questa trasposizione iniziale, quindi il campione verrà riprodotto in modo identico su tutti i tasti della tastiera.

Filtro
: Applica un filtro passa-basso, specificando la frequenza di taglio e la risonanza.

Inviluppo del volume
: Modifica l'inviluppo del volume ed eventualmente la sua variazione in funzione della nota.

Modulazione
: Imposta una modulazione (inviluppo, ritardo, frequenza, variazione in funzione della nota) e il suo effetto su intonazione, filtro e volume.

Vibrato
: Imposta un vibrato (ritardo, frequenza) e il suo effetto sull'intonazione.

Classe esclusiva
: Questo parametro permette di definire una o più classi esclusive, assegnando a un insieme di divisioni di una classe lo stesso valore diverso da 0.
  Quando una classe esclusiva è definita, ogni nota suonata da una delle divisioni della classe esclusiva interrompe tutti gli altri suoni della stessa classe.
  L'ambito di una classe esclusiva è il preset in cui la classe è definita (un suono attivato in uno strumento interrompe il suono di un altro strumento se i due strumenti si trovano nello stesso preset).
  Questo effetto può essere usato per strumenti a percussione come un piatto.
  In pratica, suonare una nota interrompe automaticamente un'altra nota dello stesso gruppo.

Chorus e riverbero
: Indica l'intensità degli effetti di chorus e riverbero.
  Gli effetti sono configurabili nelle [impostazioni](manual/settings.md#doc_sound) del software.

Nota e velocità fisse
: Forza il valore della nota e della velocità su tutta l'estensione della divisione.

Offset
: Cambia le posizioni di inizio e di fine di un suono durante la riproduzione, così come l'inizio e la fine del loop.
  L'unità di un offset è il campione audio (numero di valori).


### Facilitazioni di modifica nella tabella


Per facilitare e velocizzare la modifica nella tabella, sono state implementate le seguenti funzionalità:

* la rotellina del mouse può essere usata per cambiare un valore (la cella deve prima essere in modalità di modifica),
* una tastiera MIDI esterna (o la [tastiera virtuale](manual/soundfont-editor/toolbar.md#doc_keyboard)) può essere usata per impostare la nota di base, l'intervallo di note e l'intervallo di velocità (la cella deve prima essere in modalità di modifica),
* una selezione multipla, seguita dalla modifica dell'ultima cella, modifica tutte le celle contemporaneamente,
* i tasti :kbd:`Del` e :kbd:`Backspace` cancellano il contenuto di tutte le celle selezionate,
* le divisioni stereo possono essere modificate contemporaneamente se l'opzione è attiva nelle [impostazioni](manual/settings.md#doc_general) del software,
* i nomi delle note possono essere numerici o testuali (con C3, C4 o C5 come Do centrale), a seconda dell'opzione scelta nelle [impostazioni](manual/settings.md#doc_interface),
* copia / incolla / taglia di un insieme di celle (non necessariamente adiacenti) si possono fare con :kbd:`Ctrl`+:kbd:`C` / :kbd:`Ctrl`+:kbd:`V` / :kbd:`Ctrl`+:kbd:`X`.

Non dimenticare le [funzioni](manual/soundfont-editor/tools/instrument-tools.md) disponibili!
In particolare la funzione che permette una [configurazione basata su tasti](manual/soundfont-editor/tools/instrument-tools.md#doc_global).


### Silenziare le divisioni {#doc_solo}


A scopo di test, nelle tabelle è possibile silenziare una divisione oppure silenziare tutte le divisioni tranne una.
Le opzioni sono accessibili con un clic destro sull'intestazione di una divisione, e anche nel menu contestuale dell'[albero](manual/soundfont-editor/tree.md#doc_menu).


![Silenziare le divisioni in una tabella](images/table_mute_divisions.png "Silenziare le divisioni in una tabella")


## Editor degli intervalli {#doc_range}


L'editor degli intervalli permette di disporre rapidamente e graficamente i campioni in base ai loro intervalli di note e di velocità.
Questo grafico mette inoltre in evidenza problemi di configurazione, come zone prive di campioni o sovrapposizioni indesiderate.


![Editor degli intervalli](images/edit_range.png "Editor degli intervalli")


Ogni campione è rappresentato da un rettangolo semitrasparente, così che la sovrapposizione di più elementi sia riconoscibile.


### Selezione dei rettangoli


Un semplice clic sinistro su un rettangolo lo seleziona e annulla tutte le altre selezioni.
Se più rettangoli sono sovrapposti, clic successivi cambiano la selezione sotto il cursore del mouse.
Un doppio clic su un rettangolo apre il [campione](manual/soundfont-editor/editing-pages/sample-editor.md) corrispondente (o lo strumento corrispondente nell'[editor dei preset](manual/soundfont-editor/editing-pages/preset-editor.md)).

È possibile una selezione multipla:

* tenendo premuto il tasto :kbd:`Ctrl` e facendo poi clic sui diversi rettangoli per selezionarli uno a uno,
* usando il tasto :kbd:`Shift`: vengono selezionati tutti i rettangoli compresi tra il primo rettangolo cliccato e il successivo.


### Modificare i rettangoli


Dopo aver selezionato uno o più rettangoli, con il tasto sinistro è possibile:

* cambiarne la dimensione, afferrando uno dei bordi di un rettangolo;
* cambiarne la posizione, puntando al centro di un rettangolo.

Se sono selezionati più rettangoli, tutte le modifiche vengono applicate contemporaneamente.
Il tasto :kbd:`Esc` annulla una modifica in corso.


### Trascinamento e zoom


Tenendo premuto il tasto destro si può cambiare lo zoom attorno al punto selezionato.

Tenendo premuto il tasto sinistro si può spostare il grafico, se il punto iniziale non si trova su un rettangolo.

Un clic con il tasto centrale suona una nota: la nota è data dalla posizione orizzontale del mouse e la velocità dalla sua posizione verticale.


## Editor degli inviluppi {#doc_envelope}


L'editor degli inviluppi offre una visione grafica degli 8 parametri che definiscono l'inviluppo del volume e degli 8 parametri che definiscono l'inviluppo di modulazione.
È quindi uno strumento per capire meglio l'effetto di ciascuno di questi parametri.


![Editor degli inviluppi](images/edit_envelope.png "Editor degli inviluppi")


I pulsanti sopra il grafico selezionano alternativamente per la modifica l'inviluppo del volume (mostrato in verde) o l'inviluppo di modulazione (mostrato in rosso).
Durante la modifica dell'inviluppo di modulazione, l'inviluppo del volume viene mostrato con una linea sottile.

La parte sinistra del grafico rappresenta l'evoluzione dell'inviluppo quando un tasto viene **premuto**.
7 degli 8 parametri intervengono in questa rappresentazione:

Delay
: Durata durante la quale l'inviluppo resta a 0 subito dopo la pressione di un tasto. Nella maggior parte dei casi si vuole sentire il suono subito dopo aver premuto i tasti, quindi questo parametro resta a zero. Il **delay** è però utile, ad esempio, quando si vogliono creare suoni pad più ricchi costruiti con più strati. Supponiamo di voler far entrare un secondo strato sonoro 0,5 secondi dopo la pressione del tasto: basta inserire «0,5» in questo campo. Un delay non trattiene la riproduzione del suono, ma lo silenzia. Se il delay è impostato a 0,5 secondi, si inizierà a sentire il campione così com'è dopo 0,5 secondi di riproduzione, dato che i primi 0,5 secondi sono stati riprodotti in silenzio.

Attack
: Durata dopo il **delay** durante la quale l'inviluppo sale da 0 al valore massimo. Ad esempio, il suono di una corda strofinata come il violino ha un attacco piuttosto lungo, mentre quello di una corda pizzicata come la chitarra è molto breve. Anche quando si vuole un attacco il più breve possibile, è consigliabile inserire almeno un valore di 0,001, altrimenti si potrebbe sentire un «clic» dopo aver premuto un tasto.

Hold
: Durata dopo l'**attack** durante la quale l'inviluppo mantiene il valore massimo.

Decay
: Durata dopo l'**hold** durante la quale l'inviluppo passa progressivamente dal valore massimo al valore specificato dal **sustain**. Se nel **sustain** non è specificata alcuna riduzione (0 dB, il valore predefinito), durante il **decay** il volume resta al massimo qualunque sia il valore specificato.

Sustain
: Valore dopo il **decay**, mantenuto finché il tasto premuto non viene rilasciato. Come l'**attenuazione**, il valore del sustain è una riduzione espressa in decibel. Se si vuole dimezzare il volume dopo il tempo di decay, bisogna impostare un valore di 6 dB. Si può indicare il valore «144» se si vuole silenziare completamente il suono.

Key → Hold
: Variazione della durata dell'**hold** in funzione della nota (vedi sotto).

Key → Decay
: Variazione della durata del **decay** in funzione della nota (vedi sotto).

Un valore positivo in **Key → XXX** riduce gradualmente la durata di XXX verso gli acuti, mentre un valore negativo fa il contrario.
Ad esempio:

* il valore 100 dimezza la durata di XXX per ogni ottava sopra il Do centrale (60) e la raddoppia per ogni ottava sotto il Do centrale,
* il valore -100 raddoppia la durata di XXX per ogni ottava sopra il Do centrale (60) e la dimezza per ogni ottava sotto il Do centrale.

Questi parametri possono essere utili quando si crea una chitarra o un pianoforte con il pedale di risonanza premuto: più la nota è grave, più dura a lungo prima che non si senta più alcun suono.

La parte destra rappresenta l'evoluzione dell'inviluppo quando un tasto viene **rilasciato**.
In questa rappresentazione interviene solo la durata del **release**.
Durante questa durata l'inviluppo torna gradualmente a 0.
I suoni di violino o i pad di sintetizzatore hanno un tempo di release relativamente lungo, mentre quello di altri suoni, come l'organo, è molto breve. In pratica, anche per tempi di release brevi, è consigliabile impostare una durata di almeno 0,2 secondi, perché un release troppo breve non risulta piacevole né naturale (dipende comunque da ciò che si ha in mente per lo strumento da creare).

Quando nell'albero è selezionata una sola divisione di uno strumento, il [campione](manual/soundfont-editor/editing-pages/sample-editor.md) corrispondente viene mostrato sullo sfondo.
È così possibile visualizzare meglio l'evoluzione dell'inviluppo rispetto al campione riprodotto.


## Modulatori {#doc_modulator}


La sezione inferiore della pagina di modifica degli strumenti è destinata alla creazione di modulatori, a livello globale per uno strumento o per una divisione specifica.
Quando suoni uno strumento, un modulatore ascolta uno o due segnali MIDI e modifica uno dei parametri della tabella secondo la regola matematica configurata.
L'uscita di un modulatore può anche essere usata come ingresso di un altro modulatore (questa funzionalità è comparsa nella versione 2.04 del [formato sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2) e potrebbe non essere supportata da alcuni sintetizzatori).


![Editor dei modulatori](images/modulator_editor_2.png "Editor dei modulatori")


I pulsanti a sinistra servono per:

* espandere / comprimere l'area dei modulatori,
* aggiungere / rimuovere un modulatore,
* copiare / incollare / duplicare uno o più modulatori.

Quando si aggiunge un modulatore, il bordo della cella corrispondente nella tabella diventa più spesso (vedi l'accordatura nella colonna :guilabel:`Globale` qui sopra).

I modulatori di una divisione (o della divisione globale) vengono mostrati solo se è selezionata una singola divisione.
Quando la sezione è compressa, un riepilogo indica il numero di modulatori e i parametri che modificano.
Selezionando delle celle nella tabella si selezionano i modulatori che agiscono sui parametri corrispondenti, e viceversa.

Ogni modulatore è composto, da sinistra a destra, dai seguenti elementi:

Sorgenti
: La prima sorgente è un segnale MIDI: la velocità o la nota della nota suonata, la pressione polifonica, la pressione del canale, la ruota del pitch, la sensibilità della ruota del pitch o un controller MIDI (:guilabel:`CC`).
  Può anche essere l'uscita di uno o più altri modulatori (:guilabel:`Collegamento`), cioè quelli che hanno questo modulatore come destinazione.
  La seconda sorgente è facoltativa e modifica l'effetto della prima.

Curve
: Accanto a ogni sorgente, un pulsante dà accesso a una griglia di curve per scegliere come viene convertito il segnale: lineare, concava, convessa o interruttore, crescente o decrescente, unipolare (da 0 a 1) o bipolare (da -1 a 1).

Quantità
: Ampiezza massima della modulazione.

Trasformazione
: :guilabel:`nessuna operazione` o :guilabel:`valore assoluto` (viene usato il valore assoluto del risultato della modulazione).

Destinazione
: Il parametro modificato dal modulatore, oppure un altro modulatore (:guilabel:`Modulatore`).
  Il testo sotto la destinazione indica l'escursione della modulazione: i valori aggiunti al parametro (:guilabel:`Aggiungi da:` … :guilabel:`A:`) o i fattori applicati (:guilabel:`Moltiplica da:` … :guilabel:`A:`).

Quando un modulatore è identico a uno che si trova più in alto nell'elenco, viene mostrata l'etichetta :guilabel:`sovrascritto da …`.
A livello di strumento, l'etichetta :guilabel:`sostituisce mod. predefinito` indica un modulatore che sostituisce uno dei [modulatori predefiniti](manual/soundfont-editor/tools/instrument-tools.md#doc_default_mod) del formato sf2, mentre :guilabel:`disattiva mod. predefinito` indica che la sua quantità è 0 (il modulatore predefinito viene quindi disattivato).
