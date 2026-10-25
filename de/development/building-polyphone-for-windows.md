Mit dieser Anleitung kompilieren Sie Polyphone für Windows aus der neuesten Version der Quellen auf <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>. Sie erhalten die Software auf dem neuesten Entwicklungsstand, einschließlich der neuesten Funktionen, aber möglicherweise auch mit einigen Instabilitäten.


## Voraussetzungen


### Installation von Qt

Polyphone ist in C++ mit der Entwicklungsumgebung Qt (Version 6) geschrieben. Um Qt zu installieren, gehen Sie auf diese <a href="https://www.qt.io/download-qt-installer-oss" target="_blank">Seite</a> und laden Sie das Installationsprogramm herunter. Dazu müssen Sie zuerst ein Konto anlegen.

Starten Sie dann das Installationsprogramm und wählen Sie die folgenden Komponenten aus:

![Auszuwählende Komponenten](images/select-qt-components.png "Auszuwählende Komponenten")

Nehmen Sie die neueste verfügbare Version, falls die Auswahlmöglichkeiten nicht genau übereinstimmen.


### Installation des Compilers und der Abhängigkeiten

Qt benötigt einen Compiler, um die Quellen von Polyphone zu kompilieren. Aus folgenden Gründen muss MinGW verwendet werden:

* vor allem aus einem praktischen Grund, da die unten genannten Abhängigkeiten mit MinGW kompatibel sind,
* aber auch, um den korrekten Import / Export von Dateien im sfArk-Format zu ermöglichen (die Kompilieroption ``-ffloat-store`` ist erforderlich).

Der Compiler und die von Polyphone verwendeten Bibliotheken werden mit <a href="https://www.msys2.org/" target="_blank">MSYS2</a> installiert.
Installieren Sie es im Standardverzeichnis :file:`C:/msys64` (die Projektdatei :file:`polyphone.pro` sucht die Bibliotheken in :file:`C:/msys64/mingw64`), öffnen Sie das Terminal :program:`MSYS2 MINGW64` und führen Sie diesen Befehl aus:

```
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-zlib mingw-w64-x86_64-libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-openssl mingw-w64-x86_64-flac mingw-w64-x86_64-libsndfile
```

Fügen Sie dann in den Windows-Systemeinstellungen den Ordner :file:`C:/msys64/mingw64/bin` zum Path hinzu:

* öffnen Sie das Windows-Menü,
* geben Sie „Umgebungsvariablen“ ein,
* öffnen Sie die Systemeigenschaften und klicken Sie auf „Umgebungsvariablen“,
* doppelklicken Sie in den Systemvariablen auf „Path“,
* und fügen Sie :file:`C:/msys64/mingw64/bin` zu den in der Variablen „Path“ aufgeführten Verzeichnissen hinzu.

Dadurch kann Qt die ausführbaren Dateien und Bibliotheken von MinGW finden.


## Kompilieren


### Quellen

Laden Sie zunächst die Quellen von Polyphone <a href="software" target="_blank">hier</a> oder von <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> herunter und entpacken Sie die Dateien zum Beispiel auf Ihrem Windows-Desktop.

### Mit Qt Creator kompilieren

Fahren Sie nun mit dieser Anleitung zum Kompilieren von Polyphone mit Qt Creator fort: [Polyphone mit Qt Creator kompilieren](development/using-qt-creator-to-build-polyphone.md). Eine Änderung der Datei :file:`polyphone.pro` ist nicht nötig.

### Über die Kommandozeile kompilieren

Öffnen Sie das Terminal :program:`MSYS2 MINGW64`, fügen Sie Qt zum Path hinzu und kompilieren Sie das Projekt in einem neuen Verzeichnis (passen Sie die Pfade vorher an):

```
export PATH="/c/Qt/6.6.2/mingw_64/bin:$PATH"
mkdir build
cd build
qmake ../sources/polyphone.pro CONFIG+=release
mingw32-make -j4
```

Ist die Kompilierung gut verlaufen, sollte sich die ausführbare Datei :file:`polyphone.exe` im Verzeichnis :file:`build/release` befinden.


### Polyphone starten

Damit die ausführbare Datei eigenständig laufen kann, muss ihr Ordner um die Qt-Bibliotheken ergänzt werden.
Führen Sie in einem Terminal folgenden Befehl aus (passen Sie die Pfade vorher an):

```
C:\Qt\6.6.2\mingw_64\bin\windeployqt.exe "C:\Users\User\Desktop\build\release\polyphone.exe"
```

Die Qt-Bibliotheken erscheinen (zum Beispiel :file:`Qt6Core`), und ein Doppelklick auf die Datei :file:`polyphone.exe` startet die Software.
Damit Polyphone auch auf einem anderen Windows-Computer gestartet werden kann, werden zusätzlich folgende Dateien aus :file:`C:\msys64\mingw64\bin` benötigt, um den Ordner zu vervollständigen:

* libgcc_s_seh-1.dll
* libstdc++-6.dll
* libwinpthread-1.dll
* libssp-0.dll
* libsndfile-1.dll
* libFLAC.dll
* libogg-0.dll
* libvorbis-0.dll
* libvorbisenc-2.dll
* libvorbisfile-3.dll
* libopus-0.dll
* libmp3lame-0.dll
* libmpg123-0.dll
* libcrypto-3-x64.dll
* libssl-3-x64.dll
* zlib1.dll


## Fehlerbehebung


Diese [Kategorie](forum/support-bug-reports) des Forums hilft Ihnen bei Bedarf weiter.
