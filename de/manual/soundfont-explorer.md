Mit Polyphone kann über den Button :guilabel:`Verzeichnis öffnen` auf dem [Startbildschirm](manual/index.md#doc_left) ein Verzeichnis geöffnet werden. Der Soundfont-Explorer öffnet sich dann in einem neuen Tab und zeigt die Dateien dieses Verzeichnisses an (Unterverzeichnisse werden nicht durchsucht), wobei für jeden Soundfont eine Vorschau seines Inhalts angezeigt wird.


![Soundfont-Explorer](images/soundfont_explorer.png "Soundfont-Explorer")


## Soundfont-Vorschau {#doc_preview}


Die Hauptliste auf der linken Seite der Oberfläche enthält die Liste der Dateien, die Polyphone wie folgt unterscheidet:

* kompatible Dateien, die schnell durchsucht werden können (zum Beispiel in den Formaten [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz) und [.dls](manual/annexes/the-different-soundfont-formats.md#doc_dls)), werden mit Details zu ihrem Inhalt angezeigt,
* kompatible Dateien, die nicht schnell durchsucht werden können (insbesondere im Format [.sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark)), werden nur mit grundlegenden Informationen angezeigt,
* nicht kompatible Dateien sind ausgegraut und können nicht geöffnet werden.

Ein Klick auf eine durchsuchbare Datei zeigt in den Listen auf der rechten Seite der Oberfläche die darin enthaltenen Samples, Instrumente und Presets an. Jeder Eintrag in diesen Listen enthält außerdem Informationen, die helfen, den Detailgrad des Soundfonts und den in ihn investierten Aufwand einzuschätzen.

### Angaben zur Datei

* für jede Datei werden die Größe und das Datum der letzten Änderung angezeigt,
* bei Soundfonts, die durchsucht werden können, kommen folgende Informationen hinzu:
  * Anzahl der Samples, Instrumente und Presets,
  * Namen der ersten Presets.

### Informationen zu jedem Sample

* Typ (Mono / Stereo),
* Gesamtdauer,
* Dauer des Loops, falls vorhanden,
* Samplefrequenz.

### Informationen zu jedem Instrument oder Preset

* Gesamtzahl der Aufteilungen, Parameter und Generatoren,
* Anzahl der unterschiedlichen Tastenbereiche,
* Anzahl der unterschiedlichen Anschlagsbereiche.


## Sortier- und Suchfunktionen {#doc_filter}


Im oberen Bereich befindet sich ein Textfeld (die Tastenkombination :kbd:`Ctrl`+:kbd:`F` setzt den Fokus darauf). Wird es ausgefüllt, zeigt der Explorer nur die Dateien an, die mindestens eines der folgenden Kriterien erfüllen:

* der Name enthält den gesuchten Text,
* der Name eines der Samples, Instrumente oder Presets enthält den gesuchten Text.

Auch die Listen der Samples, Instrumente und Presets werden gefiltert, um die passenden Elemente hervorzuheben.

Rechts neben dem Textfeld befindet sich eine Dropdown-Liste, mit der die Dateien nach Name (A→Z), nach Datum der letzten Änderung (neueste zuerst) oder nach Dateigröße (größte oder kleinste zuerst) sortiert werden können.


## Dateioperationen {#doc_file_operations}


Beim Klick auf eine Datei in der Hauptliste erscheinen vor dem Dateinamen 2 anklickbare Symbole:

* ein Stiftsymbol zum Umbenennen der Datei (auch über :kbd:`F2` erreichbar),
* ein Papierkorbsymbol, um die Datei nach einer Bestätigung in den Papierkorb zu verschieben (auch über :kbd:`Del` erreichbar).

Mit diesen Operationen lässt sich eine lokale Soundfont-Sammlung schnell verwalten und aufräumen.


## Einen Soundfont öffnen {#doc_open_soundfont}


Ein Doppelklick auf einen Eintrag der Hauptliste öffnet die Datei – sofern ihr Format mit Polyphone kompatibel ist – an der Stelle, die in den [Einstellungen](manual/settings.md#doc_interface) mit der Option :guilabel:`Startansicht` festgelegt ist (standardmäßig werden die [allgemeinen Informationen](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md) angezeigt).

Ein Doppelklick auf ein [Sample](manual/soundfont-editor/editing-pages/sample-editor.md), ein [Instrument](manual/soundfont-editor/editing-pages/instrument-editor.md) oder ein [Preset](manual/soundfont-editor/editing-pages/preset-editor.md) öffnet die Datei beim ausgewählten Element.
