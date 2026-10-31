Das Menü in der oberen rechten Ecke der Software enthält mehrere Einträge für den Zugriff auf verschiedene Funktionen.


![Menü](images/menu.png "Menü")


## Einen Soundfont-Editor öffnen {#doc_open}


Neu
: Erstellt einen neuen, leeren Soundfont, in dem alles von Grund auf erstellt werden muss.
  Auch über die Tastenkombination :kbd:`Ctrl`+:kbd:`N` erreichbar.

Öffnen…
: Öffnet eine Datei im Format [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [.dls](manual/annexes/the-different-soundfont-formats.md#doc_dls), [.sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) oder [.organ](manual/annexes/the-different-soundfont-formats.md#doc_organ).
  Auch über die Tastenkombination :kbd:`Ctrl`+:kbd:`O` erreichbar.


## Einen Soundfont speichern {#doc_save}


Speichern
: Speichert die aktuelle Datei.
  Auch über die Tastenkombination :kbd:`Ctrl`+:kbd:`S` erreichbar.

Speichern unter…
: Speichert die aktuelle Datei in einer anderen Datei.
  Auch über die Tastenkombination :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`S` erreichbar.

Soundfonts exportieren…
: Führt ein oder mehrere Presets aus den geöffneten Soundfonts zusammen und exportiert sie im Format [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz) oder [.dls](manual/annexes/the-different-soundfont-formats.md#doc_dls). Das zusätzliche .csv-Format exportiert alle Parameter für eine detaillierte Analyse des Soundfont-Inhalts.
  Auch über die Tastenkombination :kbd:`Ctrl`+:kbd:`E` erreichbar.


![Exportfenster](images/export.png "Exportfenster")


Im Exportfenster werden die Presets aller geöffneten Soundfonts aufgelistet, und die zu exportierenden müssen angehakt werden (mit den Buttons :guilabel:`Alle auswählen` und :guilabel:`Alle abwählen` lässt sich die Auswahl schnell ändern).
Das Feld über diesen Buttons filtert die Liste nach den Nummern und Namen der Presets (Tastenkombination :kbd:`Ctrl`+:kbd:`F`, mit den Tasten :kbd:`↓` oder :kbd:`Enter` gelangt man anschließend zum ersten angezeigten Preset): Die beiden Buttons wirken dann nur auf die angezeigten Presets, und angehakte Presets, die durch den Filter ausgeblendet sind, werden trotzdem exportiert.
Mehrere Presets lassen sich auswählen, indem Sie die Taste :kbd:`Ctrl` gedrückt halten und sie nacheinander anklicken, oder mit der Taste :kbd:`Shift`, um alle Presets zwischen zwei Klicks auszuwählen. Mit der Taste :kbd:`Space` werden sie dann gemeinsam an- oder abgehakt.
Ein Doppelklick an beliebiger Stelle in der Zeile eines Presets hakt es ebenfalls an oder ab.
Unter der Liste wird die Anzahl der angehakten Presets angezeigt, zusammen mit der Anzahl der durch den Filter ausgeblendeten.
Die weiteren Parameter sind:

Ziel
: Das Verzeichnis, in das die Dateien geschrieben werden.
  Eine Datei wird nie überschrieben: Existiert bereits eine Datei mit demselben Namen, wird am Ende des Namens der neuen Datei eine Zahl angehängt.

Format
: Eines der Formate :guilabel:`.sf2`, :guilabel:`.sf3`, :guilabel:`.sfz`, :guilabel:`.csv` oder :guilabel:`.dls`.
  Stammen die Presets aus mehreren Soundfonts, werden sie zu einem einzigen Soundfont zusammengeführt, in dem jeder Quell-Soundfont zu einer Bank wird.
  Deshalb können höchstens 127 Soundfonts mit jeweils höchstens 127 ausgewählten Presets gleichzeitig exportiert werden.

Qualität
: Nur für das .sf3-Format: Qualität der Kompression der Samples.

Eine Datei pro Preset
: Nur für die Formate .sf2, .sf3 und .dls: Erstellung einer Datei pro ausgewähltem Preset statt einer einzigen Datei.

Ein Ordner pro Bank, Presetnummer als Präfix, General-MIDI-Klassifikation
: Nur für das .sfz-Format. Der Export erstellt ein Verzeichnis mit den sfz-Dateien und einem Unterverzeichnis :file:`samples`.
  Die sfz-Dateien können in einen Ordner pro Bank gelegt, ihrem Namen kann die Presetnummer vorangestellt werden, und sie können in Verzeichnisse einsortiert werden, die den General-MIDI-Kategorien entsprechen.

Rohwerte
: Nur für das .csv-Format. Die Werte werden so geschrieben, wie sie im sf2-Format gespeichert sind, statt in übliche Einheiten (Sekunden, Dezibel, Cent usw.) umgerechnet zu werden.
  Der Export erstellt ein Verzeichnis mit der Datei :file:`samples.csv` und einer Datei pro Instrument und pro Preset.


## Ansichtsoptionen {#doc_view}


Vollbild
: Schaltet den Vollbildmodus ein oder aus.
  Auch über die Tastenkombination :kbd:`F11` erreichbar.


## Verknüpfungen {#doc_shortcuts}


Einstellungen
: Öffnet die [Einstellungen](manual/settings.md) der Software.
  Auch über die Tastenkombination :kbd:`Ctrl`+:kbd:`,` erreichbar.

Online-Handbuch
: Zeigt dieses Benutzerhandbuch in einem Webbrowser an.
  Auch über die Tastenkombination :kbd:`F1` erreichbar.

Über Polyphone…
: Zeigt die Version der Software und das Copyright an. Der Button :guilabel:`Danksagung` zeigt die Liste der Personen an, die zum Projekt beigetragen haben.


## Schließen {#doc_close}


Tab schließen
: Schließt den aktuellen Tab (einen Soundfont-Editor, einen Soundfont-Explorer, die Einstellungen usw.).
  Auch über die Tastenkombination :kbd:`Ctrl`+:kbd:`W` erreichbar.

Programm beenden
: Beendet die Software.
  Auch über die Tastenkombination :kbd:`Ctrl`+:kbd:`Q` erreichbar.
