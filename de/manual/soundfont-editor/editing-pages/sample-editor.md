Ein Klick auf ein oder mehrere Samples in der [Baumansicht](manual/soundfont-editor/tree.md) zeigt die Bearbeitungsseite der Samples an.

Diese Seite ist in mehrere Teile gegliedert:

* eine [Grafik](#doc_graph), die das Sample darstellt,
* einen [Informationsbereich](#doc_info),
* eine Analyse der [Frequenzen](#doc_freq),
* einen [Equalizer](#doc_eq),
* einen [Player](#doc_player) für das Sample.


![Sample-Seite](images/edit_sample.png "Sample-Seite")


## Die Grafik {#doc_graph}


### Zoomen & Verschieben


Die Grafik ermöglicht die Visualisierung der Schallwelle.
Durch Gedrückthalten der rechten Maustaste und Ziehen kann vertikal oder horizontal gezoomt werden:

* Ziehen nach rechts dehnt die horizontale Achse,
* Ziehen nach links staucht die horizontale Achse,
* Ziehen nach oben dehnt die vertikale Achse,
* Ziehen nach unten staucht die vertikale Achse.

Durch Gedrückthalten der linken Maustaste lässt sich die Grafik horizontal verschieben, sofern die horizontale Achse gedehnt wurde.
Auch mit dem Mausrad lässt sich die Grafik horizontal verschieben.


### Loop- und Wiedergabepositionen


Ein grüner senkrechter Balken zeigt die Startposition des Loops an (änderbar per Linksklick auf die Grafik), ein roter Balken die Position des Loop-Endes (änderbar per Rechtsklick auf die Grafik).
Zu jedem senkrechten Balken gehört eine gestrichelte Linie, die einen Wellenausschnitt darstellt; so lässt sich die Überlagerung von Anfang und Ende des Loops gut erkennen.

Während der Wiedergabe läuft ein weißer senkrechter Balken über die Grafik und zeigt die Wiedergabeposition in Echtzeit an.


### Das Sample schneiden


Bei gedrückter Taste :kbd:`Alt` kann ein zu schneidender Bereich festgelegt werden:

* das Drücken der linken Maustaste legt den Anfang des Bereichs fest,
* das zugehörige Loslassen der linken Maustaste legt das Ende des Bereichs fest.

Sobald ein Bereich festgelegt ist, erscheint ein Dialog, der vor dem Schneiden des Samples eine Bestätigung verlangt.
Die Antwort :guilabel:`Ja und ein weiteres Sample erstellen` schneidet das Sample und erstellt zusätzlich ein neues Sample aus dem entfernten Teil.


![Sample schneiden](images/cutting_sample.png "Sample schneiden")


### Farben der Grafik


Die Farben der Grafik können in den [Einstellungen](manual/settings.md#doc_interface) der Software geändert werden.


## Bereich „Information“ {#doc_info}


Im Bereich :guilabel:`Information` lassen sich die folgenden Angaben anzeigen und in den meisten Fällen auch bearbeiten:

Größe
: Die Anzahl der Werte und die entsprechende Dauer in Sekunden werden angezeigt.

Loop
: Hier können Start- und Endpunkt des Loops geändert werden.
  Ein Button setzt Start- und Endposition so, dass der Loop das gesamte Sample umfasst.

Stimmung
: Hier können die Basisnote eines Samples und seine Korrektur in Cent angepasst werden, damit der Klang gleichstufig gestimmt ist.
  Ein Button setzt Basisnote und Korrektur auf die Werte, die der geschätzten [Frequenz](#doc_freq) entsprechen.

Samplefrequenz
: Jede Änderung löst ein Resampling aus, sodass die Tonhöhe des Klangs nicht verändert wird.

Kanal
: Auswahl des Typs unter :guilabel:`Mono`, :guilabel:`Links`, :guilabel:`Rechts` oder :guilabel:`Verbindung`.
  Ist der Typ :guilabel:`Links`, :guilabel:`Rechts` oder :guilabel:`Verbindung`, kann das Stereo durch Auswahl von :guilabel:`Mono` aufgehoben werden.
  Ist :guilabel:`Mono` ausgewählt, wird durch Auswahl eines verknüpften Samples wieder in den Stereozustand gewechselt.

Verbindung
: Auswahl des Samples, das verknüpft werden soll, um den Klang zu einem Stereoklang zu machen.
  Wird ein Sample ausgewählt, wechselt sein Typ zu :guilabel:`Verbindung` (dieser Typ kann anschließend in :guilabel:`Links` oder :guilabel:`Rechts` geändert werden).

Ist die [Kompression](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md#doc_edit) für den Soundfont aktiviert ([sf3-Format](manual/annexes/the-different-soundfont-formats.md#doc_sf3)), wird für die noch nicht komprimierten Samples ein Button zum Komprimieren angezeigt.
Beim Komprimieren wird in einem Dialog die Kompressionsqualität (:guilabel:`Hoch`, :guilabel:`Mittel` oder :guilabel:`Niedrig`) abgefragt.
Der Loop eines Samples kann durch die Kompression verändert werden.\
**Hinweis:** Beim Bearbeiten des Samples (Anwenden eines Filters, Kürzen, Verstärken usw.) wird automatisch dekomprimiert.

![Auswahl der Kompressionsqualität](images/sample_compress_dialog.png "Auswahl der Kompressionsqualität")

Jede Änderung an einem Stereo-Sample kann auf das verknüpfte Sample übertragen werden, wenn die Option :guilabel:`Stereobearbeitung` in den [Einstellungen](manual/settings.md#doc_general) aktiviert ist (oder über den Button unten rechts im Fenster).
Über eine Mehrfachauswahl in der [Baumansicht](manual/soundfont-editor/tree.md) können mehrere Samples gleichzeitig bearbeitet werden.


## Frequenzanalyse {#doc_freq}


Die Frequenzanalyse umfasst eine Fourier-Transformation (eine Grafik, die die Intensität der im Signal enthaltenen Frequenzen zeigt).

![Fourier-Grafik](images/fourier_graph.png "Fourier-Grafik")

Rechts wird die Liste der dominanten Spitzen mit den folgenden Informationen angezeigt:

* Verhältnis zwischen der Intensität der Spitze und der Intensität der höchsten Spitze,
* Frequenz,
* nächstgelegene entsprechende Taste,
* Korrektur (Abstand zur nächstgelegenen entsprechenden Taste).

Aus diesen Daten wird eine Schätzung der Tonhöhe des Klangs berechnet.
Diese Schätzung kann zum Stimmen verwendet werden.

Ein Rechtsklick auf die Grafik öffnet ein Kontextmenü, mit dem Sie:

* die Grafik als .png-Bild exportieren,
* die Liste aller Spitzen sowie einen Button zum Exportieren dieser Liste in eine .csv-Datei anzeigen können.

![Spitzenfrequenzen anzeigen](images/tool_show_peak_frequencies.png "Spitzenfrequenzen anzeigen")


## Equalizer {#doc_eq}


Mit dem Equalizer lassen sich bestimmte Frequenzen um -15 bis 15 dB anheben oder absenken.
Bei aktivierter Option :guilabel:`Vorhören` kann der Effekt angehört und anschließend mit dem Button :guilabel:`Anwenden` endgültig übernommen werden.
Ein Klick auf :guilabel:`Zurücksetzen` setzt alle Regler auf 0 dB zurück (entfernt aber keinen Effekt, der bereits auf den Klang angewendet wurde: Verwenden Sie dafür den Button :guilabel:`Rückgängig` in der [Werkzeugleiste](manual/soundfont-editor/toolbar.md#doc_edit)).

Jede Änderung an einem Stereo-Sample kann automatisch auf das verknüpfte Sample übertragen werden, wenn die Option in den [Einstellungen](manual/settings.md#doc_general) aktiviert ist.


## Player {#doc_player}


Der Player spielt den Klang ab.
Während der Wiedergabe läuft ein weißer Cursor über die Grafik und zeigt die Wiedergabeposition an.
Die Wiedergabeoptionen sind:

Loop
: Spielt den Klang im Loop ab, wenn Anfang und Ende des Loops festgelegt sind.

Stereo
: Spielt das Sample zusammen mit seinem verknüpften Sample in Stereo ab.
  Ist zusätzlich ein Loop gewünscht, werden die beiden jeweiligen Loops verwendet (sie können unterschiedlich sein).

Sinus
: Dem Klang wird ein Sinus-Kalibriersignal hinzugefügt, mit dem ein Sample auf ein Hundertstel Halbton genau gestimmt werden kann.
  Zunächst muss die Basisnote eingetragen werden.
  Dabei sind Schwebungen zu hören, wenn die Frequenz des Sinus leicht von der Grundfrequenz des Klangs abweicht.
  Anschließend muss nur noch die Korrektur fein eingestellt werden, damit die Schwebungen möglichst langsam werden und das Sample so gut wie möglich gestimmt ist.
  Weitere Informationen finden Sie im Tutorial „[Ein Sample vorbereiten](tutorials/how-to-prepare-a-sample.md)“.

Ein Schieberegler legt die Lautstärke der Wiedergabe fest (diese Lautstärke wird nicht im Soundfont gespeichert).

Läuft gerade eine Wiedergabe, wenn über die [Baumansicht](manual/soundfont-editor/tree.md) von einem Sample zu einem anderen gewechselt wird, startet sie automatisch neu.
Außerdem startet oder stoppt die Taste :kbd:`Space` die Wiedergabe, wenn die Baumansicht den Fokus hat.
