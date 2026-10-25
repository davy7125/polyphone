Facendo clic su uno o più preset nell'[albero](manual/soundfont-editor/tree.md), compare la pagina di modifica dei preset.
Un preset è una combinazione di uno o più strumenti ed è l'elemento visibile dall'esterno di un [soundfont](manual/annexes/the-different-soundfont-formats.md#doc_sf2).

Come nell'[editor degli strumenti](manual/soundfont-editor/editing-pages/instrument-editor.md), sono presenti diverse parti:

* la [tabella](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table),
* l'[editor degli intervalli](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range), quando si fa clic sul pulsante :guilabel:`Intervalli` della [barra degli strumenti](manual/soundfont-editor/toolbar.md#doc_view),
* la [sezione dei modulatori](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator).


![Pagina dei preset](images/edit_preset.png "Pagina dei preset")


Se è selezionato un solo preset, la pagina permette di modificare i parametri globali del preset e i parametri di tutte le sue divisioni.
Se sono selezionati più preset, si possono modificare i parametri globali di ciascuno di essi.


## Differenze rispetto agli strumenti {#doc_diff}


I parametri modificati qui si dicono relativi, a differenza dei parametri di uno strumento che, salvo casi particolari, si dicono assoluti.
I valori dei parametri di un preset vengono sommati ai parametri degli strumenti che lo compongono, oppure moltiplicati per essi.
Per questo motivo, un'attenuazione (o qualsiasi altro parametro) può essere negativa a livello di preset, cosa che non avviene a livello di strumento.
Alcuni parametri non sono modificabili a livello di preset:

* riproduzione in loop,
* nota di base,
* classe esclusiva,
* nota fissa,
* velocità fissa,
* i quattro offset.

Un preset è identificato da un numero di preset e da un numero di banco; questa combinazione è univoca e si può regolare sotto la tabella.
Ogni banco comprende 128 preset (numerati da 0 a 127).

L'ultimo banco (128) è di solito usato per i kit di percussioni.
