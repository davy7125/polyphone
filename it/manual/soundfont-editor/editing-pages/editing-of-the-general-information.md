La pagina per la modifica delle informazioni generali viene mostrata quando nell'[albero](manual/soundfont-editor/tree.md) è selezionata l'intestazione :guilabel:`Generale`.
Questa pagina è composta da tre tipi di elementi:

* [campi modificabili](#doc_edit),
* [informazioni generali](#doc_info) non modificabili,
* [statistiche](#doc_stats) sul soundfont.

Il pulsante :guilabel:`Modulatori predefiniti` della [barra degli strumenti](manual/soundfont-editor/toolbar.md#doc_view) mostra i [modulatori predefiniti](#doc_default_mod) del soundfont, e il pulsante :guilabel:`Generale` mostra di nuovo questa pagina.


![Pagina delle informazioni generali](images/edit_general.png "Pagina delle informazioni generali")


## Campi modificabili {#doc_edit}


La pagina di modifica delle informazioni generali offre campi per modificare i seguenti attributi:

Titolo
: È il nome interno del soundfont, che non coincide necessariamente con il nome del file.

Autore
: Il nome dell'autore del soundfont.

Data
: La data di creazione del file (un calendario aiuta a sceglierla).

Prodotto
: Il prodotto per il quale il soundfont è stato concepito.

Copyright
: Indicazione da compilare se il file è protetto da diritto d'autore.

Commenti
: Informazioni non comprese in nessuna delle categorie precedenti (il testo può contenere qualsiasi carattere UTF-8).

Profondità di bit
: Indica se i campioni sono memorizzati con una risoluzione di 16 bit o di 24 bit (nel qual caso la compressione non è consentita).
  Tieni presente che alcuni sintetizzatori potrebbero non supportare i campioni a 24 bit.

Compressione
: Attiva la compressione dei campioni (nel qual caso la profondità di bit viene impostata a 16 bit). Se questa opzione è attiva, nell'[editor dei campioni](manual/soundfont-editor/editing-pages/sample-editor.md) viene mostrato un pulsante per la compressione.
  La compressione è una funzionalità offerta dal [formato sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3) e potrebbe non essere supportata da tutti i sintetizzatori.
  Attivare o disattivare questa opzione determina il formato del file al salvataggio, e Polyphone cambia l'estensione .sf2 / .sf3 se necessario.


## Informazioni generali {#doc_info}


La pagina di modifica delle informazioni generali fornisce le seguenti informazioni non modificabili:

Formato SoundFont
: Versione del [formato sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2) usata dal file.

Nome e versione della ROM
: Nome e versione di una ROM di campioni, se utilizzata.

Motore sonoro
: Nome di un motore sonoro.

Software di modifica
: Software di modifica usato per creare il soundfont.

Queste informazioni vengono mostrate in un suggerimento quando il mouse si trova sull'icona delle informazioni, alla fine della riga che mostra l'autore e la data.

Nome e dimensione del file
: Percorso e nome del file e sua dimensione su disco, mostrati nella parte inferiore della finestra (la dimensione è quella del file al momento dell'ultimo salvataggio).


## Statistiche {#doc_stats}


Vengono conteggiati:

* il numero di [campioni](manual/soundfont-editor/editing-pages/sample-editor.md), di campioni non utilizzati e di campioni duplicati,
* il numero di [strumenti](manual/soundfont-editor/editing-pages/instrument-editor.md) e di strumenti non utilizzati,
* il numero di [preset](manual/soundfont-editor/editing-pages/preset-editor.md),
* il numero di parametri e modulatori modificati per gli strumenti,
* il numero di parametri e modulatori modificati per i preset.

Il pulsante :guilabel:`Dettagli` accanto ai campioni, agli strumenti e ai preset mostra il [riepilogo della configurazione](manual/soundfont-editor/configuration-summaries.md) corrispondente.

I campioni e gli strumenti non utilizzati possono essere rimossi facilmente con la funzione «[Rimuovi elementi non utilizzati](manual/soundfont-editor/tools/global-tools.md#doc_unused)».
I campioni duplicati possono essere rimossi con la funzione «[Pulisci i duplicati](manual/soundfont-editor/tools/global-tools.md#doc_duplicates)».

**Nota:** il numero di 65536 parametri negli strumenti e nei preset non dovrebbe essere superato.
Alcuni sintetizzatori potrebbero non riuscire a leggerli tutti.



## Modulatori predefiniti {#doc_default_mod}


I modulatori predefiniti si applicano a ogni strumento del soundfont, prima dei [modulatori](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) dello strumento: un modulatore di uno strumento con le stesse sorgenti, la stessa destinazione e la stessa trasformazione sostituisce un modulatore predefinito (etichetta :guilabel:`sostituisce mod. predefinito`).

Senza una lista propria, un soundfont usa i modulatori predefiniti del formato SoundFont, mostrati senza poter essere modificati: velocità verso attenuazione e verso frequenza di taglio, pressione del canale e rotella di modulazione verso vibrato, volume, panoramica, espressione, riverbero, chorus e rotella del pitch.
Il modulatore velocità verso frequenza di taglio dipende dalle [impostazioni del suono](manual/settings.md#doc_sound).

Il pulsante :guilabel:`Personalizza` dà al soundfont una lista propria, che inizia con questi modulatori e si modifica poi come i modulatori di uno strumento (aggiunta, rimozione, copia, incolla, modifica).
Questa lista sostituisce i modulatori predefiniti del formato SoundFont: un modulatore rimosso dalla lista non si applica più a nessuno strumento, un modulatore aggiunto si applica a tutti.
Il pulsante :guilabel:`Ripristina i valori predefiniti di SoundFont` rimuove la lista e ripristina i modulatori predefiniti del formato.

La lista viene salvata nel blocco DMOD dei file sf2 e sf3, un'[estensione del formato](manual/annexes/the-different-soundfont-formats.md#doc_sf2) letta da FluidSynth 2.5 e successivi, SpessaSynth e Polyphone 2.7 e successivi ([plugin](manual/plugin.md) compreso).
Gli altri sintetizzatori la ignorano e applicano i modulatori predefiniti del formato SoundFont.

Quando uno strumento viene copiato in un altro soundfont i cui modulatori predefiniti sono diversi, vengono aggiunti modulatori globali allo strumento affinché suoni allo stesso modo.
L'[esportazione DLS](manual/annexes/the-different-soundfont-formats.md#doc_dls) converte i modulatori predefiniti del soundfont; l'esportazione sfz conserva solo il loro effetto della velocità sul volume e sulla frequenza di taglio.