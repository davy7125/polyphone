Ziel dieses Tutorials ist es, ausdrucksstarke Instrumente mit geloopten Samples zu erstellen, deren Release mit aufgenommen wurde.
Sie lernen, die Möglichkeiten eines solchen Samples voll auszuschöpfen, indem Sie nicht nur Attack und Sustain des Samples, sondern auch sein Release nutzen.


## Herkömmliches Release vs. eigenes Release


Wenn Sie eine Note auslösen, hören Sie zuerst den Anfang des Samples (den Attack) und dann, solange Sie die Taste nicht loslassen, wird der Loop im Sample immer wieder abgespielt (die gehaltene Phase).
Wenn Sie die Taste loslassen, wird der Loop weiterhin verwendet, und der Klang wird allmählich leiser, bis er nicht mehr zu hören ist.
Das ist die herkömmliche Art, ein Release in Soundfonts umzusetzen: einfach einzurichten, aber ohne Berücksichtigung der Einschwingvorgänge und Obertöne, die beim echten Release eines akustischen Instruments auftreten können.
Diese Art von Release kann als einfache Verlängerung der gehaltenen Phase betrachtet werden.

Realistischer wäre es, das echte Release aus dem aufgenommenen Sample zu verwenden.
Attack und gehaltene Phase werden genauso umgesetzt, aber beim Loslassen der Taste bricht die gehaltene Phase plötzlich ab und macht Platz für das aufgenommene Release, das zu klingen beginnt.
Statt den Klang der gehaltenen Phase zu verlängern, wird ein eigenes Release verwendet, das dem Klang Details hinzufügt.

Ein solches Instrument wird erstellt, indem das Problem in zwei Teile aufgeteilt wird:

* zuerst werden Attack und gehaltene Phase festgelegt,
* dann wird das Release ausgelöst.

Als Beispiel dient hier der Klang einer Trompete in einer Orgel, wegen ihres hohen Obertongehalts und ihrer hörbaren Entwicklung.

**Hinweis:** Seit Version 2.4 kann Polyphone dank des Loop-Wiedergabemodus ![Release](images/sample_mode_release.png "Release"){.inline} (siehe [Instrument-Editor](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)) auch beim Loslassen einer Taste ein Sample auslösen, wodurch der unten beschriebene kurze stille Loop überflüssig wird.
Dieser Modus ist jedoch im sf2-Format nicht dokumentiert und wird von anderen Playern möglicherweise nicht unterstützt, während die in diesem Tutorial beschriebene Methode mit jedem Player funktioniert.


## Vorbereitung der Samples


Zwei Fälle sind möglich: Sie haben eine Datei mit dem vollständigen Sample vom Attack bis zum Release, oder Sie haben eine zusätzliche Datei, die nur das bereits herausgelöste Release enthält.


### Vollständiges Sample mit Release


Laden Sie das Sample zweimal in Polyphone und entfernen Sie dann in der zweiten Instanz des Samples alles außer dem Release.
Zum Entfernen halten Sie die Taste :kbd:`Alt` gedrückt und legen dann in der [Grafik des Samples](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph) einen Bereich fest.
Um den Punkt zu finden, an dem das Release beginnt, spielen Sie das Sample mehrmals ab und versuchen Sie, den Punkt zu bestimmen, ab dem Ihrer Meinung nach keine Energie mehr den Klang angeregt hat.
Dieser letzte Teil ist das Release.
Folgen Sie dann den Anweisungen unten.


### Getrenntes Sample und Release


Samples für das Beispiel: <a href="files/trumpet.wav" download>Sample 1</a>, <a href="files/trumpet-release.wav" download>Sample 2</a>.

Haben Sie 2 Samples, von denen das erste den Anfang und die gehaltene Phase des Klangs und das zweite das Release enthält, müssen Sie nur sicherstellen, dass das Release mit einer sehr kurzen Leerstelle (ein paar Punkte) nahe 0 beginnt.
Mit diesem Abschnitt wird ein kurzer, unhörbarer Loop erstellt.

Stimmen und loopen Sie das erste Sample, wie im Tutorial „[Ein Sample vorbereiten](tutorials/how-to-prepare-a-sample.md)“ erklärt.

Loopen Sie das zweite Sample über einen sehr kurzen Abschnitt am Anfang, zum Beispiel von Punkt 1 bis Punkt 2 oder 3.
Geben Sie dieselbe Basisnote und Korrektur an wie beim ersten Sample.


## Erstes Instrument: Attack und gehaltene Phase


Erstellen Sie ein erstes Instrument mit den Samples, die Attack und gehaltene Phase des Klangs enthalten.
Stellen Sie die globale Aufteilung folgendermaßen ein:

* wählen Sie beim Parameter :guilabel:`Loop-Wiedergabe` ![Loop ein](images/loop_on.png "Loop ein"){.inline} aus,
* geben Sie beim Parameter :guilabel:`Vol env release (s)` 0,01 s an (sehr kurzes Release).

Nun haben Sie ein Instrument ohne Release.


## Zweites Instrument: Release


Erstellen Sie ein zweites Instrument für das Release mit den Release-Samples, die am Anfang einen sehr kurzen leeren Loop haben.
Stellen Sie es folgendermaßen ein:

* wählen Sie beim Parameter :guilabel:`Loop-Wiedergabe` ![Loop ein und Ende](images/loop_on_end.png "Loop ein und Release"){.inline} (Loop ein + Release) aus,
* wählen Sie beim Parameter :guilabel:`Vol env release (s)` einen hohen Wert wie 100 Sekunden, damit das Release nicht gedämpft wird,
* bestimmen Sie ungefähr die Dauer des Attacks im ersten Sample und verwenden Sie diesen Wert beim Parameter :guilabel:`Vol env attack (s)`.

Nun haben Sie ein Instrument, das stumm bleibt, solange Sie keine Taste loslassen; danach ist die Release-Phase zu hören.
Möglicherweise fällt Ihnen auch auf, dass ein sehr kurzer Tastenanschlag das Release gedämpft auslöst.
Im Beispiel ist das kaum wahrnehmbar, da der Attack einer Trompete sehr schnell ist.
Die Dauer des Attacks wurde im Beispiel leicht erhöht.


## Beide Instrumente zusammenführen


Erstellen Sie ein Preset mit diesen beiden Instrumenten.
Das erste Instrument ist zu hören, wenn eine Taste angeschlagen wird, und das zweite Instrument wird ausgelöst, wenn die Taste losgelassen wird.
Hören Sie sich dann das Ergebnis an und versuchen Sie, den Übergang zwischen der gehaltenen Phase und dem Release fließender zu gestalten:

* indem Sie das erste oder das zweite Instrument dämpfen,
* indem Sie die Release-Dauer des ersten Instruments verlängern.

Das Ergebnis mit den Beispieldateien finden Sie <a href="files/custom%20release.sf2" download>hier</a>.

Hier können Sie sich die Unterschiede zwischen folgenden Varianten anhören:

* einem <a href="files/classic-release.mp3" download>herkömmlichen Release</a>

![](files/classic-release.mp3)

* und einem <a href="files/custom-release.mp3" download>eigenen Release</a>

![](files/custom-release.mp3)
