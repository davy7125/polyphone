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
Beim Öffnen eines Soundfonts wird sofort seine Struktur gelesen: Die Instrumente werden aufgelistet und können gespielt werden.
Seine Samples werden danach im Hintergrund gelesen, mit einem Ladesymbol vor seinem Namen (sein Tooltip zeigt den Fortschritt): Eine Note, deren Samples noch nicht gelesen sind, bleibt stumm.
Die Samples der gespielten Noten und der gewählten Instrumente werden zuerst gelesen, wobei die Samples einer sf3-Datei dekomprimiert werden.
Sobald alles gelesen ist, wird während des Spielens nichts von der Festplatte gelesen; ein Offline-Export wartet, bis alle Samples gelesen sind.
Spielen mehrere Instanzen des Plugins dieselbe Datei (zum Beispiel eine pro Spur), wird die Datei nur einmal gelesen und im Speicher gehalten.
Hat sich die Datei geändert, zum Beispiel nachdem sie in Polyphone erneut gespeichert wurde, wird sie erneut gelesen.


## Spielmodi {#doc_modes}


Drei Schaltflächen über der Liste der Instrumente (die Presets des Soundfonts) legen fest, wie die Instrumente gespielt werden:

:guilabel:`Ein Instrument`
: Das in der Liste ausgewählte Instrument wird auf den 16 MIDI-Kanälen gespielt.
  Die Instrumente werden mit ihrer Bank- und Programmnummer angezeigt, zum Beispiel „000:024 Guitar“.

:guilabel:`16 Kanäle`
: Jeder MIDI-Kanal spielt sein eigenes Instrument, das in seiner Zeile gewählt wird.
  Die Auswahl :guilabel:`Programmwechsel` spielt das Instrument, das durch die vom Kanal empfangenen Bank-Select- und Program-Change-Nachrichten gewählt wird, wie es ein General-MIDI-Synthesizer tut: Auf Kanal 10 wird dann die Bank 128 für das Schlagzeug verwendet.
  Das gespielte Instrument wird rechts in jeder Zeile angezeigt.
  Dieser Modus eignet sich zum Abspielen einer MIDI-Datei, die für mehrere Instrumente arrangiert ist.

:guilabel:`MPE`
: Das ausgewählte Instrument wird von einem MPE-Controller (MIDI Polyphonic Expression) gespielt: Jede Note hat ihren eigenen Kanal und damit ihr eigenes Pitch Bend, ihren eigenen Druck und ihre eigene Klangfarbe (CC 74).
  Eine untere Zone mit 15 Kanälen wird verwendet, bis der Controller seine Konfiguration sendet (RPN 6).
  Der Master-Kanal einer Zone wirkt auf alle ihre Noten: Sein Pitch Bend wird zu dem jeder Note addiert, und seine Controller gelten für alle Noten.
  Der Pitch-Bend-Bereich beträgt 48 Halbtöne für die Noten und 2 Halbtöne für den Master-Kanal und kann vom Controller geändert werden (RPN 0).


![Plugin im Modus „16 Kanäle“](images/plugin_channels.png "Plugin im Modus „16 Kanäle“")


## Parameter {#doc_parameters}


