Sind in der [Baumansicht](manual/soundfont-editor/tree.md) ein oder mehrere Samples ausgewählt, enthält das [Werkzeugmenü](manual/soundfont-editor/tools/index.md) mehrere Werkzeuge in 5 Kategorien:

* [Audioverarbeitung](#doc_cat_processing),
* [Schnelle Bearbeitung](#doc_cat_fast_editing),
* [Dateien](#doc_cat_files),
* [Loop / Wiedergabe](#doc_cat_loop_playback),
* [Stereo-Samples](#doc_cat_stereo).

Die Werkzeuge können (mit einer Mehrfachauswahl) gleichzeitig auf mehrere Samples angewendet werden.


## Audioverarbeitung {#doc_cat_processing}


### Lautstärke ändern {#doc_volume}

Die Lautstärke des Samples kann auf drei Arten geändert werden:

* durch Addieren einer bestimmten Anzahl dB (positiver oder negativer Wert),
* durch Multiplizieren des Signals mit einer Zahl,
* durch Normalisieren des Signals, sodass sein Maximum einem bestimmten Prozentsatz der maximalen Intensität entspricht.

![Werkzeug „Lautstärke ändern“](images/tool_change_volume.png "Werkzeug „Lautstärke ändern“")


### Externer Befehl {#doc_externalcommand}

Mit diesem Werkzeug kann ein Sample mit einem externen Befehl oder Programm bearbeitet werden.
Ein Sample muss nicht mehr exportiert und wieder in Polyphone importiert werden, alles geschieht automatisch:

1. Laden Sie eine Software zur Bearbeitung von .wav-Dateien herunter, zum Beispiel :program:`Audacity` oder :program:`Sweep` (Linux),
2. wenden Sie das Werkzeug :guilabel:`Externer Befehl` auf ein oder mehrere Samples an,
3. tragen Sie im Feld :guilabel:`Befehl` die Kommandozeile ein, mit der die externe Software aufgerufen wird (eventuell müssen Sie dazu die Dokumentation dieser Software lesen); das Argument :command:`{wav}` ist Pflicht, da es durch den Pfad der exportierten Datei ersetzt wird (zum Beispiel :command:`sweep {wav}`),
4. bearbeiten Sie das bzw. die Samples mit dem externen Werkzeug,
5. speichern Sie die geänderte Version der WAV-Datei(en) mit dem externen Werkzeug und schließen Sie es,
6. das bzw. die Samples werden automatisch in den Soundfont geladen.

Mit dem Button rechts neben dem Feld kann eine ausführbare Datei ausgewählt werden, und die Liste :guilabel:`Verlauf` enthält die zuletzt verwendeten Befehle.
Zwei Optionen stehen zur Verfügung:

Stereobearbeitung erlauben
: Wird ein Stereo-Sample verarbeitet, wird es zusammen mit seinem verknüpften Sample in eine einzige Stereodatei exportiert, und beide Samples werden beim erneuten Import der Datei aktualisiert.

Sample-Informationen, wenn möglich, ersetzen
: Loop, Basisnote und Korrektur, die in der von der externen Software zurückgegebenen Datei gespeichert sind, ersetzen die des Samples.
  Andernfalls bleiben die vorhandenen Informationen erhalten.

![Werkzeug „Externer Befehl“](images/tool_external_command.png "Werkzeug „Externer Befehl“")

**Hinweis:** Während des Vorgangs exportiert Polyphone eine .wav-Datei und importiert genau diese Datei wieder, nachdem der externe Befehl ausgeführt wurde. Erstellt der externe Befehl andere Dateien oder speichert er das Ergebnis in einer anderen Datei, werden diese Dateien ignoriert, und Polyphone importiert die ursprünglich exportierte Datei, die vermutlich unverändert ist.


### Frequenzen filtern {#doc_filter}

Bei Verwendung dieses Werkzeugs erscheint ein Fenster mit den Frequenzen, die in den ausgewählten Samples enthalten sind.
Mit der Maus kann dann ein roter Bereich gezeichnet werden.
Alles, was sich in diesem Bereich befindet, wird entfernt: Bestimmte Frequenzen lassen sich so von Hand absenken oder entfernen.

![Werkzeug „Frequenzen filtern“](images/tool_filter_frequencies.png "Werkzeug „Frequenzen filtern“")

**Hinweis:** Dieser Filter ist ungewöhnlich, da er Frequenzen hart abschneidet.
Das Werkzeug kann nützlich sein, um einen Brickwall-Filter zu erstellen, in den meisten Fällen werden Sie aber eher den [Equalizer](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq) verwenden wollen.


### Transponieren {#doc_transpose}

Das Werkzeug fragt nach einer Verschiebung in Halbtönen und resampelt dann den Klang, um die Tonhöhe zu ändern.

![Werkzeug „Transponieren“](images/tool_transpose_smpl.png "Werkzeug „Transponieren“")


## Schnelle Bearbeitung {#doc_cat_fast_editing}


### Einen Parameter von den Instrumenten laden {#doc_load_from_instruments}

Parameter auf Instrumentebene können die auf Sample-Ebene festgelegten Parameter überschreiben oder verschieben.
Mit diesem Werkzeug lassen sich die Parameter der Samples anhand dessen korrigieren oder ergänzen, was in den Instrumenten festgelegt ist, die diese Samples verwenden:

* Ändern der Basisnote des Samples mit der auf Instrumentebene festgelegten Überschreibung,
* Verschieben der Basisnote des Samples mit dem Parameter „Tuning (Halbtöne)“,
* Verschieben der Tonhöhenkorrektur des Samples mit dem Parameter „Tuning (Cents)“,
* Festlegen von Loop-Start und Loop-Ende des Samples mit den Loop-Offset-Parametern.

![Werkzeug zum Laden eines Parameters von den Instrumenten](images/tool_load_from_instruments.png "Werkzeug zum Laden eines Parameters von den Instrumenten")


### Einen Parameter verschieben {#doc_shift_parameter}

Mit diesem Werkzeug kann einer der folgenden Parameter für alle ausgewählten Samples gleichzeitig bearbeitet werden, indem ein Wert addiert oder mit einem Wert multipliziert wird:

* Basisnote,
* Korrektur,
* Loop-Start,
* Loop-Ende.

![Werkzeug zur schnellen Bearbeitung](images/tool_shift_parameter.png "Werkzeug zur schnellen Bearbeitung")


## Dateien {#doc_cat_files}


### WAV-Export {#doc_wavexport}

Nach Auswahl eines Zielverzeichnisses werden alle ausgewählten Samples als .wav-Dateien exportiert.


## Loop / Wiedergabe {#doc_cat_loop_playback}


### Auto-Loop {#doc_autoloop}

Die Software sucht im Sample zwei weit auseinanderliegende Positionen, die sich zum Erstellen eines Loops eignen.
Bei Bedarf wird ein kurzes Klangsegment mit einer Überblendung kopiert, damit der Übergang an der Nahtstelle so fließend wie möglich ist.
Bei einem Stereo-Sample wird für beide Seiten derselbe Loop erstellt.

Schlägt dies fehl, erscheint eine Warnmeldung, dass kein Loop gefunden werden konnte (das Sample ist vermutlich zu kurz oder zu turbulent).

**Warnung:** Dieses Werkzeug verändert das ursprüngliche Sample.
Änderungen können mit der Funktion „[Rückgängig](manual/soundfont-editor/toolbar.md#doc_edit)“ zurückgenommen werden, aber wenn die Datei gespeichert und Polyphone geschlossen wurde, gibt es keinen Weg zurück.

**Hinweis:** Standardmäßig ist der Loop in einem Instrument deaktiviert.
Um ihn zu hören, müssen Sie in der Zeile des Parameters :guilabel:`Loop-Wiedergabe` in der Spalte :guilabel:`Global` oder in den Spalten der einzelnen Noten der [Tabelle](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) ![Loop ein](images/loop_on.png "Loop ein"){.inline} auswählen.
![Loop ein](images/loop_on.png "Loop ein"){.inline} schaltet ihn ein, ![Loop aus](images/loop_off.png "Loop aus"){.inline} schaltet ihn aus.
![Loop ein und Ende](images/loop_on_end.png "Loop ein und Ende"){.inline} schaltet den Loop ein, und das Sample wird nach dem Loslassen einer Taste bis zum Ende abgespielt.


### Crossfade-Loop {#doc_crossfadeloop}

Das Crossfade-Werkzeug kann als manuelle Version des [Auto-Loop-Werkzeugs](#doc_autoloop) betrachtet werden. Zunächst müssen Anfang und Ende des Loops in der [Grafik](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph) um einen stabilen Bereich herum gesetzt werden. Dann erstellt dieses Werkzeug ausgehend von diesen Positionen einen Loop, indem es eine Überblendung anwendet, deren Dauer (in Millisekunden) einstellbar ist. Alles ist also einstellbar, doch bei der Suche nach Anfang und Ende des Loops ist Sorgfalt geboten:

* an beiden Positionen müssen die Obertöne des Klangs übereinstimmen, damit beim Übergang kein Effekt wie bei einer zerkratzten Schallplatte zu hören ist,
* auch die Phasen der verschiedenen Obertöne müssen übereinstimmen, damit es während der Überblendung nicht zu einer Frequenzauslöschung kommt, die die Klangfarbe eines Instruments stark verändert,
* das Ende des Loops sollte möglichst weit vom Anfang entfernt sein, um einen zu synthetischen Klang zu vermeiden.

Ein Tipp ist, sich auf Positionen zu konzentrieren, an denen die Welle die x-Achse (Wert 0) schneidet.

![Werkzeug „Crossfade-Loop“](images/tool_crossfade_loop.png "Werkzeug „Crossfade-Loop“")


### Leere am Anfang entfernen {#doc_removeblank}

Jeder leere Bereich vor dem Beginn des Samples wird entfernt, wodurch sich die Zeit verkürzt, bis der Klang zu hören ist.
Bei einem Stereo-Sample wird auf beiden Seiten dieselbe Länge entfernt.

![Leere am Anfang entfernen](images/remove_blank.png "Leere am Anfang entfernen")

**Hinweis:** Dieses Werkzeug kann bei jedem Import eines Samples automatisch angewendet werden; die Option befindet sich in den [Einstellungen](manual/settings.md#doc_general) der Software.


### Bis zum Loop-Ende kürzen {#doc_trimloop}

Ist für das ausgewählte Sample ein Loop festgelegt, schneidet die Software den Klang nach dem Ende des Loops ab (mit einem kleinen Rand am Ende).

![Bis zum Loop-Ende kürzen](images/trim_to_loop.png "Bis zum Loop-Ende kürzen")

**Hinweis:** Je nach den [Einstellungen](manual/settings.md#doc_general) der Software kann dieses Werkzeug bei jedem Import eines Samples automatisch angewendet werden.


## Stereo-Samples {#doc_cat_stereo}


### Balance-Anpassung {#doc_balance}

Ist das Sample mit einem anderen verknüpft (Stereo), kann die Software die mittlere Intensität der beiden Samples angleichen, um die Balance anzupassen.
Dadurch lässt sich ein Sample in einem [Instrument](manual/soundfont-editor/editing-pages/instrument-editor.md) anschließend leichter genau panoramieren.
Dieses Werkzeug kann nicht mit Mono-Samples verwendet werden.


### In Mono umwandeln {#doc_mono}

Jedes ausgewählte Stereo-Sample wird in Mono umgewandelt: Ein einziges Sample ersetzt seinen linken und rechten Teil, und die Aufteilungen der Instrumente, die einen der beiden Teile verwendeten, verwenden nun dieses neue Sample.
Mono-Samples werden ignoriert, und eine Meldung erscheint, wenn die Auswahl kein Stereo-Sample enthält.

Das Mono-Signal wird je nach gewählter Option aus dem linken (L) und dem rechten (R) Kanal berechnet:

* :guilabel:`Summe: L + R`, wobei Clipping auftreten kann,
* :guilabel:`Mittelwert: (L + R) / 2`, der den Pegel eines in der Stereomitte liegenden Klangs erhält,
* :guilabel:`Konstante Leistung: (L + R) / √2`, die die Lautheit eines breiten Stereoklangs erhält (Hall, Mikrofone mit Abstand…),
* :guilabel:`Nur linker Kanal`,
* :guilabel:`Nur rechter Kanal`.

Die letzten beiden Optionen sind nicht verfügbar, wenn ein Sample im [Sample-Editor](manual/soundfont-editor/editing-pages/sample-editor.md#doc_info) den Typ :guilabel:`Verbindung` hat, da sich linker und rechter Kanal dann nicht unterscheiden lassen.

Das neue Sample übernimmt Samplefrequenz, Loop, Basisnote und Korrektur des linken Kanals bzw. des rechten Kanals, wenn nur dieser behalten wird.
Sein Name ist der des linken Kanals ohne die Kennzeichnung L&nbsp;/&nbsp;R: Aus ``bassoon45L`` und ``bassoon45R`` wird zum Beispiel ``bassoon45``.
Sind die beiden Kanäle unterschiedlich lang, wird der kürzere mit Stille aufgefüllt.
Kanäle mit unterschiedlichen Samplefrequenzen können nicht gemischt werden: Die betreffenden Samples bleiben unverändert.

Linke und rechte Aufteilungen in den Instrumenten zusammenführen
: In jedem Instrument werden eine Aufteilung, die den linken Teil verwendet, und eine Aufteilung, die den rechten Teil verwendet, zusammengeführt, wenn ihre Tasten- und Anschlagsbereiche identisch sind.
  Ihre Parameter werden gemittelt, wobei ein in einer Aufteilung nicht definierter Parameter den von der globalen Aufteilung geerbten Wert (oder den Standardwert) annimmt: Zwei ganz nach links und ganz nach rechts panoramierte Aufteilungen ergeben zum Beispiel eine mittige Aufteilung.
  :guilabel:`Loop-Wiedergabe`, :guilabel:`Exklusive Klasse`, :guilabel:`Basisnote`, :guilabel:`Fixierte Note`, :guilabel:`Fixierter Anschlag` und die Modulatoren sind die der linken Aufteilung.
  Ist diese Option nicht aktiviert, bleiben beide Aufteilungen erhalten und spielen das neue Sample.

Am Ende listet eine Meldung die Samples auf, bei denen Clipping aufgetreten ist oder die unverändert geblieben sind, sowie die Instrumente, in denen Aufteilungen nicht zusammengeführt werden konnten.


### Verknüpfung finden {#doc_link}

Unter den ausgewählten Samples werden diejenigen automatisch verknüpft, deren Namen sich nur im letzten Buchstaben (L&nbsp;/&nbsp;R) unterscheiden (Leerzeichen und die Zeichen ``-``, ``_``, ``(``, ``)``, ``[``, ``]``, ``{``, ``}`` und ``|`` werden in den Namen ignoriert).
Heißen in einem Soundfont zum Beispiel zwei Samples ``bassoon45L`` und ``bassoon45R``, wird ``bassoon45L`` als linker Teil von ``bassoon45R`` und ``bassoon45R`` als rechter Teil von ``bassoon45L`` erkannt.

Mit dieser Information wird beim Hinzufügen dieser Samples zu einem Instrument das richtige Panorama verwendet.
Außerdem können Sie in der [Tabelle](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) des Instruments den linken und rechten Teil gleichzeitig bearbeiten, wenn die Stereobearbeitung in den [Einstellungen](manual/settings.md#doc_general) aktiviert ist.

**Warnung:** Dieses Werkzeug kann bestehende Verknüpfungen aufheben, wenn die Namen nicht der obigen Regel entsprechen.
Eine Meldung listet die Samples auf, für die keine Entsprechung gefunden wurde.


### Link trennen {#doc_unlink}

Alle ausgewählten Samples, die verknüpft sind (Stereo), werden getrennt.

Dadurch werden alle Samples als Mono-Klänge behandelt, und sie erhalten alle dasselbe Panorama, wenn sie zu einem Instrument hinzugefügt werden.
Die Stereobearbeitung ist, falls in den [Einstellungen](manual/settings.md#doc_general) aktiviert, dann nicht mehr anwendbar.
