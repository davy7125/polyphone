Usa queste istruzioni se non riesci a installare Polyphone sulla tua distribuzione Linux con i programmi di installazione disponibili nella sezione «[Software](software)».
Questo è il metodo usato per compilare le versioni Linux di Polyphone, su Ubuntu 24.04.


## Prerequisiti


Sono necessari i seguenti strumenti e librerie:

* un compilatore (``build-essential``)
* qt 6 (``qt6-base-dev``, ``qt6-svg-dev``, ``qt6-tools-dev``, ``qt6-tools-dev-tools`` e ``qt6-l10n-tools``)
* alsa (``libasound2-dev``)
* jack (``libjack-jackd2-dev``)
* rtaudio (``librtaudio-dev``)
* rtmidi (``librtmidi-dev``)
* stk (``libstk-dev``)
* vorbis (``libvorbis-dev``)
* ogg (``libogg-dev``)
* flac (``libflac-dev``)
* zlib (``zlib1g-dev``)
* ssl (``libssl-dev``)
* sndfile (``libsndfile1-dev``)

Si possono installare con il comando seguente:

```
sudo apt-get install build-essential qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools libasound2-dev libjack-jackd2-dev librtaudio-dev librtmidi-dev libstk-dev libvorbis-dev libogg-dev libflac-dev zlib1g-dev libssl-dev libsndfile1-dev
```

Ti servono anche i sorgenti di Polyphone, disponibili <a href="software" target="_blank">qui</a> o su <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


## Compilazione


Nella cartella :file:`sources` del progetto (dove si trova il file :file:`polyphone.pro`), apri un terminale e compila Polyphone eseguendo il comando seguente:

```
qmake6 polyphone.pro PREFIX=/usr && make
```

Se tutto è andato bene, nella cartella :file:`bin` dovrebbe comparire un file eseguibile :file:`polyphone`.
Può essere installato nel sistema con il comando :command:`sudo make install`.

Per impostazione predefinita viene usata una copia locale della libreria :program:`RtAudio`.
Se nella tua distribuzione mancano librerie come :program:`RtMidi` o :program:`Stk`, o se non sono disponibili in una versione compatibile, puoi modificare il file :file:`.pro` per usarne delle copie locali.
Per farlo, togli il commento dalle righe corrispondenti. Ad esempio

```
#DEFINES += USE_LOCAL_RTMIDI
```

diventerà

```
DEFINES += USE_LOCAL_RTMIDI
```

(senza il «#» iniziale).

**Nota:** se usi :program:`Qt Creator`, il progetto può essere aperto tramite il suo file :file:`.pro`, presente nella cartella :file:`sources`.
