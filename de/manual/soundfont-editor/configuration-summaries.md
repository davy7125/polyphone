Die Konfigurationsübersichten bieten einen Überblick über alle in einem Soundfont enthaltenen Elemente.
Sie sind über die Überschriften :guilabel:`Samples`, :guilabel:`Instrumente` und :guilabel:`Presets` in der [Baumansicht](manual/soundfont-editor/tree.md) erreichbar und ermöglichen die Visualisierung der jeweiligen Elemente.

Die auf diesen Seiten zusammengetragenen Informationen erleichtern die Überprüfung großer Soundfonts.

Jedes Element wird in einer Zeile einer Tabelle angezeigt.
Ein Klick auf den Kopf einer Spalte sortiert die Tabelle nach dieser Spalte, und ein Doppelklick auf eine Zeile öffnet das entsprechende Element auf seiner [Bearbeitungsseite](manual/soundfont-editor/editing-pages/index.md).
Werte, die Aufmerksamkeit verdienen, werden fett, gelb (Warnung) oder rot (Fehler) dargestellt.


## Samples {#doc_sample}


Ein Klick auf die Überschrift :guilabel:`Samples` eines Soundfonts in der [Baumansicht](manual/soundfont-editor/tree.md) zeigt die Konfigurationsübersicht der Samples an.


![Konfigurationsübersicht der Samples](images/conf_summary_sample.png "Konfigurationsübersicht der Samples")


Für jedes Sample gibt es die Spalten :guilabel:`Verwendet`, :guilabel:`Gesamtdauer`, :guilabel:`Loop-Dauer`, :guilabel:`Basisnote`, :guilabel:`Korrektur`, :guilabel:`Sample-Typ`, :guilabel:`Verknüpftes Sample`, :guilabel:`Samplefrequenz` und :guilabel:`Kompression`.
Auf dieser Seite lassen sich zum Beispiel folgende Prüfungen durchführen:

* Wird das Sample verwendet?
* Ist seine Gesamtdauer (und gegebenenfalls seine Loop-Dauer) lang genug?
* Ist der Loop korrekt? (ein Loop, der wahrscheinlich ein Klicken erzeugt, wird hervorgehoben)
* Wurde das Sample gestimmt?
* Ist es korrekt als Stereo verknüpft? (die beiden Samples eines Stereopaars sollten dieselbe Basisnote haben)
* Ist die Samplefrequenz hoch genug für eine gute Audioqualität?
* Wurde es komprimiert?


## Instrumente {#doc_instrument}


Ein Klick auf die Überschrift :guilabel:`Instrumente` eines Soundfonts in der [Baumansicht](manual/soundfont-editor/tree.md) zeigt die Konfigurationsübersicht der Instrumente an.


![Konfigurationsübersicht der Instrumente](images/conf_summary_instrument.png "Konfigurationsübersicht der Instrumente")


Für jedes Instrument gibt es die Spalten :guilabel:`Verwendet`, :guilabel:`Anzahl Samples`, :guilabel:`Anzahl Parameter`, :guilabel:`Anzahl Modulatoren`, :guilabel:`Max. Tastenbereich`, :guilabel:`Max. Anschlagsbereich`, :guilabel:`Dämpfung`, :guilabel:`Loop-Wiedergabe`, :guilabel:`Chorus` und :guilabel:`Halleffekt`.
Auf dieser Seite lassen sich zum Beispiel folgende Prüfungen durchführen:

* Wird das Instrument verwendet?
* Ist die Anzahl der verknüpften Samples, Parameter und Modulatoren stimmig?
* Sind die Tasten- und Anschlagsbereiche korrekt?
* Wurden die Samples richtig gedämpft?
* Ist die Loop-Wiedergabe aktiviert?
* Wurden die Einstellungen für Chorus und Hall vergessen?


## Presets {#doc_preset}


Ein Klick auf die Überschrift :guilabel:`Presets` eines Soundfonts in der [Baumansicht](manual/soundfont-editor/tree.md) zeigt die Konfigurationsübersicht der Presets an.


![Konfigurationsübersicht der Presets](images/conf_summary_preset.png "Konfigurationsübersicht der Presets")


Für jedes Preset gibt es die Spalten :guilabel:`Bank - Preset`, :guilabel:`Anzahl Instrumente`, :guilabel:`Anzahl Parameter`, :guilabel:`Anzahl Modulatoren`, :guilabel:`Max. Tastenbereich`, :guilabel:`Max. Anschlagsbereich`, :guilabel:`Dämpfung`, :guilabel:`Chorus` und :guilabel:`Halleffekt`.
Auf dieser Seite lassen sich zum Beispiel folgende Prüfungen durchführen:

* Wurden Bank- und Presetnummer richtig ausgefüllt?
* Ist die Anzahl der verknüpften Instrumente, Parameter und Modulatoren stimmig?
* Sind die Tasten- und Anschlagsbereiche korrekt?
* Wurden die Instrumente genau gedämpft?
* Wurden die Einstellungen für Chorus und Hall (als Offsets) vergessen?