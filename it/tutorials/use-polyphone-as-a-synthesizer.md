Polyphone non è solo un editor di soundfont, è anche un sintetizzatore completo che permette di suonare con una tastiera MIDI.
Questo tutorial mostra come avviare Polyphone in modalità sintetizzatore.


## Prerequisiti

Per prima cosa ti serve una **master keyboard** o un altro controller MIDI, da collegare al computer tramite un **adattatore MIDI-USB**.

Scarica l'ultima versione di Polyphone [qui](software), poi installa e apri il programma. Nelle [impostazioni](manual/settings.md#doc_general) dovresti trovare il tuo dispositivo MIDI tra gli ingressi MIDI. In caso contrario, probabilmente il driver dell'adattatore MIDI non è installato sul computer (risolvi il problema prima di continuare).

Sempre nelle impostazioni, verifica che l'uscita audio sia corretta.

## Aprire Polyphone in modalità sintetizzatore


### Windows

Crea un collegamento a :file:`polyphone.exe` con l'argomento :command:`-s`, come spiegato nella sezione [Opzioni di avvio di Polyphone](#doc_launch) più sotto.

### Linux

Apri un terminale e scrivi la seguente riga di comando:

```
polyphone -s
```

### Mac

Apri un terminale e scrivi la seguente riga di comando:

```
/Applications/Polyphone.app/Contents/MacOS/Polyphone -s
```

**Nota:** se hai spostato l'applicazione Polyphone in una cartella diversa da :file:`Applications`, dovrai adattare il percorso indicato sopra.


## Suonare uno strumento


Per suonare uno strumento, apri semplicemente un soundfont, che puoi scaricare ad esempio dall'[archivio](soundfonts), e seleziona uno strumento contenuto nel soundfont. Gli strumenti sono ordinati per banco: seleziona un banco per mostrare sotto l'elenco degli strumenti.

![Polyphone aperto con l'opzione della modalità sintetizzatore](images/synth_mode.png "Polyphone aperto con l'opzione della modalità sintetizzatore")

Una volta selezionato uno strumento, il suo intervallo di note viene mostrato sulla tastiera virtuale e puoi suonarlo con i tasti corrispondenti del tuo controller MIDI.
Il pulsante :guilabel:`Panico MIDI` interrompe tutti i suoni in corso (utile se una nota resta bloccata), e il pulsante :guilabel:`Registratore` apre il [registratore](manual/soundfont-editor/toolbar.md#doc_recorder).


## Opzioni


### Suonare più strumenti contemporaneamente

Per suonare più strumenti quando si preme un tasto sulla tastiera, imposta semplicemente l'opzione :guilabel:`Selezione multipla` su :guilabel:`acceso`. Ora nell'elenco degli strumenti si possono selezionare più strumenti.

### Suonare più strumenti separatamente

Se al computer sono collegati più controller MIDI, puoi aprire più soundfont e scegliere un canale specifico per ciascuno dei soundfont aperti. In questo modo, ad esempio, una prima tastiera servirà a suonare uno strumento e una seconda tastiera a suonarne un altro. Lo stesso soundfont può essere aperto più volte per suonare contemporaneamente più strumenti che contiene.

L'assegnazione dei canali si fa modificando il valore di :guilabel:`Canale`. Ad esempio, la prima scheda aperta ascolterà il canale 1 e la seconda il canale 2.
Il valore :guilabel:`tutti` (predefinito) ascolta tutti i canali, e il valore :guilabel:`rileva...` seleziona il canale del prossimo tasto premuto sulla tastiera.

![Configurazione del sintetizzatore a 2 canali](images/synth_2_channels.png "Configurazione del sintetizzatore a 2 canali")

**Nota:** alcune tastiere possono essere divise in due parti, in modo che una sola tastiera possa inviare i tasti a 2 canali diversi.

### Cambiare strumento con un tasto

Il cambio di strumento avviene di solito tramite un messaggio MIDI di tipo «Program change», che Polyphone è in grado di leggere.

Si può però usare anche un altro meccanismo: tasti programmati per inviare messaggi «Note ON» / «Note OFF», con il numero di nota corrispondente al numero dello strumento. Questo meccanismo può essere utile quando il soundfont contiene solo pochi strumenti e quando i tasti possono essere programmati per inviare note specifiche (ad esempio i pad di una master keyboard).

Se l'opzione :guilabel:`Selezione per note` è impostata su :guilabel:`acceso`:

* uno strumento viene attivato quando si riceve un messaggio MIDI «Note ON»,
* uno strumento viene disattivato quando si riceve un messaggio MIDI «Note OFF».

Questa opzione può essere usata quando il pad di una master keyboard è configurato come interruttore (una pressione attiva la nota, un'altra la disattiva).

Se si seleziona :guilabel:`alterna`:

* uno strumento viene attivato quando si riceve un messaggio MIDI «Note ON»,
* uno strumento viene disattivato quando si riceve un altro messaggio MIDI «Note ON».

In questo caso la commutazione avviene a livello di Polyphone.


## Opzioni di avvio di Polyphone {#doc_launch}


Tutte le opzioni viste sopra possono essere richiamate all'apertura di Polyphone aggiungendo parametri alla riga di comando. È anche possibile specificare il soundfont da aprire.

Su **Linux** o **macOS**, basta completare la riga di comando. Su **Windows**, crea un collegamento facendo clic con il tasto destro sul file eseguibile :file:`polyphone.exe` e scegli :guilabel:`Invia a` → :guilabel:`Desktop (crea collegamento)` (da :guilabel:`Mostra altre opzioni` del menu contestuale in Windows 11). Poi, aprendo le proprietà di questo collegamento, puoi aggiungere parametri alla destinazione, ad esempio:

```
"C:\Program Files\Polyphone\polyphone.exe" -s -c "all|on|toggle"
```

Le varie opzioni sono descritte in questa [pagina](manual/annexes/command-line.md#doc_synth).


## Usi del sintetizzatore


Ora che sai come trasformare Polyphone in un sintetizzatore, perché non [usare un Raspberry Pi per creare uno strumento musicale](tutorials/create-a-synthesizer-with-raspberry-pi.md)?

La modalità sintetizzatore di Polyphone può essere usata anche per [riprodurre file MIDI](tutorials/read-midi-file-with-soundfont.md).
