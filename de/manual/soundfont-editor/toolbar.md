Die Werkzeugleiste oben im Editor enthält Symbole für:

* häufig verwendete [Bearbeitungsfunktionen](#doc_edit),
* das Umschalten der [Ansicht](#doc_view) in den Editoren,
* das Anzeigen des [Rekorders](#doc_recorder),
* das Anzeigen der [virtuellen Tastatur](#doc_keyboard),
* das Anzeigen des [MIDI-Players](#doc_midi_player).


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

* beim Bearbeiten eines Instruments kann zwischen der ![Tabelle](images/toolbar_table.png "Tabelle"){.inline} [Tabellenansicht](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table), dem ![Bereiche](images/toolbar_range.png "Bereiche"){.inline} [Bereichseditor](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range), dem ![Hüllkurven](images/toolbar_adsr.png "Hüllkurven"){.inline} [Hüllkurveneditor](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope) und der ![Klangfarbe](images/toolbar_timbre.png "Klangfarbe"){.inline} [Klangfarbenansicht](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_timbre) gewechselt werden,
* beim Bearbeiten eines Presets kann zwischen der ![Tabelle](images/toolbar_table.png "Tabelle"){.inline} Tabellenansicht und dem ![Bereiche](images/toolbar_range.png "Bereiche"){.inline} Bereichseditor gewechselt werden,
* andernfalls wird nichts angezeigt.

Die gewählte Ansicht bleibt erhalten, wenn ein anderes Instrument oder Preset ausgewählt wird. Die Ansicht, die beim Öffnen eines Soundfonts für das erste Instrument oder Preset angezeigt wird, kann mit der Option :guilabel:`Startansicht` in den [Einstellungen](manual/settings.md#doc_interface) gewählt werden.


## Rekorder {#doc_recorder}


Ein Klick auf das Symbol ![Rekorder](images/toolbar_recorder.png "Rekorder"){.inline} öffnet ein Fenster mit dem Rekorder, der das, was Polyphone spielt, den Audioeingang oder die MIDI-Signale in einer Datei aufnehmen kann. Die verstrichene Zeit wird im Fenster angezeigt.


![Rekorder](images/recorder.png "Rekorder")


Die Liste im Banner wählt den Aufnahmemodus:

Polyphone → wav
: Nimmt die Ausgabe von Polyphone in einer :file:`.wav`-Datei auf.

Mikrofon → wav
: Nimmt den in den [Einstellungen](manual/settings.md#doc_general) gewählten Audioeingang in einer :file:`.wav`-Datei auf, je nach Gerät in Stereo oder Mono.
  Der Eingang wird nicht an die Audioausgabe gesendet.

MIDI-Signale
: Nimmt in einer :file:`.mid`-Datei auf, was mit einem MIDI-Keyboard oder der [virtuellen Tastatur](#doc_keyboard) gespielt wird.

Der linke Button kann zwei Zustände haben (Tastenkürzel :kbd:`Ctrl`+:kbd:`R`, in allen Fenstern verfügbar, der Rekorder wird bei Bedarf geöffnet):

Aufnehmen
: Startet eine neue Aufnahme, wenn gerade nicht aufgenommen wird.
  Die Datei wird im Verzeichnis der letzten Aufnahme erstellt, wobei dem Namen eine Nummer angefügt wird, falls bereits eine Datei mit diesem Namen existiert.

Stopp
: Beendet die laufende Aufnahme.
  Die Aufnahme wird nach einer Bestätigung auch beendet, wenn das Fenster geschlossen wird.

Der rechte Button kann ebenfalls zwei Zustände haben (nur verfügbar, während aufgenommen wird, Tastenkürzel :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`R`):

Pause
: Unterbricht die laufende Aufnahme.

Fortsetzen
: Setzt die laufende Aufnahme fort.

Unter diesen Buttons zeigt die Anzeige den Pegel der Kanäle, auch wenn nichts aufgenommen wird: So kann die Lautstärke vor einer Aufnahme eingestellt werden. Der Wert rechts daneben ist der höchste seit Beginn der Aufnahme erreichte Pegel. Er wird auf rotem Hintergrund angezeigt, wenn der maximale Pegel erreicht wurde und der Klang somit übersteuert ist. Ein Klick auf die Anzeige setzt diesen Wert zurück. Im Modus :guilabel:`MIDI-Signale` zeigt die Anzeige den Anschlag der gespielten Noten, was bestätigt, dass die Signale empfangen werden.

Der untere Teil des Fensters listet die letzten 5 Aufnahmen auf, je nach Modus Audio oder MIDI, mit ihrer Größe oder der Anzahl ihrer Noten. Eine Datei, die nicht mehr existiert, wird beim Öffnen des Fensters aus der Liste entfernt. Ein Klick wählt eine Zeile aus, die dann vier Buttons anzeigt:

Anhören
: Spielt die Audiodatei in Polyphone über die Audioausgabe ab (ein zweiter Klick stoppt sie, und die Auswahl einer anderen Zeile während der Wiedergabe spielt stattdessen deren Datei ab) oder öffnet die MIDI-Datei im [MIDI-Player](#doc_midi_player).

Verzeichnis
: Öffnet das Verzeichnis der Datei.

Löschen
: Verschiebt die Datei nach einer Bestätigung in den Papierkorb.

Importieren
: Importiert die Audiodatei als Sample in die angezeigte Soundfont.
  Dieser Button ist nur verfügbar, wenn ein Soundfont-Editor angezeigt wird.

Das Menü rechts im Banner enthält folgende Optionen:

Automatisch starten
: Die Aufnahmetaste wartet auf den ersten Klang oder das erste MIDI-Signal, bevor die Aufnahme beginnt.
  Nach einem automatischen Stopp wird auf die gleiche Weise auf die nächste Aufnahme gewartet, bis zu einem Klick auf die Stopp-Taste.

Nach einer Stille stoppen
: Anzahl der Sekunden ohne Klang oder MIDI-Signal, nach denen eine Aufnahme stoppt, von 1 bis 10 (:guilabel:`deaktiviert` für keinen automatischen Stopp).
  Die Stille wird aus der Datei entfernt, und eine noch gehaltene Taste gilt nicht als Stille.

Klangpegel
: Nur für den Audioeingang: Pegel, ab dem der Eingang als Klang gilt, über seinem Grundrauschen.
  Für die Ausgabe von Polyphone ist dieser Pegel auf -90 dB festgelegt.

Nach einem Dateinamen fragen
: Ein Dialog fragt zu Beginn einer Aufnahme nach dem Dateinamen.

Verzeichnis ändern...
: Wählt das Verzeichnis der nächsten Aufnahmen, gemeinsam für Audio- und MIDI-Dateien.

Eine Meldung erscheint, wenn eine Aufnahme nicht fortgesetzt werden kann (Datenträger voll oder zu langsam, Datei erreicht 4 GB, Änderung des Audioeingangs oder der Samplefrequenz der Audioausgabe): Das bis dahin Aufgenommene bleibt in der Datei erhalten.


## Virtuelle Tastatur {#doc_keyboard}


Ein Klick auf das Symbol ![Klavier](images/toolbar_piano.png "Klavier"){.inline} öffnet ein Fenster mit der virtuellen Tastatur, die zum Testen von Samples, Instrumenten und Presets gedacht ist.
Dieses Fenster sendet MIDI-Signale:

* **Note On** / **Note Off** / **Aftertouch** mit der Klaviatur,
* weitere MIDI-Signale, wenn der **Bereich der MIDI-Controller** ausgeklappt ist.

Das Banner oben im Fenster enthält eine Liste zur Auswahl des Tastaturtyps (5 Oktaven, 6 Oktaven, 88 Tasten oder 128 Tasten) und zeigt Taste, Anschlag und Aftertouch der gespielten Note an.
Das Menü rechts im Banner blendet die [MIDI-Controller](#doc_keyboard_controllers) und die [Audioeffekte](#doc_keyboard_effects) ein oder aus, die oberhalb der Klaviatur angezeigt werden.

Das Fenster behält die Proportionen der Klaviatur bei: Wird seine Breite geändert, ändert sich seine Höhe.
Das Ein- oder Ausblenden eines Bereichs ändert nur die Höhe des Fensters, das an seinem oberen Rand wächst oder schrumpft, damit die Klaviatur an derselben Stelle des Bildschirms bleibt (unter Linux mit Wayland nicht möglich).

Die virtuelle Tastatur kann auch im [Soundfont-Explorer](manual/soundfont-explorer.md#doc_play) angezeigt werden, um die Elemente der Dateien eines Verzeichnisses zu spielen, ohne sie zu öffnen.


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
  Sie spielen, sobald das Fenster der virtuellen Tastatur aktiv ist, auch wenn ein anderes Element dieses Fensters den Fokus hat (ein Drehregler, die Dropdown-Liste …).

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
  Sie funktionieren ebenfalls, sobald das Fenster der virtuellen Tastatur aktiv ist.

:kbd:`→` / :kbd:`←`
: Der Pfeil nach rechts bzw. links spielt die Tonhöhe direkt über bzw. unter der zuletzt gespielten Taste, mit derselben Anschlagsstärke.
  Zusammen mit der Taste :kbd:`Shift` werden 6 Tasten statt einer übersprungen.

:kbd:`↑` / :kbd:`↓`
: Der Pfeil nach oben bzw. unten spielt dieselbe Tonhöhe wie die zuletzt gespielte Taste, aber mit stärkerem bzw. schwächerem Anschlag.
  Zusammen mit der Taste :kbd:`Shift` ändert sich die Anschlagsstärke um 10 statt um 1.

Die Pfeiltasten spielen nur dann Noten, wenn die Klaviatur den Fokus hat: Andernfalls wirken sie auf das Element mit dem Fokus, zum Beispiel auf einen Drehregler.


### MIDI-Controller {#doc_keyboard_controllers}


Ist die Option :guilabel:`MIDI-Controller` im Menü rechts im Banner angehakt, wird der Bereich der MIDI-Controller oberhalb der Klaviatur angezeigt.
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


### Audioeffekte {#doc_keyboard_effects}


Ist die Option :guilabel:`Effekte` im Menü rechts im Banner angehakt, wird oben im Fenster ein Bereich angezeigt, in dem sich die wichtigsten Einstellungen des Synthesizers während des Spielens anpassen lassen:

Halleffekt
: Eine LED schaltet den Hall ein oder aus, dann ändern 4 Drehregler seine Intensität, Raumgröße, Weite und Dämpfung.
  Der Name eines Drehreglers wird angezeigt, wenn sich die Maus darüber befindet.

Chorus
: Eine LED schaltet den Chorus ein oder aus, dann ändern 3 Drehregler seine Intensität, Tiefe und Frequenz.

Lautstärke
: Der Schieberegler ändert die Gesamtlautstärke.

Diese Einstellungen sind dieselben wie in den [Sound-Einstellungen](manual/settings.md#doc_sound): Eine Änderung an einer Stelle wird auch an der anderen angezeigt.
Eine Änderung erfolgt schrittweise und unterbricht die gespielten Noten nicht.


## MIDI-Player {#doc_midi_player}


Ein Klick auf das Symbol ![MIDI](images/toolbar_midi.png "MIDI"){.inline} öffnet ein Fenster mit dem MIDI-Player, der eine MIDI-Datei (:file:`.mid`, :file:`.midi`, :file:`.kar` oder :file:`.rmi`) mit den geöffneten Soundfonts oder mit den in einem [Soundfont-Explorer](manual/soundfont-explorer.md#doc_play) ausgewählten Dateien abspielt, um sie in einem Musikstück zu testen.
Die Soundfonts werden so gespielt, wie sie im Editor sind, einschließlich ihrer nicht gespeicherten Änderungen.
Der MIDI-Player ist auch im [Synth-Modus](tutorials/use-polyphone-as-a-synthesizer.md) verfügbar.


![MIDI-Player](images/midi_player.png "MIDI-Player")


Das Banner oben im Fenster enthält von links nach rechts:

* eine Schaltfläche zum Öffnen einer MIDI-Datei (beim Öffnen des Fensters wird die zuletzt geöffnete Datei erneut geladen, sofern sie noch existiert),
* den Namen der Datei, die verstrichene Zeit und die Gesamtdauer über einem Schieberegler, mit dem man an eine andere Stelle des Stücks springen kann,
* eine Schaltfläche zum Starten oder Pausieren der Wiedergabe, auch mit :kbd:`Space` erreichbar,
* eine Schaltfläche zum Stoppen der Wiedergabe und Zurückkehren zum Anfang,
* eine Schaltfläche, die die [Wiedergabeeinstellungen](#doc_midi_player_settings) öffnet.

Kann die Datei nicht gelesen werden, wird der Grund anstelle der Kanäle angezeigt.
Die Wiedergabe stoppt, wenn das Fenster geschlossen wird.

Während der Wiedergabe kann mit einer externen MIDI-Tastatur oder der [virtuellen Tastatur](#doc_keyboard) weiterhin das im Editor ausgewählte Element zur Datei gespielt werden (die Noten der Datei werden nicht auf der virtuellen Tastatur angezeigt), und der [Rekorder](#doc_recorder) kann das Ergebnis aufnehmen.


### Kanäle


Die Tabelle listet die Kanäle der Datei auf, die Noten enthalten, wobei jeder Kanal eigene Einstellungen hat:

Kanal
: Die Nummer des Kanals; der Name seiner Spur wird in einem Tooltip angezeigt.
  Die Schaltflächen :guilabel:`M` (Stummschalten) und :guilabel:`S` (Solo) schalten den Kanal stumm oder lassen nur die Kanäle im Solo hören.
  Die MIDI-Schaltfläche zeigt die aktuellen MIDI-Werte des Kanals an, die von ihrem Standardwert abweichen: Controller, Pitch-Rad (in Halbtönen), seine Empfindlichkeit und Kanaldruck.

Soundfont
: Der vom Kanal gespielte Soundfont: :guilabel:`Auswahl`, einer der in Tabs geöffneten Soundfonts, oder :guilabel:`Keine` für Stille.
  :guilabel:`Auswahl`, die beim Öffnen einer Datei für alle Kanäle gewählt wird, spielt das, was im Hauptfenster ausgewählt ist: den Soundfont des aktuellen Tabs oder die in einem [Soundfont-Explorer](manual/soundfont-explorer.md#doc_play) ausgewählte Datei, deren Name in Klammern angezeigt wird.
  Die anderen Tabs (Startbildschirm, Einstellungen, Online-Fundgrube…) behalten den gespielten Soundfont bei.
  Wenn sich die Auswahl ändert, dreht sich vor dem Namen ein Ladesymbol, solange der Soundfont geladen wird, wobei der vorherige so lange weitergespielt wird; danach kehren die Presets dieser Kanäle zu :guilabel:`Auto` zurück.

Preset
: Mit :guilabel:`Auto` werden die Presets durch die Programmwechsel der Datei gewählt (Bank 128 auf Kanal 10, für das Schlagzeug), und das aktuelle Preset wird in Klammern angezeigt.
  Andernfalls wird das in der Liste gewählte Preset gespielt, unabhängig davon, was die Datei anfordert.
  Enthält der Soundfont das von der Datei angeforderte Preset nicht, wird ein Warnsymbol angezeigt und die Noten des Kanals werden nicht gespielt.

Lautstärke
: Die LED leuchtet auf, wenn Noten beginnen, und der Schieberegler ändert die Lautstärke des Kanals von -40 bis +10 dB (ein Doppelklick setzt sie auf 0 dB zurück).

Die Liste der Soundfonts oder Presets öffnet sich mit einem Doppelklick auf eine Zelle oder mit einem Klick auf eine bereits ausgewählte Zelle.
Diese Einstellungen werden zurückgesetzt, wenn eine andere Datei geöffnet wird.


### Wiedergabeeinstellungen {#doc_midi_player_settings}


Die Schaltfläche rechts im Banner öffnet ein Menü mit den folgenden Einstellungen, die beim Öffnen einer anderen Datei erhalten bleiben:

Loop-Wiedergabe
: Das Stück beginnt wieder von vorne, wenn es endet.

Tempo
: Wiedergabegeschwindigkeit, von 25 % bis 200 % des Tempos der Datei (ihre Tempowechsel bleiben erhalten).
  Das Tempo der Datei wird in einem Tooltip angezeigt, und die im Banner angezeigte verstrichene Zeit und Dauer folgen der Wiedergabegeschwindigkeit.

Tonhöhe
: Transposition von -12 bis +12 Halbtönen, außer für das Schlagzeug (Bank 128).

Ein Doppelklick auf einen Schieberegler setzt ihn auf seinen Standardwert zurück (100 % bzw. 0).


![Wiedergabeeinstellungen](images/midi_player_settings.png "Wiedergabeeinstellungen")


Das Menü endet mit :guilabel:`Audio exportieren...`, das das Stück so in eine :file:`.wav`-Datei schreibt, wie es im Player zu hören ist: dieselben Soundfonts, Presets, Lautstärken, Stummschaltungen und Solos, dasselbe Tempo und dieselbe Tonhöhe.
Die Wiedergabe wird während des Exports pausiert, der endet, sobald der Klang der letzten Noten verklungen ist.
Wird der Export abgebrochen, wird die unvollständige Datei gelöscht.
Eine MIDI-Datei kann auch über die [Kommandozeile](manual/annexes/command-line.md#doc_render) als wav-Datei gespeichert werden.

*[MIDI]: Musical Instrument Digital Interface
