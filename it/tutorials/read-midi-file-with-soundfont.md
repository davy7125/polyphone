I file MIDI contengono messaggi MIDI che, messi insieme, formano una melodia. Possono essere abbinati a un sintetizzatore che usa un soundfont, e questo tutorial mostra come usare un file MIDI per provare la resa di un soundfont.


## Un po' di contesto


<div class="too_complicated">
<p>Suonando sulla tastiera di un sintetizzatore vengono emessi segnali <a href="https://it.wikipedia.org/wiki/MIDI" target="_blank">MIDI</a>, che contengono informazioni come le note suonate e la loro velocità (termine che indica la forza con cui il tasto viene premuto). Un file MIDI, in parole semplici, contiene una registrazione cronologica dei segnali emessi da una tastiera di questo tipo. Fin qui non è stato ancora creato alcun suono.</p>

<p>Perché un suono esca dall'altoparlante è necessaria una fase di conversione, ed è qui che entra in gioco il sintetizzatore: genera suoni a partire dai segnali MIDI ricevuti. Esistono diversi tipi di sintetizzatori; qui parleremo di sintetizzatori basati su campioni, e più precisamente di sintetizzatori che usano banchi di suoni nel formato SoundFont.</p>

<p>Un file MIDI può controllare fino a 16 canali diversi, permettendo di usare in parallelo 16 strumenti diversi. Oltre a indicare quali note suonare, il file MIDI contiene istruzioni che dicono al sintetizzatore di usare lo strumento X del banco Y per le note successive del canale Z. Per garantire una selezione corretta degli strumenti, il banco di suoni usato dal sintetizzatore deve rispettare lo standard <a href="https://it.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a>, che assegna uno strumento preciso a ogni coppia di numeri X e Y. Ad esempio, lo strumento <strong>001</strong> deve corrispondere a un pianoforte nel soundfont.</p>

<p>Riassumendo, parleremo qui della riproduzione di un file MIDI che pilota un sintetizzatore, il quale genera i suoni contenuti in un soundfont conforme allo standard General MIDI. Il seguito è più semplice!</p>
</div>


## Prerequisiti


Per riprodurre un file MIDI con il metodo descritto più sotto ti servono:

- l'ultima versione di Polyphone, disponibile in questa [pagina](software),
- un file MIDI, ad esempio <a href="files/ff7choco.mid" download>questo</a>,
- un soundfont che vuoi provare e che sia conforme allo standard <a href="https://it.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> (nell'[archivio](soundfonts/midi-standard/gm) esiste un filtro apposito).

Si può provare anche un soundfont non conforme a questo standard, [scegliendo tu stesso i suoi preset](#doc_presets).


## Riprodurre un file MIDI


Apri il tuo soundfont in Polyphone, poi fai clic sull'icona ![MIDI](images/toolbar_midi.png "MIDI"){.inline} a destra della [barra degli strumenti](manual/soundfont-editor/toolbar.md) per mostrare il [lettore MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player).

Fai clic sul pulsante della cartella in alto a sinistra nella finestra, seleziona il tuo file MIDI, poi fai clic sul pulsante di riproduzione (o premi :kbd:`Space`) e sei pronto.

![Il lettore MIDI di Polyphone](images/midi_player.png "Il lettore MIDI di Polyphone")

Ogni canale del file che contiene note ha una propria riga nella tabella. La colonna :guilabel:`Preset` mostra il preset richiesto dal file, che può cambiare nel corso del brano, e il LED della colonna :guilabel:`Volume` si accende ogni volta che inizia una nota. Il cursore sotto il nome del file permette di spostarsi in un altro passaggio del brano.

Il lettore MIDI è disponibile anche quando Polyphone viene aperto in [modalità sintetizzatore](tutorials/use-polyphone-as-a-synthesizer.md).


## Provare il soundfont


Il soundfont viene suonato così com'è nell'editor, comprese le modifiche non salvate: puoi modificare uno strumento mentre il brano suona e sentire subito il risultato, ad esempio regolandone il volume, l'inviluppo o il filtro.

Il lettore ti aiuta anche ad ascoltare i dettagli:

- i pulsanti :guilabel:`S` (solo) e :guilabel:`M` (silenzia) di un canale isolano uno strumento o lo tolgono dal mix, e il cursore della colonna :guilabel:`Volume` lo bilancia con gli altri;
- un'icona di avviso nella colonna :guilabel:`Preset` indica che il soundfont non contiene il preset richiesto dal file: le note di quel canale non vengono suonate, il che rivela un soundfont non del tutto conforme allo standard General MIDI;
- il pulsante MIDI di un canale elenca i controller usati in quel momento dal file (rotella di modulazione, espressione, riverbero…), utile per verificare i [modulatori](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) del soundfont;
- le [impostazioni di riproduzione](manual/soundfont-editor/toolbar.md#doc_midi_player_settings), a destra della fascia in alto, permettono di riprodurre il brano in loop, di rallentarlo per ascoltare un passaggio veloce o di trasporlo per provare altre note.


## Scegliere i soundfont e i preset {#doc_presets}


Ogni canale può suonare un soundfont diverso: apri più soundfont in Polyphone, poi fai doppio clic su una cella della colonna :guilabel:`Soundfont` per scegliere quello suonato dal canale. Puoi ad esempio confrontare i pianoforti di due soundfont sulla stessa parte, oppure tenere un soundfont General MIDI per l'accompagnamento mentre il tuo soundfont suona la melodia.

Per impostazione predefinita, i canali sono impostati su :guilabel:`Selezione`: suonano il soundfont della scheda corrente, o il file selezionato in un [esploratore di soundfont](manual/soundfont-explorer.md#doc_play). Per confrontare tutti i soundfont di una cartella, aprila in Polyphone e seleziona i suoi file uno dopo l'altro mentre il brano è in riproduzione: ogni soundfont viene caricato in background e si sente non appena è pronto, senza aprire una scheda.

![Scegliere il soundfont di un canale](images/midi_player_soundfonts.png "Scegliere il soundfont di un canale")

Allo stesso modo, un doppio clic su una cella della colonna :guilabel:`Preset` impone un preset del soundfont al posto di quello richiesto dal file. È così che si prova un soundfont non conforme allo standard General MIDI, ad esempio un soundfont con un solo strumento: scegli il suo preset per il canale della melodia, e i cambi di programma del file vengono allora ignorati su quel canale.


## Esportare il risultato


Quando il risultato ti soddisfa, :guilabel:`Esporta audio...` nelle impostazioni di riproduzione scrive il brano in un file wav così come si sente nel lettore, con gli stessi soundfont, preset, volumi, tempo e intonazione.
Un file MIDI può anche essere convertito in un file wav dalla [riga di comando](manual/annexes/command-line.md#doc_render), ad esempio in uno script.
