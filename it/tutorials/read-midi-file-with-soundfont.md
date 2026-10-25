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

- un accesso a internet,
- un browser web,
- un file MIDI, ad esempio <a href="files/ff7choco.mid" download>questo</a>,
- facoltativamente, un soundfont che vuoi provare e che sia conforme allo standard <a href="https://it.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> (nell'[archivio](soundfonts/midi-standard/gm) esiste un filtro apposito),
- facoltativamente, l'ultima versione di Polyphone, disponibile in questa [pagina](software).


## Riprodurre un file MIDI


Un metodo semplice è usare il sintetizzatore <a href="https://spessasus.github.io/SpessaSynth/" target="_blank">SpessaSynth</a>, che è allo stesso tempo un lettore di file MIDI e un sintetizzatore di soundfont. Ha il vantaggio di essere disponibile online e non richiede alcuna installazione particolare.

Fai clic sul pulsante :guilabel:`Load MIDI file`, seleziona il tuo file MIDI e sei pronto.

![Il lettore MIDI e sintetizzatore SpessaSynth](images/SpessaSynth.jpg "Il lettore MIDI e sintetizzatore SpessaSynth")


## Scegliere un soundfont


Per usare un soundfont a tua scelta, fai clic sul pulsante :guilabel:`Upload soundfont` e seleziona il tuo file.


## Collegare l'uscita MIDI a Polyphone


Come detto sopra, **SpessaSynth** è allo stesso tempo un lettore di file MIDI e un sintetizzatore. È possibile usare solo la funzione di lettore di file MIDI e inviare i segnali a Polyphone, che si occuperà della sintesi del suono. Questo può essere utile se il soundfont è troppo grande per essere inviato via internet.

Per collegare SpessaSynth a Polyphone (metodo provato su Linux):

- avvia Polyphone in [modalità sintetizzatore](tutorials/use-polyphone-as-a-synthesizer.md) e apri le [impostazioni](manual/settings.md#doc_general),
- seleziona «MIDI through» come ingresso MIDI (ricordane il nome),
- apri SpessaSynth nel tuo browser web e apri le sue impostazioni,
- seleziona l'uscita MIDI «MIDI through» (nome identico all'ingresso MIDI di Polyphone).

Poiché un file MIDI può inviare messaggi a 16 canali, dovrai aprire il soundfont 16 volte e specificare per ogni scheda un canale diverso da 1 a 16. Per comodità, all'apertura di Polyphone puoi specificare il soundfont da aprire e l'opzione «each» per aprire tutte le 16 schede in una volta. Il comando sarebbe ad esempio:

```
polyphone -s -i "/home/davy/Downloads/gm.sf2" -c "each|off|off"
```

Per maggiori informazioni sulle righe di comando vedi questa [pagina](manual/annexes/command-line.md#doc_synth).
