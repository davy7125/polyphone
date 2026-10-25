Sind in der [Baumansicht](manual/soundfont-editor/tree.md) ein Instrument oder seine Aufteilungen ausgewählt, bietet das [Werkzeugmenü](manual/soundfont-editor/tools/index.md) Werkzeuge in 4 Kategorien:

* [Analyse](#doc_cat_analyze),
* [Schnelle Bearbeitung](#doc_cat_fast),
* [Modulatoren](#doc_cat_mod),
* [Transformation](#doc_cat_transformation).

Mit Ausnahme der Erstellung von Akkorden und Mixturen sowie der Anzeige von Parametern können die Werkzeuge (mit einer Mehrfachauswahl) gleichzeitig auf mehrere Instrumente angewendet werden.


## Analyse {#doc_cat_analyze} 


### Parameter anzeigen {#doc_display}


Dieses Werkzeug zeigt, wie sich ein Parameter in Abhängigkeit von der Taste verändert.
Die Legende unterscheidet zwischen den in der globalen Aufteilung festgelegten Werten, den in jeder Aufteilung festgelegten Werten und dem Standardwert des Parameters.
Eine logarithmische Skala ist verfügbar (Vorsicht bei Werten kleiner oder gleich 0!).


![Werkzeug „Parameter anzeigen“](images/tool_display_parameters.png "Werkzeug „Parameter anzeigen“")


## Schnelle Bearbeitung {#doc_cat_fast}


### Dämpfungen ändern {#doc_attenuation}


Dieses Werkzeug berechnet die minimale und maximale Dämpfung aller ausgewählten Instrumente und ermöglicht es, allen Dämpfungswerten einen Offset hinzuzufügen, ohne die Ober- und Untergrenze zu überschreiten.

Beträgt zum Beispiel die geringste Dämpfung unter den Aufteilungen der ausgewählten Instrumente 16 dB und soll die minimale Dämpfung 5 dB betragen (um die Gesamtlautstärke zu erhöhen), kann die Dämpfung aller Aufteilungen aller Instrumente um 11 dB verringert werden.


![Werkzeug „Dämpfungen ändern“](images/tool_change_attenuation.png "Werkzeug „Dämpfungen ändern“")


### Verstimmen {#doc_detune}


Diese Funktion verstimmt die Noten eines Instruments leicht, um zusammen mit einem korrekt gestimmten Instrument einen schwebenden Effekt zu erzeugen (Celesta-Effekt).
Die Stärke des Effekts wird in Schwebungen pro Sekunde ausgedrückt, und folgende Einstellungen sind möglich:

* Anzahl der Schwebungen pro Sekunde an Taste 60 (das „C“ in der Mitte der Tastatur; das Vorzeichen dieser Zahl bestimmt die Richtung der Verstimmung),
* Veränderung der Anzahl der Schwebungen in Abhängigkeit von der Note: Pro Oktave wird die Anzahl der Schwebungen durch einen bestimmten Faktor geteilt oder mit ihm multipliziert (beim Wert „1“ bleibt die Anzahl der Schwebungen über die ganze Tastatur konstant).


![Werkzeug „Verstimmen“](images/tool_detune.png "Werkzeug „Verstimmen“")


Beispiel eines <a href="files/celeste_tuning_off.mp3" download>gestimmten</a> Instruments:

![](files/celeste_tuning_off.mp3)

Dieses Instrument zusammen mit seiner <a href="files/celeste_tuning_on.mp3" download>verstimmten</a> Kopie:

![](files/celeste_tuning_on.mp3)

Die zugehörige .sf2-Datei kann <a href="files/celeste_tuning.sf2" download>hier</a> heruntergeladen werden.


### Tastenbasierte Konfiguration {#doc_global}


Mit diesem Werkzeug lassen sich die Parameter aller Aufteilungen eines Instruments in Abhängigkeit vom Tastenbereich jeder Aufteilung gemeinsam bearbeiten.


![Werkzeug „Tastenbasierte Konfiguration“](images/tool_keybased_configuration.png "Werkzeug „Tastenbasierte Konfiguration“")


Die Grafik stellt die Stärke der Änderung dar: Der erste Wert ganz links entspricht der Änderung, die auf die Aufteilung mit der Taste 0 angewendet wird, der letzte Wert ganz rechts der Änderung, die auf die Aufteilung mit der Taste 127 angewendet wird.
Minimum und Maximum der Grafik sind einstellbar.
Durch Ändern des Musters kann die Kurve bearbeitet werden:

manuell zeichnen
: Die Kurve wird mit der Maus gezeichnet.

linear ansteigend / linear absteigend
: Die Kurve ist eine Gerade.

exponentiell ansteigend / exponentiell absteigend
: Die Kurve ist eine Exponentialfunktion, deren Krümmung durch den Parameter :guilabel:`Steilheit` bestimmt wird.

Zufall
: Die Werte werden zufällig erzeugt, entsprechend dem Parameter :guilabel:`Verteilung`.

Bei Verwendung eines linearen oder exponentiellen Verlaufs legt ein Linksklick auf die Grafik den Anfang und ein Rechtsklick das Ende des linearen bzw. exponentiellen Bereichs fest.

Der zu ändernde Parameter wird unter :guilabel:`Parameter` und die Art der Änderung unter :guilabel:`Modifikation` ausgewählt:

Addition
: Der Änderungswert wird zum Wert des ausgewählten Parameters addiert.

Multiplikation
: Der Wert des ausgewählten Parameters wird mit dem Änderungswert multipliziert.

Ersetzung
: Der Änderungswert ersetzt den Wert des ausgewählten Parameters.

Die Änderung kann auch nur auf Aufteilungen angewendet werden, deren Anschlagsbereich innerhalb eines bestimmten Bereichs liegt, zum Beispiel bei einem Klavier mit mehreren Anschlagsebenen.


### Natürliches Release {#doc_release}


Dieses Werkzeug erstellt für jede Aufteilung eines Instruments ein Release.
Die Release-Zeit wird anhand der ersten beiden Parameter berechnet (die Release-Zeit an Taste 36 und der Faktor zwischen benachbarten Oktaven, :guilabel:`Teilung zur nächsten Oktave`); mit dem dritten Parameter (:guilabel:`Induzierte Verstimmung (Halbtöne)`) kann zusätzlich eine Tonhöhenänderung hinzugefügt werden.


![Werkzeug „Natürliches Release“](images/tool_natural_release.png "Werkzeug „Natürliches Release“")


### Sample-Autopositionierung {#doc_position}


Die Software legt die Tastenbereiche aller Aufteilungen eines Instruments so fest, dass die Tastatur optimal genutzt wird.
Dies geschieht auch automatisch, wenn mehrere Samples mit unterschiedlichen Basisnoten mit einem leeren Instrument verknüpft werden.


### Räumliche Klangverteilung {#doc_spatialization}


Diese Funktion verteilt die Klänge in Abhängigkeit von ihrer Tonhöhe im Raum, was zum Beispiel bei einem Klavier nützlich ist (Bässe links, Höhen rechts).


![Werkzeug „Räumliche Klangverteilung“](images/tool_sound_spatialization.png "Werkzeug „Räumliche Klangverteilung“")


Jeder Balken der Grafik steht für eine Saite oder eine Pfeife, deren Länge mit der Tonhöhe zusammenhängt.
Der kürzeste Balken entspricht dem höchsten Ton, der längste dem tiefsten Ton.

Jede Note wird im Raum positioniert: Ganz links entspricht einem Klang, der von links zu hören ist, ganz rechts einem Klang, der von rechts zu hören ist.
Folgende Muster stehen zur Verfügung:

Aufsteigend
: Hohe Töne befinden sich links, tiefe Töne rechts.

Absteigend
: Tiefe Töne befinden sich links, hohe Töne rechts.

Senke
: Tiefe Töne befinden sich an den Rändern, hohe Töne in der Mitte.

Spitze
: Hohe Töne befinden sich an den Rändern, tiefe Töne in der Mitte.

Zufall
: Hohe und tiefe Töne werden zufällig verteilt.

Weitere Optionen sind verfügbar:

* Anzahl der Wiederholungen des Musters (:guilabel:`Anzahl der Teilungen`),
* Stärke des Effekts (:guilabel:`Spreizung (%)`),
* Füllung der Aufteilungen (:guilabel:`Füllgrad (%)`),
* Verschiebung nach links oder rechts, wenn die Spreizung unter 100 % liegt (:guilabel:`Offset (0-100)`),
* :guilabel:`Umkehrungen`.

Die Grafik wird bei jeder Änderung der Einstellungen aktualisiert, sodass das Ergebnis leicht zu erkennen ist.
Beim Überfahren mit der Maus wird die Zuordnung zwischen Notennummer und Panorama angezeigt.


### Transponieren {#doc_transpose}


Mit dem Werkzeug „Transponieren“ lässt sich ein ganzes Instrument transponieren.
Der Klang des Instruments bleibt gleich, aber seine Position auf der Tastatur ändert sich.

Als Eingabe wird eine Verschiebung in Halbtönen benötigt.
Um den Klang höher zu machen, muss die Verschiebung positiv sein.
Um den Klang tiefer zu machen, muss die Verschiebung dagegen negativ sein.
Das Werkzeug führt für jede Aufteilung des Instruments folgende Schritte aus:

* Anpassen der Basisnote,
* Ändern der Feinstimmung, falls um eine nicht ganzzahlige Anzahl von Halbtönen transponiert wird,
* Verschieben des Tastenbereichs entsprechend der Anzahl der Halbtöne, wenn :guilabel:`Tastenbereich anpassen` aktiviert ist.


![Werkzeug „Transponieren“](images/tool_transpose_inst.png "Werkzeug „Transponieren“")


## Modulatoren {#doc_cat_mod}


### Standardmodulator überschreiben {#doc_default_mod}


Jedem Instrument werden automatisch Standardmodulatoren zugewiesen.
Dieses Werkzeug zeigt die Liste dieser Modulatoren an und ermöglicht es, Modulatoren zu erstellen, die die ausgewählten Modulatoren überschreiben oder deaktivieren (Buttons :guilabel:`Überschreiben` und :guilabel:`Deaktivieren`).


![Werkzeug zum Überschreiben](images/tool_default_mod.png "Werkzeug zum Überschreiben")


### Modulatoren entfernen {#doc_remove_mod}


Alle Modulatoren aller ausgewählten Instrumente werden entfernt.
Eine Meldung gibt an, wie viele Modulatoren entfernt wurden.

**Hinweis:** Die Standardmodulatoren werden weiterhin angewendet.
Die einzige Möglichkeit, sie zu deaktivieren, besteht darin, sie mit dem Werkzeug „[Standardmodulator überschreiben](#doc_default_mod)“ zu überschreiben.


## Transformation {#doc_cat_transformation}


### Akkorde erstellen {#doc_chords}


Mit diesem Werkzeug lassen sich auf der Grundlage eines Instruments alle möglichen Akkorde erstellen.
Dies ist zum Beispiel hilfreich, um Chorklänge oder Pads zu erstellen.

Akkorde werden definiert, indem die enthaltenen Töne (Grundton, Terz, Quinte, Septime, None) mit ihrer Art (Dur, Moll, rein, vermindert, übermäßig), die Anzahl der Umkehrungen und die Oktave gewählt werden.
Samples mit den Akkorden werden für jede Taste, alle 3 Tasten oder alle 6 Tasten erstellt, mit oder ohne Auto-Loop, in Mono oder Stereo.


![Werkzeug „Akkorde erstellen“](images/tool_create_chords.png "Werkzeug „Akkorde erstellen“")


Beispiel. <a href="files/chord_base.mp3" download>Vorher</a>:

![](files/chord_base.mp3)

<a href="files/chord_done.mp3" download>Nachher</a>:

![](files/chord_done.mp3)

Eine .sf2-Beispieldatei auf Basis einer Flöte kann <a href="files/chords.sf2" download>hier</a> heruntergeladen werden.


### Aufteilungen duplizieren {#doc_division}


Dieser Vorgang dupliziert alle Aufteilungen eines Instruments.
Hat zum Beispiel ein mit einem Instrument verknüpftes Sample den Tastenbereich 36-38, dupliziert die Software diese Aufteilung, sodass das Sample den Tastenbereichen 36-36, 37-37 und 38-38 zugeordnet ist.
So lassen sich die Parameter eines Samples Note für Note fein bearbeiten, statt eine ganze Aufteilung mit denselben Parametern zu bearbeiten.

Aufteilungen können auch nach Anschlagsbereichen dupliziert werden.
Instrumente ohne Samples sind mit diesem Werkzeug nicht kompatibel.


![Werkzeug „Aufteilungen duplizieren“](images/tool_division_duplication.png "Werkzeug „Aufteilungen duplizieren“")


### Mixtur erzeugen {#doc_mixture}


Dieses Werkzeug ist speziell für Orgeln gedacht und ermöglicht es, auf Grundlage eines vorhandenen Instruments eine Mixtur zu erzeugen.
Es erscheint ein Fenster, in dem die Aufteilungen der Mixtur und für jede von ihnen die Liste der Chöre angegeben werden.
Die Auswahl an möglichen Chören ist sehr groß und reicht von der Oktave bis zur 27. Harmonischen.


![Werkzeug „Mixtur erzeugen“](images/tool_mixture.png "Werkzeug „Mixtur erzeugen“")


Das Werkzeug arbeitet mit allen Samples, die dem ausgewählten Instrument zugeordnet sind, den für jedes Sample festgelegten Bereichen und den zugehörigen Dämpfungen.
Die Samples müssen unabhängig von der Definition des Instruments gestimmt sein (im [Sample-Editor](manual/soundfont-editor/editing-pages/sample-editor.md)), und die angegebene Basisnote darf nicht widerspiegeln, zu welchem Register sie gehören (zum Beispiel keine Transposition, auch wenn der Klang ein Prinzipal 4' ist).

Eine Option erzeugt Samples mit Auto-Loop, eine andere legt fest, in welchem Abstand Samples erzeugt werden (für jede Taste, alle 3 Tasten, alle 6 Tasten).
Schließlich kann die Art des erzeugten Klangs gewählt werden (Mono oder Stereo).

Beispiel. <a href="files/mixture_base.mp3" download>Vorher</a>:

![](files/mixture_base.mp3)

<a href="files/mixture_done.mp3" download>Nachher</a>:

![](files/mixture_done.mp3)

Eine .sf2-Beispieldatei auf Basis eines Orgelregisters kann <a href="files/mixture.sf2" download>hier</a> heruntergeladen werden.
