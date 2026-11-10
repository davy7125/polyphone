Verwenden Sie diese Anleitung, wenn Sie Polyphone auf Ihrer Fedora-Distribution nicht mit den im Bereich „[Software](software)“ verfügbaren Installationsprogrammen installieren können.


## Voraussetzungen


### Installation von Qt

Installieren Sie nach einer Grundinstallation von Fedora die folgenden Pakete (einschließlich eines Compilers und :program:`CMake`) als Voraussetzung für die Entwicklung mit Qt:

```
sudo dnf update
sudo dnf install @development-tools cmake pkgconf
```

Installieren Sie dann das Qt-6-Framework mit folgendem Befehl:

```
sudo dnf install qt-creator qt6-qtbase-devel qt6-qtsvg-devel qt6-qttools-devel
```


### Abhängigkeiten von Polyphone

Installieren Sie die folgenden Abhängigkeiten (ist ein grafischer Paketmanager installiert, können Sie alternativ auch diesen verwenden):

```
sudo dnf install alsa-lib-devel jack-audio-connection-kit-devel rtaudio-devel zlib-devel libogg-devel flac-devel libvorbis-devel glib2-devel openssl-devel rtmidi-devel libsndfile-devel
```

**Hinweis:** In neueren Fedora-Versionen kann das Paket :program:`jack-audio-connection-kit-devel` durch :program:`pipewire-jack-audio-connection-kit-devel` ersetzt sein.


## Kompilieren


### Quellen


Besorgen Sie sich zunächst die Quellen von Polyphone <a href="software" target="_blank">hier</a> oder von <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.

Wenn Sie mit Qt Creator weiterkompilieren möchten, folgen Sie nun diesem Tutorial: [mit Qt Creator kompilieren](development/using-qt-creator-to-build-polyphone.md). Andernfalls lesen Sie dieses Dokument weiter.


### Die ausführbare Datei erzeugen


Polyphone wird mit :program:`CMake` kompiliert, wobei das Projekt durch die Datei :file:`CMakeLists.txt` im Verzeichnis :file:`sources` beschrieben wird.
Öffnen Sie im Stammverzeichnis des Projekts (dem Verzeichnis, das :file:`sources` enthält) ein Terminal und kompilieren Sie Polyphone mit diesen Befehlen:

```
cmake -S sources -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 4
```

Läuft alles gut, befindet sich die kompilierte Datei :file:`polyphone` im Verzeichnis :file:`build/bin`.
Wird mit Qt Creator kompiliert, befindet sich diese Datei dagegen im Unterverzeichnis :file:`bin` des in Qt Creator gewählten Build-Verzeichnisses.

Sind :program:`RtAudio` oder :program:`RtMidi` nicht in einer kompatiblen Version verfügbar, können die in den Quellen von Polyphone enthaltenen Kopien verwendet werden, indem dem ersten Befehl die Optionen ``-DPOLYPHONE_LOCAL_RTAUDIO=ON -DPOLYPHONE_LOCAL_RTMIDI=ON`` hinzugefügt werden.
Die weiteren Optionen sind auf der Seite [Polyphone für Ubuntu kompilieren](development/building-polyphone-for-ubuntu.md) beschrieben.


## Fehlerbehebung


Diese [Kategorie](forum/support-bug-reports) des Forums hilft Ihnen bei Bedarf weiter.