Die Parameter sind die des Synthesizers von Polyphone, die sich auch in seinen Einstellungen [Sound](manual/settings.md#doc_sound) und [Tastatur](manual/settings.md#doc_keyboard) finden.
:guilabel:`Verstärkung` (von -24 bis +24 dB) befindet sich in der Kopfzeile, die anderen Parameter sind rechts gruppiert:

:guilabel:`Note`
: Der Spielmodus, in der Liste der ersten Zeile (siehe die [Spielmodi](manual/plugin.md#doc_play_modes)): :guilabel:`Poly` (Standard), :guilabel:`Mono` oder :guilabel:`Legato`, wobei das Gleiten des Legato rechts davon eingestellt wird.
  :guilabel:`Referenztonhöhe`: eine Note von A#3 bis A4 und ihre Frequenz (von 400 bis 480 Hz für A4, standardmäßig 440 Hz); diese Note klingt unabhängig von der Temperatur mit dieser Frequenz.
  :guilabel:`Transposition`: von -24 bis +24 Halbtönen, wobei das Schlagzeug nicht transponiert wird.

:guilabel:`Anschlag`
: Die Reaktion auf den Anschlag der Noten, wobei jede Einstellung in einer Grafik dargestellt wird (Anschlag von 0 bis 127 von links nach rechts):
  
  * :guilabel:`Filterfrequenz`: Standardmodulator, der die Grenzfrequenz des Filters je nach Anschlag um bis zu 2 Oktaven senkt, wie in den [Sound](manual/settings.md#doc_sound)-Einstellungen von Polyphone. Die Version 2.04 des SoundFont-Formats (Standard) senkt sie umso stärker, je schwächer der Anschlag ist. Die Version 2.01 hatte einen Sprung bei Anschlag 64, einen Fehler, den die Version 2.04 behoben hat: Sie ist ein Kompatibilitätsmodus für alte Soundfonts, die diesen Sprung ausgleichen. Mit :guilabel:`Deaktiviert` hängt die Grenzfrequenz nicht vom Anschlag ab. Sie hat keine Wirkung auf einen Soundfont mit eigenen [Standardmodulatoren](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md#doc_default_mod).
  * :guilabel:`Kurve`: Kurve, die auf den Anschlag der empfangenen Noten angewendet wird, linear in der Mitte (Standard, durch Doppelklick wiederhergestellt), konkav nach links (weicherer Anschlag: hohe Anschlagswerte erfordern einen kräftigeren Anschlag) und konvex nach rechts (der Anschlag steigt schnell).

:guilabel:`Temperaturen und Tonleitern`
: Die Temperaturen und Tonleitern von Polyphone in ihren Kategorien, dann die benutzerdefinierte Temperatur, transponiert in die gewählte :guilabel:`Tonart`.
  Die Tastatur rechts zeigt die Feinstimmung jeder Note in Cent und wird wie in den [Tastatureinstellungen](manual/settings.md#doc_keyboard) von Polyphone bearbeitet: Mausrad oder vertikales Ziehen, Doppelklick zum Zurücksetzen auf 0, Kontextmenü zum Ändern der Verwendung einer Note oder zum Legen der Tonart auf sie. Eine bearbeitete Temperatur wird zur benutzerdefinierten Temperatur.
  Die von einer Tonleiter nicht verwendeten Noten werden nicht gespielt, außer wenn :guilabel:`Alle spielen` aktiviert ist.

:guilabel:`Hüllkurve`
: Attack, Decay, Sustain und Release der Instrumente, verändert (:guilabel:`Relativ`) oder ersetzt (:guilabel:`Fest`), siehe die [Hüllkurve](manual/plugin.md#doc_envelope).

:guilabel:`Halleffekt`
: :guilabel:`Intensität`, :guilabel:`Größe`, :guilabel:`Weite` und :guilabel:`Dämpfung`.

:guilabel:`Chorus`
: :guilabel:`Intensität`, :guilabel:`Tiefe` und :guilabel:`Frequenz`.

Die Gruppen :guilabel:`Temperaturen und Tonleitern`, :guilabel:`Hüllkurve`, :guilabel:`Halleffekt` und :guilabel:`Chorus` haben rechts neben ihrem Titel einen Schalter: Ausgeschaltet ist die Gruppe eingeklappt und ihre Temperatur, ihre Hüllkurve oder ihr Effekt wird nicht angewendet (gleichstufige Stimmung mit allen Noten, Hüllkurven des Soundfonts, kein Effekt), ihre Einstellungen bleiben erhalten.
Die Höhe des Fensters folgt den ausgeklappten Gruppen.

Der Host kann alle diese Parameter automatisieren, außer der Note der Referenztonhöhe (automatisiert wird die Frequenz von A4) und den Feinstimmungen der benutzerdefinierten Temperatur, die mit dem Projekt gespeichert werden.


## Spielmodi {#doc_play_modes}


Die Liste in der ersten Zeile der Gruppe :guilabel:`Note` legt fest, wie die Noten jedes MIDI-Kanals gespielt werden:

:guilabel:`Poly`
: Die Noten werden zusammen gespielt (Standard).

:guilabel:`Mono`
: Eine Note nach der anderen: Eine Note beginnt mit ihrem Attack, die vorherige endet schnell.

:guilabel:`Legato`
: Eine Note nach der anderen: Eine Note, die vor dem Loslassen der vorherigen gespielt wird, setzt diese ohne neuen Attack fort.
  Enthält eine Aufteilung, die die vorherige Note spielt, auch die neue Taste, spielt ihre Stimme mit demselben Sample weiter, nur die Tonhöhe ändert sich.
  Die anderen Samples der neuen Taste beginnen am Anfang ihres Loops (an ihrem Anfang, wenn sie keinen Loop haben), mit einer Überblendung.
  Der Balken rechts von der Liste stellt das Gleiten ein: :guilabel:`Aus` (Standard, die Tonhöhe ändert sich sofort) oder die Dauer des Gleitens der Tonhöhe von der vorherigen Note aus, bis zu 5 s.
  Eine Note, die nach dem Loslassen der vorherigen gespielt wird, beginnt wie im Modus :guilabel:`Mono` mit ihrem Attack.

In Klammern steht die Taste, die erklingt, wenn mehrere Tasten gehalten werden: die letzte, die höchste oder die tiefste.
Wird die erklingende Taste losgelassen, während andere noch gehalten werden, erklingt wieder die Note derjenigen, die Vorrang hat: D halten und E wiederholen spielt einen Triller, im Legato gleitend.
Das Schlagzeug (Bank 128) und der Modus :guilabel:`MPE` bleiben polyphon.


## Hüllkurve {#doc_envelope}


Die Gruppe :guilabel:`Hüllkurve` verändert die Lautstärkehüllkurve aller gespielten Instrumente außer dem Schlagzeug (Bank 128), ohne den Soundfont zu bearbeiten.
Ihr Modus wird links neben ihrem Schalter gewählt, wobei jeder Modus seine eigenen Werte behält:

:guilabel:`Relativ`
: Die Hüllkurve jedes Klangs des Soundfonts wird verändert (Standard): :guilabel:`Attack`, :guilabel:`Decay` und :guilabel:`Release` multiplizieren ihre Zeiten mit ×0,10 bis ×10, und :guilabel:`Sustain` ändert ihren Sustain-Pegel um -20 bis +20 dB (bis zum Höchstpegel).
  Jeder Klang behält die Form seiner Hüllkurve: Ein Release, das sich mit den Tasten oder den Velocity-Layern ändert, ändert sich weiterhin.
  Die Zeiten der Modulationshüllkurve (Filter und Tonhöhe) ändern sich im gleichen Verhältnis, sodass die Klangfarbe eines Klangs, dessen Filter sich mit dem Attack öffnet, dem neuen Attack folgt.
  Ein Faktor von ×1,00 und eine Änderung von 0 dB behalten den Soundfont bei.

:guilabel:`Fest`
: Die Werte ersetzen die des Soundfonts: :guilabel:`Attack` von 1 ms bis 10 s, :guilabel:`Decay` und :guilabel:`Release` von 1 ms bis 20 s, :guilabel:`Sustain` von -96 bis 0 dB.
  Ein ganz nach links gedrehter Regler (:guilabel:`Soundfont`) behält den Wert des Soundfonts bei, um zum Beispiel nur das Release zu ersetzen.
  Die Modulationshüllkurve wird nicht verändert.

Die Grafik rechts zeichnet die Hüllkurve, wobei die Zeiten gestaucht sind, damit kurze und lange Phasen sichtbar sind.
Im Modus :guilabel:`Relativ` wird unter der gespielten Hüllkurve ein Beispiel einer Hüllkurve eines Soundfonts gestrichelt gezeichnet; im Modus :guilabel:`Fest` werden die Phasen, die den Soundfont beibehalten, gestrichelt gezeichnet.

Ein Sample ohne Loop (zum Beispiel ein Klavier) endet an seinem Ende, unabhängig von seiner Hüllkurve: Ein längeres Decay oder ein höheres Sustain kann es nicht länger klingen lassen.
Die Modulatoren des Soundfonts wirken weiterhin (zum Beispiel eine Velocity, die den Attack verkürzt).
Die Änderungen wirken sofort auf die klingenden Noten, sodass die Hüllkurve automatisiert werden kann: Eine gehaltene Note übernimmt den neuen Sustain-Pegel und wird mit dem zu diesem Zeitpunkt eingestellten Release losgelassen.
Ein Doppelklick auf einen Regler stellt seinen Standardwert wieder her (den Soundfont).


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

:guilabel:`Feste Latenz (Noten an ihrer genauen Position gespielt)`
: Ist diese Option deaktiviert (Standard), werden die MIDI-Nachrichten eines Audioblocks gemeinsam vorgezogen, sodass die erste Note des Blocks an seinem Anfang beginnt: Live gespielte Noten erklingen im Durchschnitt um eine halbe Pufferlänge früher, ihre Latenz hängt jedoch von ihrer Position im Puffer des Hosts ab.
  Ist sie aktiviert, wird jede Nachricht an ihrer genauen Position verarbeitet: Die Latenz der live gespielten Noten ist konstant.
  Während eines Offline-Exports werden die Nachrichten immer an ihrer genauen Position verarbeitet.

:guilabel:`Clipping auf [-1; 1]`
: Die Ausgänge werden auf das Intervall [-1; 1] begrenzt (Standard), wie in Polyphone.
  Ohne Clipping werden größere Werte an den Host übergeben, der sie als Gleitkommazahlen mischt: Die Verstärkung kann dann ohne Übersteuerung erhöht werden, wobei der Pegel vor dem Ausgang des Hosts angepasst wird.

:guilabel:`Soundfont im Projekt speichern`
: Der Soundfont wird im Projekt des Hosts gespeichert, das dann ohne die Datei auf einem anderen Computer geöffnet werden kann.
  Mit einer sfz-Datei ist dies nicht möglich, da ihre Samples andere Dateien sind.

:guilabel:`Kanäle und Effekte auf eigenen Ausgängen`
: Zusätzlich zum Hauptausgang empfangen die Ausgänge „Kanal 1“ bis „Kanal 16“ die Noten ihres Kanals ohne Effekt, und der Ausgang „Effekte“ empfängt den Anteil, der an Hall und Chorus gesendet wird.
  Ein Kanal, dessen Ausgang vom Host nicht aktiviert ist, bleibt im Hauptausgang.
  Ist diese Option deaktiviert (Standard), wird alles im Hauptausgang gemischt.


## Verwendung in einem Projekt {#doc_project}


Das Projekt des Hosts speichert die Parameter, die gespielten Instrumente, die Polyphonie und den Pfad des Soundfonts (oder seinen Inhalt, wenn er im Projekt gespeichert wird).
Beim Öffnen eines Projekts wird nur die Struktur des Soundfonts gelesen, bevor der Rest wiederhergestellt wird: Der Host wartet nicht auf die Samples, und ein Offline-Export kann sofort beginnen (er wartet auf die noch nicht gelesenen Samples).

Eine Note beginnt genau an der Position ihrer MIDI-Nachricht, unabhängig von der Puffergröße des Hosts, während eines Offline-Exports und in Echtzeit mit [fester Latenz](manual/plugin.md#doc_settings).
Während eines Offline-Exports werden die Stimmen nie durch die Prozessorlast begrenzt.
