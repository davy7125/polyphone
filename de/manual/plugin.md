Polyphone gibt es auch als Plugin für digitale Audio-Workstations (DAW) und die anderen Hosts für Audio-Plugins.
Das Plugin spielt Soundfonts in den Formaten sf2, sf3, sfz und dls mit dem Synthesizer von Polyphone: Ein Soundfont klingt in einem Host genauso wie im Editor.
Es spielt die Soundfonts ab, bearbeitet sie aber nicht: Die Änderungen werden in Polyphone vorgenommen, und der Soundfont wird anschließend im Plugin erneut geöffnet.

Das Plugin ist in den Formaten VST3, CLAP und LV2 unter Linux, Windows und macOS verfügbar, unter macOS außerdem als Audio Unit (AU).


![Polyphone-Plugin](images/plugin.png "Polyphone-Plugin")


## Installation {#doc_install}


Das Plugin wird für jedes Betriebssystem in einem Archiv verteilt, das im Bereich „[Software](software)“ verfügbar ist.
Kopieren Sie die von Ihrem Host verwendeten Formate in die Ordner, in denen er nach Plugins sucht, und lassen Sie den Host dann erneut nach Plugins suchen.

Linux
: :file:`Polyphone.vst3` nach :file:`~/.vst3/`,\
  :file:`Polyphone.clap` nach :file:`~/.clap/`,\
  :file:`Polyphone.lv2` nach :file:`~/.lv2/`.

macOS
: :file:`Polyphone.vst3` nach :file:`~/Library/Audio/Plug-Ins/VST3/`,\
  :file:`Polyphone.clap` nach :file:`~/Library/Audio/Plug-Ins/CLAP/`,\
  :file:`Polyphone.component` nach :file:`~/Library/Audio/Plug-Ins/Components/` (Format Audio Unit, verwendet von Logic Pro und GarageBand),\
  :file:`Polyphone.lv2` nach :file:`~/Library/Audio/Plug-Ins/LV2/`.

Windows
: :file:`Polyphone.vst3` nach :file:`C:\Program Files\Common Files\VST3`,\
  :file:`Polyphone.clap` nach :file:`C:\Program Files\Common Files\CLAP`,\
  :file:`Polyphone.lv2` nach :file:`%APPDATA%\LV2`.

Unter macOS sind die Plugins nicht notarisiert: Weigert sich der Host, sie zu laden, entfernen Sie das Quarantäne-Attribut der heruntergeladenen Dateien, zum Beispiel:

```
xattr -dr com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/Polyphone.vst3
```


## Soundfont {#doc_soundfont}


Oben im Fenster werden der Name des Soundfonts und sein Pfad angezeigt.
Ein Soundfont wird geöffnet:

* mit der Schaltfläche :guilabel:`Öffnen...`,
* mit dem Pfeil rechts daneben, der die 10 zuletzt geöffneten Soundfonts auflistet,
* durch Ablegen einer Datei auf dem Fenster.

sfz- und dls-Dateien werden so umgewandelt, wie Polyphone sie importiert, wobei die Samples einer sfz-Datei die Audiodateien sind, auf die sie verweist.
Der gesamte Soundfont wird beim Öffnen gelesen, wobei die Samples einer sf3-Datei dekomprimiert werden: Während des Spielens wird nichts von der Festplatte gelesen.
Spielen mehrere Instanzen des Plugins dieselbe Datei (zum Beispiel eine pro Spur), wird die Datei nur einmal gelesen und im Speicher gehalten.
Hat sich die Datei geändert, zum Beispiel nachdem sie in Polyphone erneut gespeichert wurde, wird sie erneut gelesen.


## Spielmodi {#doc_modes}


Drei Schaltflächen über der Liste der Presets legen fest, wie die Presets gespielt werden:

:guilabel:`Ein Preset`
: Das in der Liste ausgewählte Preset wird auf den 16 MIDI-Kanälen gespielt.
  Die Presets werden mit ihrer Bank- und Presetnummer angezeigt, zum Beispiel „000:024 Guitar“.

:guilabel:`16 Kanäle`
: Jeder MIDI-Kanal spielt sein eigenes Preset, das in seiner Zeile gewählt wird.
  Die Auswahl :guilabel:`Programmwechsel` spielt das Preset, das durch die vom Kanal empfangenen Bank-Select- und Program-Change-Nachrichten gewählt wird, wie es ein General-MIDI-Synthesizer tut: Auf Kanal 10 wird dann die Bank 128 für das Schlagzeug verwendet.
  Das gespielte Preset wird rechts in jeder Zeile angezeigt.
  Dieser Modus eignet sich zum Abspielen einer MIDI-Datei, die für mehrere Instrumente arrangiert ist.

