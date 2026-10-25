Ein Klick auf ein oder mehrere Instrumente in der [Baumansicht](manual/soundfont-editor/tree.md) zeigt die Bearbeitungsseite der Instrumente an.

Diese Seite umfasst mehrere Teile:

* die [Tabelle](#doc_table),
* den [Bereichseditor](#doc_range), wenn in der [Werkzeugleiste](manual/soundfont-editor/toolbar.md#doc_view) auf den Button :guilabel:`Bereiche` geklickt wird,
* den [Hüllkurveneditor](#doc_envelope), wenn in der [Werkzeugleiste](manual/soundfont-editor/toolbar.md#doc_view) auf den Button :guilabel:`Hüllkurven` geklickt wird,
* den [Modulatorbereich](#doc_modulator).


![Instrument-Seite](images/edit_instrument.png "Instrument-Seite")


Ist nur ein Instrument ausgewählt, können auf der Seite die globalen Parameter des Instruments und die Parameter aller seiner Aufteilungen bearbeitet werden.

Sind mehrere Instrumente ausgewählt, können die globalen Parameter jedes dieser Instrumente bearbeitet werden.


![Instrument-Seite mit mehreren ausgewählten Instrumenten](images/edit_multiple_instruments.png "Instrument-Seite mit mehreren ausgewählten Instrumenten")


## Tabelle {#doc_table}


### Überblick


Die Tabelle bietet eine Gesamtansicht der Konfiguration eines Instruments und ermöglicht deren Bearbeitung.
Eine Änderung eines Parameters in der Aufteilung :guilabel:`Global` gilt für alle Aufteilungen, außer für Aufteilungen, in denen derselbe Parameter bereits festgelegt ist (lokale Überschreibung).

Die kursiv geschriebenen Zeilen (:guilabel:`Sample-Basisnote`, :guilabel:`Samplelänge` und :guilabel:`Loop`) können nicht bearbeitet werden: Sie erinnern nur zur Information an die Eigenschaften des Samples, das mit der jeweiligen Aufteilung verknüpft ist.

Die bearbeitbaren Parameter sind:

Tastenbereich
: Tastenbereich, in dem das Sample zu hören ist.

Anschlagsbereich
: Anschlagsbereich, in dem das Sample zu hören ist.
  Für dieselbe Note können verschiedene Klänge mit unterschiedlichen Anschlagsbereichen festgelegt werden, um zum Beispiel verschiedene Nuancen einer Klaviertaste zu unterscheiden.

Dämpfung (dB)
: Auf das Sample angewendete Dämpfung in Dezibel.\
  Hinweis: Das sf2-Format erlaubt auf Instrumentebene keine negativen Werte, eine Verstärkung ist daher nicht möglich.

Pan
: Auf den Klang angewendete Balance, um ihn in Stereo links oder rechts zu hören.

Loop-Wiedergabe
: ![Loop aus](images/loop_off.png "Loop aus"){.inline} steht für einen Klang, der ohne Loop wiedergegeben wird.
  ![Loop ein](images/loop_on.png "Loop ein"){.inline} steht für einen Klang, der fortlaufend geloopt wird.
  ![Release](images/sample_mode_release.png "Release"){.inline} dient dazu, die Wiedergabe eines Samples beim Loslassen einer Taste auszulösen. Die Lautstärke wird weiterhin durch die Lautstärke-Hüllkurve bestimmt, und ein kurzer Attack (0,2 ms) wird hinzugefügt, um den Beginn des Samples beim Release zu glätten.
  ![Loop ein + Release](images/loop_on_end.png "Loop ein und Release"){.inline} steht für einen Klang, der geloopt wird, solange die Taste gedrückt ist, und danach den Rest des Samples abspielt.\
  Hinweis: Der Wiedergabemodus „Release“ ist im sf2-Format NICHT dokumentiert, daher können andere Player wie FluidSynth sich dafür entscheiden, ihn nicht zu implementieren.
  Eine andere Möglichkeit, dieses Verhalten zu erreichen, besteht darin, jedem Sample ein weiteres Sample zur Seite zu stellen, das einen kurzen stillen Loop enthält, gefolgt von dem Klang, der beim Release erklingen soll.
  Das hinzugefügte Sample wird mit dem Wiedergabeparameter „Loop ein + Release“ abgespielt.
  Siehe das Tutorial „[Eigene Releases in einem Instrument verwenden](tutorials/using-custom-releases-in-an-instrument.md)“.

Basisnote
: Ermöglicht es, die normale Basisnote eines Samples auf eine andere als die üblicherweise verwendete Note zu ändern.
  Hat ein Sample zum Beispiel die Basisnote 50 und wird es über den Tastenbereich [49 — 51] gespielt, wird es auf den Tasten 49, 50 und 51 zunächst um -1, 0 bzw. +1 Halbton transponiert.
  Wird die Basisnote dann auf Instrumentebene mit 51 überschrieben, wird dasselbe Sample auf den Tasten 49, 50 und 51 zunächst um -2, -1 bzw. 0 Halbtöne transponiert.

Tuning (Halbtöne und Cents)
: Passt die Stimmung in Halbtönen und Hundertstel Halbtönen an.
  Diese Transposition kommt zur anfänglichen Transposition hinzu, die sich aus der angeschlagenen Note ergibt (zum Beispiel beträgt die anfängliche Transposition eines Samples mit der Basisnote 60 +2 Halbtöne, wenn auf der Tastatur die Taste 62 gedrückt wird).

Stimmen (Tonleiter)
: Dieser Parameter ändert das Intervall zwischen zwei aufeinanderfolgenden Noten und wirkt sich daher auf die anfängliche Transposition aus, die sich aus der angeschlagenen Note ergibt.
  Ein Wert von 100 entspricht der gleichstufigen Stimmung: Es werden 100 Hundertstel Halbtöne verwendet, um ein Sample von einer Taste zur nächsten zu transponieren.
  Ein Wert von 0 deaktiviert diese anfängliche Transposition, sodass das Sample auf allen Tasten der Tastatur gleich klingt.

Filter
: Wendet einen Tiefpassfilter an, indem die Grenzfrequenz und die Resonanz angegeben werden.

Lautstärke-Hüllkurve
: Ändert die Lautstärke-Hüllkurve und gegebenenfalls deren Veränderung in Abhängigkeit von der Note.

Modulation
: Legt eine Modulation fest (Hüllkurve, Verzögerung, Frequenz, Veränderung in Abhängigkeit von der Note) sowie deren Wirkung auf Tonhöhe, Filter und Lautstärke.

Vibrato
: Legt ein Vibrato fest (Verzögerung, Frequenz) sowie dessen Wirkung auf die Tonhöhe.

Exklusive Klasse
: Mit diesem Parameter lassen sich eine oder mehrere exklusive Klassen festlegen, indem einer Gruppe von Aufteilungen innerhalb einer Klasse derselbe Parameterwert ungleich 0 zugewiesen wird.
  Ist eine exklusive Klasse festgelegt, beendet jede Note, die von einer der Aufteilungen der exklusiven Klasse ausgelöst wird, alle anderen Klänge derselben Klasse.
  Der Geltungsbereich einer exklusiven Klasse ist das Preset, in dem die Klasse festgelegt ist (ein in einem Instrument ausgelöster Klang beendet einen Klang eines anderen Instruments, wenn sich die beiden Instrumente im selben Preset befinden).
  Dieser Effekt kann für Perkussionsinstrumente wie ein Becken verwendet werden.
  Im Grunde beendet das Spielen einer Note automatisch eine andere Note derselben Gruppe.

Chorus und Hall
: Gibt die Intensität der Effekte Chorus und Hall an.
  Die Effekte lassen sich in den [Einstellungen](manual/settings.md#doc_sound) der Software konfigurieren.

Fixierte Note und fixierter Anschlag
: Erzwingt den Wert der Taste und des Anschlags über den gesamten Bereich der Aufteilung.

Offsets
: Ändert während der Wiedergabe die Start- und Endposition eines Klangs sowie Start und Ende des Loops.
  Die Einheit eines Offsets sind Samples (Anzahl der Werte).


### Bearbeitungshilfen in der Tabelle


Um die Bearbeitung in der Tabelle zu erleichtern und zu beschleunigen, wurden folgende Funktionen implementiert:

* mit dem Mausrad kann ein Wert geändert werden (die Zelle muss sich zuvor im Bearbeitungsmodus befinden),
* mit einer externen MIDI-Tastatur (oder der [virtuellen Tastatur](manual/soundfont-editor/toolbar.md#doc_keyboard)) lassen sich Basisnote, Tastenbereich und Anschlagsbereich festlegen (die Zelle muss sich zuvor im Bearbeitungsmodus befinden),
* eine Mehrfachauswahl, gefolgt von der Bearbeitung der letzten Zelle, bearbeitet alle Zellen gleichzeitig,
* die Tasten :kbd:`Del` und :kbd:`Backspace` löschen den Inhalt aller ausgewählten Zellen,
* Stereo-Aufteilungen können gleichzeitig bearbeitet werden, wenn die Option in den [Einstellungen](manual/settings.md#doc_general) der Software aktiviert ist,
* Notennamen können je nach der in den [Einstellungen](manual/settings.md#doc_interface) gewählten Option numerisch oder als Text angezeigt werden (mit C3, C4 oder C5 als mittlerem C),
* Kopieren / Einfügen / Ausschneiden einer Gruppe von Zellen (die nicht unbedingt nebeneinanderliegen müssen) ist mit :kbd:`Ctrl`+:kbd:`C` / :kbd:`Ctrl`+:kbd:`V` / :kbd:`Ctrl`+:kbd:`X` möglich.

Vergessen Sie nicht die verfügbaren [Werkzeuge](manual/soundfont-editor/tools/instrument-tools.md)!
Insbesondere das Werkzeug für eine [tastenbasierte Konfiguration](manual/soundfont-editor/tools/instrument-tools.md#doc_global).


### Aufteilungen stummschalten {#doc_solo}


Zu Testzwecken kann in den Tabellen eine Aufteilung stummgeschaltet werden, oder es können alle Aufteilungen bis auf eine stummgeschaltet werden.
Die Optionen sind per Rechtsklick auf den Kopf einer Aufteilung sowie im Kontextmenü der [Baumansicht](manual/soundfont-editor/tree.md#doc_menu) erreichbar.


![Aufteilungen in einer Tabelle stummschalten](images/table_mute_divisions.png "Aufteilungen in einer Tabelle stummschalten")


## Bereichseditor {#doc_range}


Mit dem Bereichseditor lassen sich Samples schnell und grafisch nach ihren Tasten- und Anschlagsbereichen anordnen.
Diese Grafik macht außerdem Konfigurationsprobleme sichtbar, etwa Zonen ohne Samples oder unerwünschte Überlagerungen.


![Bereichseditor](images/edit_range.png "Bereichseditor")


Jedes Sample wird durch ein halbtransparentes Rechteck dargestellt, sodass die Überlagerung mehrerer Elemente erkennbar ist.


### Auswahl von Rechtecken


Ein einfacher Linksklick auf ein Rechteck wählt es aus und hebt alle anderen Auswahlen auf.
Überlagern sich mehrere Rechtecke, ändern aufeinanderfolgende Klicks die Auswahl unter dem Mauszeiger.
Ein Doppelklick auf ein Rechteck öffnet das zugehörige [Sample](manual/soundfont-editor/editing-pages/sample-editor.md) (bzw. im [Preset-Editor](manual/soundfont-editor/editing-pages/preset-editor.md) das zugehörige Instrument).

Eine Mehrfachauswahl ist möglich:

* indem Sie die Taste :kbd:`Ctrl` gedrückt halten und dann nacheinander auf die verschiedenen Rechtecke klicken, um sie einzeln auszuwählen,
* mit der Taste :kbd:`Shift`: Alle Rechtecke zwischen dem zuerst angeklickten und dem nächsten werden ausgewählt.


### Rechtecke ändern


Nach der Auswahl eines oder mehrerer Rechtecke kann mit der linken Maustaste:

* die Größe geändert werden, indem einer der Ränder eines Rechtecks gezogen wird;
* die Position geändert werden, indem auf die Mitte eines Rechtecks gezeigt wird.

Sind mehrere Rechtecke ausgewählt, werden alle Änderungen gleichzeitig vorgenommen.
Die Taste :kbd:`Esc` bricht eine laufende Änderung ab.


### Verschieben & Zoomen


Durch Gedrückthalten der rechten Maustaste lässt sich der Zoom um den gewählten Punkt herum ändern.

Durch Gedrückthalten der linken Maustaste lässt sich die Grafik verschieben, wenn der Ausgangspunkt nicht auf einem Rechteck liegt.

Ein Klick mit der mittleren Maustaste spielt eine Note: Die Taste ergibt sich aus der horizontalen Position der Maus, der Anschlag aus ihrer vertikalen Position.


## Hüllkurveneditor {#doc_envelope}


Der Hüllkurveneditor bietet einen grafischen Überblick über die 8 Parameter, die die Lautstärke-Hüllkurve festlegen, und die 8 Parameter, die die Modulations-Hüllkurve festlegen.
Er ist damit ein Werkzeug, um die Auswirkung jedes dieser Parameter besser zu verstehen.


![Hüllkurveneditor](images/edit_envelope.png "Hüllkurveneditor")


Mit den Buttons über der Grafik wird abwechselnd die Lautstärke-Hüllkurve (grün dargestellt) oder die Modulations-Hüllkurve (rot dargestellt) zur Bearbeitung ausgewählt.
Während der Bearbeitung der Modulations-Hüllkurve wird die Lautstärke-Hüllkurve als dünne Linie angezeigt.

Der linke Teil der Grafik stellt den Verlauf der Hüllkurve dar, wenn eine Taste **gedrückt** wird.
7 der 8 Parameter sind an dieser Darstellung beteiligt:

Delay
: Dauer, während der die Hüllkurve direkt nach dem Drücken einer Taste bei 0 bleibt. In den meisten Fällen möchte man den Klang sofort nach dem Drücken der Tasten hören, daher wird dieser Parameter auf null belassen. Das **Delay** ist jedoch zum Beispiel nützlich, wenn man reichhaltigere Pad-Klänge aus mehreren Schichten aufbauen möchte. Angenommen, eine zweite Klangschicht soll 0,5 Sekunden nach dem Drücken der Taste einsetzen, dann kann man dies festlegen, indem man in diesem Feld „0,5“ eingibt. Ein Delay hält die Wiedergabe des Klangs nicht an – es schaltet den Ton stumm. Wurde das Delay auf 0,5 Sekunden gesetzt, hört man das Sample so, wie es nach 0,5 Sekunden Wiedergabe klingt; die ersten 0,5 Sekunden wurden stumm abgespielt.

Attack
: Dauer nach dem **Delay**, während der die Hüllkurve von 0 auf den Maximalwert ansteigt. Zum Beispiel hat ein gestrichener Saitenklang wie eine Violine einen recht langen Attack, während ein gezupfter Saitenklang wie eine Gitarre einen sehr kurzen hat. Selbst wenn man einen möglichst kurzen Attack verwenden möchte, empfiehlt es sich, mindestens den Wert 0,001 einzugeben, da sonst nach dem Drücken einer Taste ein „Klicken“ zu hören sein könnte.

Hold
: Dauer nach dem **Attack**, während der die Hüllkurve ihren Maximalwert beibehält.

Decay
: Dauer nach dem **Hold**, während der die Hüllkurve allmählich vom Maximalwert auf den durch **Sustain** festgelegten Wert absinkt. Ist bei **Sustain** keine Absenkung festgelegt (0 dB, der Standardwert), bleibt die Lautstärke während des **Decay** unabhängig vom angegebenen Wert auf ihrem Maximum.

Sustain
: Wert nach dem **Decay**, der gehalten wird, solange die gedrückte Taste nicht losgelassen wird. Wie die **Dämpfung** ist der Sustain-Wert eine in Dezibel ausgedrückte Absenkung. Soll die Lautstärke nach der Decay-Zeit halbiert werden, muss ein Wert von 6 dB eingestellt werden. Der Wert „144“ kann angegeben werden, wenn der Klang vollständig stummgeschaltet werden soll.

Key → Hold
: Änderung der **Hold**-Dauer in Abhängigkeit von der Taste (siehe unten).

Key → Decay
: Änderung der **Decay**-Dauer in Abhängigkeit von der Taste (siehe unten).

Ein positiver Wert bei **Key → XXX** verkürzt die Dauer von XXX zu den Höhen hin allmählich, ein negativer Wert bewirkt das Gegenteil.
Zum Beispiel:

* der Wert 100 halbiert die Dauer von XXX pro Oktave oberhalb des mittleren C (60) und verdoppelt die Dauer von XXX pro Oktave unterhalb des mittleren C,
* der Wert -100 verdoppelt die Dauer von XXX pro Oktave oberhalb des mittleren C (60) und halbiert die Dauer von XXX pro Oktave unterhalb des mittleren C.

Diese Parameter können beim Erstellen einer Gitarre oder eines Klaviers mit getretenem Sustain-Pedal nützlich sein: Je tiefer der Ton, desto länger dauert es, bis nichts mehr zu hören ist.

Der rechte Teil stellt den Verlauf der Hüllkurve dar, wenn eine Taste **losgelassen** wird.
Nur die **Release**-Dauer ist an dieser Darstellung beteiligt.
Während dieser Dauer kehrt die Hüllkurve allmählich auf 0 zurück.
Violinen oder Synthesizer-Pads haben eine relativ lange Release-Zeit, während die anderer Klänge wie Orgeln sehr kurz ist. In der Praxis empfiehlt es sich, selbst für kurze Release-Zeiten mindestens 0,2 Sekunden einzustellen, da eine zu kurze Release-Zeit weder angenehm noch natürlich klingt (dies hängt jedoch auch davon ab, was man sich für das zu erstellende Instrument vorstellt).

Ist in der Baumansicht nur eine Aufteilung eines Instruments ausgewählt, wird das zugehörige [Sample](manual/soundfont-editor/editing-pages/sample-editor.md) im Hintergrund angezeigt.
So lässt sich der Verlauf der Hüllkurve im Verhältnis zum abgespielten Sample besser erkennen.


## Modulatoren {#doc_modulator}


Der untere Bereich der Bearbeitungsseite der Instrumente dient zum Erstellen von Modulatoren, global für ein Instrument oder für eine bestimmte Aufteilung.
Beim Spielen eines Instruments wertet ein Modulator ein oder zwei MIDI-Signale aus und ändert gemäß der konfigurierten mathematischen Regel einen der Parameter der Tabelle.
Der Ausgang eines Modulators kann auch als Eingang eines anderen Modulators verwendet werden (diese Funktion wurde mit Version 2.04 des [sf2-Formats](manual/annexes/the-different-soundfont-formats.md#doc_sf2) eingeführt und wird von manchen Synthesizern möglicherweise nicht unterstützt).


![Modulator-Editor](images/modulator_editor_2.png "Modulator-Editor")


Die Buttons auf der linken Seite dienen zum:

* Ausklappen / Einklappen des Modulatorbereichs,
* Hinzufügen / Entfernen eines Modulators,
* Kopieren / Einfügen / Duplizieren eines oder mehrerer Modulatoren.

Wird ein Modulator hinzugefügt, wird der Rand der zugehörigen Zelle in der Tabelle dicker (siehe die Stimmung in der Spalte :guilabel:`Global` oben).

Die Modulatoren einer Aufteilung (oder der globalen Aufteilung) werden nur angezeigt, wenn eine einzelne Aufteilung ausgewählt ist.
Ist der Bereich eingeklappt, listet eine Zusammenfassung die Anzahl der Modulatoren und die von ihnen geänderten Parameter auf.
Die Auswahl von Zellen in der Tabelle wählt die Modulatoren aus, die auf die entsprechenden Parameter wirken, und umgekehrt.

Jeder Modulator besteht von links nach rechts aus folgenden Elementen:

Quellen
: Die erste Quelle ist ein MIDI-Signal: der Anschlag oder die Taste der gespielten Note, der polyphone Druck, der Kanaldruck, das Pitch-Rad, die Empfindlichkeit des Pitch-Rads oder ein MIDI-Controller (:guilabel:`CC`).
  Sie kann auch der Ausgang eines oder mehrerer anderer Modulatoren sein (:guilabel:`Link`), nämlich derjenigen, die diesen Modulator als Ziel haben.
  Die zweite Quelle ist optional und verändert die Wirkung der ersten.

Kurven
: Neben jeder Quelle öffnet ein Button ein Raster von Kurven, mit dem festgelegt wird, wie das Signal umgewandelt wird: linear, konkav, konvex oder Schalter, steigend oder fallend, unipolar (von 0 bis 1) oder bipolar (von -1 bis 1).

Stärke
: Maximale Amplitude der Modulation.

Umwandlung
: :guilabel:`keine Funktion` oder :guilabel:`Absoluter Wert` (es wird der Absolutwert des Ergebnisses der Modulation verwendet).

Ziel
: Der vom Modulator geänderte Parameter oder ein anderer Modulator (:guilabel:`Modulator`).
  Der Text unter dem Ziel gibt den Bereich der Modulation an: die zum Parameter addierten Werte (:guilabel:`Addieren von:` … :guilabel:`Bis:`) oder die auf ihn angewendeten Faktoren (:guilabel:`Multiplizieren von:` … :guilabel:`Bis:`).

Ist ein Modulator identisch mit einem weiter oben in der Liste stehenden, wird die Beschriftung :guilabel:`überschrieben von …` angezeigt.
Auf Instrumentebene kennzeichnet die Beschriftung :guilabel:`ersetzt StandardMod.` einen Modulator, der einen der [Standardmodulatoren](manual/soundfont-editor/tools/instrument-tools.md#doc_default_mod) des sf2-Formats ersetzt, und :guilabel:`deaktiviert StandardMod.` zeigt an, dass seine Stärke 0 ist (der Standardmodulator ist dann ausgeschaltet).
