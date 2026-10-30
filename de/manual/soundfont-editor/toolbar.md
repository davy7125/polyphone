Die Werkzeugleiste oben im Editor enthält Symbole für:

* häufig verwendete [Bearbeitungsfunktionen](#doc_edit),
* das Umschalten der [Ansicht](#doc_view) in den Editoren,
* das Anzeigen des [Rekorders](#doc_recorder),
* das Anzeigen der [virtuellen Tastatur](#doc_keyboard).


![Werkzeugleiste](images/toolbar.png "Werkzeugleiste")


## Bearbeitungsfunktionen {#doc_edit}


Die Elemente im linken Teil der Werkzeugleiste dienen häufig verwendeten Bearbeitungsfunktionen:

![neues Sample](images/toolbar_sample.png "neues Sample"){.inline} Sample hinzufügen
: Fügt dem Soundfont neue [Samples](manual/soundfont-editor/editing-pages/sample-editor.md) hinzu.
  Es erscheint ein Dialog zur Auswahl von Audiodateien (Dateien im Format :file:`.wav`, :file:`.flac`, :file:`.mp3`, :file:`.ogg`, :file:`.aif`, :file:`.aiff`, :file:`.aifc` und :file:`.snd` können importiert werden), wobei mehrere Dateien auf einmal ausgewählt werden können.

![neues Instrument](images/toolbar_instrument.png "neues Instrument"){.inline} Instrument hinzufügen
: Erstellt ein neues [Instrument](manual/soundfont-editor/editing-pages/instrument-editor.md).
  Ein Dialog fragt nach dem Namen des Instruments und bietet, falls in der [Baumansicht](manual/soundfont-editor/tree.md) Samples ausgewählt sind, an, diese mit dem neuen Instrument zu verknüpfen.
  In diesem Fall werden Stereo-Samples nach links und rechts gepannt, der Loop wird aktiviert, wenn alle Samples einen Loop haben, und wenn die Samples unterschiedliche Basisnoten haben, werden die Tastenbereiche automatisch festgelegt (siehe das Werkzeug [Sample-Autopositionierung](manual/soundfont-editor/tools/instrument-tools.md#doc_position)).

![neues Preset](images/toolbar_preset.png "neues Preset"){.inline} Preset hinzufügen
: Erstellt ein neues [Preset](manual/soundfont-editor/editing-pages/preset-editor.md).
  Ein Dialog fragt nach dem Namen des Presets und bietet, falls in der Baumansicht Instrumente ausgewählt sind, an, diese mit dem neuen Preset zu verknüpfen.
  Es werden die ersten verfügbaren Bank- und Presetnummern verwendet.

![Werkzeuge](images/toolbar_toolbox.png "Werkzeuge"){.inline} Werkzeuge
: Ein Klick auf diesen Button zeigt ein Menü mit allen verfügbaren [Werkzeugen](manual/soundfont-editor/tools/index.md) für das bzw. die in der [Baumansicht](manual/soundfont-editor/tree.md) ausgewählten Elemente.

![Rückgängig](images/toolbar_undo.png "Rückgängig"){.inline} Rückgängig
: Macht die letzte Änderung rückgängig, auch über :kbd:`Ctrl`+:kbd:`Z` erreichbar.

![Wiederherstellen](images/toolbar_redo.png "Wiederherstellen"){.inline} Wiederherstellen
: Stellt die zuletzt rückgängig gemachte Änderung wieder her, auch über :kbd:`Ctrl`+:kbd:`Y` oder :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`Z` erreichbar.

![Speichern](images/toolbar_save.png "Speichern"){.inline} Speichern
: Speichert den Soundfont, auch über :kbd:`Ctrl`+:kbd:`S` erreichbar.


## Ansicht wechseln {#doc_view}


Im rechten Teil der Werkzeugleiste können je nach angezeigtem [Editor](manual/soundfont-editor/editing-pages/index.md) einige Symbole erscheinen:

* beim Bearbeiten eines Instruments kann zwischen der ![Tabelle](images/toolbar_table.png "Tabelle"){.inline} [Tabellenansicht](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table), dem ![Bereiche](images/toolbar_range.png "Bereiche"){.inline} [Bereichseditor](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range) und dem ![Hüllkurven](images/toolbar_adsr.png "Hüllkurven"){.inline} [Hüllkurveneditor](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope) gewechselt werden,
* beim Bearbeiten eines Presets kann zwischen der ![Tabelle](images/toolbar_table.png "Tabelle"){.inline} Tabellenansicht und dem ![Bereiche](images/toolbar_range.png "Bereiche"){.inline} Bereichseditor gewechselt werden,
* andernfalls wird nichts angezeigt.

Die gewählte Ansicht bleibt erhalten, wenn ein anderes Instrument oder Preset ausgewählt wird. Die Ansicht, die beim Öffnen eines Soundfonts für das erste Instrument oder Preset angezeigt wird, kann mit der Option :guilabel:`Startansicht` in den [Einstellungen](manual/settings.md#doc_interface) gewählt werden.


## Rekorder {#doc_recorder}


Ein Klick auf das Symbol ![Rekorder](images/toolbar_recorder.png "Rekorder"){.inline} öffnet ein Fenster mit dem Rekorder, der die Ausgabe von Polyphone in einer :file:`.wav`-Datei aufnehmen kann. Die verstrichene Zeit wird im Fenster angezeigt.


![Rekorder](images/recorder.png "Rekorder")


Der linke Button kann zwei Zustände haben:

Aufnehmen
: Startet eine neue Aufnahme, wenn gerade nicht aufgenommen wird.
  Ein Dialog fragt nach dem Namen der Datei, die das aufgenommene Audiosignal enthalten soll.

Stopp
: Beendet die laufende Aufnahme.
  Die Aufnahme wird auch beendet, wenn das Fenster geschlossen wird.

Der rechte Button kann ebenfalls zwei Zustände haben (nur verfügbar, während aufgenommen wird):

Pause
: Unterbricht die laufende Aufnahme.

Fortsetzen
: Setzt die laufende Aufnahme fort.


## Virtuelle Tastatur {#doc_keyboard}


Ein Klick auf das Symbol ![Klavier](images/toolbar_piano.png "Klavier"){.inline} öffnet ein Fenster mit der virtuellen Tastatur, die zum Testen von Samples, Instrumenten und Presets gedacht ist.
Dieses Fenster sendet MIDI-Signale:

* **Note On** / **Note Off** / **Aftertouch** mit der Klaviatur,
* weitere MIDI-Signale, wenn der **Bereich der MIDI-Controller** ausgeklappt ist.

Taste, Anschlag und Aftertouch der gespielten Note werden am unteren Rand des Fensters angezeigt, zusammen mit einer Dropdown-Liste zur Auswahl der Tastaturgröße (5 Oktaven, 6 Oktaven, 88 Tasten oder 128 Tasten).


![Virtuelle Tastatur](images/virtual_keyboard.png "Virtuelle Tastatur")


### Klaviatur


Die Klaviatur sendet Signale, die Tasten- und Anschlagswerte enthalten.
Sie kann auf drei Arten gesteuert werden:

mit der Maus
: Die gewünschte Taste kann mit der Maus angeklickt werden.
  Die Anschlagsstärke richtet sich nach der vertikalen Position des Klicks.
  Nach einem Klick erzeugen weitere Bewegungen innerhalb derselben Taste Aftertouch-Signale.

mit der Computertastatur
: Die verwendeten Tasten werden in den [Einstellungen](manual/settings.md#doc_keyboard) festgelegt.

mit einer externen MIDI-Tastatur
: Der MIDI-Eingang wird in den [Einstellungen](manual/settings.md#doc_general) festgelegt.
  Ist die Klaviatur sichtbar, zeigt sie an, was Sie gerade spielen.


#### Markierungen und Farben


* Wenn ein [Instrument](manual/soundfont-editor/editing-pages/instrument-editor.md) oder ein [Preset](manual/soundfont-editor/editing-pages/preset-editor.md) ausgewählt ist, wird der nicht von den Aufteilungen abgedeckte Bereich **grau** dargestellt.
  So lässt sich der gesamte Tonumfang eines Instruments oder eines Presets erkennen.
* Wenn eine Aufteilung eines Instruments oder eines Presets ausgewählt ist, wird ihr Bereich auf die gleiche Weise angezeigt.
  Außerdem wird ihre Basisnote durch eine **orangefarbene Markierung** gekennzeichnet.
* Wenn auf der Tastatur eine Taste ausgewählt ist, erscheint sie **blau** (bzw. in der Auswahlfarbe, die in den [Einstellungen](manual/settings.md#doc_interface) der Software gewählt wurde).
  Außerdem erscheint der Bereich der zugehörigen Aufteilung **hellblau**.
  Dies gilt nur auf Instrumentebene.
* Das mittlere C (Taste 60) ist durch eine **schwarze Markierung** gekennzeichnet.


#### Nützliche Tastenkombinationen


:kbd:`Ctrl`+:kbd:`K`
: Diese Tastenkombination setzt jederzeit den Fokus auf die virtuelle Tastatur (die Tastatur wird angezeigt, falls sie ausgeblendet war).
  So kann man zum Beispiel beim Bearbeiten einer Tabelle mit der Computertastatur spielen, anstatt eine Zelle zu bearbeiten.
  Ein Leuchteffekt auf der virtuellen Tastatur bestätigt, dass sie den Fokus hat.

:kbd:`Ctrl`+:kbd:`1`, :kbd:`Ctrl`+:kbd:`2`, … :kbd:`Ctrl`+:kbd:`8`
: Diese Tastenkombinationen ändern die Oktaven, die mit der Computertastatur gespielt werden.
  Nach :kbd:`Ctrl`+:kbd:`1` können die tiefsten Oktaven gespielt werden.
  Die höchsten Oktaven erreicht man mit :kbd:`Ctrl`+:kbd:`8`.

:kbd:`→` / :kbd:`←`
: Der Pfeil nach rechts bzw. links spielt die Tonhöhe direkt über bzw. unter der zuletzt gespielten Taste, mit derselben Anschlagsstärke.
  Zusammen mit der Taste :kbd:`Shift` werden 6 Tasten statt einer übersprungen.

:kbd:`↑` / :kbd:`↓`
: Der Pfeil nach oben bzw. unten spielt dieselbe Tonhöhe wie die zuletzt gespielte Taste, aber mit stärkerem bzw. schwächerem Anschlag.
  Zusammen mit der Taste :kbd:`Shift` ändert sich die Anschlagsstärke um 10 statt um 1.


### MIDI-Controller


Mit einem Klick auf den Pfeil unten rechts kann der Bereich der MIDI-Controller ein- oder ausgeblendet werden.
Wenn Sie mit Modulatoren arbeiten, empfiehlt es sich, diese Funktionen zum Testen Ihres Soundfonts zu verwenden.


![Virtuelle Tastatur mit MIDI-Controllern](images/virtual_keyboard_2.png "Virtuelle Tastatur mit MIDI-Controllern")


Dieser Bereich besteht aus verschiedenen Bedienelementen:

Pitch-Rad
: Das Pitch-Rad erhöht oder senkt die Tonhöhe.
  Es kehrt automatisch in die Ausgangsposition 0 zurück.

Empfindlichkeit
: Die Empfindlichkeit legt den Änderungsbereich des Pitch-Rads in Halbtönen fest.
  Standardmäßig beträgt sie 2 (ein Ganzton).

Druck
: Der Druck ist die Stärke, mit der auf die Tastatur gedrückt wird.
  Er ist mit dem Aftertouch vergleichbar; während der Aftertouch jedoch tastenspezifisch ist, ist der Druck ein globales Signal für alle Tasten.

Drehregler
: Mit 3 Drehreglern können Werte von MIDI-Controllern geändert werden.
  Für jeden Regler können Sie eine Controllernummer und dann deren Wert festlegen.

Ein/Aus-LED
: Diese LED setzt den Wert eines MIDI-Controllers auf 0 oder 127 (zum Beispiel nützlich für das Sustain-Pedal).
  Sie funktioniert wie ein Drehregler, jedoch mit nur zwei möglichen Werten.

Alle diese Elemente können mit einer an den Computer angeschlossenen MIDI-Tastatur synchronisiert werden, wenn der MIDI-Eingang in den [Einstellungen](manual/settings.md#doc_general) korrekt festgelegt ist. Relative Werte von MIDI-Controllern werden zur Steuerung der Drehregler unterstützt.

*[MIDI]: Musical Instrument Digital Interface
