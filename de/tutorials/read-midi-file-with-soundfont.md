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

- die neueste Version von Polyphone, die auf dieser [Seite](software) verfügbar ist,
- eine MIDI-Datei, zum Beispiel <a href="files/ff7choco.mid" download>diese hier</a>,
- einen Soundfont, den Sie testen möchten und der dem Standard <a href="https://de.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> entspricht (in der [Fundgrube](soundfonts/midi-standard/gm) gibt es dafür einen Filter).

Ein Soundfont, der diesem Standard nicht entspricht, kann ebenfalls getestet werden, indem Sie [seine Presets selbst auswählen](#doc_presets).


## Eine MIDI-Datei abspielen


Öffnen Sie Ihren Soundfont in Polyphone und klicken Sie dann auf das Symbol ![MIDI](images/toolbar_midi.png "MIDI"){.inline} rechts in der [Werkzeugleiste](manual/soundfont-editor/toolbar.md), um den [MIDI-Player](manual/soundfont-editor/toolbar.md#doc_midi_player) anzuzeigen.

Klicken Sie auf die Ordner-Schaltfläche oben links im Fenster, wählen Sie Ihre MIDI-Datei aus und klicken Sie dann auf die Wiedergabe-Schaltfläche (oder drücken Sie :kbd:`Space`), und schon kann es losgehen.

![Der MIDI-Player von Polyphone](images/midi_player.png "Der MIDI-Player von Polyphone")

Jeder Kanal der Datei, der Noten enthält, hat eine eigene Zeile in der Tabelle. Die Spalte :guilabel:`Preset` zeigt das von der Datei angeforderte Preset, das sich im Laufe des Stücks ändern kann, und die LED der Spalte :guilabel:`Lautstärke` leuchtet bei jedem Notenbeginn auf. Mit dem Schieberegler unter dem Dateinamen springen Sie an eine andere Stelle des Stücks.

Der MIDI-Player ist auch verfügbar, wenn Polyphone im [Synth-Modus](tutorials/use-polyphone-as-a-synthesizer.md) geöffnet wird.


## Den Soundfont testen


Der Soundfont wird so gespielt, wie er im Editor ist, einschließlich der nicht gespeicherten Änderungen: Sie können ein Instrument bearbeiten, während das Stück läuft, und das Ergebnis sofort hören, zum Beispiel beim Anpassen seiner Lautstärke, seiner Hüllkurve oder seines Filters.

Der Player hilft Ihnen außerdem, auf Details zu hören:

- die Schaltflächen :guilabel:`S` (Solo) und :guilabel:`M` (Stummschalten) eines Kanals isolieren ein Instrument oder nehmen es aus dem Mix, und der Schieberegler der Spalte :guilabel:`Lautstärke` gleicht es mit den anderen ab;
- ein Warnsymbol in der Spalte :guilabel:`Preset` bedeutet, dass der Soundfont das von der Datei angeforderte Preset nicht enthält: Die Noten dieses Kanals werden nicht gespielt, was auf einen Soundfont hinweist, der dem General-MIDI-Standard nicht vollständig entspricht;
- die MIDI-Schaltfläche eines Kanals listet die Controller auf, die die Datei gerade verwendet (Modulationsrad, Expression, Hall…), was beim Prüfen der [Modulatoren](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) des Soundfonts hilft;
- die [Wiedergabeeinstellungen](manual/soundfont-editor/toolbar.md#doc_midi_player_settings) rechts im Banner spielen das Stück in einer Schleife ab, verlangsamen es, um eine schnelle Passage anzuhören, oder transponieren es, um andere Tasten zu testen.


## Soundfonts und Presets auswählen {#doc_presets}


Jeder Kanal kann einen anderen Soundfont spielen: Öffnen Sie mehrere Soundfonts in Polyphone und doppelklicken Sie dann auf eine Zelle der Spalte :guilabel:`Soundfont`, um den Soundfont zu wählen, den der Kanal spielt. So können Sie zum Beispiel die Klaviere zweier Soundfonts in derselben Stimme vergleichen oder einen General-MIDI-Soundfont für die Begleitung behalten, während Ihr eigener Soundfont die Melodie spielt.

![Den Soundfont eines Kanals wählen](images/midi_player_soundfonts.png "Den Soundfont eines Kanals wählen")

Auf dieselbe Weise erzwingt ein Doppelklick auf eine Zelle der Spalte :guilabel:`Preset` ein Preset des Soundfonts anstelle des von der Datei angeforderten. So testen Sie einen Soundfont, der dem General-MIDI-Standard nicht entspricht, zum Beispiel einen Soundfont mit einem einzigen Instrument: Wählen Sie sein Preset für den Kanal der Melodie, die Programmwechsel der Datei werden dann auf diesem Kanal ignoriert.


## Das Ergebnis exportieren


Wenn Ihnen das Ergebnis gefällt, schreibt :guilabel:`Audio exportieren...` in den Wiedergabeeinstellungen das Stück so in eine wav-Datei, wie es im Player zu hören ist, mit denselben Soundfonts, Presets und Lautstärken, demselben Tempo und derselben Tonhöhe.
Eine MIDI-Datei kann auch über die [Kommandozeile](manual/annexes/command-line.md#doc_render) in eine wav-Datei umgewandelt werden, zum Beispiel in einem Skript.
