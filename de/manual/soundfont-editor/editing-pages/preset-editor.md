Ein Klick auf ein oder mehrere Presets in der [Baumansicht](manual/soundfont-editor/tree.md) zeigt die Bearbeitungsseite der Presets an.
Ein Preset ist eine Kombination aus einem oder mehreren Instrumenten und das Element, das von außerhalb eines [Soundfonts](manual/annexes/the-different-soundfont-formats.md#doc_sf2) sichtbar ist.

Wie beim [Instrument-Editor](manual/soundfont-editor/editing-pages/instrument-editor.md) gibt es mehrere Teile:

* die [Tabelle](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table),
* den [Bereichseditor](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range), wenn in der [Werkzeugleiste](manual/soundfont-editor/toolbar.md#doc_view) auf den Button :guilabel:`Bereiche` geklickt wird,
* den [Modulatorbereich](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator).


![Preset-Seite](images/edit_preset.png "Preset-Seite")


Ist nur ein Preset ausgewählt, können auf der Seite die globalen Parameter des Presets und die Parameter aller seiner Aufteilungen bearbeitet werden.
Sind mehrere Presets ausgewählt, können die globalen Parameter jedes dieser Presets bearbeitet werden.


## Unterschiede zu den Instrumenten {#doc_diff}


Die hier bearbeiteten Parameter werden als relativ bezeichnet, im Gegensatz zu den Parametern eines Instruments, die – von Sonderfällen abgesehen – als absolut bezeichnet werden.
Die Werte der Parameter eines Presets werden zu den Parametern der Instrumente, aus denen es besteht, addiert oder mit ihnen multipliziert.
Aus diesem Grund kann eine Dämpfung (oder jeder andere Parameter) auf Preset-Ebene negativ sein, was auf Instrumentebene nicht der Fall ist.
Einige Parameter können auf Preset-Ebene nicht bearbeitet werden:

* Loop-Wiedergabe,
* Basisnote,
* exklusive Klasse,
* fixierte Note,
* fixierter Anschlag,
* die vier Offsets.

Ein Preset wird durch eine Presetnummer und eine Banknummer identifiziert; diese Kombination ist eindeutig und kann unterhalb der Tabelle eingestellt werden.
Jede Bank umfasst 128 Presets (nummeriert von 0 bis 127).

Die letzte Bank (128) wird üblicherweise für Percussion-Kits verwendet.
