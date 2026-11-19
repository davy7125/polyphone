Die Einstellungen der Software sind entweder über den [Startbildschirm](manual/index.md) oder über das [Menü](manual/menu.md#doc_shortcuts) erreichbar (Tastenkombination :kbd:`Ctrl`+:kbd:`,`).
Sie sind in sechs Bereiche unterteilt:

* [Allgemein](#doc_general)
* [Oberfläche](#doc_interface)
* [Sound](#doc_sound)
* [Tastatur](#doc_keyboard)
* [KI-Assistent](#doc_ai_assistant)
* [Online-Fundgrube](#doc_repository)

Alle Änderungen werden sofort gespeichert.


## Allgemein {#doc_general}


Im Bereich :guilabel:`Allgemein` können folgende Parameter geändert werden:

Audioausgang
: Legt das Audiosystem und das Gerät fest, mit dem die Klänge wiedergegeben werden.
  Die verfügbaren Systeme hängen vom Betriebssystem ab (zum Beispiel WASAPI und ASIO unter Windows, ALSA und JACK unter Linux, Core Audio unter macOS).
  Die Auswahl „-“ schaltet die Audioausgabe ab.

Audioeingang
: Legt das Gerät fest, das der [Rekorder](manual/soundfont-editor/toolbar.md#doc_recorder) im Modus :guilabel:`Mikrofon → wav` aufnimmt. Es ist nur geöffnet, solange der Rekorder in diesem Modus angezeigt wird.
  ASIO kann für den Eingang nicht verwendet werden.

Pufferlänge
: Legt die Größe des Audiopuffers fest.
  Ein kleiner Puffer führt zu einer geringen Latenz, der Klang kann dabei aber kratzig werden.
  Mit dem Wert :guilabel:`Standard` hängt die Größe vom Audiosystem ab.

Polyphonie
: Maximale Anzahl gleichzeitig gespielter Stimmen, von 64 bis 4096 (standardmäßig 1024).
  Eine Stimme ist ein Sample, das gerade abgespielt wird: Eine Note kann mehrere Stimmen nutzen, zum Beispiel zwei bei einem Stereo-Sample.
  Oberhalb dieser Anzahl werden Noten ausgeblendet: zuerst die losgelassenen, dann die leisesten.
  Gerade begonnene Noten sowie die höchste und die tiefste gehaltene Taste bleiben so lange wie möglich erhalten.

Prozessorlast
: Maximaler Anteil der Prozessorzeit, der für die Berechnung der Stimmen genutzt wird, von 20 bis 100 % (standardmäßig 60 %).
  Oberhalb dieses Anteils werden Noten auf dieselbe Weise ausgeblendet.
  Ein hoher Wert erlaubt mehr gleichzeitig gespielte Noten, der Klang kann dabei aber kratzig werden.

MIDI-Eingang
: Legt den MIDI-Eingang fest, der den Synthesizer und die virtuelle Tastatur steuert.

Außerdem können folgende Optionen aktiviert werden:

Stereobearbeitung: Verknüpftes Sample ändern
: Wird eine Seite eines Stereo-Samples bearbeitet, wird die andere Seite automatisch genauso bearbeitet.
  Dasselbe gilt für die Bearbeitung einer Stereo-Aufteilung eines Instruments: Die Aufteilung des anderen Kanals (mit denselben Tasten- und Anschlagsbereichen) wird genauso bearbeitet.

Sample importieren: Leere am Anfang entfernen
: Beim Import eines Samples wird jeder leere Bereich vor dem Beginn des Attacks automatisch entfernt (gleiche Wirkung wie das Werkzeug „[Kürzen](manual/soundfont-editor/tools/sample-tools.md#doc_trim)“ mit automatischer Erkennung des Grundrauschens).

Sample importieren: Leere am Ende entfernen
: Beim Import eines Samples wird jeder leere Bereich am Ende des Klangs automatisch entfernt und durch eine kurze Ausblendung ersetzt (gleiche Wirkung wie das Werkzeug „[Kürzen](manual/soundfont-editor/tools/sample-tools.md#doc_trim)“ mit automatischer Erkennung des Grundrauschens).


![Einstellungen, Allgemein](images/settings_general.png "Einstellungen, Allgemein")


## Oberfläche {#doc_interface}


Dieser Bereich enthält Optionen zur Oberfläche der Software:

Sprache
: Hier kann die Sprache geändert werden.

Notennamen
: Tasten können nach ihrer Nummer (gemäß der MIDI-Spezifikation) oder nach dem Namen des mittleren C (C3, C4 oder C5) benannt werden.
  Notennamen können Kreuze oder Bs enthalten.

Einzelne Instanz
: Öffnet alle Dateien in einem einzigen Fenster.

Tabs wiederherstellen
: Öffnet beim Start die Soundfonts und Verzeichnisse, die beim Schließen von Polyphone geöffnet waren, in derselben Reihenfolge erneut; der [Startbildschirm](manual/index.md#doc_home) bleibt dabei angezeigt. Inzwischen gelöschte, umbenannte oder verschobene Elemente werden übersprungen.

Benachrichtigungen
: Zeigt auf dem [Startbildschirm](manual/index.md#doc_home) die auf der Website veröffentlichten [Benachrichtigungen](manual/index.md#doc_notifications) an (Warnung, neue Version, Ankündigung, Aufruf zur Unterstützung). Ist die Option deaktiviert, wird nichts heruntergeladen.

Startansicht
: Wählt die Ansicht, die beim Öffnen eines Soundfonts angezeigt wird, unter folgenden aus: :guilabel:`Allgemein` (Standard), :guilabel:`Samples`, :guilabel:`Samples - erstes Sample`, :guilabel:`Instrumente`, :guilabel:`Instrumente - erstes Instrument - Tabelle`, :guilabel:`Instrumente - erstes Instrument - Bereiche`, :guilabel:`Instrumente - erstes Instrument - Hüllkurven`, :guilabel:`Instrumente - erstes Instrument - Klangfarbe`, :guilabel:`Presets`, :guilabel:`Presets - erstes Preset - Tabelle`, :guilabel:`Presets - erstes Preset - Bereiche`. Beim ersten Instrument oder Preset legt der letzte Teil die angezeigte Seite fest: [Tabelle](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table), [Bereichseditor](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range), [Hüllkurveneditor](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope) oder [Klangfarbenansicht](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_timbre).

Aufteilungen sortieren
: Hier kann die Reihenfolge der Aufteilungen innerhalb eines Instruments oder eines Presets geändert werden: :guilabel:`Nach Tastenbereich`, :guilabel:`Nach Anschlagsbereich`, :guilabel:`Alphabetische Reihenfolge` oder :guilabel:`Keine` (die Reihenfolge der Erstellung).

Stil
: Der Stil der Oberflächenelemente.
  Der Stil :guilabel:`Fusion` ist der Standard; je nach Betriebssystem können weitere angeboten werden (Adwaita, Windows, macOS…).

Farben
: Verschiedene vordefinierte Farbschemata stehen zur Verfügung, um das Erscheinungsbild von Polyphone anzupassen.
  Die Farben von Text, Fenster, Buttons, Auswahl und Listen können auch manuell gewählt werden.
  Die Farben lassen sich nur mit den Stilen :guilabel:`Fusion` und :guilabel:`Windows` ändern.

Fensterrahmen
: Blendet die Fensterrahmen ein oder aus, um Platz auf dem Bildschirm zu sparen (unter macOS nicht verfügbar).
  Ohne Rahmen werden die Fenster der virtuellen Tastatur, des Rekorders und des MIDI-Players durch Ziehen ihres Banners verschoben, das ihren Titel und eine Schaltfläche zum Schließen anzeigt, und durch Ziehen ihrer Ränder in der Größe verändert.

Dekorationen
: Hintergrunddekorationen in einigen Listen können zur besseren Lesbarkeit deaktiviert werden.

Eine Änderung der Sprache, der Notennamen, des Stils, der Farben, der Fensterrahmen oder der Dekorationen wird beim nächsten Start der Software wirksam.


![Einstellungen, Oberfläche](images/settings_interface.png "Einstellungen, Oberfläche")


## Sound {#doc_sound}


Im Bereich :guilabel:`Sound` lassen sich der Hall (Intensität, Raumgröße, Dämpfung und Weite) und der Chorus (Intensität, Tiefe und Frequenz) des Synthesizers sowie seine Gesamtlautstärke ändern.
Ist eine externe Tastatur angeschlossen, kann die Gesamtlautstärke mit dem MIDI-Lautstärkeregler geändert werden.
Eine LED links neben :guilabel:`Halleffekt` und :guilabel:`Chorus` schaltet den jeweiligen Effekt ein oder aus, ohne seine Einstellungen zu verlieren.
Diese Einstellungen lassen sich während des Spielens auch im Fenster der [virtuellen Tastatur](manual/soundfont-editor/toolbar.md#doc_keyboard_effects) ändern: Eine Änderung erfolgt schrittweise und unterbricht die gespielten Noten nicht.

Da der Standardmodulator, der die Grenzfrequenz des Filters ändert, zwischen den Versionen 2.01 und 2.04 der sf2-Spezifikation geändert wurde, kann hier festgelegt werden, welche Version verwendet werden soll:

* :guilabel:`SoundFont 2.01`: verwendet die Regel „senke die Filterfrequenz in Abhängigkeit vom Anschlag, WENN der Anschlag kleiner als 64 ist“ (zwischen den Anschlagswerten 63 und 64 ist ein deutlicher und störender Sprung zu hören);
* :guilabel:`SoundFont 2.04` (Standard): verwendet die Regel „senke die Filterfrequenz in Abhängigkeit vom Anschlag“ (kein Sprung mehr);
* :guilabel:`deaktiviert`: dieser Standardmodulator wird nicht gesetzt.

Diese Einstellung hat keine Wirkung auf einen Soundfont mit eigenen [Standardmodulatoren](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md#doc_default_mod).


![Einstellungen, Sound](images/settings_sound.png "Einstellungen, Sound")


## Tastatur {#doc_keyboard}


Die Tasten der Computertastatur, mit denen die [virtuelle Tastatur](manual/soundfont-editor/toolbar.md#doc_keyboard) gesteuert wird, werden im Bereich :guilabel:`Tastatur` festgelegt.
Mit der Einstellung :guilabel:`Erstes C` kann die Tonhöhe oktavweise erhöht oder verringert werden.

Die Stimmung des Synthesizers wird im Bereich :guilabel:`Stimmung` eingestellt, der auch in der [virtuellen Tastatur](manual/soundfont-editor/toolbar.md#doc_keyboard_tuning) angezeigt wird:

:guilabel:`Referenztonhöhe`
: Eine Note von A#3 bis A4 und ihre Frequenz: Diese Note klingt unabhängig von der Temperatur mit dieser Frequenz.
  Die Frequenz liegt für A4 zwischen 390 und 470 Hz (standardmäßig 440 Hz), für die anderen Noten im gleichen Verhältnis.

:guilabel:`Transposition`
: Transposition in Halbtönen (von -12 bis +12) der Noten, die mit der virtuellen Tastatur, der Computertastatur oder über einen MIDI-Eingang gespielt werden.

:guilabel:`Temperatur`
: Gleichstufige Stimmung (Standard), eine der 226 nach Kategorien geordneten Temperaturen und Tonleitern (historische Temperaturen, westliche Tonleitern, Tonleitern anderer Musiktraditionen, gleichstufige Teilungen der Oktave…) oder die benutzerdefinierte Temperatur.
  :guilabel:`Tonart` transponiert die Temperatur oder die Tonleiter, sodass ihr Grundton die gewählte Note ist.

Die Tastatur darunter zeigt die Feinstimmung jeder Note in Cent gegenüber der gleichstufigen Stimmung, ein Punkt markiert die Tonart:

* Ein Wert wird mit dem Mausrad oder durch vertikales Ziehen geändert (feiner mit :kbd:`Ctrl`) und per Doppelklick auf 0 zurückgesetzt; die angezeigte Temperatur wird dann zur benutzerdefinierten Temperatur.
* Das Kontextmenü einer Note ändert ihre Verwendung (:guilabel:`Note verwendet`) und legt die Tonart auf sie (:guilabel:`Tonart auf diese Note`).
* Die von einer Tonleiter nicht verwendeten Noten sind grau: Sie werden nicht gespielt, außer wenn :guilabel:`Auch die nicht verwendeten Noten spielen` aktiviert ist.
* Der Wert der Note der Referenztonhöhe ist orange, wenn er nicht 0 ist: Diese Note behält die Frequenz der Referenztonhöhe, alle Noten werden verschoben, um ihre Feinstimmung auszugleichen.

Die Schaltflächen rechts stellen die Standardwerte der Referenztonhöhe und der Transposition sowie der Temperatur wieder her.
Referenztonhöhe und Temperatur können auch über MIDI-RPN-Nachrichten geändert werden (Master Fine Tuning, Tuning Program Select und Tuning Bank Select).


![Einstellungen, Tastatur](images/settings_keyboard.png "Einstellungen, Tastatur")


## KI-Assistent {#doc_ai_assistant}


Im Bereich :guilabel:`KI-Assistent` kann einem KI-Assistenten, der das Model Context Protocol (MCP) unterstützt, etwa Claude, erlaubt werden, die in Polyphone geöffneten Soundfonts zu durchsuchen, zu bearbeiten und zu analysieren.
Seine Änderungen erscheinen sofort im Editor und können wie jede andere Änderung rückgängig gemacht werden.

KI-Assistenten den Zugriff auf die geöffneten Soundfonts erlauben
: Startet den MCP-Server von Polyphone (standardmäßig deaktiviert).
  Die Zeile darunter zeigt den Zustand des Servers und seine Adresse an, zum Beispiel „Server gestartet, Adresse: http://127.0.0.1:8642/mcp“.
  Der Server nimmt nur Verbindungen von diesem Computer an.

Port
: Port des Servers, standardmäßig 8642.
  Wenn dieser Port bereits von einem anderen Programm belegt ist, kann ein anderer gewählt werden.

Befehl zum Hinzufügen von Polyphone zu Claude Code
: Befehl, der in einem Terminal auszuführen ist, damit Claude Code Polyphone verwenden kann.
  Die Schaltfläche :guilabel:`Kopieren` kopiert ihn in die Zwischenablage.

Für die KI verfügbare Funktionen
: Liste der Funktionen, die der Assistent aufrufen kann, gruppiert in vier Kategorien: :guilabel:`Durchsuchen`, :guilabel:`Bearbeitung`, :guilabel:`Werkzeuge` und :guilabel:`Klanganalyse`.
  Ihre Beschreibungen sind die, die der Assistent liest, auf Englisch.

Das Tutorial „[Soundfonts mit Claude bearbeiten](tutorials/edit-soundfonts-with-claude.md)“ erklärt, wie Claude mit Polyphone verbunden wird, und gibt Beispiele für Anfragen.


![Einstellungen, KI-Assistent](images/settings_ai_assistant.png "Einstellungen, KI-Assistent")


## Online-Fundgrube {#doc_repository}


Hier können die E-Mail-Adresse und das Passwort eines Kontos eingegeben werden, um Polyphone mit diesem Konto zu verbinden; ein [Premium-Konto](premium) schaltet die gesamte [Online-Fundgrube](manual/online-repository.md) frei.
Nach einer erfolgreichen Verbindung verbindet sich Polyphone bei jedem Start automatisch mit dem Konto, bis der Button :guilabel:`Abmelden` verwendet wird.

Hier kann auch das Verzeichnis geändert werden, das beim Herunterladen von Soundfonts aus der [Online-Fundgrube](manual/online-repository.md) verwendet wird.


![Einstellungen, Fundgrube](images/settings_repository.png "Einstellungen, Fundgrube")



*[JACK]: JACK Audio Connection Kit
*[ASIO]: Audio Stream Input/Output
*[MIDI]: Musical Instrument Digital Interface
