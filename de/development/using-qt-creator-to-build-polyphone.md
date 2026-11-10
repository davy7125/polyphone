Diese Anleitung zeigt, wie man Polyphone mit Qt Creator kompiliert.
Es wird vorausgesetzt, dass auf Ihrem Computer bereits Qt Creator mit dem Qt-Framework, ein Compiler und die zum Kompilieren von Polyphone nötigen Abhängigkeiten installiert sind (siehe die entsprechenden Seiten).


## Voraussetzungen


Folgende Elemente werden benötigt:

* ein C++-Compiler (GCC, Clang, MinGW…),
* Qt Creator mit seinem Framework (Qt 6 oder Qt 5),
* :program:`CMake` (Version 3.16 oder höher), das mit Qt Creator installiert werden kann,
* die Abhängigkeiten von Polyphone.

Die Installationsanleitung hängt vom verwendeten Betriebssystem ab; siehe dazu die entsprechenden Seiten der Dokumentation.


## Polyphone kompilieren


### Die Quellen kopieren


Besorgen Sie sich die Quellen von Polyphone <a href="software" target="_blank">hier</a> oder von <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


### Das Projekt öffnen und konfigurieren


Starten Sie Qt Creator und öffnen Sie die Datei :file:`CMakeLists.txt` im Verzeichnis :file:`sources`.


![Qt Creator öffnen](images/open-Qt-Creator.png "Qt Creator öffnen")


Konfigurieren Sie dann das Projekt, indem Sie ein Kit wählen, also eine Qt-Version und einen Compiler.

![Projekt konfigurieren](images/configure-project.png "Projekt konfigurieren")


Qt Creator führt dann :program:`CMake` aus, das prüft, ob die Abhängigkeiten gefunden werden.
Die Meldungen werden im Bereich :guilabel:`General Messages` angezeigt: Fehlt eine Bibliothek, installieren Sie sie und führen Sie :program:`CMake` mit dem Eintrag :guilabel:`Run CMake` des Menüs :guilabel:`Build` erneut aus.

Die Optionen des Projekts befinden sich im Bereich :guilabel:`Projects` der linken Spalte unter :guilabel:`Build` → :guilabel:`CMake`.
Unter Linux können statt der Bibliotheken des Systems die in den Quellen enthaltenen Kopien von :program:`RtAudio` und :program:`RtMidi` verwendet werden: Aktivieren Sie die Optionen ``POLYPHONE_LOCAL_RTAUDIO`` und ``POLYPHONE_LOCAL_RTMIDI`` und klicken Sie dann auf :guilabel:`Run CMake`.
Unter Windows und macOS ist dies nicht nötig, da dort immer die mitgelieferten Versionen verwendet werden.
Wählen Sie im selben Bereich die Build-Konfiguration aus (:guilabel:`Release` wird empfohlen).


### Das Projekt kompilieren


Klicken Sie schließlich im Menü :guilabel:`Build` auf die Aktion :guilabel:`Build Project "Polyphone"`. Während des Kompilierens können Warnungen erscheinen, es sollten aber keine Fehler auftreten.

Nun sollte sich die ausführbare Datei im Unterverzeichnis :file:`bin` des von Qt Creator erstellten Build-Verzeichnisses befinden (zum Beispiel :file:`build/Desktop_Qt_6_…-Release` im Verzeichnis :file:`sources`):

* unter Linux die Datei :file:`polyphone`,
* unter Windows die Datei :file:`polyphone.exe`,
* unter macOS das Bundle :file:`Polyphone.app`.


## Fehlerbehebung


In dieser [Kategorie](forum/support-bug-reports) finden Sie weitere Informationen oder Hilfe.


<p class="endpage">Besonderer Dank an <a href="dashboard/820-tomotello">Tomotello</a> für diesen Artikel.</p>
