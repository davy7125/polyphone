Quando nell'[albero](manual/soundfont-editor/tree.md) sono selezionati uno o più campioni, il [menu delle funzioni](manual/soundfont-editor/tools/index.md) contiene diverse funzioni suddivise in 5 categorie:

* [elaborazione audio](#doc_cat_processing),
* [modifica veloce](#doc_cat_fast_editing),
* [file](#doc_cat_files),
* [loop / riproduzione](#doc_cat_loop_playback),
* [campioni stereo](#doc_cat_stereo).

È possibile usare le funzioni su più campioni contemporaneamente (con una selezione multipla).


## Elaborazione audio {#doc_cat_processing}


### Modifica volume {#doc_volume}

Puoi modificare il volume del campione in tre modi diversi:

* sommando una certa quantità di dB (positiva o negativa),
* moltiplicando il segnale per un numero,
* normalizzando il segnale in modo che il suo massimo rappresenti una certa percentuale dell'intensità massima.

![Funzione Modifica volume](images/tool_change_volume.png "Funzione Modifica volume")


### Comando esterno {#doc_externalcommand}

Questa funzione permette di usare un comando o un programma esterno per modificare un campione.
Non è più necessario esportare e reimportare un campione in Polyphone: tutto avviene automaticamente.

1. scarica un software di modifica per file .wav, come :program:`Audacity` o :program:`Sweep` (Linux),
2. usa la funzione :guilabel:`Comando esterno` su uno o più campioni,
3. compila il campo :guilabel:`Comando` con la riga di comando che richiama il software esterno (potrebbe essere necessario leggere la documentazione di quel software); l'argomento :command:`{wav}` è obbligatorio perché viene sostituito dal percorso del file esportato (ad esempio :command:`sweep {wav}`),
4. modifica i campioni con il programma esterno,
5. salva la versione modificata dei file wav con il programma esterno e chiudilo,
6. i campioni verranno caricati automaticamente nel soundfont.

Il pulsante a destra del campo aiuta a selezionare un file eseguibile, e l'elenco :guilabel:`Cronologia` richiama gli ultimi comandi utilizzati.
Sono disponibili due opzioni:

Consenti la modifica stereo
: Quando si elabora un campione stereo, questo viene esportato insieme al campione collegato in un unico file stereo, ed entrambi i campioni vengono aggiornati alla reimportazione del file.

Sostituisci le informazioni del campione se possibile
: Il loop, la nota di base e la correzione memorizzati nel file restituito dal software esterno sostituiscono quelli del campione.
  Altrimenti vengono mantenute le informazioni esistenti.

![Funzione Comando esterno](images/tool_external_command.png "Funzione Comando esterno")

**Nota:** durante il processo, Polyphone esporta un file .wav e reimporta lo stesso file dopo l'esecuzione del comando esterno. Se il comando esterno crea altri file o salva il risultato in un altro file, questi file verranno ignorati e Polyphone reimporterà il file esportato inizialmente, che probabilmente non sarà stato modificato.


### Filtra frequenze {#doc_filter}

Quando usi questa funzione, compare una finestra che mostra le frequenze contenute nei campioni selezionati.
Puoi quindi usare il mouse per disegnare un'area rossa.
Tutto ciò che si trova in quest'area verrà rimosso: in questo modo puoi ridurre o eliminare manualmente alcune frequenze.

![Funzione Filtra frequenze](images/tool_filter_frequencies.png "Funzione Filtra frequenze")

**Nota:** questo filtro non è comune perché taglia le frequenze in modo netto.
Può essere utile per realizzare un filtro «brick-wall», ma nella maggior parte dei casi probabilmente vorrai usare l'[equalizzatore](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq).


### Trasponi {#doc_transpose}

La funzione chiede uno spostamento in semitoni e poi ricampiona il suono per cambiarne l'altezza.

![Funzione Trasponi](images/tool_transpose_smpl.png "Funzione Trasponi")


## Modifica veloce {#doc_cat_fast_editing}


### Carica un parametro dagli strumenti {#doc_load_from_instruments}

I parametri a livello di strumento possono sovrascrivere o spostare i parametri definiti a livello di campione.
Con questa funzione è possibile correggere o completare i parametri dei campioni con quanto definito negli strumenti che li usano:

* modificare la nota di base del campione con la sovrascrittura definita a livello di strumento,
* spostare la nota di base del campione con il parametro «Accordatura (semitoni)»,
* spostare la correzione dell'intonazione del campione con il parametro «Accordatura (centesimi)»,
* definire l'inizio e la fine del loop del campione con i parametri di offset del loop.

![Funzione per caricare un parametro dagli strumenti](images/tool_load_from_instruments.png "Funzione per caricare un parametro dagli strumenti")


### Sposta un parametro {#doc_shift_parameter}

Questa funzione permette di modificare contemporaneamente, sommando un valore o moltiplicando per un valore, uno dei seguenti parametri per tutti i campioni selezionati:

* nota di base,
* correzione,
* inizio loop,
* fine loop.

![Funzione di modifica veloce](images/tool_shift_parameter.png "Funzione di modifica veloce")


## File {#doc_cat_files}


### Esportazione WAV {#doc_wavexport}

Dopo aver selezionato una cartella di destinazione, tutti i campioni selezionati verranno esportati come file .wav.


## Loop / riproduzione {#doc_cat_loop_playback}


### Auto loop {#doc_autoloop}

Il software cerca nel campione due posizioni distanti e adatte a creare un loop.
Se necessario, un breve frammento sonoro viene copiato con una dissolvenza incrociata, in modo che la transizione nel punto di giunzione sia il più fluida possibile.
Con un campione stereo, viene creato lo stesso loop per entrambi i lati.

In caso di insuccesso, compare un messaggio di avviso che indica che non è stato trovato alcun loop (il campione è probabilmente troppo corto o troppo turbolento).

**Attenzione:** questa funzione modifica il campione originale.
Le modifiche possono essere annullate con la funzione «[annulla](manual/soundfont-editor/toolbar.md#doc_edit)», ma se il file viene salvato e Polyphone viene chiuso non è più possibile tornare indietro.

**Nota:** per impostazione predefinita, il loop è disattivato in uno strumento.
Per sentirlo, devi selezionare ![loop acceso](images/loop_on.png "loop acceso"){.inline} nella riga del parametro :guilabel:`Riproduzione in loop`, nella colonna :guilabel:`Globale` o nelle colonne delle singole note della [tabella](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).
![loop acceso](images/loop_on.png "loop acceso"){.inline} lo attiva, ![loop spento](images/loop_off.png "loop spento"){.inline} lo disattiva.
![loop acceso e fine](images/loop_on_end.png "loop acceso e fine"){.inline} attiva il loop e il campione verrà riprodotto fino alla fine dopo il rilascio del tasto.


### Loop con dissolvenza incrociata {#doc_crossfadeloop}

La funzione di dissolvenza incrociata può essere vista come la versione manuale della [funzione Auto loop](#doc_autoloop). Per prima cosa devi posizionare l'inizio e la fine del loop nel [grafico](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph) attorno a una zona stabile. Poi questa funzione crea un loop a partire da queste posizioni applicando una dissolvenza incrociata la cui durata (in millisecondi) è configurabile. Tutto è quindi configurabile, ma bisogna fare attenzione nella ricerca dell'inizio e della fine del loop:

* in entrambe le posizioni, le armoniche del suono devono corrispondere, per non sentire un effetto di disco graffiato durante la transizione,
* anche le fasi delle diverse armoniche devono corrispondere, per non sentire una cancellazione di frequenze durante la dissolvenza incrociata, che altera fortemente il timbro di uno strumento,
* per quanto possibile, la fine del loop deve essere lontana dall'inizio, per evitare un suono troppo sintetico.

Un consiglio è concentrarsi sulle posizioni in cui l'onda attraversa l'asse x (valore 0).

![Funzione Loop con dissolvenza incrociata](images/tool_crossfade_loop.png "Funzione Loop con dissolvenza incrociata")


### Rimuovi silenzio all'inizio {#doc_removeblank}

Qualsiasi zona vuota prima dell'inizio del campione viene eliminata, riducendo il tempo prima che il suono si senta.
Con un campione stereo, viene rimossa la stessa lunghezza da entrambi i lati.

![Rimuovi silenzio all'inizio](images/remove_blank.png "Rimuovi silenzio all'inizio")

**Nota:** questa funzione può essere applicata automaticamente a ogni importazione di un campione; l'opzione si trova nelle [impostazioni](manual/settings.md#doc_general) del software.


### Taglia alla fine del loop {#doc_trimloop}

Se per il campione selezionato è definito un loop, il software taglierà il suono dopo la fine del loop (lasciando un piccolo margine alla fine).

![Taglia alla fine del loop](images/trim_to_loop.png "Taglia alla fine del loop")

**Nota:** in base alle [impostazioni](manual/settings.md#doc_general) del software, questa funzione può essere applicata automaticamente a ogni importazione di un campione.


## Campioni stereo {#doc_cat_stereo}


### Regolazione del bilanciamento {#doc_balance}

Se il campione è collegato a un altro (stereo), il software può uguagliare l'intensità media dei due campioni per regolare il bilanciamento.
In questo modo sarà più facile impostare con precisione il pan di un campione in uno [strumento](manual/soundfont-editor/editing-pages/instrument-editor.md).
Questa funzione non può essere usata con campioni mono.


### Trova un collegamento {#doc_link}

Tra i campioni selezionati, quelli i cui nomi differiscono solo per l'ultima lettera (L&nbsp;/&nbsp;R) vengono collegati automaticamente (gli spazi e i caratteri ``-``, ``_``, ``(``, ``)``, ``[``, ``]``, ``{``, ``}`` e ``|`` vengono ignorati nei nomi).
Ad esempio, se in un soundfont due campioni si chiamano ``bassoon45L`` e ``bassoon45R``, ``bassoon45L`` sarà riconosciuto come la parte sinistra di ``bassoon45R`` e ``bassoon45R`` come la parte destra di ``bassoon45L``.

Grazie a questa informazione, quando questi campioni vengono aggiunti a uno strumento verrà usato il pan corretto.
Potrai anche modificare contemporaneamente la parte sinistra e quella destra nella [tabella](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) dello strumento, se la modifica stereo è attiva nelle [impostazioni](manual/settings.md#doc_general).

**Attenzione:** questa funzione può interrompere collegamenti esistenti se i nomi non rispettano la regola indicata sopra.
Un messaggio elenca i campioni per i quali non è stata trovata alcuna corrispondenza.


### Scollega {#doc_unlink}

Tutti i campioni selezionati che sono collegati (stereo) vengono scollegati.

In questo modo tutti i campioni saranno trattati come suoni mono e avranno tutti lo stesso pan se aggiunti a uno strumento.
La modifica stereo, se attiva nelle [impostazioni](manual/settings.md#doc_general), non sarà più applicabile.
