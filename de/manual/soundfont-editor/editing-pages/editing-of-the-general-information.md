Die Seite zur Bearbeitung der allgemeinen Informationen wird angezeigt, wenn in der [Baumansicht](manual/soundfont-editor/tree.md) die Überschrift :guilabel:`Allgemein` ausgewählt ist.
Diese Seite besteht aus drei Arten von Elementen:

* [bearbeitbaren Feldern](#doc_edit),
* nicht bearbeitbaren [allgemeinen Informationen](#doc_info),
* [Statistiken](#doc_stats) zum Soundfont.


![Seite der allgemeinen Informationen](images/edit_general.png "Seite der allgemeinen Informationen")


## Bearbeitbare Felder {#doc_edit}


Die Bearbeitungsseite der allgemeinen Informationen bietet Felder zum Ändern der folgenden Attribute:

Titel
: Der interne Name des Soundfonts, der nicht unbedingt etwas mit dem Dateinamen zu tun haben muss.

Autor
: Der Name des Autors des Soundfonts.

Datum
: Das Erstellungsdatum der Datei (ein Kalender hilft bei der Auswahl).

Produkt
: Das Produkt, für das der Soundfont gedacht ist.

Copyright
: Ein Hinweis, der auszufüllen ist, wenn die Datei urheberrechtlich geschützt ist.

Kommentare
: Informationen, die in keine der vorherigen Kategorien passen (der Text kann beliebige UTF-8-Zeichen enthalten).

Bittiefe
: Gibt an, ob die Samples mit einer Auflösung von 16 Bit oder 24 Bit gespeichert werden (in diesem Fall ist keine Kompression möglich).
  Bitte beachten Sie, dass 24-Bit-Samples von manchen Synthesizern nicht unterstützt werden.

Kompression
: Aktiviert die Kompression der Samples (in diesem Fall wird die Bittiefe auf 16 Bit gesetzt). Ist diese Option aktiviert, wird im [Sample-Editor](manual/soundfont-editor/editing-pages/sample-editor.md) ein Button zum Komprimieren angezeigt.
  Die Kompression ist eine Funktion des [sf3-Formats](manual/annexes/the-different-soundfont-formats.md#doc_sf3) und wird möglicherweise nicht von allen Synthesizern unterstützt.
  Das Aktivieren oder Deaktivieren dieser Option bestimmt das Dateiformat beim Speichern, und Polyphone ändert bei Bedarf die Dateiendung .sf2 / .sf3.


## Allgemeine Informationen {#doc_info}


Die Bearbeitungsseite der allgemeinen Informationen zeigt die folgenden nicht bearbeitbaren Informationen an:

SoundFont-Format
: Version des von der Datei verwendeten [sf2-Formats](manual/annexes/the-different-soundfont-formats.md#doc_sf2).

ROM-Name und -Version
: Name und Version eines Sample-ROMs, falls eines verwendet wird.

Sound-Engine
: Name einer Sound-Engine.

Bearbeitungssoftware
: Die zur Erstellung des Soundfonts verwendete Bearbeitungssoftware.

Diese Angaben werden in einem Tooltip angezeigt, wenn sich die Maus über dem Informationssymbol am Ende der Zeile mit Autor und Datum befindet.

Dateiname und -größe
: Pfad und Name der Datei sowie ihre Größe auf dem Datenträger, angezeigt am unteren Rand des Fensters (es handelt sich um die Größe der Datei beim letzten Speichern).


## Statistiken {#doc_stats}


Gezählt werden:

* die Anzahl der [Samples](manual/soundfont-editor/editing-pages/sample-editor.md) und der nicht verwendeten Samples,
* die Anzahl der [Instrumente](manual/soundfont-editor/editing-pages/instrument-editor.md) und der nicht verwendeten Instrumente,
* die Anzahl der [Presets](manual/soundfont-editor/editing-pages/preset-editor.md),
* die Anzahl der für die Instrumente bearbeiteten Parameter und Modulatoren,
* die Anzahl der für die Presets bearbeiteten Parameter und Modulatoren.

Der Button :guilabel:`Details` neben den Samples, Instrumenten und Presets zeigt die zugehörige [Konfigurationsübersicht](manual/soundfont-editor/configuration-summaries.md) an.

Nicht verwendete Samples und Instrumente lassen sich mit dem Werkzeug „[Nicht verwendete Elemente entfernen](manual/soundfont-editor/tools/global-tools.md#doc_unused)“ leicht entfernen.

**Hinweis:** Die Anzahl von 65536 Parametern in den Instrumenten und Presets sollte nicht überschritten werden.
Manche Synthesizer können sie sonst möglicherweise nicht alle lesen.
