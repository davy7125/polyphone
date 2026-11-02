Viele Soundfonts spielen nur ein einziges Sample pro Note: Ganz gleich, wie stark eine Taste angeschlagen wird, es erklingt immer dieselbe Aufnahme, nur lauter oder leiser.
Akustische Instrumente verhalten sich nicht so.
Eine leise angeschlagene Klaviertaste klingt rund und weich, während dieselbe Taste bei starkem Anschlag heller und obertonreicher klingt, mit einem perkussiveren Attack.

Anschlagsebenen bringen diesen Reichtum in einen Soundfont: Jede Note wird in mehreren Dynamikstufen aufgenommen, und die gespielte Aufnahme wird je nach Anschlagsstärke der Note ausgewählt.
Dieses Tutorial nimmt das Klavier als Beispiel, die Methode gilt aber für alle Instrumente, deren Klangfarbe sich mit der Spielstärke ändert: Gitarren, Schlagzeug, Blechbläser, Streicher…
Am Ende werden weitere Details vorgestellt, die ein Instrument noch realistischer machen.


## Warum ein einziges Sample nicht genügt {#doc_why}


Hören Sie die Taste C4, viermal von leise bis laut angeschlagen (Anschlagsstärken 30, 60, 90 und 120):

* mit einem <a href="files/velocity-layers-one-layer.mp3" download>einzigen Sample</a>, das in mittlerer Dynamik aufgenommen wurde,

![](files/velocity-layers-one-layer.mp3)

* mit <a href="files/velocity-layers-four-layers.mp3" download>vier Ebenen</a>, wobei die Aufnahme je nach Anschlagsstärke ausgewählt wird.

![](files/velocity-layers-four-layers.mp3)

Mit einem einzigen Sample klingen die leisen Noten wie laute Noten, die man aus der Ferne hört, und der lautesten Note fehlt es an Brillanz.
Der Synthesizer senkt bei leisen Noten zwar leicht die Grenzfrequenz eines Filters (das ist einer der Standardmodulatoren), doch das ist weit entfernt von der Klangänderung eines echten Klaviers.
Mit vier Ebenen folgt die Klangfarbe der Spielstärke.

Der Unterschied ist auch im musikalischen Zusammenhang deutlich, hier mit immer lauter werdenden Arpeggien:

* <a href="files/velocity-layers-phrase-one-layer.mp3" download>einziges Sample</a>,

![](files/velocity-layers-phrase-one-layer.mp3)

* <a href="files/velocity-layers-phrase-four-layers.mp3" download>vier Ebenen</a>.

![](files/velocity-layers-phrase-four-layers.mp3)


## Die Samples vorbereiten {#doc_samples}


### Die Ebenen aufnehmen


Jede Note muss in mehreren Dynamikstufen aufgenommen werden, zum Beispiel *pp* (pianissimo), *mp* (mezzo-piano), *mf* (mezzo-forte) und *ff* (fortissimo).
Mit 3 oder 4 Ebenen ist die Verbesserung bereits deutlich, während die besten Klavierbibliotheken 8 bis 16 Ebenen für weichere Übergänge verwenden.
Alle Aufnahmen müssen unter denselben Bedingungen entstehen (Mikrofone, Abstand, Verstärkung): Nur die Spielstärke darf sich ändern.

Das Beispiel dieses Tutorials verwendet 4 Ebenen, mit einem Sample alle 3 Tasten über den gesamten Tonumfang des Klaviers.


### Import


Benennen Sie die Dateien nach Note und Dynamik, zum Beispiel :file:`C4 pp.wav`, :file:`C4 mp.wav`, :file:`C4 mf.wav` und :file:`C4 ff.wav`.
Wenn die Basisnote nicht in einer Audiodatei gespeichert ist, findet Polyphone sie im Dateinamen, sofern dieser den Namen der Note in englischer Schreibweise (C4 ist die Taste 60, B statt H) oder die Nummer der Taste enthält.
Die im Namen angegebene Dynamik ist im nächsten Schritt nützlich.