:guilabel:`MPE`
: Das ausgewählte Preset wird von einem MPE-Controller (MIDI Polyphonic Expression) gespielt: Jede Note hat ihren eigenen Kanal und damit ihr eigenes Pitch Bend, ihren eigenen Druck und ihre eigene Klangfarbe (CC 74).
  Eine untere Zone mit 15 Kanälen wird verwendet, bis der Controller seine Konfiguration sendet (RPN 6).
  Der Master-Kanal einer Zone wirkt auf alle ihre Noten: Sein Pitch Bend wird zu dem jeder Note addiert, und seine Controller gelten für alle Noten.
  Der Pitch-Bend-Bereich beträgt 48 Halbtöne für die Noten und 2 Halbtöne für den Master-Kanal und kann vom Controller geändert werden (RPN 0).


![Plugin im Modus „16 Kanäle“](images/plugin_channels.png "Plugin im Modus „16 Kanäle“")


## Parameter {#doc_parameters}


Die Parameter auf der rechten Seite sind die des Synthesizers von Polyphone, die sich auch in seinen Einstellungen [Sound](manual/settings.md#doc_sound) und [Tastatur](manual/settings.md#doc_keyboard) finden:

:guilabel:`Synthesizer`
: :guilabel:`Verstärkung` (von -50 bis +50 dB), :guilabel:`Transposition` (von -24 bis +24 Halbtönen, wobei das Schlagzeug nicht transponiert wird), :guilabel:`Stimmung` (Referenztonhöhe des A4, von 400 bis 480 Hz) und :guilabel:`Temperatur`, deren Feinstimmungen sich auf die unter der Liste gewählte Tonart beziehen.

:guilabel:`Halleffekt`
: :guilabel:`Intensität`, :guilabel:`Größe`, :guilabel:`Weite` und :guilabel:`Dämpfung`.

:guilabel:`Chorus`
: :guilabel:`Intensität`, :guilabel:`Tiefe` und :guilabel:`Frequenz`.

Alle diese Parameter können vom Host automatisiert werden.


## Klaviatur und Status {#doc_keyboard}


Die Klaviatur unten im Fenster zeigt die empfangenen Noten an und kann mit der Maus gespielt werden.
Darunter werden die Anzahl der spielenden Stimmen und der Anteil des Prozessors angezeigt, den das Plugin beansprucht.

Die Schaltfläche :guilabel:`Panik` stoppt alle Klänge und setzt die Controller zurück, was nützlich ist, wenn eine Note hängen bleibt.


## Einstellungen {#doc_settings}


Die Schaltfläche :guilabel:`Einstellungen` öffnet ein Menü mit folgenden Optionen:

:guilabel:`Polyphonie`
: Maximale Anzahl an Stimmen, von 64 bis 4096 (standardmäßig 1024).
  Oberhalb dieser Anzahl werden die leisesten Noten ausgeblendet.

:guilabel:`Tastatur`
: MIDI-Kanal der Noten, die mit der Klaviatur des Fensters gespielt werden.

:guilabel:`Sprache`
: Sprache des Plugins, standardmäßig die des Systems.
  Diese Auswahl gilt für alle Instanzen des Plugins.
  Die Namen der Parameter und der Ausgänge, die dem Host übermittelt werden, sind die der Sprache, die beim Laden des Plugins verwendet wird (mit LV2 immer Englisch).

:guilabel:`Soundfont im Projekt speichern`
: Der Soundfont wird im Projekt des Hosts gespeichert, das dann ohne die Datei auf einem anderen Computer geöffnet werden kann.
  Mit einer sfz-Datei ist dies nicht möglich, da ihre Samples andere Dateien sind.

:guilabel:`Kanäle und Effekte auf eigenen Ausgängen`
: Zusätzlich zum Hauptausgang empfangen die Ausgänge „Kanal 1“ bis „Kanal 16“ die Noten ihres Kanals ohne Effekt, und der Ausgang „Effekte“ empfängt den Anteil, der an Hall und Chorus gesendet wird.
  Ein Kanal, dessen Ausgang vom Host nicht aktiviert ist, bleibt im Hauptausgang.
  Ist diese Option deaktiviert (Standard), wird alles im Hauptausgang gemischt.


## Verwendung in einem Projekt {#doc_project}


Das Projekt des Hosts speichert die Parameter, die gespielten Presets, die Polyphonie und den Pfad des Soundfonts (oder seinen Inhalt, wenn er im Projekt gespeichert wird).
Beim Öffnen eines Projekts wird der Soundfont gelesen, bevor der Rest wiederhergestellt wird, sodass ein Offline-Export sofort beginnen kann.

Eine Note beginnt genau an der Position ihrer MIDI-Nachricht, unabhängig von der Puffergröße des Hosts.
Während eines Offline-Exports werden die Stimmen nie durch die Prozessorlast begrenzt.
