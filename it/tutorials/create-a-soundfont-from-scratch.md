Per creare un [soundfont](manual/annexes/the-different-soundfont-formats.md#doc_sf2), fai clic su :guilabel:`Nuovo soundfont` nella [schermata iniziale](manual/index.md#doc_left).
I soundfont hanno una struttura a 3 livelli, quindi la creazione di un nuovo soundfont prevede tre passaggi fondamentali:

* la [preparazione dei campioni](#doc_sample),
* la [creazione degli strumenti](#doc_instrument), composti da campioni,
* la [creazione dei preset](#doc_preset), composti da strumenti.


## Preparazione dei campioni {#doc_sample}


### Caricamento dei campioni


La preparazione dei campioni inizia con il **caricamento** di file audio (ad esempio .wav).
Per farlo, fai clic su :guilabel:`Aggiungi un campione` nella [barra degli strumenti](manual/soundfont-editor/toolbar.md#doc_edit) e seleziona uno o più file audio (i file possono anche essere trascinati nell'[albero](manual/soundfont-editor/tree.md#doc_dragdrop)).
I campioni si possono ottenere:

* da internet,
* registrando uno strumento musicale reale,
* per sintesi, con un software specializzato.


### Loop dei campioni


Poi può essere necessario un **loop** nel campione, se vuoi che suoni più a lungo della sua durata normale.
Ad esempio, se hai solo un campione di flauto di 1 secondo ma vuoi che suoni all'infinito.
Per farlo si definiscono, manualmente o automaticamente, dei punti di loop (inizio + fine) entro i limiti del campione, in modo che la zona del loop possa essere ripetuta e il campione suoni più a lungo.
I punti di loop si possono definire manualmente, ma usare la funzione «[Auto loop](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop)» per definirli automaticamente di solito dà risultati migliori ed è molto più veloce.

Per definire manualmente i punti di loop:

1. fai clic nell'[albero](manual/soundfont-editor/tree.md) sul campione da mettere in loop,
2. nell'[editor dei campioni](manual/soundfont-editor/editing-pages/sample-editor.md), posiziona a piacere l'inizio e la fine del loop nel [grafico](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph) (area di visualizzazione WAV), a meno che non sia già definito un loop.
  Usa il clic sinistro per l'inizio e il clic destro per la fine.
  Il punto di loop destro va inserito per primo (perché il punto destro si trova per impostazione predefinita nella posizione 0 e non è possibile posizionare un punto sinistro dopo il punto destro).
3. premi il pulsante :guilabel:`Riproduci` dopo aver attivato l'opzione :guilabel:`loop`,
4. regola l'inizio e / o la fine del loop durante la riproduzione del campione, finché la transizione tra le due posizioni non è il più fluida possibile.


**Nota:** per sentire il loop in uno strumento, devi selezionare ![loop acceso](images/loop_on.png "loop acceso"){.inline} nella riga del parametro :guilabel:`Riproduzione in loop`, nella colonna :guilabel:`Globale` o nelle colonne delle singole note della [tabella dei parametri](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table). ![loop acceso](images/loop_on.png "loop acceso"){.inline} lo attiva, ![loop spento](images/loop_off.png "loop spento"){.inline} o una cella vuota lo disattivano.


![Illustrazione del loop](images/loop_illustration.png "Illustrazione del loop")


### Accordatura dei campioni


Infine bisogna eseguire l'**accordatura**.
Per farlo, nella pagina di modifica dei [campioni](manual/soundfont-editor/editing-pages/sample-editor.md#doc_player) è disponibile una funzione di calibrazione (sinusoide).
Per ogni campione si procede così:

1. avvia la riproduzione (premi il pulsante :guilabel:`Riproduci`), se possibile con l'opzione :guilabel:`loop` attiva,
2. attiva l'opzione :guilabel:`sinusoide`,
3. regola il cursore del volume per sentire i due suoni il più chiaramente possibile,
4. cambia la :guilabel:`Nota di base` finché i due suoni non si avvicinano il più possibile,
5. regola la :guilabel:`Correzione` (in centesimi) per accordare il campione sulla nota di base.
   Per farlo, presta attenzione agli eventuali battimenti e fai in modo che siano il più lenti possibile.

L'analisi delle [frequenze](manual/soundfont-editor/editing-pages/sample-editor.md#doc_freq) può essere un buon riferimento per iniziare l'accordatura.


### Per andare oltre


Per modificare i campioni sono disponibili diverse funzioni, ad esempio per:

* [rimuovere il silenzio](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank) all'inizio,
* [equalizzare](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq) il suono,
* [trasporre](manual/soundfont-editor/tools/sample-tools.md#doc_transpose),
* [normalizzare](manual/soundfont-editor/tools/sample-tools.md#doc_volume) il volume,
* [regolare il bilanciamento](manual/soundfont-editor/tools/sample-tools.md#doc_balance) dei campioni stereo.


## Creazione degli strumenti {#doc_instrument}


Per questo passaggio si presuppone che siano disponibili uno o più campioni per creare uno strumento.


### Creare uno strumento e aggiungere campioni


Per prima cosa, fai clic su :guilabel:`Aggiungi uno strumento` nella [barra degli strumenti](manual/soundfont-editor/toolbar.md#doc_edit).
Ora bisogna inserire un nome (se nell'albero sono selezionati dei campioni, possono essere collegati direttamente al nuovo strumento).

Altrimenti, aggiungi i campioni allo strumento con un «[trascinamento](manual/soundfont-editor/tree.md#doc_dragdrop)».
Nell'albero noterai che nello strumento compaiono delle divisioni.

**Nota:** quando si creano le divisioni, i campioni non vengono copiati ma collegati. Non ci sono limiti al numero di divisioni.


### Distribuzione delle divisioni


Quando si aggiungono campioni agli strumenti, questi compaiono come divisioni (colonne) nella [tabella dell'editor degli strumenti](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).
Ogni divisione deve poi essere posizionata sulla tastiera modificando l':guilabel:`Intervallo di note` nella tabella.
In generale è buona norma che l'intervallo di una divisione comprenda la nota di base del campione che rappresenta.
Deve essere coperta l'intera superficie della tastiera (ad esempio dalla nota 36 alla nota 96 per una tastiera di sintetizzatore classica).

La funzione «[Auto-posizionamento campione](manual/soundfont-editor/tools/instrument-tools.md#doc_position)» distribuisce automaticamente i campioni sulla tastiera.


### Impostazione delle divisioni


Per attivare la modalità di riproduzione in loop in uno strumento che contiene campioni in loop, seleziona ![loop acceso](images/loop_on.png "loop acceso"){.inline} nella riga del parametro :guilabel:`Riproduzione in loop`, nella colonna :guilabel:`Globale` o nelle colonne delle singole note della tabella dei parametri.

* ![loop acceso](images/loop_on.png "loop acceso"){.inline} lo attiva,
* ![loop spento](images/loop_off.png "loop spento"){.inline} o una cella vuota lo disattivano.
* ![loop acceso e fine](images/loop_on_end.png "loop acceso e fine"){.inline} attiva il loop e il resto del campione viene letto dopo il rilascio del tasto.

La divisione globale, come indica il nome, permette di inserire un parametro per l'intero strumento.
Un parametro globale si applica solo alle divisioni in cui lo stesso parametro non è specificato.
Ciò significa che i parametri delle divisioni normali hanno la precedenza sulle impostazioni della divisione globale.

Nell'ambito di questo semplice tutorial, non è necessario modificare altri parametri perché lo strumento funzioni.
Dovrebbe già essere possibile suonare con la [tastiera virtuale](manual/soundfont-editor/toolbar.md#doc_keyboard).

Tuttavia, per migliorare lo strumento è consigliabile inserire un valore nella riga :guilabel:`Vol env release (s)`, che evita che il suono si interrompa bruscamente non appena si rilascia un tasto.
Gli altri parametri sono descritti nella descrizione delle [tabelle](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).


## Creazione dei preset {#doc_preset}


Per questo passaggio si presuppone che siano disponibili uno o più strumenti per creare un preset.


### Che cos'è un preset?


Un preset è l'equivalente del nome di una patch sonora nei sintetizzatori hardware.
È visibile dall'esterno di un soundfont ed è identificato da un numero di banco e da un numero di preset.
Un soundfont può avere uno o più preset.
Ogni preset contiene uno o più strumenti, così come gli strumenti contengono uno o più campioni.


### Creare un preset e aggiungere strumenti


Per prima cosa, fai clic su :guilabel:`Aggiungi un preset` nella [barra degli strumenti](manual/soundfont-editor/toolbar.md#doc_edit).
Ora bisogna inserire un nome (se nell'albero sono selezionati degli strumenti, possono essere collegati direttamente al nuovo preset).

Altrimenti, aggiungi gli strumenti al preset con un «[trascinamento](manual/soundfont-editor/tree.md#doc_dragdrop)».
Nell'albero noterai che nel preset compaiono una o più divisioni.
Spesso ce n'è una sola, come nel caso di un preset che contiene un unico strumento.

È possibile creare tanti preset quanti sono gli strumenti (o anche di più), e ogni preset può contenere uno o più strumenti.


### Impostazione di un preset


Potrebbe non essere necessario modificare i parametri del preset, perché:

* quando si crea un preset, il numero di banco e il numero di preset vengono assegnati automaticamente,
* quando si aggiunge uno strumento a un preset, l'intervallo di note viene calcolato automaticamente in base allo strumento.

Nell'ambito di questo tutorial, la creazione di un soundfont è terminata!
