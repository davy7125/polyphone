Die Baumansicht ist das erste Element des Editors, mit dem Sie sich beschäftigen sollten.
Sie stellt den Aufbau des geöffneten Soundfonts dar und steuert den rechten Teil der Software, wenn Sie ein oder mehrere Elemente auswählen.

Die Baumansicht wird immer mit derselben [Struktur](#doc_structure) angezeigt, bietet ein [Kontextmenü](#doc_menu) und verfügt über die Funktionen [Drag & Drop](#doc_dragdrop), [Kopieren / Einfügen](#doc_copypaste) und [Suche](#doc_search).


## Struktur {#doc_structure}


Die Baumansicht besteht aus 4 Überschriften, wobei die Überschriften :guilabel:`Samples`, :guilabel:`Instrumente` und :guilabel:`Presets` Unterelemente enthalten.


![Baumansicht: Struktur](images/tree_1.png "Baumansicht: Struktur")


### Überschrift „Allgemein“


Ein Klick auf die Überschrift :guilabel:`Allgemein` öffnet den Editor für die [allgemeinen Informationen](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md).


### Bereich „Samples“


Dieser Bereich listet alle im Soundfont enthaltenen Samples auf; sie sind das Rohmaterial, auf dem alle Instrumente aufbauen.
Die Auswahl eines oder mehrerer Samples führt zum [Sample-Editor](manual/soundfont-editor/editing-pages/sample-editor.md).
Ein Klick auf die Überschrift :guilabel:`Samples` zeigt die [Konfigurationsübersicht](manual/soundfont-editor/configuration-summaries.md#doc_sample) aller Samples an.


### Bereich „Instrumente“


Dieser Bereich listet alle im Soundfont enthaltenen Instrumente auf. Ein Instrument ist ein Zwischenelement, das aus Samples besteht und von Presets verwendet wird.
Instrumente enthalten Verknüpfungen zu Samples, die „Aufteilungen“ genannt und als Liste unter jedem Instrument angezeigt werden.
Die Auswahl eines Instruments oder einer seiner Aufteilungen führt zum [Instrument-Editor](manual/soundfont-editor/editing-pages/instrument-editor.md).
Ein Klick auf die Überschrift :guilabel:`Instrumente` zeigt die [Konfigurationsübersicht](manual/soundfont-editor/configuration-summaries.md#doc_instrument) aller Instrumente an.


### Bereich „Presets“


Dieser Bereich listet alle im Soundfont enthaltenen Presets auf. Ein Preset ist ein Endelement, das aus Instrumenten besteht und von außerhalb des Soundfonts zugänglich ist.
Presets enthalten Verknüpfungen zu Instrumenten, die „Aufteilungen“ genannt und als Liste unter jedem Preset angezeigt werden.
Die Auswahl eines Presets oder einer seiner Aufteilungen zeigt den [Preset-Editor](manual/soundfont-editor/editing-pages/preset-editor.md) an.
Ein Klick auf die Überschrift :guilabel:`Presets` zeigt die [Konfigurationsübersicht](manual/soundfont-editor/configuration-summaries.md#doc_preset) aller Presets an.


## Kontextmenü {#doc_menu}


Ein Rechtsklick auf ein Element zeigt ein Menü mit den folgenden Aktionen:

Verknüpfen mit…
: Verknüpft das bzw. die ausgewählten Samples mit einem Instrument oder das bzw. die ausgewählten Instrumente mit einem Preset.
  Das Ziel wird in einer filterbaren Liste ausgewählt.
  Werden Samples mit unterschiedlichen Basisnoten mit einem leeren Instrument verknüpft, werden ihre Tastenbereiche automatisch festgelegt (wie mit dem Werkzeug [Sample-Autopositionierung](manual/soundfont-editor/tools/instrument-tools.md#doc_position)).

Ersetzen durch…
: Ersetzt in einer Instrument-Aufteilung ein Sample durch ein anderes Sample oder in einer Preset-Aufteilung ein Instrument durch ein anderes Instrument.
  Die Konfiguration der Aufteilung bleibt erhalten.

Kopieren
: Kopiert das bzw. die ausgewählten Elemente, auch über :kbd:`Ctrl`+:kbd:`C` erreichbar.

Einfügen
: Fügt das bzw. die zuvor kopierten Elemente ein, auch über :kbd:`Ctrl`+:kbd:`V` erreichbar.

Duplizieren
: Dupliziert das bzw. die ausgewählten Elemente; dies können Samples, Instrumente, Presets oder Aufteilungen sein.
  Auch über :kbd:`Ctrl`+:kbd:`D` erreichbar.

Löschen
: Löscht das bzw. die ausgewählten Elemente, auch über :kbd:`Del` erreichbar.
  Ein Sample, das von einem Instrument verwendet wird, und ein Instrument, das von einem Preset verwendet wird, können nicht gelöscht werden.

Stummschalten
: Zu Testzwecken kann eine Aufteilung innerhalb eines Instruments oder eines Presets stummgeschaltet werden. Diese Option ist auch im Kontextmenü des [Tabellenkopfs](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_solo) zu finden.

Immer spielen
: Im Gegensatz zur Funktion „Stummschalten“ können beim Spielen ein oder mehrere Instrumente oder Presets hinzugefügt werden. So lässt sich eine gute Balance zwischen verschiedenen Instrumenten oder Presets finden, die zusammen erklingen.

Wiedergabe wiederherstellen
: Hebt die Wirkung von „Stummschalten“ und „Immer spielen“ im gesamten Soundfont auf.

Umbenennen oder Mehrfach umbenennen
: Benennt das bzw. die ausgewählten Elemente um, auch über :kbd:`F2` erreichbar.
  Das Umbenennen einer Aufteilung benennt das Sample bzw. das Instrument um, mit dem sie verknüpft ist.
  Sind mehrere Elemente ausgewählt, bietet ein Dialog verschiedene Optionen an:
  
  * vorhandenen Namen mit Tastennamen als Suffix überschreiben (nur für Samples und Instrument-Aufteilungen),
  * vorhandenen Namen mit Tastennamen und Anschlag als Suffix überschreiben (nur für Instrument-Aufteilungen),
  * vorhandenen Namen mit numerisch aufsteigendem Suffix überschreiben,
  * Zeichen ersetzen,
  * Text an einer bestimmten Position einfügen,
  * Zeichenbereich löschen.


![Baumansicht: Kontextmenü](images/tree_2.png "Baumansicht: Kontextmenü")


## Drag & Drop {#doc_dragdrop}


Mit „Drag & Drop“ in der Baumansicht lassen sich schnell Zuordnungen oder Kopien erstellen.
Vor einem Drag & Drop müssen Elemente ausgewählt werden.
Dies kann ein einzelnes Element oder eine Mehrfachauswahl sein, die mit den Tasten :kbd:`Ctrl` und :kbd:`Shift` möglich ist.

Das Ergebnis eines Drag & Drop hängt dann von der Art der gezogenen Elemente und von der Art des Zielelements ab.

Sample → Instrument
: Zuordnung des Samples zum Instrument, Erstellung einer Instrument-Aufteilung.

Sample → Überschrift :guilabel:`Instrumente`
: Erstellung eines Instruments, das alle gezogenen Samples enthält, oder Erstellung eines Instruments pro gezogenem Sample.

Instrument → Preset
: Zuordnung des Instruments zum Preset, Erstellung einer Preset-Aufteilung.

Instrument → Instrument
: Kopie aller Aufteilungen des Quellinstruments in das Zielinstrument (es kann sich um dasselbe Instrument handeln).

Instrument-Aufteilung → Instrument oder Instrument-Aufteilung
: Kopie der ausgewählten Aufteilungen des Quellinstruments in das Zielinstrument (es kann sich um dasselbe Instrument handeln).

Instrument → Überschrift :guilabel:`Presets`
: Erstellung eines Presets, das alle gezogenen Instrumente enthält, oder Erstellung eines Presets pro gezogenem Instrument.

Preset → Preset
: Kopie aller Aufteilungen des Quellpresets in das Zielpreset (es kann sich um dasselbe Preset handeln).

Preset-Aufteilung → Preset oder Preset-Aufteilung
: Kopie der ausgewählten Aufteilungen des Quellpresets in das Zielpreset (es kann sich um dasselbe Preset handeln).

Audiodatei (aus dem Dateimanager des Computers) → Baumansicht
: Import der Datei als neues Sample, sofern ihr Format unterstützt wird (siehe [Sample-Editor](manual/soundfont-editor/editing-pages/sample-editor.md)).


## Kopieren / Einfügen {#doc_copypaste}


Alles, was wie oben beschrieben per Drag & Drop möglich ist, ist auch mit Kopieren / Einfügen möglich – über das Kontextmenü oder mit den Tastenkombinationen :kbd:`Ctrl`+:kbd:`C` und :kbd:`Ctrl`+:kbd:`V`.

Kopieren / Einfügen ist auch zwischen verschiedenen Soundfonts möglich.
Man kann ein Element in einem ersten Soundfont kopieren und es in einem zweiten Soundfont einfügen.
Eine Mehrfachauswahl ist erlaubt.

Ausgewählte Samples
: Kopie der Samples.

Ausgewählte Instrumente
: Kopie der Instrumente einschließlich aller verknüpften Samples.

Ausgewählte Presets
: Kopie der Presets einschließlich aller verknüpften Instrumente und Samples.

Wenn beim Kopieren von Elementen zwischen verschiedenen Soundfonts bereits ein Element mit demselben Namen vorhanden ist, stehen mehrere Optionen zur Verfügung:

* den Vorgang ignorieren; in diesem Fall wird das Kopieren für das Element mit demselben Namen abgebrochen;
* das vorhandene Element ersetzen; in diesem Fall wird das vorhandene Element durch das kopierte Element überschrieben;
* das Element duplizieren; in diesem Fall wird das Element unter einem anderen Namen neben das vorhandene Element kopiert.


## Suche {#doc_search}


Um das Auffinden von Elementen in der Baumansicht zu erleichtern, steht eine Suchleiste zum Filtern zur Verfügung (die Tastenkombination :kbd:`Ctrl`+:kbd:`F` setzt den Fokus darauf).


![Baumansicht: Suchleiste](images/tree_3.png "Baumansicht: Suchleiste")


Wenn in die Suchleiste ein Text eingegeben wird, werden die Elemente angezeigt, die ihn enthalten.
Außerdem gilt:

* Ist der Text im Namen eines Samples enthalten, werden die Instrumente, die dieses Sample verwenden, und die Presets, die diese Instrumente verwenden, angezeigt;
* ist der Text im Namen eines Instruments enthalten, werden die vom Instrument verwendeten Samples und die Presets, die das Instrument verwenden, angezeigt;
* ist der Text im Namen eines Presets enthalten, werden die vom Preset verwendeten Instrumente und die von diesen Instrumenten verwendeten Samples angezeigt.

Ein Klick auf das Kreuz rechts neben dem Eingabefeld hebt den Filter auf.
