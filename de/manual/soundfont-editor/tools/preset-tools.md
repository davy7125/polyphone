Sind in der [Baumansicht](manual/soundfont-editor/tree.md) ein Preset oder eine seiner Aufteilungen ausgewählt, bietet das [Werkzeugmenü](manual/soundfont-editor/tools/index.md) mehrere Werkzeuge in 4 Kategorien:

* [Analyse](#doc_cat_analyze),
* [Schnelle Bearbeitung](#doc_cat_fast),
* [Modulatoren](#doc_cat_mod),
* [Transformation](#doc_cat_transformation).

Mit Ausnahme der Anzeige von Parametern können die Werkzeuge (mit einer Mehrfachauswahl) gleichzeitig auf mehrere Presets angewendet werden.
Alle Werkzeuge stehen auch für die [Instrumente](manual/soundfont-editor/tools/instrument-tools.md) zur Verfügung.


## Analyse {#doc_cat_analyze}


### Parameter anzeigen {#doc_display}

Dasselbe Werkzeug wie „[Parameter anzeigen](manual/soundfont-editor/tools/instrument-tools.md#doc_display)“ für Instrumente.


## Schnelle Bearbeitung {#doc_cat_fast}


### Dämpfungen ändern {#doc_attenuation}

Dasselbe Werkzeug wie „[Dämpfungen ändern](manual/soundfont-editor/tools/instrument-tools.md#doc_attenuation)“ für Instrumente.


### Tastenbasierte Konfiguration {#doc_global}

Dasselbe Werkzeug wie „[Tastenbasierte Konfiguration](manual/soundfont-editor/tools/instrument-tools.md#doc_global)“ für Instrumente.


### Räumliche Klangverteilung {#doc_spatialization}

Dasselbe Werkzeug wie „[Räumliche Klangverteilung](manual/soundfont-editor/tools/instrument-tools.md#doc_spatialization)“ für Instrumente.


## Modulatoren {#doc_cat_mod}


### Modulatoren entfernen {#doc_remove_mod}

Dasselbe Werkzeug wie „[Modulatoren entfernen](manual/soundfont-editor/tools/instrument-tools.md#doc_remove_mod)“ für Instrumente.


## Transformation {#doc_cat_transformation}


### Aufteilungen duplizieren {#doc_division}

Dasselbe Werkzeug wie „[Aufteilungen duplizieren](manual/soundfont-editor/tools/instrument-tools.md#doc_division)“ für Instrumente.


### Samples für jede Note zusammenführen {#doc_mergesamples}

Ruft ein Preset beim Anschlagen einer Taste gleichzeitig mehrere Instrumente und Samples auf, führt dieses Werkzeug für jede Note alle gespielten Samples zusammen. Das Ergebnis ist ein neues Preset, das pro Taste nur ein Sample abspielt und so die zum Abspielen des Presets benötigte CPU-Last verringert.
Das Fenster fragt nach dem Namen des neuen Presets, der Sustain-Dauer und der Release-Dauer der aufgenommenen Klänge, dem Abstand zwischen den erstellten Samples (für jede Taste, alle 3 Tasten oder alle 6 Tasten), dem Auto-Loop und der Art des Klangs (Mono oder Stereo).

Mit den im Bild unten gezeigten Optionen simuliert das Werkzeug alle 3 Noten einen 5 Sekunden langen Tastendruck. Die Audioaufnahme erfolgt in Stereo, und das Release wird nicht aufgenommen. Die so erstellten Samples werden anschließend mit dem [Auto-Loop-Werkzeug](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop) geloopt, und das Instrument mit all diesen Samples sowie das neue Preset, das es verwendet, erhalten den Namen „Cymbale III“.

**Hinweis:** Die Option Auto-Loop entfernt den Release-Teil und verkürzt die gehaltene Dauer jedes Samples geringfügig.

![Samples für jede Note zusammenführen](images/tool_merge_samples.png "Samples für jede Note zusammenführen")
