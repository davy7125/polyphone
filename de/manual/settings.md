Die Einstellungen der Software sind entweder über den [Startbildschirm](manual/index.md) oder über das [Menü](manual/menu.md#doc_shortcuts) erreichbar (Tastenkombination :kbd:`Ctrl`+:kbd:`,`).
Sie sind in fünf Bereiche unterteilt:

* [Allgemein](#doc_general)
* [Oberfläche](#doc_interface)
* [Sound](#doc_sound)
* [Tastatur](#doc_keyboard)
* [Online-Fundgrube](#doc_repository)

Alle Änderungen werden sofort gespeichert.


## Allgemein {#doc_general}


Im Bereich :guilabel:`Allgemein` können folgende Parameter geändert werden:

Audioausgang
: Legt das Audiosystem und das Gerät fest, mit dem die Klänge wiedergegeben werden.
  Die verfügbaren Systeme hängen vom Betriebssystem ab (zum Beispiel WASAPI und ASIO unter Windows, ALSA und JACK unter Linux, Core Audio unter macOS).
  Die Auswahl „-“ schaltet die Audioausgabe ab.

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

Sample importieren: bis zum Loop-Ende kürzen
: Beim Import eines Samples werden die Daten nach dem Ende des Loops automatisch entfernt (gleiche Wirkung wie das Werkzeug „[Bis zum Loop-Ende kürzen](manual/soundfont-editor/tools/sample-tools.md#doc_trimloop)“).

Sample importieren: Leere am Anfang entfernen
: Beim Import eines Samples wird jeder leere Bereich vor dem Beginn des Attacks automatisch entfernt (gleiche Wirkung wie das Werkzeug „[Leere am Anfang entfernen](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)“).


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

Startansicht
: Wählt die Ansicht, die beim Öffnen eines Soundfonts angezeigt wird, unter folgenden aus: :guilabel:`Allgemeine Informationen` (Standard), :guilabel:`Sample-Liste`, :guilabel:`Erstes Sample`, :guilabel:`Instrumentenliste`, :guilabel:`Erstes Instrument`, :guilabel:`Presetliste`, :guilabel:`Erstes Preset`.

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

Dekorationen
: Hintergrunddekorationen in einigen Listen können zur besseren Lesbarkeit deaktiviert werden.

Eine Änderung der Sprache, der Notennamen, des Stils, der Farben, der Fensterrahmen oder der Dekorationen wird beim nächsten Start der Software wirksam.


![Einstellungen, Oberfläche](images/settings_interface.png "Einstellungen, Oberfläche")


## Sound {#doc_sound}


Im Bereich :guilabel:`Sound` lassen sich der Hall (Intensität, Raumgröße, Dämpfung und Weite) und der Chorus (Intensität, Tiefe und Frequenz) des Synthesizers sowie seine Gesamtlautstärke ändern.
Ist eine externe Tastatur angeschlossen, kann die Gesamtlautstärke mit dem MIDI-Lautstärkeregler geändert werden.

Da der Standardmodulator, der die Grenzfrequenz des Filters ändert, zwischen den Versionen 2.01 und 2.04 der sf2-Spezifikation geändert wurde, kann hier festgelegt werden, welche Version verwendet werden soll:

* :guilabel:`SoundFont 2.01`: verwendet die Regel „senke die Filterfrequenz in Abhängigkeit vom Anschlag, WENN der Anschlag kleiner als 64 ist“ (zwischen den Anschlagswerten 63 und 64 ist ein deutlicher und störender Sprung zu hören);
* :guilabel:`SoundFont 2.04` (Standard): verwendet die Regel „senke die Filterfrequenz in Abhängigkeit vom Anschlag“ (kein Sprung mehr);
* :guilabel:`deaktiviert`: dieser Standardmodulator wird nicht gesetzt.


![Einstellungen, Sound](images/settings_sound.png "Einstellungen, Sound")


## Tastatur {#doc_keyboard}


Die Tasten der Computertastatur, mit denen die [virtuelle Tastatur](manual/soundfont-editor/toolbar.md#doc_keyboard) gesteuert wird, werden im Bereich :guilabel:`Tastatur` festgelegt.
Mit der Einstellung :guilabel:`Erstes C` kann die Tonhöhe oktavweise erhöht oder verringert werden.

Für fortgeschrittene Musiker kann hier die Stimmung geändert werden:

Referenztonhöhe
: Die Frequenz des A über dem mittleren C (Taste 69), zwischen 390 und 470 Hz (standardmäßig 440 Hz).

Temperatur
: Beim Spielen kann eine andere Temperatur als die gleichstufige Stimmung angewendet werden.
  Ist eine Temperatur ausgewählt, kann auch die Taste gewählt werden, die als Bezug dieser Temperatur dient (ihr Grundton).

Referenztonhöhe und Temperatur können auch über MIDI-RPN-Nachrichten geändert werden (Master Fine Tuning, Tuning Program Select und Tuning Bank Select).


![Einstellungen, Tastatur](images/settings_keyboard.png "Einstellungen, Tastatur")


## Online-Fundgrube {#doc_repository}


Hier können die E-Mail-Adresse und das Passwort eines Kontos eingegeben werden, um Polyphone mit diesem Konto zu verbinden; ein [Premium-Konto](premium) schaltet die gesamte [Online-Fundgrube](manual/online-repository.md) frei.
Nach einer erfolgreichen Verbindung verbindet sich Polyphone bei jedem Start automatisch mit dem Konto, bis der Button :guilabel:`Abmelden` verwendet wird.

Hier kann auch das Verzeichnis geändert werden, das beim Herunterladen von Soundfonts aus der [Online-Fundgrube](manual/online-repository.md) verwendet wird.


![Einstellungen, Fundgrube](images/settings_repository.png "Einstellungen, Fundgrube")



*[JACK]: JACK Audio Connection Kit
*[ASIO]: Audio Stream Input/Output
*[MIDI]: Musical Instrument Digital Interface
