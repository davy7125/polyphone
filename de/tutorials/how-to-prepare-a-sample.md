Ein in einem Soundfont aufgebautes Instrument besteht aus Samples, die zum Beispiel aus Aufnahmen eines echten Instruments stammen.
Dieses Tutorial beschreibt, wie man ein Sample zu einem Soundfont hinzufügt und vorbereitet.


## Import {#doc_import}


Zunächst brauchen Sie Samples auf Ihrem Computer.
Im Internet gibt es viele kostenlose Samples, die entweder von echten Instrumenten aufgenommen (Flöte, Gitarre, Cello, …) oder mit verschiedenen Arten der Audiosynthese erzeugt wurden.

Für dieses Tutorial wird dieses Flötensample bereitgestellt: <a href="files/flute.wav" download>flute.wav</a>.

Sobald Sie es haben, erstellen Sie mit Polyphone einen neuen Soundfont (:menuselection:`Menu --> Neu`) und importieren das Sample (:guilabel:`Sample hinzufügen` in der [Werkzeugleiste](manual/soundfont-editor/toolbar.md#doc_edit)).


![Sample importieren](images/import-sample.png "Sample importieren")


## Loop {#doc_loop}


### Einführung


Ein Loop ist ein Bereich innerhalb eines Samples, der wiederholt werden kann, um die Dauer des Samples zu verlängern.
Dauert ein Sample zum Beispiel nur 0,5 Sekunden, kann der Klang mit einem guten Loop auf 10 Sekunden oder mehr verlängert werden.
Dies ist nur bei Samples nicht perkussiver Instrumente wie Flöten, Trompeten, Orgeln oder Violinen sinnvoll.
Schlagzeug dagegen braucht keine Loops.
Je nachdem, wie die Klänge erzeugt werden, kann dies auch bei einem Klavier oder einer Gitarre der Fall sein.

Dieser Abschnitt ist also nur dann nötig, wenn Sie die ursprüngliche Länge eines Samples verlängern möchten.
Andernfalls gehen Sie direkt zum nächsten Abschnitt „Stimmung“.


### Einen Loop einrichten


Klicken Sie in der Baumansicht auf das Sample, das Sie loopen möchten, und konzentrieren Sie sich auf die Loop-Parameter.


![Sample loopen: Loop-Parameter](images/loop-sample-1.png "Sample loopen: Loop-Parameter")


Mit etwas Glück sind diese Werte bereits im .wav-Sample enthalten und die Felder schon ausgefüllt.
Ist das nicht der Fall oder sind sie schlecht eingestellt (das kommt vor …), klicken Sie beliebig links und rechts in die Grafik (WAV-Anzeigebereich), um Anfang und Ende des Loops festzulegen.
Dies geschieht in der Regel im stabilen Teil des Klangs, nicht im Attack oder im Release.
Zoomen Sie dann um den Anfang des Loops herum und versuchen Sie, die grüne (oder rote) und die blaue Kurve zur Deckung zu bringen.


![Sample loopen: Ausrichtung der Wellen](images/loop-sample-2.png "Sample loopen: Ausrichtung der Wellen")


### Das Ergebnis anhören


Wenn der Loop eingerichtet ist, drücken Sie im Player bei aktivierter Option :guilabel:`Loop` auf :guilabel:`Wiedergabe`, um das Ergebnis zu prüfen.


![Sample loopen: Ergebnis prüfen](images/loop-sample-3.png "Sample loopen: Ergebnis prüfen")


Hören Sie den Übergang nicht, wenn das Ende des Loops erreicht ist, ist der Loop gut.
Andernfalls lassen Sie den Player laufen und verschieben Ende und / oder Anfang des Loops leicht, bis Sie Punkte mit einem möglichst fließenden Übergang finden.
Hören Sie sich die folgenden Aufnahmen an, um den Unterschied zwischen einem schlechten und einem guten Loop zu hören:

* <a href="files/bad-loop.mp3" download>schlechter Loop</a>

![](files/bad-loop.mp3)

* <a href="files/good-loop.mp3" download>guter Loop</a>

![](files/good-loop.mp3)


**Hinweis:** Es gibt ein Werkzeug, um zu „schummeln“, wenn sich kein Loop leicht finden lässt: das Werkzeug [Auto-Loop](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop).
Es erstellt automatisch einen Loop, indem es das Sample schneidet und zwei Abschnitte überlagert, sodass das Ende des Abschnitts genau zu seinem Anfang passt.
Weitere Informationen finden Sie in der zugehörigen Dokumentation.


## Stimmung {#doc_tuning}


Alle Samples müssen gestimmt werden, wenn das Instrument nicht verstimmt sein soll.
Wird im Soundfont die genaue Tonhöhe aller importierten Samples angegeben, kann der Soundfont sie an die Tasten anpassen.
Manche Samples enthalten diese Information bereits; in diesem Fall sollten Sie die Werte in :guilabel:`Basisnote` und :guilabel:`Korrektur` nicht ändern müssen.
Zur Sicherheit ist es aber immer gut, sie zu überprüfen.


![Sample stimmen: Tonhöheninformationen](images/tune-sample-1.png "Sample stimmen: Tonhöheninformationen")


Aktivieren Sie den Loop, falls das Sample einen hat, und drücken Sie im Player auf :guilabel:`Wiedergabe`.
Aktivieren Sie dann das Kalibriersignal :guilabel:`Sinus` und stellen Sie die Lautstärke so ein, dass beide Klänge möglichst deutlich zu hören sind.


![Sample stimmen: Sample zusammen mit dem Kalibriersignal abspielen](images/tune-sample-2.png "Sample stimmen: Sample zusammen mit dem Kalibriersignal abspielen")


Beginnen Sie mit dem Stimmen, indem Sie die Basisnote ändern, bis die beiden Klänge möglichst gut übereinstimmen.
Passen Sie anschließend die Korrektur an.
Achten Sie bei diesem Schritt auf auftretende Schwebungen und sorgen Sie dafür, dass sie so langsam wie möglich sind.
Hier einige Beispiele mit dem Klang einer Flöte und einem Kalibrier-Sinus:

* <a href="files/very-bad-tune.mp3" download>sehr schlechte Stimmung</a> (die Basisnote stimmt nicht)

![](files/very-bad-tune.mp3)

* <a href="files/bad-tune.mp3" download>schlechte Stimmung</a> (die Korrektur stimmt nicht, Schwebungen sind zu hören)

![](files/bad-tune.mp3)

* <a href="files/good-tune.mp3" download>gute Stimmung</a> (beide Werte stimmen)

![](files/good-tune.mp3)


**Hinweis:** Die [Frequenzanalyse](manual/soundfont-editor/editing-pages/sample-editor.md#doc_freq) kann ein guter Anhaltspunkt für den Beginn des Stimmens sein. Bedenken Sie aber, dass kein Algorithmus für diese Aufgabe perfekt ist und nur Ihre Ohren die richtige Stimmung finden.


![Sample stimmen: Frequenzanalyse](images/tune-sample-3.png "Sample stimmen: Frequenzanalyse")


## Weiterführendes {#doc_further}


Im Menü :guilabel:`Werkzeuge` der [Werkzeugleiste](manual/soundfont-editor/toolbar.md#doc_edit) stehen mehrere Werkzeuge zur Verfügung.
Sie können Samples bearbeiten durch:

* [Entfernen der Leere](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank) am Anfang,
* [Entzerren](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq) des Klangs,
* [Transponieren](manual/soundfont-editor/tools/sample-tools.md#doc_transpose),
* [Normalisieren](manual/soundfont-editor/tools/sample-tools.md#doc_volume) der Lautstärke,
* [Anpassen der Balance](manual/soundfont-editor/tools/sample-tools.md#doc_balance) bei Stereo-Samples.


## Hinweis zu Stereo-Samples {#doc_stereo}


Stereo-Samples bestehen aus zwei Mono-Samples.
Die Verknüpfung ist in den Informationen eines Samples gespeichert.


![Stereo-Sample](images/stereo-sample.png "Stereo-Sample")


Bei zwei Mono-Samples kann das Loopen schwieriger sein, da ein Loop auf der einen Seite gut klingen kann und auf der anderen nicht.
Scheuen Sie sich in diesem Fall nicht zu „schummeln“, indem Sie auf der rechten und linken Seite zwei verschiedene Loops einrichten.
Die Klangqualität leidet darunter nicht.
