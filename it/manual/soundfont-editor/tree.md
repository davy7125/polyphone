L'albero è il primo elemento dell'editor a cui dovresti prestare attenzione.
Rappresenta la struttura del soundfont aperto e controlla la parte destra del software quando selezioni uno o più elementi.

L'albero viene sempre mostrato con la stessa [struttura](#doc_structure), offre un [menu contestuale](#doc_menu) e dispone delle funzionalità di [trascinamento](#doc_dragdrop), [copia / incolla](#doc_copypaste) e [ricerca](#doc_search).


## Struttura {#doc_structure}


La struttura dell'albero è composta da 4 intestazioni, con sottoelementi per le intestazioni :guilabel:`Campioni`, :guilabel:`Strumenti` e :guilabel:`Preset`.


![Albero: struttura](images/tree_1.png "Albero: struttura")


### Intestazione «Generale»


Un clic sull'intestazione :guilabel:`Generale` apre l'editor delle [informazioni generali](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md).


### Sezione «Campioni»


Questa sezione elenca tutti i campioni contenuti nel soundfont, che costituiscono la materia prima su cui sono costruiti tutti gli strumenti.
Selezionando uno o più campioni si apre l'[editor dei campioni](manual/soundfont-editor/editing-pages/sample-editor.md).
Un clic sull'intestazione :guilabel:`Campioni` mostra il [riepilogo della configurazione](manual/soundfont-editor/configuration-summaries.md#doc_sample) di tutti i campioni.


### Sezione «Strumenti»


Questa sezione elenca tutti gli strumenti contenuti nel soundfont; uno strumento è un elemento intermedio composto da campioni e utilizzato dai preset.
Gli strumenti contengono collegamenti a campioni chiamati «divisioni», mostrati come elenco sotto ciascuno strumento.
Selezionando uno strumento o una delle sue divisioni si apre l'[editor degli strumenti](manual/soundfont-editor/editing-pages/instrument-editor.md).
Un clic sull'intestazione :guilabel:`Strumenti` mostra il [riepilogo della configurazione](manual/soundfont-editor/configuration-summaries.md#doc_instrument) di tutti gli strumenti.


### Sezione «Preset»


Questa sezione elenca tutti i preset contenuti nel soundfont; un preset è un elemento finale, composto da strumenti, accessibile dall'esterno del soundfont.
I preset contengono collegamenti a strumenti chiamati «divisioni», mostrati come elenco sotto ciascun preset.
Selezionando un preset o una delle sue divisioni si apre l'[editor dei preset](manual/soundfont-editor/editing-pages/preset-editor.md).
Un clic sull'intestazione :guilabel:`Preset` mostra il [riepilogo della configurazione](manual/soundfont-editor/configuration-summaries.md#doc_preset) di tutti i preset.


## Menu contestuale {#doc_menu}


Un clic destro su un elemento mostra un menu con le seguenti azioni:

