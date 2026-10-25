Polyphone unterstützt mehrere Soundfont-Formate:

* [sf2](#doc_sf2),
* [sf3](#doc_sf3),
* [sfz](#doc_sfz),
* [sfArk](#doc_sfark),
* [organ](#doc_organ).


## Sf2-Soundfonts {#doc_sf2}


Sf2-Soundfonts sind die wichtigsten Dateien, die mit Polyphone bearbeitet werden.
Jede sf2-Datei enthält ein oder mehrere virtuelle Musikinstrumente, die aus Audiosamples und zahlreichen Parametern bestehen.
Die Parameter legen fest, wie die Samples über die Tastatur hinweg abgespielt werden sollen, gegebenenfalls moduliert durch vordefinierte Signale ([Modulatoren](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator)).
Sf2-Dateien sind in 3 Ebenen aufgebaut:

* [Samples](manual/soundfont-editor/editing-pages/sample-editor.md), zum Beispiel von einer Trompete oder einem Klavier,
* [Instrumente](manual/soundfont-editor/editing-pages/instrument-editor.md), die aus Samples bestehen,
* [Presets](manual/soundfont-editor/editing-pages/preset-editor.md), die aus Instrumenten bestehen.

Neben der Definition und Einstellung dieser Elemente enthält eine sf2-Datei auch [allgemeine Informationen](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md) (zum Beispiel Autor, Copyright oder Bearbeitungszeitpunkt).

Soundfonts werden von Software-Synthesizern wie <a href="http://www.fluidsynth.org/" target="_blank">FluidSynth</a> verwendet, die über MIDI-Signale gesteuert werden.
Mit sf2-Soundfonts lassen sich auch MIDI-Dateien mit <a href="https://sourceforge.net/projects/timidity" target="_blank">TiMidity</a>, <a href="https://www.mindwerks.net/projects/wildmidi" target="_blank">WildMIDI</a> oder QuickTime anhören und Partituren mit <a href="https://musescore.org/" target="_blank">MuseScore</a> abspielen.
Auch Hardware wie Sampler, Soundkarten oder sogar Synthesizer kann das .sf2-Format unterstützen.

Weitere Informationen zu Soundfonts finden sich zum Beispiel auf <a href="https://de.wikipedia.org/wiki/SoundFont" target="_blank">Wikipedia</a>.
Die sf2-Spezifikationen sind [hier](files/sf_specifications_v2.01.pdf) für Version 2.01 und [dort](files/sf_specifications_v2.04.pdf) für Version 2.04 verfügbar.


## Sf3-Soundfonts {#doc_sf3}


Das von <a href="https://musescore.org/" target="_blank">MuseScore</a> entwickelte sf3-Format entspricht in jeder Hinsicht dem sf2-Format, mit dem Unterschied, dass die [Samples](manual/soundfont-editor/editing-pages/sample-editor.md) im OGG-Format (ähnlich wie MP3, aber quelloffen) statt als Rohdaten gespeichert werden können.
Dadurch ist das sf3-Format bei vergleichbarer Qualität etwa 10-mal kleiner als das sf2-Format.
In Polyphone wird die Kompression in den [allgemeinen Informationen](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md#doc_edit) des Soundfonts aktiviert; anschließend kann jedes Sample im [Sample-Editor](manual/soundfont-editor/editing-pages/sample-editor.md#doc_info) komprimiert werden (die Kompressionsqualität ist wählbar).

Die komprimierten Samples eines in diesem Format exportierten Soundfonts sind nicht dafür gedacht, später bearbeitet zu werden, da wiederholte Kompressionen zu einer schlechteren Klangqualität führen würden.
Dieses Format ist als Endprodukt zu betrachten und für alle Endnutzer von Soundfonts sehr interessant, denn:

* Downloads aus dem Internet werden einfacher,
* auf dem Computer wird Speicherplatz gespart,
* die Kombination MIDI + sf3 bietet eine schlanke und leistungsfähige Lösung zum Abspielen von Soundtracks (Videospiele, mobile Apps, …).

Im Gegensatz zum Format <a href="http://www.fmjsoft.com/fmt/sf2pack.htm" target="_blank">sf2pack</a>, das dieselben Probleme löst, ist das sf3-Format vollständig quelloffen.
Der Quellcode ist verfügbar, was seine Zukunftssicherheit gewährleistet (vermeiden wir die Fehler, die mit sfArk oder sfPack gemacht wurden!).


## Sfz-Soundfonts {#doc_sfz}


Das sfz-Format verfolgt dasselbe Ziel wie das sf2-Format: ein Musikinstrument zu erstellen, indem Samples über die Tastatur angeordnet und konfiguriert werden.
Der Hauptunterschied besteht darin, dass das sf2-Format nur aus 1 Datei besteht, die alles enthält, während das sfz-Format eine Textdatei ist, die zusammen mit einer Reihe von .wav-Samples geliefert wird.
Da es von Menschen bearbeitbar sein sollte, bestand der Hauptvorteil darin, die Datei ohne einen komplexen Editor bearbeiten zu können.
Für große Instrumente ist ein Editor dennoch sehr zu empfehlen, denn die Menge an Parametern kann schnell entmutigend sein.
Das sfz-Format ist außerdem nicht so streng definiert wie das sf2-Format: Bei der Bearbeitung und bei der Wiedergabe eines sfz-Instruments können Unterschiede auftreten. Gleichzeitig wurden die Möglichkeiten von sfz-Instrumenten gegenüber dem sf2-Standard erweitert.


### Einschränkungen beim sfz-Import


* Es werden nur Opcodes importiert, die eine Entsprechung in den [Tabellen](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) haben.
* Da die auf Tonhöhe und Filter angewendete Hüllkurve in sf2-Dateien dieselbe ist (Mod env), können keine unterschiedlichen Hüllkurven für Tonhöhe und Filter importiert werden (Opcodes ``fileg_*`` und ``pitcheg_*``).
* Da der auf Lautstärke und Filter angewendete LFO in sf2-Dateien derselbe ist (Mod LFO), können keine unterschiedlichen LFOs für Lautstärke und Filter importiert werden (Opcodes ``amplfo_*`` und ``fillfo_*``).
* Der einzige Filter, der importiert werden kann, ist der Tiefpassfilter zweiter Ordnung (``fil_type=lpf_2p``).
* Um korrekt importiert zu werden, müssen die Opcodes ``group`` und ``off_by`` innerhalb derselben Gruppe denselben Wert enthalten.

**Warnung:** Die Opcodes ``width`` und ``position`` werden möglicherweise nicht korrekt interpretiert.


### Einschränkungen beim sfz-Export


* [Modulatoren](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) werden derzeit nicht exportiert.
* Da das sfz-Format (Version 1) nur eine einzige Tonhöhenmodulation erlaubt, können die beiden Parameter :guilabel:`Vib LFO → Pitch (c)` und :guilabel:`Mod LFO → Pitch (c)` nicht gleichzeitig exportiert werden.
* Ein Loop 0-1 kann exportiert werden, wird aber von den meisten sfz-Playern ignoriert.
* 24-Bit-Samples werden von manchen Synthesizern möglicherweise nicht unterstützt; ziehen Sie dazu deren Spezifikationen zu Rate.


### Allgemein


Die Übereinstimmung der Pegel (in dB) bei der Umwandlung sfz / sf2 wurde mit der Software Sforzando getestet.
Es können jedoch einige Unterschiede bestehen bleiben.

Außerdem gibt es für die Art, wie sich Parameter in Abhängigkeit von der Taste ändern (über :guilabel:`Key → Vol env hold / decay` und :guilabel:`Key → Mod env hold / decay`), keine exakte Entsprechung.
Das sf2-Format verwendet einen exponentiellen Verlauf, um die Decay- und Hold-Dauer in Abhängigkeit von der Taste festzulegen, während das sfz-Format einen linearen Verlauf verwendet.


## SfArk-Archive {#doc_sfark}


SfArk-Archive sind wie das sf3-Format ein komprimiertes Format.
Das bedeutet, dass die Qualität der Klänge etwas verringert ist (oft aber nicht wahrnehmbar).
Dieses Format gilt zunehmend als veraltet, weshalb Polyphone sfArk-Archive nur importieren kann (kein Export).


## GrandOrgue-Dispositionen {#doc_organ}


GrandOrgue ist ein Programm, dessen Hauptzweck die Emulation von Instrumenten wie Pfeifenorgeln ist. Eine Orgel wird durch eine Reihe von WAV-Dateien, die jeweils den Klang einer Pfeife enthalten, und eine Datei mit der Endung „.organ“ definiert, die alle Parameter enthält, die zum Aufbau einer Orgel aus allen WAV-Dateien nötig sind. Im Wesentlichen sind alle Samples bestimmten Tasten zugeordnet und nach Registern gruppiert, die beim Spielen der Orgel ein- oder ausgeschaltet werden können. Polyphone kann auf Grundlage dieser Zuordnung Soundfonts erstellen, unterstützt aber nicht die Umwandlung von Soundfonts in das GrandOrgue-Format.

### Einschränkung beim Import

Transpositionen werden derzeit nicht unterstützt.


*[MIDI]: Musical Instrument Digital Interface
