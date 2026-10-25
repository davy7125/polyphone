Verwenden Sie diese Anleitung, wenn Sie Polyphone auf Ihrem Mac nicht mit den im Bereich „[Software](software)“ verfügbaren Installationsprogrammen installieren können.


## Voraussetzungen


Folgende Anwendungen werden benötigt:

* <a href="https://www.qt.io/download-open-source" target="_blank">Qt Creator mit seinem Framework</a> (Version 6),
* Xcode oder nur die „Command Line Tools“ (die zu verwendende Version ist auf der Qt-Website angegeben – zum Beispiel <a href="https://doc.qt.io/qt-6/supported-platforms.html" target="_blank">hier</a>).

Außerdem benötigen Sie die Quellen von Polyphone, die <a href="software" target="_blank">hier</a> oder auf <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> verfügbar sind.

Damit Qt Creator ein Projekt korrekt öffnen kann, muss ihm zunächst die Berechtigung erteilt werden, alle Dateien auf Ihrem Computer zu lesen (unter „Datenschutz & Sicherheit“ => „Festplattenvollzugriff“ => „Qt Creator.app“ hinzufügen, das sich in seinem Installationsverzeichnis befindet).

Die von Polyphone verwendeten Bibliotheken lassen sich einfach mit <a href="https://brew.sh/" target="_blank">Homebrew</a> installieren. Führen Sie nach der Installation dieses Werkzeugs folgenden Befehl aus:

```
brew install libsndfile libogg libvorbis flac lame mpg123 opus openssl
```

Die Projektdatei :file:`polyphone.pro` sucht die Header und Bibliotheken in :file:`/opt/homebrew` (Apple Silicon), in :file:`/usr/local` (Intel) und im Verzeichnis :file:`macos`, das sich neben dem Verzeichnis :file:`sources` befindet.
Das [hier](files/macos.zip) verfügbare Archiv mit statischen Bibliotheken kann ebenfalls entpackt werden, um das Verzeichnis :file:`macos` zu erhalten, sein Inhalt ist aber möglicherweise veraltet.

Sind einige Bibliotheken nicht aktuell oder nicht mit der Version Ihres Macs kompatibel, wie zum Beispiel SSL, müssen Sie ihre Quellen im Internet suchen und sie mit aktivierter statischer Verlinkung neu kompilieren:

```
configure --enable-static --disable-shared --prefix=...
make
make install
```

Kopieren Sie dann alle .h-Dateien (Header) nach :file:`macos/include` und alle .a-Dateien (statische Bibliotheken) nach :file:`macos/lib`.


## Kompilieren


Öffnen Sie die Datei :file:`polyphone.pro` mit :program:`Qt Creator` (siehe [Polyphone mit Qt Creator kompilieren](development/using-qt-creator-to-build-polyphone.md)) und kompilieren Sie das Projekt.
Im Verzeichnis :file:`macos` neben dem Verzeichnis :file:`sources` sollte ein Bundle :file:`Polyphone.app` erscheinen.

Das Kompilieren kann auch über die Kommandozeile erfolgen:

```
cd sources
qmake polyphone.pro CONFIG+=release
make -j4
```

Beschwert sich der Compiler, dass einige Funktionen mit dem angegebenen macOS-Ziel nicht verfügbar sind, können Sie diese Variable in der Datei :file:`polyphone.pro` erhöhen (der Standardwert ist 12.0):

```
QMAKE_MACOSX_DEPLOYMENT_TARGET = 12.0
```

Dieser Wert muss auch in der Datei :file:`packaging/mac/polyphone.plist` unterhalb von „LSMinimumSystemVersion“ eingetragen werden.


## Das Bundle vervollständigen


Führen Sie folgenden Befehl aus, um Bibliotheken und Frameworks in das Bundle aufzunehmen (passen Sie zuerst den Pfad an!):

```
/Path/To/Qt/6.7.3/macos/bin/macdeployqt /Path/To/macos/Polyphone.app
```

Auf einem Mac mit Apple-Silicon-Prozessor muss das Bundle außerdem signiert werden (selbst mit einer Ad-hoc-Signatur), damit es ausgeführt werden kann:

```
xattr -cr /Path/To/macos/Polyphone.app
codesign --force --deep --sign - /Path/To/macos/Polyphone.app
```

Anschließend können Sie das Programm ausführen oder es als .zip- oder .dmg-Datei komprimieren, um es weiterzugeben.


## Fehlerbehebung


In diesem [Thema](forum/support-bug-reports/8-success-build-polyphone-on-osx-10-11-6-qt-5-7) finden Sie weitere Informationen oder Hilfe.
