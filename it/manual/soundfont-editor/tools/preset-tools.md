Quando nell'[albero](manual/soundfont-editor/tree.md) sono selezionati un preset o una delle sue divisioni, il [menu delle funzioni](manual/soundfont-editor/tools/index.md) contiene diverse funzioni suddivise in 4 categorie:

* [analisi](#doc_cat_analyze),
* [modifica veloce](#doc_cat_fast),
* [modulatori](#doc_cat_mod),
* [trasformazione](#doc_cat_transformation).

Fatta eccezione per la visualizzazione dei parametri, è possibile usare le funzioni su più preset contemporaneamente (con una selezione multipla).
Tutte le funzioni sono disponibili anche per gli [strumenti](manual/soundfont-editor/tools/instrument-tools.md).


## Analisi {#doc_cat_analyze}


### Visualizza parametri {#doc_display}

Stessa funzione di «[Visualizza parametri](manual/soundfont-editor/tools/instrument-tools.md#doc_display)» per gli strumenti.


## Modifica veloce {#doc_cat_fast}


### Cambia attenuazioni {#doc_attenuation}

Stessa funzione di «[Cambia attenuazioni](manual/soundfont-editor/tools/instrument-tools.md#doc_attenuation)» per gli strumenti.


### Configurazione basata su tasti {#doc_global}

Stessa funzione di «[Configurazione basata su tasti](manual/soundfont-editor/tools/instrument-tools.md#doc_global)» per gli strumenti.


### Spazializzazione del suono {#doc_spatialization}

Stessa funzione di «[Spazializzazione del suono](manual/soundfont-editor/tools/instrument-tools.md#doc_spatialization)» per gli strumenti.


## Modulatori {#doc_cat_mod}


### Rimuovi modulatori {#doc_remove_mod}

Stessa funzione di «[Rimuovi modulatori](manual/soundfont-editor/tools/instrument-tools.md#doc_remove_mod)» per gli strumenti.


## Trasformazione {#doc_cat_transformation}


### Duplicazione divisione {#doc_division}

Stessa funzione di «[Duplicazione divisione](manual/soundfont-editor/tools/instrument-tools.md#doc_division)» per gli strumenti.


### Unisci campioni per ogni nota {#doc_mergesamples}

Se un preset richiama contemporaneamente più strumenti e campioni quando si preme un tasto, questa funzione unisce, per ogni nota, tutti i campioni suonati. Il risultato è un nuovo preset che suona un solo campione per tasto, riducendo così il carico della CPU necessario per eseguire il preset.
La finestra chiede il nome del nuovo preset, la durata del sustain e la durata del release dei suoni registrati, l'intervallo tra i campioni creati (per ogni nota, ogni 3 note oppure ogni 6 note), l'auto loop e il tipo di suono (mono o stereo).

Con le opzioni definite nell'immagine qui sotto, la funzione simulerà ogni 3 note una pressione di tasto di 5 secondi. La registrazione audio sarà in stereo e il release non verrà registrato. I campioni così creati verranno poi messi in loop con la [funzione Auto loop](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop), e lo strumento che contiene tutti questi campioni, così come il nuovo preset che lo usa, si chiameranno «Cymbale III».

**Nota:** l'opzione auto loop rimuove la parte di release e riduce leggermente la durata sostenuta di ciascun campione.

![Unisci campioni per ogni nota](images/tool_merge_samples.png "Unisci campioni per ogni nota")
