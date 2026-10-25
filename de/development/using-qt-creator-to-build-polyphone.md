Diese Anleitung zeigt, wie man Polyphone mit Qt Creator kompiliert.
Es wird vorausgesetzt, dass auf Ihrem Computer bereits Qt Creator mit dem Qt-Framework, ein Compiler und die zum Kompilieren von Polyphone nötigen Abhängigkeiten installiert sind (siehe die entsprechenden Seiten).


## Voraussetzungen


Folgende Elemente werden benötigt:

* ein C++-Compiler (VC++, Clang, GCC, …),
* Qt Creator mit seinem Framework (Qt 6),
* die Abhängigkeiten von Polyphone.

Die Installationsanleitung hängt vom verwendeten Betriebssystem ab; siehe dazu die entsprechenden Seiten der Dokumentation.


## Polyphone kompilieren


### Die Quellen kopieren


Besorgen Sie sich die Quellen von Polyphone <a href="software" target="_blank">hier</a> oder von <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


### Das Projekt öffnen und konfigurieren


Öffnen Sie zunächst die Datei :file:`polyphone.pro` und bearbeiten Sie sie, um gegebenenfalls die mitgelieferten Versionen von :program:`rtmidi` und :program:`stk` zu verwenden, falls Ihr Betriebssystem sie nicht bereitstellt (die mitgelieferte Version von :program:`rtaudio` wird standardmäßig verwendet).
Entfernen Sie dazu das vorangestellte „#“ in den folgenden Zeilen:

```
DEFINES += USE_LOCAL_RTMIDI
DEFINES += USE_LOCAL_STK
```

Diese Änderungen können Sie überspringen, wenn Sie bereits auf Ihrem Computer installierte Bibliotheken verwenden möchten (stellen Sie sicher, dass sie vorhanden sind!).
Unter Windows und macOS ist dieser Schritt nicht nötig, da dort immer die mitgelieferten Versionen verwendet werden.

Starten Sie Qt Creator und öffnen Sie die Datei :file:`polyphone.pro`.


![Qt Creator öffnen](images/open-Qt-Creator.png "Qt Creator öffnen")


Konfigurieren Sie dann das Projekt.

![Projekt konfigurieren](images/configure-project.png "Projekt konfigurieren")


### qmake ausführen und das Projekt kompilieren


Öffnen Sie das Menü :guilabel:`Build` und wählen Sie den Eintrag :guilabel:`Run qmake`.


![qmake ausführen](images/run-qmake.png "qmake ausführen")


Die Ausführung von :command:`qmake` ist abgeschlossen, wenn der Fortschrittsbalken unten rechts grün 100 % erreicht hat.


![qmake abgeschlossen](images/qmake-finished.png "qmake abgeschlossen")


Wählen Sie im Bereich :guilabel:`Project` in der linken Spalte und dann im Bereich :guilabel:`Build&Run` die Build-Konfiguration aus (:guilabel:`Release` wird empfohlen).

Klicken Sie schließlich im Menü :guilabel:`Build` auf die Aktion :guilabel:`Build project Polyphone`. Während des Kompilierens können Warnungen erscheinen, es sollten aber keine Fehler auftreten.

Nun sollte sich die ausführbare Datei in dem von Qt Creator erstellten Build-Verzeichnis befinden (zum Beispiel :file:`build-XXX` neben den Quellen), je nach Ihrer Wahl in der Build-Konfiguration von Qt Creator:

* unter Linux befindet sich die Datei :file:`polyphone` im Unterverzeichnis :file:`bin`,
* unter Windows befindet sich die Datei :file:`polyphone.exe` im Unterverzeichnis :file:`release` oder :file:`debug`,
* unter macOS befindet sich das Bundle :file:`Polyphone.app` im Verzeichnis :file:`macos` neben dem Verzeichnis :file:`sources`.


## Fehlerbehebung


In dieser [Kategorie](forum/support-bug-reports) finden Sie weitere Informationen oder Hilfe.


<p class="endpage">Besonderer Dank an <a href="dashboard/820-tomotello">Tomotello</a> für diesen Artikel.</p>
