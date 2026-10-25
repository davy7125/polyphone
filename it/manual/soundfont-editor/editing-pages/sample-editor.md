Facendo clic su uno o più campioni nell'[albero](manual/soundfont-editor/tree.md), compare la pagina di modifica dei campioni.

Questa pagina è divisa in diverse parti:

* un [grafico](#doc_graph) che rappresenta il campione,
* un'area di [informazioni](#doc_info),
* un'analisi delle [frequenze](#doc_freq),
* un [equalizzatore](#doc_eq),
* un [lettore](#doc_player) del campione.


![Pagina dei campioni](images/edit_sample.png "Pagina dei campioni")


## Il grafico {#doc_graph}


### Zoom e trascinamento


Il grafico permette di visualizzare l'onda sonora.
È possibile ingrandire in verticale o in orizzontale tenendo premuto il tasto destro del mouse e trascinando:

* trascinando verso destra si espande l'asse orizzontale,
* trascinando verso sinistra si contrae l'asse orizzontale,
* trascinando verso l'alto si espande l'asse verticale,
* trascinando verso il basso si contrae l'asse verticale.

Tenendo premuto il tasto sinistro del mouse si può spostare il grafico in orizzontale, se l'asse orizzontale è stato espanso.
Anche la rotellina del mouse fa scorrere il grafico in orizzontale.


### Posizioni del loop e della riproduzione


Una barra verticale verde indica la posizione di inizio del loop (modificabile con un clic sinistro sul grafico), una barra rossa indica la posizione di fine del loop (modificabile con un clic destro sul grafico).
A ogni barra verticale è associata una linea tratteggiata che rappresenta una porzione d'onda, utile per visualizzare la sovrapposizione dell'inizio e della fine del loop.

Durante la riproduzione, una barra verticale bianca scorre sul grafico e indica in tempo reale la posizione di riproduzione.


### Tagliare il campione


Tenendo premuto il tasto :kbd:`Alt` è possibile definire un'area da tagliare:

* la pressione del tasto sinistro del mouse definisce l'inizio dell'area,
* il rilascio corrispondente del tasto sinistro definisce la fine dell'area.

Una volta definita un'area, compare una finestra di dialogo che chiede conferma prima di tagliare il campione.
La risposta :guilabel:`Sì e crea un altro campione` taglia il campione e crea anche un nuovo campione a partire dalla parte rimossa.


![Taglio del campione](images/cutting_sample.png "Taglio del campione")


### Colori del grafico


I colori del grafico sono modificabili nelle [impostazioni](manual/settings.md#doc_interface) del software.


## Sezione «Informazioni» {#doc_info}


La sezione :guilabel:`Informazioni` permette di visualizzare, e nella maggior parte dei casi modificare, le seguenti informazioni:

Dimensione
: Vengono mostrati il numero di valori e la durata corrispondente in secondi.

Loop
: Qui si possono modificare i punti di inizio e di fine del loop.
  Un pulsante imposta le posizioni di inizio e di fine in modo che il loop copra l'intero campione.

Accordatura
: Permette di regolare la nota di base di un campione e la sua correzione in centesimi, in modo che il suono sia accordato secondo il temperamento equabile.
  Un pulsante imposta la nota di base e la correzione sui valori corrispondenti alla [frequenza](#doc_freq) stimata.

Frequenza di campionamento
: Qualsiasi modifica provoca un ricampionamento, in modo che l'altezza del suono non cambi.

Canale
: Scelta del tipo tra :guilabel:`mono`, :guilabel:`sinistra`, :guilabel:`destra` o :guilabel:`collegamento`.
  Se il tipo è :guilabel:`sinistra`, :guilabel:`destra` o :guilabel:`collegamento`, è possibile annullare lo stereo selezionando :guilabel:`mono`.
  Quando è selezionato :guilabel:`mono`, si torna allo stato stereo selezionando un campione collegato.

Collegamento
: Scelta del campione da collegare per rendere il suono stereo.
  Se si seleziona un campione, il suo tipo passa a :guilabel:`collegamento` (questo tipo può poi essere cambiato in :guilabel:`sinistra` o :guilabel:`destra`).

Se la [compressione](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md#doc_edit) è attiva per il soundfont ([formato sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3)), per i campioni non ancora compressi viene mostrato un pulsante di compressione.
Durante la compressione, una finestra di dialogo chiede di scegliere la qualità della compressione (:guilabel:`Alta`, :guilabel:`Media` o :guilabel:`Bassa`).
Il loop di un campione può essere alterato dalla compressione.\
**Nota:** la decompressione avviene automaticamente quando si modifica il campione (applicazione di un filtro, taglio, amplificazione, ecc.).

![Scelta della qualità di compressione](images/sample_compress_dialog.png "Scelta della qualità di compressione")

Qualsiasi modifica a un campione stereo può essere applicata al campione collegato se l'opzione :guilabel:`Modifica stereo` è spuntata nelle [impostazioni](manual/settings.md#doc_general) (o con il pulsante situato in basso a destra nella finestra).
Con una selezione multipla nell'[albero](manual/soundfont-editor/tree.md) si possono modificare più campioni contemporaneamente.


## Analisi delle frequenze {#doc_freq}


L'analisi delle frequenze comprende una trasformata di Fourier (grafico che mostra l'intensità delle frequenze contenute nel segnale).

![Grafico di Fourier](images/fourier_graph.png "Grafico di Fourier")

A destra è mostrato l'elenco dei picchi dominanti con le seguenti informazioni:

* rapporto tra l'intensità del picco e l'intensità del picco massimo,
* frequenza,
* nota corrispondente più vicina,
* correzione (distanza dalla nota corrispondente più vicina).

A partire da questi dati viene stimata l'altezza del suono.
Questa stima può essere usata per l'accordatura.

Un clic destro sul grafico apre un menu contestuale che permette di:

* esportare il grafico in un'immagine .png,
* mostrare l'elenco di tutti i picchi e un pulsante per esportare questo elenco in un file .csv.

![Mostra i picchi di frequenza](images/tool_show_peak_frequencies.png "Mostra i picchi di frequenza")


## Equalizzatore {#doc_eq}


L'equalizzatore permette di aumentare o diminuire alcune frequenze, da -15 a 15 dB.
È possibile ascoltare l'effetto con l'opzione :guilabel:`anteprima` attiva e poi applicarlo definitivamente premendo il pulsante :guilabel:`Applica`.
Premendo :guilabel:`Reimposta` tutti i cursori tornano a 0 dB (ma un effetto già applicato al suono non viene rimosso: per questo usa il pulsante :guilabel:`Annulla` della [barra degli strumenti](manual/soundfont-editor/toolbar.md#doc_edit)).

Qualsiasi modifica a un campione stereo può essere applicata automaticamente al campione collegato se l'opzione è spuntata nelle [impostazioni](manual/settings.md#doc_general).


## Lettore {#doc_player}


Il lettore riproduce il suono.
Durante la riproduzione, un cursore bianco scorre sul grafico per indicare la posizione di riproduzione.
Le opzioni di riproduzione sono:

Loop
: Riproduce il suono in loop se l'inizio e la fine del loop sono impostati.

Stereo
: Riproduce il campione in stereo, insieme al campione collegato.
  Se è richiesto anche il loop, vengono usati i due loop rispettivi (che possono essere diversi).

Sinusoide
: Al suono viene aggiunto un segnale sinusoidale di calibrazione, che permette di accordare un campione al centesimo di semitono.
  Per prima cosa bisogna inserire la nota di base.
  A questo punto si possono sentire dei battimenti se la frequenza della sinusoide differisce leggermente dalla frequenza fondamentale del suono.
  Resta da regolare finemente la correzione per rendere i battimenti il più lenti possibile, in modo che il campione sia accordato al meglio.
  Per maggiori informazioni vedi il tutorial «[Come preparare un campione](tutorials/how-to-prepare-a-sample.md)».

Un cursore regola il volume della riproduzione (questo volume non viene memorizzato nel soundfont).

Se una riproduzione è in corso quando si passa da un campione all'altro tramite l'[albero](manual/soundfont-editor/tree.md), riparte automaticamente.
Inoltre, il tasto :kbd:`Space` avvia o interrompe la riproduzione se il focus è sull'albero.