Importieren Sie alle Dateien mit :guilabel:`Sample hinzufügen` in der [Werkzeugleiste](manual/soundfont-editor/toolbar.md#doc_edit) oder per [Drag & Drop](manual/soundfont-editor/tree.md#doc_dragdrop) in die Baumansicht.
Eine Stereodatei ergibt zwei Samples, deren Namen auf „L“ und „R“ enden.
Prüfen Sie anschließend die Stimmung jedes Samples, wie im Tutorial „[Ein Sample vorbereiten](tutorials/how-to-prepare-a-sample.md#doc_tuning)“ erklärt.
Klaviersamples brauchen keinen Loop: Der Klang klingt von selbst aus.


## Ein Instrument pro Ebene {#doc_instruments}


Jede Ebene wird zu einem Instrument, das die Samples einer einzigen Dynamikstufe enthält.
Für jede Dynamikstufe:

1. geben Sie ihren Namen (zum Beispiel „pp“) in die [Suchleiste](manual/soundfont-editor/tree.md#doc_search) der Baumansicht ein, sodass nur die Samples dieser Ebene angezeigt werden,
2. wählen Sie alle diese Samples aus: Klicken Sie auf das erste und dann mit gedrückter Taste :kbd:`Shift` auf das letzte,
3. klicken Sie in der [Werkzeugleiste](manual/soundfont-editor/toolbar.md#doc_edit) auf :guilabel:`Instrument hinzufügen`, benennen Sie das Instrument (zum Beispiel „Piano pp“) und lassen Sie die Option :guilabel:`Ausgewählte Samples verknüpfen` aktiviert.

Die Stereo-Samples werden automatisch nach links und rechts gelegt, und die Tastenbereiche werden anhand der Basisnoten über die Tastatur verteilt (siehe das Werkzeug „[Sample-Autopositionierung](manual/soundfont-editor/tools/instrument-tools.md#doc_position)“).
Jedes Instrument kann bereits allein mit der [virtuellen Tastatur](manual/soundfont-editor/toolbar.md#doc_keyboard) gespielt werden.

Die Parameter, die allen Ebenen gemeinsam sind, lassen sich auf einmal bearbeiten: Wenn die vier Instrumente in der Baumansicht ausgewählt sind, zeigt die [Tabelle](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) ihre globalen Aufteilungen nebeneinander an, und ein Wert, der nach Auswahl der vier Zellen einer Zeile eingegeben wird, wird in alle geschrieben.
Im Beispiel ist der Parameter :guilabel:`Vol env release (s)` auf 1 s gesetzt, damit der Klang beim Loslassen einer Taste nicht abrupt endet.


## Die Ebenen in einem Preset stapeln {#doc_preset}


Wählen Sie die vier Instrumente in der Baumansicht aus und klicken Sie auf :guilabel:`Preset hinzufügen`, wobei die Option :guilabel:`Ausgewählte Instrumente verknüpfen` aktiviert ist.
Das Preset enthält dann vier Aufteilungen, die alle die gesamte Tastatur abdecken.
Die Ebenen werden gestapelt, indem jede Aufteilung des Presets im [Preset-Editor](manual/soundfont-editor/editing-pages/preset-editor.md) einen anderen :guilabel:`Anschlagsbereich` erhält:

* 0-39 für die Ebene *pp*,
* 40-69 für die Ebene *mp*,
* 70-99 für die Ebene *mf*,
* 100-127 für die Ebene *ff*.

Die Bereiche müssen alle Anschlagsstärken von 0 bis 127 abdecken, ohne Lücken (manche Anschlagsstärken blieben stumm) und ohne Überschneidungen (zwei Ebenen wären gleichzeitig zu hören).
Die Grenzen müssen nicht regelmäßig sein: Sie können je nach Aufnahmen verschoben werden.
Die Werte der Zeile :guilabel:`Dämpfung (dB)` werden im nächsten Abschnitt erklärt.


![Anschlagsebenen in der Tabelle des Presets](images/velocity_layers_table.png "Anschlagsebenen in der Tabelle des Presets")


Im [Bereichseditor](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range) ist jede Ebene ein waagerechter Streifen: Die ausgewählte Aufteilung (hier *mp*) ist schraffiert.
Diese Ansicht zeigt schnell eine Lücke oder eine Überschneidung.


![Anschlagsebenen im Bereichseditor](images/velocity_layers_ranges.png "Anschlagsebenen im Bereichseditor")


**Hinweis:** Die Anschlagsbereiche können auch in einem einzigen Instrument festgelegt werden, das alle Samples enthält, wobei jede Aufteilung ihren eigenen Anschlagsbereich hat.
In diesem Fall hält die Wahl :guilabel:`Nach Anschlagsbereich` der Option :guilabel:`Aufteilungen sortieren` in den [Einstellungen](manual/settings.md#doc_interface) die Aufteilungen einer Ebene beieinander.


## Die Ebenen ausgleichen {#doc_balance}


Innerhalb einer Ebene ändert sich die Lautstärke bereits mit der Anschlagsstärke: Dafür sorgt ein anderer Standardmodulator, der im sf2-Format definiert ist.
An jeder Grenze steigt die Lautstärke jedoch plötzlich an, weil die Aufnahmen der oberen Ebene lauter sind: im Beispiel um etwa 5 bis 7 dB.
Ziel ist, dass sich beim Übergang von einer Ebene zur nächsten nur die Klangfarbe ändert und die Lautstärke gleichmäßig zunimmt.

Vergleichen Sie die beiden Seiten einer Grenze mit der [virtuellen Tastatur](manual/soundfont-editor/toolbar.md#doc_keyboard): Die Anschlagsstärke einer Note hängt von der senkrechten Position des Klicks ab, und die Pfeiltasten :kbd:`↑` und :kbd:`↓` spielen die zuletzt gespielte Taste erneut mit einer um 1 erhöhten bzw. verringerten Anschlagsstärke.
Gehen Sie dann für jede Grenze von unten nach oben vor:

1. spielen Sie eine Taste mit der letzten Anschlagsstärke einer Ebene (zum Beispiel 39) und dann mit der ersten Anschlagsstärke der nächsten Ebene (40),
2. erhöhen Sie die Dämpfung der oberen Ebene in der Tabelle des Presets, bis beide Noten ungefähr gleich laut sind,
3. fahren Sie mit der nächsten Grenze fort: Die Dämpfungen summieren sich, jede Ebene ist mindestens so stark gedämpft wie die Ebene darunter.

Im Beispiel betragen die Dämpfungen 0, 6, 11 und 16 dB, wie in der Tabelle oben zu sehen.
Prüfen Sie mehrere Tasten über die ganze Tastatur: Bleibt ein Register unausgeglichen, kann die Dämpfung der betroffenen Aufteilungen im Instrument der Ebene verfeinert werden, zum Beispiel mit dem Werkzeug „[Tastenbasierte Konfiguration](manual/soundfont-editor/tools/instrument-tools.md#doc_global)“.

Der gesamte Dynamikumfang des Instruments, von der leisesten bis zur lautesten Note, wird dann durch den Standardmodulator bestimmt.
Sind die leisen Noten zu leise oder nicht leise genug, kann dieser Modulator mit dem Werkzeug „[Standardmodulator überschreiben](manual/soundfont-editor/tools/instrument-tools.md#doc_default_mod)“ geändert werden.

Zum Testen des Ergebnisses geht schließlich nichts über ein echtes Musikstück: Spielen Sie den Soundfont mit einem MIDI-Keyboard oder mit dem [MIDI-Player](manual/soundfont-editor/toolbar.md#doc_midi_player) (siehe das Tutorial „[Eine MIDI-Datei mit einem Soundfont abspielen](tutorials/read-midi-file-with-soundfont.md)“).


## Mehr Details für mehr Realismus {#doc_details}


Anschlagsebenen sind ein großer Schritt, doch ein echtes Instrument hat weitere Eigenschaften, die ein Soundfont nachbilden kann.
Hier sind zwei davon für das Klavier.


### Loslassgeräusche


Wenn eine Taste losgelassen wird, fällt der Dämpfer auf die Saiten zurück und die Mechanik macht ein leises Geräusch.
Manche Sample-Bibliotheken liefern diese Geräusche mit: Die im Beispiel verwendete hat eines pro Taste.

Sie werden in einem fünften Instrument, „Piano Release“, abgelegt, das dem Preset über den gesamten Anschlagsbereich hinzugefügt wird:

* wählen Sie in der Zeile :guilabel:`Loop-Wiedergabe` den Wiedergabemodus ![Release](images/sample_mode_release.png "Release"){.inline} (Release), damit ein Sample beim Loslassen der Taste abgespielt wird,
* verwenden Sie einen langen Wert für :guilabel:`Vol env release (s)` (5 s im Beispiel), damit das Geräusch vollständig abgespielt wird,
* dämpfen Sie das Instrument stark (40 dB im Beispiel): Das Geräusch muss dezent bleiben.

Dank des Standardmodulators, der die Anschlagsstärke mit der Lautstärke verknüpft, ist das Geräusch lauter, wenn die Note stärker angeschlagen wurde.

**Hinweis:** Der Wiedergabemodus „Release“ ist im sf2-Format nicht dokumentiert und wird von anderen Playern möglicherweise nicht unterstützt.
Das Tutorial „[Eigene Releases in einem Instrument verwenden](tutorials/using-custom-releases-in-an-instrument.md)“ beschreibt eine Methode, die mit jedem Player funktioniert.


### Saiten ohne Dämpfer


Beim Klavier haben die höchsten Saiten keine Dämpfer (ungefähr die letzten anderthalb Oktaven): Sie klingen nach dem Loslassen der Taste weiter.
In jedem Instrument einer Ebene erhalten die Aufteilungen dieser Tasten (im Beispiel ab F6, Taste 89) daher einen längeren Wert für :guilabel:`Vol env release (s)`: 3 s statt 1 s.

Hören Sie kurze Noten in der Mitte der Tastatur, dann in der höchsten Oktave und schließlich einen Akkord im Bass:

* <a href="files/velocity-layers-without-details.mp3" download>ohne diese Details</a>,

![](files/velocity-layers-without-details.mp3)

* <a href="files/velocity-layers-with-details.mp3" download>mit den Loslassgeräuschen und den Saiten ohne Dämpfer</a>.

![](files/velocity-layers-with-details.mp3)


### Weitere Ideen


Weitere Verbesserungen sind einen Versuch wert:

* jede Taste aufnehmen statt nur jede dritte: Beim Klavier hat jede Taste ihre eigenen Saiten, und ein transponiertes Sample ist nie genau die Nachbarnote,
* Stereoaufnahmen verwenden, die dem Klang Breite verleihen,
* Release-Zeiten, die sich über die Tastatur verändern, mit dem Werkzeug „[Natürliches Release](manual/soundfont-editor/tools/instrument-tools.md#doc_release)“ berechnen,
* die Lautstärke oder den Filter je nach Register mit dem Werkzeug „[Tastenbasierte Konfiguration](manual/soundfont-editor/tools/instrument-tools.md#doc_global)“ anpassen.

Und dieselbe Vorgehensweise funktioniert auch bei anderen Instrumenten: eine sanft oder kräftig gezupfte Gitarrensaite, eine kleine Trommel zwischen Ghost Notes und Akzenten, eine Trompete, die von einem runden zu einem schmetternden Klang übergeht…
Jedes Detail, das dem echten Instrument abgeschaut wird, macht den Soundfont lebendiger.
