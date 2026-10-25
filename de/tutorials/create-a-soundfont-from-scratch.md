Um einen [Soundfont](manual/annexes/the-different-soundfont-formats.md#doc_sf2) zu erstellen, klicken Sie auf dem [Startbildschirm](manual/index.md#doc_left) auf :guilabel:`Neuer Soundfont`.
Soundfonts sind in 3 Ebenen aufgebaut, daher umfasst die Erstellung eines neuen Soundfonts drei grundlegende Schritte:

* die [Vorbereitung der Samples](#doc_sample),
* die [Erstellung der Instrumente](#doc_instrument), die aus Samples bestehen,
* die [Erstellung der Presets](#doc_preset), die aus Instrumenten bestehen.


## Vorbereitung der Samples {#doc_sample}


### Samples laden


Die Vorbereitung der Samples beginnt mit dem **Laden** von Audiodateien (zum Beispiel .wav).
Klicken Sie dazu in der [Werkzeugleiste](manual/soundfont-editor/toolbar.md#doc_edit) auf :guilabel:`Sample hinzufügen` und wählen Sie eine oder mehrere Audiodateien aus (die Dateien können auch per Drag & Drop in die [Baumansicht](manual/soundfont-editor/tree.md#doc_dragdrop) gezogen werden).
Samples können folgendermaßen beschafft werden:

* über das Internet,
* durch Aufnahme eines echten Musikinstruments,
* durch Synthese mit einer spezialisierten Software.


### Samples loopen


Als Nächstes kann ein **Loop** im Sample erforderlich sein, wenn es länger klingen soll als seine normale Länge.
Ein Beispiel wäre, wenn Sie nur ein 1 Sekunde langes Flötensample haben, es aber unbegrenzt klingen soll.
Dazu werden innerhalb der Grenzen des Samples manuell oder automatisch Loop-Punkte (Start + Ende) gesetzt, sodass der Loop-Bereich wiederholt werden kann und das Sample dadurch länger klingt.
Loop-Punkte können zwar manuell gesetzt werden, doch die Funktion „[Auto-Loop](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop)“ zum automatischen Setzen liefert meist bessere Ergebnisse und ist viel schneller.

So setzen Sie Loop-Punkte manuell:

1. klicken Sie in der [Baumansicht](manual/soundfont-editor/tree.md) auf das zu loopende Sample,
2. setzen Sie im [Sample-Editor](manual/soundfont-editor/editing-pages/sample-editor.md) Anfang und Ende des Loops beliebig in der [Grafik](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph) (WAV-Anzeigebereich), sofern noch kein Loop festgelegt ist.
  Verwenden Sie einen Linksklick für den Anfang und einen Rechtsklick für das Ende.
  Der rechte Loop-Punkt sollte zuerst gesetzt werden (da der rechte Punkt standardmäßig auf Position 0 liegt und ein linker Punkt nicht hinter den rechten Punkt gesetzt werden kann).
3. drücken Sie den Button :guilabel:`Wiedergabe`, nachdem Sie die Option :guilabel:`Loop` aktiviert haben,
4. passen Sie Anfang und / oder Ende des Loops während der Wiedergabe des Samples an, bis der Übergang zwischen den beiden Positionen so fließend wie möglich ist.


**Hinweis:** Um den Loop in einem Instrument zu hören, müssen Sie in der Zeile des Parameters :guilabel:`Loop-Wiedergabe` in der Spalte :guilabel:`Global` oder in den Spalten der einzelnen Noten der [Parametertabelle](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) ![Loop ein](images/loop_on.png "Loop ein"){.inline} auswählen. ![Loop ein](images/loop_on.png "Loop ein"){.inline} schaltet ihn ein, ![Loop aus](images/loop_off.png "Loop aus"){.inline} oder ein leeres Feld schaltet ihn aus.


![Veranschaulichung eines Loops](images/loop_illustration.png "Veranschaulichung eines Loops")


### Samples stimmen


Schließlich muss eine **Stimmung** vorgenommen werden.
Dazu steht auf der Bearbeitungsseite der [Samples](manual/soundfont-editor/editing-pages/sample-editor.md#doc_player) ein Kalibrierwerkzeug (Sinus) zur Verfügung.
Für jedes Sample gehen Sie wie folgt vor:

1. starten Sie die Wiedergabe (Button :guilabel:`Wiedergabe` drücken), möglichst mit aktivierter Option :guilabel:`Loop`,
2. aktivieren Sie die Option :guilabel:`Sinus`,
3. stellen Sie den Lautstärkeregler so ein, dass beide Klänge möglichst deutlich zu hören sind,
4. ändern Sie die :guilabel:`Basisnote`, bis die beiden Klänge möglichst gut übereinstimmen,
5. passen Sie die :guilabel:`Korrektur` (in Cent) an, um das Sample auf die Basisnote zu stimmen.
   Achten Sie dabei auf auftretende Schwebungen und sorgen Sie dafür, dass sie so langsam wie möglich sind.

Die [Frequenzanalyse](manual/soundfont-editor/editing-pages/sample-editor.md#doc_freq) kann ein guter Anhaltspunkt für den Beginn des Stimmens sein.


### Weiterführendes


Zum Bearbeiten von Samples stehen mehrere Werkzeuge zur Verfügung, zum Beispiel:

* [Entfernen der Leere](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank) am Anfang,
* [Entzerren](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq) des Klangs,
* [Transponieren](manual/soundfont-editor/tools/sample-tools.md#doc_transpose),
* [Normalisieren](manual/soundfont-editor/tools/sample-tools.md#doc_volume) der Lautstärke,
* [Anpassen der Balance](manual/soundfont-editor/tools/sample-tools.md#doc_balance) bei Stereo-Samples.


## Erstellung der Instrumente {#doc_instrument}


Für diesen Schritt wird angenommen, dass ein oder mehrere Samples für die Erstellung eines Instruments zur Verfügung stehen.


### Ein Instrument erstellen und Samples hinzufügen


Klicken Sie zunächst in der [Werkzeugleiste](manual/soundfont-editor/toolbar.md#doc_edit) auf :guilabel:`Instrument hinzufügen`.
Nun muss ein Name eingegeben werden (sind in der Baumansicht Samples ausgewählt, können sie direkt mit dem neuen Instrument verknüpft werden).

Andernfalls fügen Sie dem Instrument per „[Drag & Drop](manual/soundfont-editor/tree.md#doc_dragdrop)“ Samples hinzu.
In der Baumansicht sehen Sie, dass im Instrument Aufteilungen erscheinen.

**Hinweis:** Beim Erstellen von Aufteilungen werden die Samples nicht kopiert, sondern verknüpft. Die Anzahl der Aufteilungen ist nicht begrenzt.


### Verteilung der Aufteilungen


Werden Samples zu Instrumenten hinzugefügt, erscheinen sie als Aufteilungen (Spalten) in der [Tabelle des Instrument-Editors](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).
Jede Aufteilung muss dann auf der Tastatur positioniert werden, indem der :guilabel:`Tastenbereich` in der Tabelle geändert wird.
Im Allgemeinen empfiehlt es sich, dass der Bereich einer Aufteilung die Basisnote des dargestellten Samples enthält.
Die gesamte Fläche der Tastatur muss abgedeckt sein (zum Beispiel Taste 36 bis Taste 96 bei einer klassischen Synthesizer-Tastatur).

Das Werkzeug „[Sample-Autopositionierung](manual/soundfont-editor/tools/instrument-tools.md#doc_position)“ verteilt die Samples automatisch über die Tastatur.


### Einstellung der Aufteilungen


Um in einem Instrument mit geloopten Samples den Loop-Wiedergabemodus zu aktivieren, wählen Sie in der Zeile des Parameters :guilabel:`Loop-Wiedergabe` in der Spalte :guilabel:`Global` oder in den Spalten der einzelnen Noten der Parametertabelle ![Loop ein](images/loop_on.png "Loop ein"){.inline} aus.

* ![Loop ein](images/loop_on.png "Loop ein"){.inline} schaltet ihn ein,
* ![Loop aus](images/loop_off.png "Loop aus"){.inline} oder ein leeres Feld schaltet ihn aus.
* ![Loop ein und Ende](images/loop_on_end.png "Loop ein und Ende"){.inline} schaltet den Loop ein, und der Rest des Samples wird nach dem Loslassen der Taste abgespielt.

Mit der globalen Aufteilung kann, wie der Name schon sagt, ein Parameter für das gesamte Instrument eingegeben werden.
Ein globaler Parameter gilt nur für die Aufteilungen, in denen derselbe Parameter nicht festgelegt ist.
Das heißt, die Parameter der normalen Aufteilungen haben Vorrang vor den Einstellungen der globalen Aufteilung.

Im Rahmen dieses einfachen Tutorials müssen keine weiteren Parameter bearbeitet werden, damit das Instrument funktioniert.
Es sollte bereits möglich sein, mit der [virtuellen Tastatur](manual/soundfont-editor/toolbar.md#doc_keyboard) zu spielen.

Um das Instrument zu verbessern, empfiehlt es sich jedoch, in der Zeile :guilabel:`Vol env release (s)` einen Wert einzugeben, damit der Klang nicht abrupt abbricht, sobald eine Taste losgelassen wird.
Die übrigen Parameter werden in der Beschreibung der [Tabellen](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) erläutert.


## Erstellung der Presets {#doc_preset}


Für diesen Schritt wird angenommen, dass ein oder mehrere Instrumente für die Erstellung eines Presets zur Verfügung stehen.


### Was ist ein Preset?


Ein Preset entspricht dem Namen eines Klangprogramms (Patch) bei Hardware-Synthesizern.
Es ist von außerhalb eines Soundfonts sichtbar und wird durch eine Banknummer und eine Presetnummer identifiziert.
Ein Soundfont kann ein oder viele Presets enthalten.
Jedes Preset enthält ein oder mehrere Instrumente, so wie Instrumente ein oder mehrere Samples enthalten.


### Ein Preset erstellen und Instrumente hinzufügen


Klicken Sie zunächst in der [Werkzeugleiste](manual/soundfont-editor/toolbar.md#doc_edit) auf :guilabel:`Preset hinzufügen`.
Nun muss ein Name eingegeben werden (sind in der Baumansicht Instrumente ausgewählt, können sie direkt mit dem neuen Preset verknüpft werden).

Andernfalls fügen Sie dem Preset per „[Drag & Drop](manual/soundfont-editor/tree.md#doc_dragdrop)“ Instrumente hinzu.
In der Baumansicht sehen Sie, dass im Preset eine oder mehrere Aufteilungen erscheinen.
Oft gibt es nur eine, etwa bei einem Preset, das ein einziges Instrument enthält.

Es können so viele Presets wie Instrumente (oder sogar mehr) erstellt werden, und jedes Preset kann ein oder mehrere Instrumente enthalten.


### Ein Preset einstellen


Möglicherweise müssen die Parameter des Presets gar nicht geändert werden, denn:

* beim Erstellen eines Presets werden Bank- und Presetnummer automatisch vergeben,
* beim Hinzufügen eines Instruments zu einem Preset wird der Tastenbereich automatisch anhand des Instruments berechnet.

Im Rahmen dieses Tutorials ist die Erstellung eines Soundfonts damit abgeschlossen!
