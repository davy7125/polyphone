Usa queste istruzioni se non riesci a installare Polyphone sulla tua distribuzione Fedora con i programmi di installazione disponibili nella sezione «[Software](software)».


## Prerequisiti


### Installazione di Qt

Dopo un'installazione di base di Fedora, installa i seguenti pacchetti (compresi un compilatore e :program:`CMake`) come prerequisiti per lo sviluppo con Qt:

```
sudo dnf update
sudo dnf install @development-tools cmake pkgconf
```

Poi installa il framework Qt 6 con il comando seguente:

```
sudo dnf install qt-creator qt6-qtbase-devel qt6-qtsvg-devel qt6-qttools-devel
```


### Dipendenze di Polyphone

Installa le seguenti dipendenze (se hai installato un gestore di pacchetti grafico, in alternativa puoi usare quello):

```
sudo dnf install alsa-lib-devel jack-audio-connection-kit-devel rtaudio-devel zlib-devel libogg-devel flac-devel libvorbis-devel glib2-devel openssl-devel rtmidi-devel libsndfile-devel
```

**Nota:** con le versioni recenti di Fedora, il pacchetto :program:`jack-audio-connection-kit-devel` potrebbe essere sostituito da :program:`pipewire-jack-audio-connection-kit-devel`.


## Compilazione


### Sorgenti


Per prima cosa, procurati i sorgenti di Polyphone <a href="software" target="_blank">qui</a> o da <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.

Se vuoi proseguire la compilazione con Qt Creator, segui ora questo tutorial: [compilare con Qt Creator](development/using-qt-creator-to-build-polyphone.md). Altrimenti, continua a leggere questo documento.


### Ottenere l'eseguibile


Polyphone si compila con :program:`CMake`, e il progetto è descritto dal file :file:`CMakeLists.txt` della cartella :file:`sources`.
Nella radice del progetto (la cartella che contiene :file:`sources`), apri un terminale e compila Polyphone con questi comandi:

```
cmake -S sources -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 4
```

Se tutto va bene, troverai il file compilato :file:`polyphone` nella cartella :file:`build/bin`.
Se la compilazione è stata eseguita con Qt Creator, questo file si trova invece nella sottocartella :file:`bin` della cartella di compilazione scelta in Qt Creator.

Se :program:`RtAudio` o :program:`RtMidi` non sono disponibili in una versione compatibile, si possono usare le copie incluse nei sorgenti di Polyphone aggiungendo le opzioni ``-DPOLYPHONE_LOCAL_RTAUDIO=ON -DPOLYPHONE_LOCAL_RTMIDI=ON`` al primo comando.
Le altre opzioni sono descritte nella pagina [Compilare Polyphone per Ubuntu](development/building-polyphone-for-ubuntu.md).


## Risoluzione dei problemi


Questa [categoria](forum/support-bug-reports) del forum ti darà aiuto, se necessario.
