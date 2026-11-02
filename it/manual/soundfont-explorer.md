Con Polyphone è possibile aprire una cartella dalla [schermata iniziale](manual/index.md#doc_left) facendo clic sul pulsante :guilabel:`Apri directory`. L'esploratore di soundfont si apre quindi in una nuova scheda e mostra i file di quella cartella (le sottocartelle non vengono analizzate), presentando per ogni soundfont un'anteprima del suo contenuto.


![Esploratore di soundfont](images/soundfont_explorer.png "Esploratore di soundfont")


## Anteprima dei soundfont {#doc_preview}


L'elenco principale, situato nella parte sinistra dell'interfaccia, contiene l'elenco dei file, che Polyphone distingue come segue:

* i file compatibili che possono essere analizzati rapidamente (ad esempio nei formati [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz) e [.dls](manual/annexes/the-different-soundfont-formats.md#doc_dls)) vengono mostrati con i dettagli del loro contenuto,
* i file compatibili che non possono essere analizzati rapidamente (in particolare nel formato [.sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark)) vengono mostrati solo con informazioni di base,
* i file non compatibili sono in grigio e non possono essere aperti.

Un clic su un file analizzabile mostra, negli elenchi nella parte destra dell'interfaccia, i campioni, gli strumenti e i preset che contiene. Ogni voce di questi elenchi include anche informazioni utili per valutare il livello di dettaglio del soundfont e la quantità di lavoro che vi è stata dedicata.

### Dettagli a livello di file

* per ogni file vengono mostrate la dimensione e la data dell'ultima modifica,
* per i soundfont che possono essere analizzati si aggiungono le seguenti informazioni:
  * numero di campioni, strumenti e preset,
  * nomi dei primi preset.

Questi dettagli sono allineati in colonne sotto un'intestazione. Il pulsante :guilabel:`Visualizzazione`, a destra del campo di testo, apre un menu che permette di scegliere le colonne mostrate (tutte possono essere nascoste tranne il nome) e di mostrare o meno il :guilabel:`Riepilogo dei preset`, cioè i nomi dei primi preset su una seconda riga. Senza questo riepilogo, ogni file occupa una sola riga, come in una tabella.

### Informazioni per ogni campione

* tipo (mono / stereo),
* durata totale,
* durata del loop, se presente,
* frequenza di campionamento.

### Informazioni per ogni strumento o preset

* numero totale di divisioni, parametri e generatori,
* numero di intervalli di note distinti,
* numero di intervalli di velocità distinti.


## Funzioni di ordinamento e ricerca {#doc_filter}


La parte superiore presenta un campo di testo (la scorciatoia :kbd:`Ctrl`+:kbd:`F` porta il focus su di esso). Quando è compilato, l'esploratore mostra solo i file che soddisfano almeno uno dei seguenti criteri:

* il nome contiene il testo cercato,
* il nome di uno dei campioni, strumenti o preset contiene il testo cercato.

Anche gli elenchi di campioni, strumenti e preset vengono filtrati per evidenziare gli elementi corrispondenti.

Un clic sull'intestazione di una colonna ordina i file per nome, dimensione, data o numero di campioni, strumenti o preset, e un nuovo clic inverte l'ordine. Il primo clic ordina i nomi dalla A alla Z e le altre colonne partendo dal valore più grande o dal file più recente. Quando si ordina per numero di elementi, i file il cui contenuto è sconosciuto sono sempre posti in fondo all'elenco.

Le opzioni di visualizzazione, l'ordinamento, così come la larghezza e lo stato espanso o compresso degli elenchi a destra vengono conservati per la volta successiva.


## Operazioni sui file {#doc_file_operations}


Facendo clic su un file nell'elenco principale, prima del nome del file compaiono 2 icone cliccabili:

* un'icona a forma di matita per rinominare il file (accessibile anche con :kbd:`F2`),
* un'icona a forma di cestino per spostare il file nel cestino, dopo una conferma (accessibile anche con :kbd:`Del`).

Queste operazioni permettono di gestire e riordinare rapidamente una raccolta locale di soundfont.


## Suonare gli elementi {#doc_play}


Il pulsante ![piano](images/toolbar_piano.png "piano"){.inline}, a destra del pulsante :guilabel:`Visualizzazione`, mostra o nasconde la [tastiera virtuale](manual/soundfont-editor/toolbar.md#doc_keyboard), e il pulsante ![registratore](images/toolbar_recorder.png "registratore"){.inline} accanto mostra o nasconde il [registratore](manual/soundfont-editor/toolbar.md#doc_recorder), come i pulsanti della barra degli strumenti dell'editor.

Il campione, lo strumento o il preset selezionato negli elenchi di destra può essere suonato con la tastiera virtuale, oppure con una tastiera MIDI esterna anche se la tastiera virtuale è nascosta. Come nell'editor, la tastiera virtuale mostra l'estensione dell'elemento, o la nota di base di un campione. Un campione viene riprodotto in loop se ne ha uno, e in stereo se l'opzione :guilabel:`stereo` del [lettore](manual/soundfont-editor/editing-pages/sample-editor.md#doc_player) del campione è selezionata.

La prima volta che viene selezionato un elemento di un file, il soundfont viene caricato in background senza aprire una scheda. Il soundfont viene poi conservato finché vengono selezionati elementi dello stesso file, e rilasciato quando nell'elenco principale viene selezionato un altro file.

Un soundfont già aperto in una scheda viene suonato così com'è nell'editor, con le sue modifiche non salvate.


## Aprire un soundfont {#doc_open_soundfont}


Un doppio clic su una voce dell'elenco principale apre il file, purché il suo formato sia compatibile con Polyphone, nella posizione configurata nelle [impostazioni](manual/settings.md#doc_interface) con l'opzione :guilabel:`Vista all'avvio` (per impostazione predefinita vengono mostrate le [informazioni generali](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md)).

Un doppio clic su un [campione](manual/soundfont-editor/editing-pages/sample-editor.md), uno [strumento](manual/soundfont-editor/editing-pages/instrument-editor.md) o un [preset](manual/soundfont-editor/editing-pages/preset-editor.md) apre il file in corrispondenza dell'elemento selezionato.

Se il soundfont è già stato caricato per suonare uno dei suoi elementi, la scheda si apre immediatamente.
