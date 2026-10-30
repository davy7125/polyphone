Il menu, situato nell'angolo in alto a destra del software, contiene diverse voci che danno accesso a varie funzionalità.


![Menu](images/menu.png "Menu")


## Aprire un editor di soundfont {#doc_open}


Nuovo
: Crea un nuovo soundfont vuoto, in cui tutto deve essere creato da zero.
  Accessibile anche con la scorciatoia :kbd:`Ctrl`+:kbd:`N`.

Apri…
: Apre un file [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [.dls](manual/annexes/the-different-soundfont-formats.md#doc_dls), [.sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) o [.organ](manual/annexes/the-different-soundfont-formats.md#doc_organ).
  Accessibile anche con la scorciatoia :kbd:`Ctrl`+:kbd:`O`.


## Salvare un soundfont {#doc_save}


Salva
: Salva il file corrente.
  Accessibile anche con la scorciatoia :kbd:`Ctrl`+:kbd:`S`.

Salva con nome…
: Salva il file corrente in un altro file.
  Accessibile anche con la scorciatoia :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`S`.

Esporta soundfont…
: Unisce ed esporta uno o più preset dei soundfont aperti nel formato [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz) o [.dls](manual/annexes/the-different-soundfont-formats.md#doc_dls). Il formato aggiuntivo .csv esporta tutti i parametri per un'analisi dettagliata del contenuto del soundfont.
  Accessibile anche con la scorciatoia :kbd:`Ctrl`+:kbd:`E`.


![Finestra di esportazione](images/export.png "Finestra di esportazione")


Nella finestra di esportazione sono elencati i preset di tutti i soundfont aperti, e occorre spuntare quelli da esportare (i pulsanti :guilabel:`Seleziona tutto` e :guilabel:`Deseleziona tutto` aiutano a cambiare rapidamente la selezione).
Gli altri parametri sono:

Destinazione
: La cartella in cui vengono scritti i file.
  Un file non viene mai sovrascritto: se esiste già un file con lo stesso nome, alla fine del nome del nuovo file viene aggiunto un numero.

Formato
: Uno tra :guilabel:`.sf2`, :guilabel:`.sf3`, :guilabel:`.sfz`, :guilabel:`.csv` e :guilabel:`.dls`.
  Se i preset provengono da più soundfont, vengono uniti in un unico soundfont in cui ogni soundfont di origine diventa un banco.
  Per questo motivo si possono esportare contemporaneamente al massimo 127 soundfont, ciascuno con al massimo 127 preset selezionati.

Qualità
: Solo per il formato .sf3, qualità della compressione dei campioni.

Un file per preset
: Solo per i formati .sf2, .sf3 e .dls, creazione di un file per ogni preset selezionato invece di un unico file.

Una directory per banco, numero preset come prefisso, classificazione General MIDI
: Solo per il formato .sfz. L'esportazione crea una cartella contenente i file sfz e una sottocartella :file:`samples`.
  I file sfz possono essere messi in una cartella per ogni banco, il loro nome può essere preceduto dal numero del preset e possono essere suddivisi in cartelle corrispondenti alle categorie General MIDI.

Valori grezzi
: Solo per il formato .csv. I valori vengono scritti così come sono memorizzati nel formato sf2, invece di essere convertiti nelle unità abituali (secondi, decibel, centesimi, ecc.).
  L'esportazione crea una cartella contenente il file :file:`samples.csv` e un file per ogni strumento e per ogni preset.


## Opzioni di visualizzazione {#doc_view}


Schermo intero
: Attiva o disattiva la modalità a schermo intero.
  Accessibile anche con la scorciatoia :kbd:`F11`.


## Scorciatoie {#doc_shortcuts}


Impostazioni
: Apre le [impostazioni](manual/settings.md) del software.
  Accessibile anche con la scorciatoia :kbd:`Ctrl`+:kbd:`,`.

Manuale in linea
: Mostra questo manuale utente in un browser web.
  Accessibile anche con la scorciatoia :kbd:`F1`.

Info su Polyphone…
: Mostra la versione del software e il copyright. Il pulsante :guilabel:`Crediti` mostra l'elenco delle persone che hanno contribuito al progetto.


## Chiusura {#doc_close}


Chiudi scheda
: Chiude la scheda corrente (un editor di soundfont, un esploratore di soundfont, le impostazioni, ecc.).
  Accessibile anche con la scorciatoia :kbd:`Ctrl`+:kbd:`W`.

Esci
: Chiude il software.
  Accessibile anche con la scorciatoia :kbd:`Ctrl`+:kbd:`Q`.
