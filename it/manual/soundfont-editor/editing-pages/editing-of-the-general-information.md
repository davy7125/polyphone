La pagina per la modifica delle informazioni generali viene mostrata quando nell'[albero](manual/soundfont-editor/tree.md) è selezionata l'intestazione :guilabel:`Generale`.
Questa pagina è composta da tre tipi di elementi:

* [campi modificabili](#doc_edit),
* [informazioni generali](#doc_info) non modificabili,
* [statistiche](#doc_stats) sul soundfont.


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

* il numero di [campioni](manual/soundfont-editor/editing-pages/sample-editor.md) e di campioni non utilizzati,
* il numero di [strumenti](manual/soundfont-editor/editing-pages/instrument-editor.md) e di strumenti non utilizzati,
* il numero di [preset](manual/soundfont-editor/editing-pages/preset-editor.md),
* il numero di parametri e modulatori modificati per gli strumenti,
* il numero di parametri e modulatori modificati per i preset.

Il pulsante :guilabel:`Dettagli` accanto ai campioni, agli strumenti e ai preset mostra il [riepilogo della configurazione](manual/soundfont-editor/configuration-summaries.md) corrispondente.

I campioni e gli strumenti non utilizzati possono essere rimossi facilmente con la funzione «[Rimuovi elementi non utilizzati](manual/soundfont-editor/tools/global-tools.md#doc_unused)».

**Nota:** il numero di 65536 parametri negli strumenti e nei preset non dovrebbe essere superato.
Alcuni sintetizzatori potrebbero non riuscire a leggerli tutti.
