Verwenden Sie diese Anleitung, wenn Sie Polyphone auf Ihrer Linux-Distribution nicht mit den im Bereich „[Software](software)“ verfügbaren Installationsprogrammen installieren können.
Mit dieser Methode werden die Linux-Versionen von Polyphone unter Ubuntu 24.04 kompiliert.


## Voraussetzungen


Folgende Werkzeuge und Bibliotheken werden benötigt:

* ein Compiler (``build-essential``)
* cmake (``cmake``) und pkg-config (``pkgconf``)
* qt 6 (``qt6-base-dev``, ``qt6-svg-dev``, ``qt6-tools-dev``, ``qt6-tools-dev-tools`` und ``qt6-l10n-tools``)
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

Sie können mit folgendem Befehl installiert werden:

```
sudo apt-get install build-essential cmake pkgconf qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools libasound2-dev libjack-jackd2-dev librtaudio-dev librtmidi-dev libvorbis-dev libogg-dev libflac-dev zlib1g-dev libssl-dev libsndfile1-dev
```

Außerdem benötigen Sie die Quellen von Polyphone, die <a href="software" target="_blank">hier</a> oder auf <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> verfügbar sind.


## Kompilieren


Polyphone wird mit :program:`CMake` kompiliert, wobei das Projekt durch die Datei :file:`CMakeLists.txt` im Verzeichnis :file:`sources` beschrieben wird.
Öffnen Sie im Stammverzeichnis des Projekts (dem Verzeichnis, das :file:`sources` enthält) ein Terminal und führen Sie die folgenden Befehle aus:

```
cmake -S sources -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build --parallel 4
```

Ist alles gut gegangen, sollte dann im Verzeichnis :file:`build/bin` eine ausführbare Datei :file:`polyphone` erscheinen.
Mit dem Befehl :command:`sudo cmake --install build` kann sie im System installiert werden.

Fehlen in Ihrer Distribution Bibliotheken wie :program:`RtAudio` oder :program:`RtMidi` oder liegen sie nicht in einer kompatiblen Version vor, können stattdessen die in den Quellen von Polyphone enthaltenen Kopien verwendet werden, indem Sie dem ersten Befehl diese Optionen hinzufügen:

```
-DPOLYPHONE_LOCAL_RTAUDIO=ON -DPOLYPHONE_LOCAL_RTMIDI=ON
```

Die Pakete ``librtaudio-dev`` und ``librtmidi-dev`` werden dann nicht benötigt.

Dem ersten Befehl können weitere Optionen hinzugefügt werden:

-DPOLYPHONE_QT_VERSION=5
: Mit Qt 5 statt Qt 6 kompilieren (standardmäßig wird die neueste gefundene Version verwendet).
  Die Pakete von Qt 5 sind dann ``qtbase5-dev``, ``libqt5svg5-dev``, ``qttools5-dev`` und ``qttools5-dev-tools``.

-DPOLYPHONE_BUILD_PLUGIN=ON
: Auch das [Plugin](development/building-the-polyphone-plugin.md) kompilieren.

-DPOLYPHONE_WOLFSSL=ON
: :program:`wolfSSL` statt :program:`OpenSSL` verwenden.

**Hinweis:** Wenn Sie :program:`Qt Creator` verwenden, wird das Projekt über die Datei :file:`CMakeLists.txt` im Verzeichnis :file:`sources` geöffnet (siehe [Polyphone mit Qt Creator kompilieren](development/using-qt-creator-to-build-polyphone.md)).
