MIDI-Dateien enthalten MIDI-Nachrichten, die zusammen eine Melodie ergeben. Sie lassen sich mit einem Synthesizer kombinieren, der einen Soundfont verwendet, und dieses Tutorial zeigt, wie man mit einer MIDI-Datei die Wiedergabe eines Soundfonts testet.


## Ein wenig Hintergrund


<div class="too_complicated">
<p>Beim Spielen auf der Tastatur eines Synthesizers werden <a href="https://de.wikipedia.org/wiki/MIDI" target="_blank">MIDI</a>-Signale gesendet, die Informationen wie die gespielten Noten und ihre Anschlagsstärke enthalten (ein Begriff für die Kraft, mit der die Taste angeschlagen wird). Eine MIDI-Datei enthält, vereinfacht gesagt, eine chronologische Aufzeichnung der von einer solchen Tastatur gesendeten Signale. Bis hierhin ist noch kein Klang entstanden.</p>

<p>Damit der Lautsprecher einen Klang ausgibt, ist ein Umwandlungsschritt nötig, und hier kommt der Synthesizer ins Spiel: Er löst anhand der empfangenen MIDI-Signale Klänge aus. Es gibt verschiedene Arten von Synthesizern; hier geht es um samplebasierte Synthesizer, genauer gesagt um Synthesizer, die Klangbibliotheken im SoundFont-Format verwenden.</p>

<p>Eine MIDI-Datei kann bis zu 16 verschiedene Kanäle steuern, sodass 16 verschiedene Instrumente parallel verwendet werden können. Neben der Angabe, welche Noten gespielt werden sollen, enthält die MIDI-Datei Anweisungen, die den Synthesizer auffordern, für die folgenden Noten auf Kanal Z das Instrument X aus Bank Y zu verwenden. Damit die Instrumente korrekt ausgewählt werden, muss die vom Synthesizer verwendete Klangbibliothek dem Standard <a href="https://de.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> entsprechen, der jeder Nummernkombination X und Y ein bestimmtes Instrument zuordnet. Zum Beispiel muss das Instrument <strong>001</strong> im Soundfont einem Klavier entsprechen.</p>

<p>Zusammengefasst geht es hier also um die Wiedergabe einer MIDI-Datei, die einen Synthesizer steuert, der Klänge aus einem Soundfont auslöst, der dem Standard General MIDI entspricht. Was nun folgt, ist einfacher!</p>
</div>


## Voraussetzungen


Um eine MIDI-Datei mit der unten beschriebenen Methode abzuspielen, benötigen Sie:

- einen Internetzugang,
- einen Webbrowser,
- eine MIDI-Datei, zum Beispiel <a href="files/ff7choco.mid" download>diese hier</a>,
- optional einen Soundfont, den Sie testen möchten und der dem Standard <a href="https://de.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> entspricht (in der [Fundgrube](soundfonts/midi-standard/gm) gibt es dafür einen Filter),
- optional die neueste Version von Polyphone, die auf dieser [Seite](software) verfügbar ist.


## Eine MIDI-Datei abspielen


Eine einfache Methode ist die Verwendung des Synthesizers <a href="https://spessasus.github.io/SpessaSynth/" target="_blank">SpessaSynth</a>, der zugleich MIDI-Player und Soundfont-Synthesizer ist. Er hat den Vorteil, online verfügbar zu sein, und erfordert keine besondere Installation.

Klicken Sie auf den Button :guilabel:`Load MIDI file`, wählen Sie Ihre MIDI-Datei aus, und schon kann es losgehen.

![MIDI-Player und Synthesizer SpessaSynth](images/SpessaSynth.jpg "MIDI-Player und Synthesizer SpessaSynth")


## Einen Soundfont auswählen


Um einen Soundfont Ihrer Wahl zu verwenden, klicken Sie auf den Button :guilabel:`Upload soundfont` und wählen Sie Ihre Datei aus.


## Den MIDI-Ausgang mit Polyphone verbinden


Wie oben erwähnt, ist **SpessaSynth** zugleich MIDI-Player und Synthesizer. Es ist möglich, nur die Funktion des MIDI-Players zu nutzen und die Signale an Polyphone zu senden, das dann die Klangsynthese übernimmt. Das kann nützlich sein, wenn der Soundfont zu groß ist, um über das Internet übertragen zu werden.

So verbinden Sie SpessaSynth mit Polyphone (unter Linux getestete Methode):

- starten Sie Polyphone im [Synth-Modus](tutorials/use-polyphone-as-a-synthesizer.md) und öffnen Sie die [Einstellungen](manual/settings.md#doc_general),
- wählen Sie „MIDI through“ als MIDI-Eingang (merken Sie sich den Namen),
- öffnen Sie SpessaSynth in Ihrem Webbrowser und öffnen Sie dessen Einstellungen,
- wählen Sie den MIDI-Ausgang „MIDI through“ (derselbe Name wie der MIDI-Eingang von Polyphone).

Da eine MIDI-Datei Nachrichten an 16 Kanäle senden kann, müssen Sie den Soundfont 16-mal öffnen und für jeden Tab einen anderen Kanal von 1 bis 16 festlegen. Bequemer ist es, beim Öffnen von Polyphone den zu öffnenden Soundfont und die Option „each“ anzugeben, um alle 16 Tabs auf einmal zu öffnen. Der Befehl lautet dann zum Beispiel:

```
polyphone -s -i "/home/davy/Downloads/gm.sf2" -c "each|off|off"
```

Weitere Informationen zu Kommandozeilen finden Sie auf dieser [Seite](manual/annexes/command-line.md#doc_synth).
