Verwenden Sie diese Anleitung, wenn Sie Polyphone auf Ihrer Linux-Distribution nicht mit den im Bereich „[Software](software)“ verfügbaren Installationsprogrammen installieren können.
Mit dieser Methode werden die Linux-Versionen von Polyphone unter Ubuntu 24.04 kompiliert.


## Voraussetzungen


Folgende Werkzeuge und Bibliotheken werden benötigt:

* ein Compiler (``build-essential``)
* qt 6 (``qt6-base-dev``, ``qt6-svg-dev``, ``qt6-tools-dev``, ``qt6-tools-dev-tools`` und ``qt6-l10n-tools``)
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

Sie können mit folgendem Befehl installiert werden:

```
sudo apt-get install build-essential qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools libasound2-dev libjack-jackd2-dev librtaudio-dev librtmidi-dev libstk-dev libvorbis-dev libogg-dev libflac-dev zlib1g-dev libssl-dev libsndfile1-dev
```

Außerdem benötigen Sie die Quellen von Polyphone, die <a href="software" target="_blank">hier</a> oder auf <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> verfügbar sind.


## Kompilieren


Öffnen Sie im Verzeichnis :file:`sources` des Projekts (in dem sich die Datei :file:`polyphone.pro` befindet) ein Terminal und kompilieren Sie Polyphone mit folgendem Befehl:

```
qmake6 polyphone.pro PREFIX=/usr && make
```

Ist alles gut gegangen, sollte dann im Verzeichnis :file:`bin` eine ausführbare Datei :file:`polyphone` erscheinen.
Mit dem Befehl :command:`sudo make install` kann sie im System installiert werden.

Standardmäßig wird eine lokale Kopie der Bibliothek :program:`RtAudio` verwendet.
Fehlen in Ihrer Distribution Bibliotheken wie :program:`RtMidi` oder :program:`Stk` oder liegen sie nicht in einer kompatiblen Version vor, können Sie die :file:`.pro`-Datei bearbeiten, um lokale Kopien davon zu verwenden.
Entfernen Sie dazu die Kommentarzeichen in den entsprechenden Zeilen. Zum Beispiel wird

```
#DEFINES += USE_LOCAL_RTMIDI
```

zu

```
DEFINES += USE_LOCAL_RTMIDI
```

(ohne vorangestelltes „#“).

**Hinweis:** Wenn Sie :program:`Qt Creator` verwenden, kann das Projekt über seine :file:`.pro`-Datei im Verzeichnis :file:`sources` geöffnet werden.
