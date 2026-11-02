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

Diese Angaben sind in Spalten unter einer Kopfzeile ausgerichtet. Die Schaltfläche :guilabel:`Anzeige` rechts neben dem Textfeld öffnet ein Menü, in dem die angezeigten Spalten gewählt werden (alle außer dem Namen können ausgeblendet werden) und festgelegt wird, ob die :guilabel:`Preset-Übersicht` angezeigt wird, also die Namen der ersten Presets in einer zweiten Zeile. Ohne diese Übersicht belegt jede Datei nur eine Zeile, wie in einer Tabelle.

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

Ein Klick auf eine Spaltenüberschrift sortiert die Dateien nach Name, Größe, Datum oder Anzahl der Samples, Instrumente oder Presets, ein erneuter Klick kehrt die Reihenfolge um. Der erste Klick sortiert die Namen von A bis Z und die anderen Spalten beginnend mit dem größten Wert bzw. der neuesten Datei. Bei einer Sortierung nach einer Anzahl von Elementen stehen die Dateien, deren Inhalt unbekannt ist, immer am Ende der Liste.

Die Anzeigeoptionen, die Sortierung sowie die Breite und der auf- oder zugeklappte Zustand der Listen auf der rechten Seite bleiben für das nächste Mal erhalten.


## Dateioperationen {#doc_file_operations}


Beim Klick auf eine Datei in der Hauptliste erscheinen vor dem Dateinamen 2 anklickbare Symbole:

* ein Stiftsymbol zum Umbenennen der Datei (auch über :kbd:`F2` erreichbar),
* ein Papierkorbsymbol, um die Datei nach einer Bestätigung in den Papierkorb zu verschieben (auch über :kbd:`Del` erreichbar).

Mit diesen Operationen lässt sich eine lokale Soundfont-Sammlung schnell verwalten und aufräumen.


## Elemente abspielen {#doc_play}


Der Button ![piano](images/toolbar_piano.png "piano"){.inline} rechts neben dem Button :guilabel:`Anzeige` blendet die [virtuelle Tastatur](manual/soundfont-editor/toolbar.md#doc_keyboard) ein oder aus, und der Button ![Rekorder](images/toolbar_recorder.png "Rekorder"){.inline} daneben den [Rekorder](manual/soundfont-editor/toolbar.md#doc_recorder), wie die entsprechenden Buttons der Werkzeugleiste des Editors.

Das in den Listen auf der rechten Seite ausgewählte Sample, Instrument oder Preset kann mit der virtuellen Tastatur gespielt werden, oder mit einer externen MIDI-Tastatur, auch wenn die virtuelle Tastatur ausgeblendet ist. Wie im Editor zeigt die virtuelle Tastatur den Tonumfang des Elements bzw. die Basisnote eines Samples. Ein Sample wird im Loop abgespielt, wenn es einen Loop besitzt, und in Stereo, wenn die Option :guilabel:`Stereo` des [Players](manual/soundfont-editor/editing-pages/sample-editor.md#doc_player) für Samples aktiviert ist.

Wenn zum ersten Mal ein Element einer Datei ausgewählt wird, wird der Soundfont im Hintergrund geladen, ohne einen Tab zu öffnen. Der Soundfont bleibt dann geladen, solange Elemente derselben Datei ausgewählt werden, und wird freigegeben, sobald in der Hauptliste eine andere Datei ausgewählt wird.

Ein bereits in einem Tab geöffneter Soundfont wird so gespielt, wie er im Editor vorliegt, einschließlich seiner nicht gespeicherten Änderungen.


## Einen Soundfont öffnen {#doc_open_soundfont}


Ein Doppelklick auf einen Eintrag der Hauptliste öffnet die Datei – sofern ihr Format mit Polyphone kompatibel ist – an der Stelle, die in den [Einstellungen](manual/settings.md#doc_interface) mit der Option :guilabel:`Startansicht` festgelegt ist (standardmäßig werden die [allgemeinen Informationen](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md) angezeigt).

Ein Doppelklick auf ein [Sample](manual/soundfont-editor/editing-pages/sample-editor.md), ein [Instrument](manual/soundfont-editor/editing-pages/instrument-editor.md) oder ein [Preset](manual/soundfont-editor/editing-pages/preset-editor.md) öffnet die Datei beim ausgewählten Element.

Wurde der Soundfont bereits geladen, um eines seiner Elemente zu spielen, öffnet sich der Tab sofort.