Collega a…
: Collega i campioni selezionati a uno strumento, oppure gli strumenti selezionati a un preset.
  La destinazione si sceglie in un elenco che può essere filtrato.
  Quando si collegano a uno strumento vuoto campioni con note di base diverse, i loro intervalli di note vengono definiti automaticamente (come con la funzione [Auto-posizionamento campione](manual/soundfont-editor/tools/instrument-tools.md#doc_position)).

Sostituisci con…
: Sostituisce un campione con un altro campione in una divisione di strumento, oppure uno strumento con un altro strumento in una divisione di preset.
  La configurazione della divisione viene mantenuta.

Copia
: Copia gli elementi selezionati, accessibile anche con :kbd:`Ctrl`+:kbd:`C`.

Incolla
: Incolla gli elementi copiati in precedenza, accessibile anche con :kbd:`Ctrl`+:kbd:`V`.

Duplica
: Duplica gli elementi selezionati, che possono essere campioni, strumenti, preset o divisioni.
  Accessibile anche con :kbd:`Ctrl`+:kbd:`D`.

Elimina
: Elimina gli elementi selezionati, accessibile anche con :kbd:`Del`.
  Non è possibile eliminare un campione utilizzato da uno strumento né uno strumento utilizzato da un preset.

Silenzia
: A scopo di test, una divisione all'interno di uno strumento o di un preset può essere silenziata. Questa opzione si trova anche nel menu contestuale dell'[intestazione della tabella](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_solo).

Riproduci sempre
: Al contrario della funzione «Silenzia», si possono aggiungere uno o più strumenti o preset durante l'esecuzione. In questo modo è possibile trovare un buon equilibrio tra diversi strumenti o preset suonati insieme.

Ripristina riproduzione
: Annulla gli effetti di «Silenzia» e «Riproduci sempre» in tutto il soundfont.

Rinomina o Rinomina in blocco
: Rinomina gli elementi selezionati, accessibile anche con :kbd:`F2`.
  Rinominare una divisione rinomina il campione o lo strumento a cui è collegata.
  Quando sono selezionati più elementi, una finestra di dialogo propone diverse opzioni:
  
  * sovrascrivere il nome esistente usando il nome della nota come suffisso (solo per campioni e divisioni di strumento),
  * sovrascrivere il nome esistente usando il nome della nota e la velocità come suffisso (solo per divisioni di strumento),
  * sovrascrivere il nome esistente usando un suffisso numerico crescente,
  * sostituire caratteri,
  * inserire un testo in una posizione specifica,
  * eliminare un intervallo di caratteri.


![Albero: menu contestuale](images/tree_2.png "Albero: menu contestuale")


## Trascinamento {#doc_dragdrop}


Il «trascinamento» (drag & drop) nell'albero permette di creare rapidamente associazioni o copie.
Prima di un trascinamento occorre selezionare degli elementi.
Può trattarsi di un singolo elemento o di una selezione multipla, resa possibile dai tasti :kbd:`Ctrl` e :kbd:`Shift`.

Il risultato di un trascinamento dipende poi dal tipo di elementi trascinati e dal tipo di elemento di destinazione.

Campione → strumento
: Associazione del campione allo strumento, creazione di una divisione di strumento.

Campione → intestazione :guilabel:`Strumenti`
: Creazione di uno strumento che contiene tutti i campioni trascinati, oppure creazione di uno strumento per ogni campione trascinato.

Strumento → preset
: Associazione dello strumento al preset, creazione di una divisione di preset.

Strumento → strumento
: Copia di tutte le divisioni dello strumento di origine nello strumento di destinazione (può trattarsi dello stesso strumento).

Divisione di strumento → strumento o divisione di strumento
: Copia delle divisioni selezionate dello strumento di origine nello strumento di destinazione (può trattarsi dello stesso strumento).

Strumento → intestazione :guilabel:`Preset`
: Creazione di un preset che contiene tutti gli strumenti trascinati, oppure creazione di un preset per ogni strumento trascinato.

Preset → preset
: Copia di tutte le divisioni del preset di origine nel preset di destinazione (può trattarsi dello stesso preset).

Divisione di preset → preset o divisione di preset
: Copia delle divisioni selezionate del preset di origine nel preset di destinazione (può trattarsi dello stesso preset).

File audio (dal gestore di file del computer) → albero
: Importazione del file come nuovo campione, se il suo formato è supportato (vedi l'[editor dei campioni](manual/soundfont-editor/editing-pages/sample-editor.md)).


## Copia / incolla {#doc_copypaste}


Tutto ciò che è possibile con il trascinamento, come visto sopra, è possibile anche con copia / incolla, accessibile dal menu contestuale o con le scorciatoie :kbd:`Ctrl`+:kbd:`C` e :kbd:`Ctrl`+:kbd:`V`.

Copia / incolla è consentito anche tra soundfont diversi.
È possibile copiare un elemento in un primo soundfont e incollarlo in un secondo soundfont.
È consentita la selezione multipla.

Campioni selezionati
: Copia dei campioni.

Strumenti selezionati
: Copia degli strumenti, compresi tutti i campioni collegati.

Preset selezionati
: Copia dei preset, compresi tutti gli strumenti e i campioni collegati.

Durante la copia di elementi tra soundfont diversi, se esiste già un elemento con lo stesso nome, vengono proposte diverse opzioni:

* ignorare l'operazione: in questo caso la copia viene annullata per l'elemento con lo stesso nome;
* sostituire l'elemento esistente: in questo caso l'elemento esistente viene sovrascritto dall'elemento copiato;
* duplicare l'elemento: in questo caso l'elemento viene copiato accanto a quello esistente con un nome diverso.


## Ricerca {#doc_search}


Per facilitare la ricerca degli elementi nell'albero, è disponibile una barra di ricerca per filtrarli (la scorciatoia :kbd:`Ctrl`+:kbd:`F` porta il focus su di essa).


![Albero: barra di ricerca](images/tree_3.png "Albero: barra di ricerca")


Quando si scrive un testo nella barra di ricerca, vengono mostrati gli elementi che lo contengono.
Inoltre:

* se il testo è contenuto nel nome di un campione, vengono mostrati gli strumenti che usano il campione e i preset che usano questi strumenti;
* se il testo è contenuto nel nome di uno strumento, vengono mostrati i campioni usati dallo strumento e i preset che usano lo strumento;
* se il testo è contenuto nel nome di un preset, vengono mostrati gli strumenti usati dal preset e i campioni usati da questi strumenti.

Un clic sulla croce a destra del campo modificabile annulla il filtro.
