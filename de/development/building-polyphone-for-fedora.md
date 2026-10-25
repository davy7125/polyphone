Verwenden Sie diese Anleitung, wenn Sie Polyphone auf Ihrer Fedora-Distribution nicht mit den im Bereich „[Software](software)“ verfügbaren Installationsprogrammen installieren können.


## Voraussetzungen


### Installation von Qt

Installieren Sie nach einer Grundinstallation von Fedora die folgenden Pakete (einschließlich eines Compilers) als Voraussetzung für die Entwicklung mit Qt:

```
sudo dnf update
sudo dnf install @development-tools
```

Installieren Sie dann das Qt-6-Framework mit folgendem Befehl:

```
sudo dnf install qt-creator qt6-qtbase-devel qt6-qtsvg-devel qt6-qttools-devel
```


### Abhängigkeiten von Polyphone

Installieren Sie die folgenden Abhängigkeiten (ist ein grafischer Paketmanager installiert, können Sie alternativ auch diesen verwenden):

```
sudo dnf install alsa-lib-devel jack-audio-connection-kit-devel rtaudio-devel zlib-devel libogg-devel flac-devel libvorbis-devel glib2-devel openssl-devel rtmidi-devel stk-devel libsndfile-devel
```

**Hinweis:** In neueren Fedora-Versionen kann das Paket :program:`jack-audio-connection-kit-devel` durch :program:`pipewire-jack-audio-connection-kit-devel` ersetzt sein.


## Kompilieren


### Quellen


Besorgen Sie sich zunächst die Quellen von Polyphone <a href="software" target="_blank">hier</a> oder von <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.

Wenn Sie mit Qt Creator weiterkompilieren möchten, folgen Sie nun diesem Tutorial: [mit Qt Creator kompilieren](development/using-qt-creator-to-build-polyphone.md). Andernfalls lesen Sie dieses Dokument weiter.


### Die ausführbare Datei erzeugen


Wechseln Sie in das Quellverzeichnis (in dem sich die Datei :file:`polyphone.pro` befindet), öffnen Sie ein Terminal und kompilieren Sie Polyphone mit diesem Befehl:

```
qmake6 && make
```

Läuft alles gut, befindet sich die kompilierte Datei :file:`polyphone` im Unterverzeichnis :file:`bin`.
Wird mit Qt Creator kompiliert, befindet sich diese Datei dagegen im Verzeichnis :file:`build-XXX`, das neben :file:`sources` erstellt wird.


## Fehlerbehebung


Diese [Kategorie](forum/support-bug-reports) des Forums hilft Ihnen bei Bedarf weiter.
