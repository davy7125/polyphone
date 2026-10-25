Diese Seite listet typische Mängel in Soundfonts auf (die in Bewertungen angemerkt werden können) und bietet für jeden von ihnen Techniken und Links zur Dokumentation, um sie zu beheben:

* [minderwertige oder schlecht vorbereitete Samples](#doc_sample),
* [falsche Stimmung](#doc_tuning),
* [ungeeignete ADSR-Hüllkurve](#doc_adsr),
* [fehlende oder ungenaue Loops](#doc_loop),
* [MIDI-Standard nicht eingehalten](#doc_standard).


## Minderwertige oder schlecht vorbereitete Samples {#doc_sample}


Es kann vorkommen, dass ein Soundfont keine überzeugenden Instrumente bietet. Liegt die Ursache in minderwertigen Samples, besteht die einzige Lösung darin, sie zu ersetzen:

* online nach anderen Samples suchen,
* mit einem hochwertigeren Mikrofon aufnehmen (oder externe Werkzeuge zur Signalverarbeitung verwenden),
* Samples auf anderem Wege erstellen.

Manchmal lässt sich das Problem lösen, indem die vorhandenen Samples besser vorbereitet oder eingesetzt werden.

### Schlechte Klangqualität, besonders in den Höhen

Ein einzelnes Audiosample reicht in der Regel nicht aus, um ein hochwertiges Instrument zu erstellen. Ein Sample hat seine eigene Tonhöhe (außer bei nicht melodischer Perkussion), und wird es über einen großen Tonbereich verwendet, verzerrt sich der Klang, besonders in den hohen Noten. Um dieses Problem zu beheben, sollten mehrere Samples verwendet werden. Steht nur ein Sample zur Verfügung, können Sie:

* das Sample duplizieren,
* das duplizierte Sample zum Beispiel mit diesem [Werkzeug](manual/soundfont-editor/tools/sample-tools.md#doc_transpose) um eine Oktave transponieren,
* hohe Obertöne mit diesem [Werkzeug](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq) oder [jenem](manual/soundfont-editor/tools/sample-tools.md#doc_filter) herausfiltern.

Die Samples sollten dann Tonbereichen zugeordnet werden, in denen ihre Wiedergabe die ursprüngliche Klangfarbe nicht verändert.

### Der Klang enthält regelmäßige „Klicks“

Das Problem rührt wahrscheinlich von einem schlecht gesetzten Loop-Punkt her, was in einem anderen [Abschnitt](#doc_loop) behandelt wird.

### Schlecht gekürzte Samples

Samples werden oft nur teilweise genutzt, und das Entfernen des Überschusses kann die Dateigröße verringern:

* der nützliche Teil eines Samples liegt manchmal vor dem letzten Loop-Punkt: Der Teil des Samples nach dem Loop-Ende kann daher gelöscht werden, und mit diesem [Werkzeug](manual/soundfont-editor/tools/sample-tools.md#doc_trimloop) geht das mit wenigen Klicks;
* Samples können mit Stille beginnen, die entfernt werden sollte, um die Latenz beim Auslösen einer Note zu verringern (auch dafür gibt es ein [Werkzeug](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank));
* schließlich werden manche Samples möglicherweise gar nicht verwendet: Diese [Konfigurationsübersicht](manual/soundfont-editor/configuration-summaries.md#doc_sample) listet sie auf, und dieses [Werkzeug](manual/soundfont-editor/tools/global-tools.md#doc_unused) entfernt sie automatisch.


## Falsche Stimmung {#doc_tuning}


Das Stimmen eines virtuellen Instruments umfasst mehrere Schritte, und es ist wichtig, sie alle der Reihe nach durchzugehen.

### Stimmung auf Sample-Ebene

Dies ist der wichtigste Schritt beim Stimmen; er wird auf dieser [Seite](tutorials/how-to-prepare-a-sample.md#doc_tuning) beschrieben.

### Stimmung auf Instrumentebene

4 Parameter, die auf dieser [Seite](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) beschrieben werden, legen fest, mit welcher Transposition ein Sample abgespielt wird: **Basisnote**, **Tuning (Halbtöne)**, **Tuning (Cents)** und **Stimmen (Tonleiter)**. Diese Werte können während der Wiedergabe des Instruments durch [Modulatoren](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) verändert werden, die daher ebenfalls sorgfältig überprüft werden sollten.

Sind für die mit einem Sample verknüpfte Instrument-Aufteilung keine Parameter oder Modulatoren festgelegt, sollte geprüft werden, ob in der globalen Aufteilung des Instruments Parameter oder Modulatoren festgelegt sind (diese Aufteilung bestimmt die Standardkonfiguration für alle anderen Aufteilungen).

### Stimmung auf Preset-Ebene

Auf Preset-Ebene beeinflussen 3 Parameter die Transposition eines Samples: **Tuning (Halbtöne)**, **Tuning (Cents)** und **Stimmen (Tonleiter)**. Diese Werte werden zur Konfiguration der Instrumente addiert, die in den Aufteilungen des Presets verwendet werden.

Wie Instrumente können auch Presets Modulatoren enthalten, und ihre globale Aufteilung dient als Standardkonfiguration, die nicht übersehen werden sollte.


## Ungeeignete ADSR-Hüllkurve {#doc_adsr}


Die ADSR-Hüllkurve beschreibt, wie sich die Lautstärke beim Spielen einer Note, aber auch beim Loslassen dieser Note entwickelt. Jeder der vier Buchstaben des Begriffs ADSR entspricht einer bestimmten Phase, und die in Soundfonts verwendete Lautstärke-Hüllkurve umfasst tatsächlich 6 Phasen:

* Delay,
* Attack (A),
* Hold,
* Decay (D),
* Sustain (S),
* Release (R).

Diese Phasen werden auf dieser [Seite](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope) ausführlicher beschrieben; im Folgenden sind einige mögliche Probleme im Zusammenhang mit der Hüllkurve aufgeführt.

### Ein Klang setzt zu abrupt ein

Um den Beginn einer Note weicher zu machen, legen Sie auf [Instrument](manual/soundfont-editor/editing-pages/instrument-editor.md)- oder [Preset](manual/soundfont-editor/editing-pages/preset-editor.md)-Ebene eine **Attack-Dauer** für die Note fest oder verlängern Sie sie.

### Ein Klang setzt zu weich ein

Im Gegensatz zum vorherigen Fall verkürzen oder entfernen Sie die **Attack-Dauer** der Note.

### Vor dem Einsetzen des Klangs ist eine Verzögerung spürbar

Drei mögliche, einander ergänzende Lösungen:

* die **Delay-Dauer** entfernen, wenn sie auf [Instrument](manual/soundfont-editor/editing-pages/instrument-editor.md)- oder [Preset](manual/soundfont-editor/editing-pages/preset-editor.md)-Ebene festgelegt ist,
* die Stille am Anfang des Audiosamples entfernen (siehe das Kürzen von Samples auf dieser [Seite](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph) oder das Werkzeug zum Entfernen von Stille [hier](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)),
* schließlich kann die wahrgenommene Latenz durch den auf Ihrem Computer verwendeten Audioserver verursacht werden (in diesem Fall handelt es sich nicht um einen Mangel des Soundfonts selbst, und Sie können den Audioausgang in den [Einstellungen](manual/settings.md#doc_general) ändern).

### Der Klang bricht plötzlich ab

Bricht der Klang plötzlich ab, obwohl er weiterklingen sollte, solange die Note gehalten wird, lesen Sie den Abschnitt zum [Loop](#doc_loop).

Bricht der Klang beim Loslassen einer Note abrupt ab, legen Sie auf [Instrument](manual/soundfont-editor/editing-pages/instrument-editor.md)- oder [Preset](manual/soundfont-editor/editing-pages/preset-editor.md)-Ebene eine **Release-Dauer** fest oder verlängern Sie sie. Mit diesem [Werkzeug](manual/soundfont-editor/tools/instrument-tools.md#doc_release) lässt sich diese Dauer so einstellen, dass sie über den gesamten Tonumfang, von tief bis hoch, natürlich klingt.


## Fehlende oder ungenaue Loops {#doc_loop}


Loops ermöglichen es, aus einem Sample mit fester Länge einen Klang unbegrenzter Dauer zu erzeugen, ohne dass das Ohr die Wiederholungen bemerkt. Für bestimmte Instrumententypen wie Flöten sind Loops unverzichtbar, für andere wie Perkussion nicht. Loops können auch in Klaviersamples verwendet werden, um mit einem kurzen Sample ein Sustain-Pedal zu simulieren.

### Der Klang bricht ab, bevor die Note losgelassen wird

Bricht der Klang ab, obwohl er weiterklingen sollte, sind die wahrscheinlichsten Ursachen:

* der Loop des Samples ist nicht festgelegt – in diesem Fall können Sie anhand dieser [Seite](tutorials/how-to-prepare-a-sample.md#doc_loop) einen Loop einrichten,
* die Loop-Wiedergabe ist nicht aktiviert – in diesem Fall müssen Sie den Parameter **Loop-Wiedergabe** auf [Instrument](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)-Ebene ändern.

### Der Klang enthält regelmäßige „Klicks“

Der Loop ist höchstwahrscheinlich fehlerhaft; auf dieser [Seite](tutorials/how-to-prepare-a-sample.md#doc_loop) wird erklärt, wie man saubere Loops erstellt.


## MIDI-Standard nicht eingehalten {#doc_standard}


MIDI-Standards schreiben für die in Soundfonts enthaltenen Instrumente eine bestimmte Nummerierung vor. So können verschiedene Soundfonts, die demselben Standard folgen, ausgetauscht werden, ohne dass sich die gespielten Klangfarben wesentlich unterscheiden: Ein Klavier und eine Flöte werden zum Beispiel auf dieselbe Weise aufgerufen. Wird in einer Bewertung angemerkt, dass ein Standard nicht eingehalten wird, gibt es zwei mögliche Lösungen:

* entweder die Angabe entfernen, dass der Soundfont einem Standard entspricht,
* oder überprüfen, ob alle Presets des Soundfonts korrekt der passenden Instrumentenart zugeordnet sind (siehe diese [Seite](manual/annexes/midi-standards.md) für die Liste der Instrumente) und ob alle vom Standard geforderten Instrumente vorhanden sind.
