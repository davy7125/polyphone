Usa queste istruzioni se non riesci a installare Polyphone sulla tua distribuzione Linux con i programmi di installazione disponibili nella sezione «[Software](software)».
Questo è il metodo usato per compilare le versioni Linux di Polyphone, su Ubuntu 24.04.


## Prerequisiti


Sono necessari i seguenti strumenti e librerie:

* un compilatore (``build-essential``)
* cmake (``cmake``) e pkg-config (``pkgconf``)
* qt 6 (``qt6-base-dev``, ``qt6-svg-dev``, ``qt6-tools-dev``, ``qt6-tools-dev-tools`` e ``qt6-l10n-tools``)
* alsa (``libasound2-dev``)
* jack (``libjack-jackd2-dev``)
* rtaudio (``librtaudio-dev``)
* rtmidi (``librtmidi-dev``)
* vorbis (``libvorbis-dev``)
* ogg (``libogg-dev``)
* flac (``libflac-dev``)
* zlib (``zlib1g-dev``)
* ssl (``libssl-dev``)
* sndfile (``libsndfile1-dev``)

Si possono installare con il comando seguente:

```
sudo apt-get install build-essential cmake pkgconf qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools libasound2-dev libjack-jackd2-dev librtaudio-dev librtmidi-dev libvorbis-dev libogg-dev libflac-dev zlib1g-dev libssl-dev libsndfile1-dev
```

Ti servono anche i sorgenti di Polyphone, disponibili <a href="software" target="_blank">qui</a> o su <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


## Compilazione


Polyphone si compila con :program:`CMake`, e il progetto è descritto dal file :file:`CMakeLists.txt` della cartella :file:`sources`.
Nella radice del progetto (la cartella che contiene :file:`sources`), apri un terminale ed esegui i comandi seguenti:

```
cmake -S sources -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build --parallel 4
```

Se tutto è andato bene, nella cartella :file:`build/bin` dovrebbe comparire un file eseguibile :file:`polyphone`.
Può essere installato nel sistema con il comando :command:`sudo cmake --install build`.

Se nella tua distribuzione mancano librerie come :program:`RtAudio` o :program:`RtMidi`, o se non sono disponibili in una versione compatibile, puoi usare al loro posto le copie incluse nei sorgenti di Polyphone, aggiungendo queste opzioni al primo comando:

```
-DPOLYPHONE_LOCAL_RTAUDIO=ON -DPOLYPHONE_LOCAL_RTMIDI=ON
```

I pacchetti ``librtaudio-dev`` e ``librtmidi-dev`` non sono allora necessari.

Al primo comando si possono aggiungere altre opzioni:

-DPOLYPHONE_QT_VERSION=5
: Compila con Qt 5 invece di Qt 6 (per impostazione predefinita viene usata la versione più recente trovata).
  I pacchetti di Qt 5 sono allora ``qtbase5-dev``, ``libqt5svg5-dev``, ``qttools5-dev`` e ``qttools5-dev-tools``.

-DPOLYPHONE_BUILD_PLUGIN=ON
: Compila anche il [plugin](development/building-the-polyphone-plugin.md).

-DPOLYPHONE_WOLFSSL=ON
: Usa :program:`wolfSSL` invece di :program:`OpenSSL`.

**Nota:** se usi :program:`Qt Creator`, il progetto si apre tramite il file :file:`CMakeLists.txt` presente nella cartella :file:`sources` (vedi [Compilare Polyphone con Qt Creator](development/using-qt-creator-to-build-polyphone.md)).
