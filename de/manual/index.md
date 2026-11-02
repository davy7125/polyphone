Willkommen im **Benutzerhandbuch** von Polyphone! Um schnell zu entdecken, was Soundfonts sind und wie man sie mit Polyphone bearbeitet, können Sie mit den [Tutorials](tutorials/index.md) beginnen oder sich Videos auf <a href="https://www.youtube.com/results?search_query=polyphone+sf2" target="_blank">YouTube</a> ansehen (ein besonderer Dank geht an die Polyphone-Nutzer für ihre hilfreichen Videos). Und wenn Sie alles über die Funktionen der Software erfahren möchten, lesen Sie einfach weiter.

Wenn Sie weitere Hilfe benötigen oder die Lektüre dieser Seiten Sie abschreckt, finden Sie Unterstützung im [Forum](forum).


## Startbildschirm {#doc_home}


Der erste Bildschirm von Polyphone besteht im Wesentlichen aus 3 Teilen:

* einem [linken Teil](#doc_left) mit einem Verlauf der geöffneten Dateien und Verzeichnisse sowie mit Verknüpfungen;
* einem [rechten Teil](#doc_right), der der Soundfont-Fundgrube gewidmet ist;
* einem [oberen Bereich](#doc_top) mit Tabs und einem Menü.

Unter den Tabs kann außerdem ein Banner eine auf der Website veröffentlichte [Benachrichtigung](#doc_notifications) anzeigen.


![Startbildschirm von Polyphone](images/page_home.png "Startbildschirm von Polyphone")


## Linker Teil {#doc_left}


### Einen Soundfont öffnen


Es gibt mehrere Möglichkeiten, einen Soundfont zu öffnen:

* Ein Klick auf :guilabel:`Soundfont öffnen` öffnet einen Dateibrowser, in dem Dateien mit der Endung :file:`.sf2`, :file:`.sf3`, :file:`.sfArk`, :file:`.sfz`, :file:`.dls` oder :file:`.organ` ausgewählt werden können (die verschiedenen Formate werden [hier](manual/annexes/the-different-soundfont-formats.md) beschrieben). Es können mehrere Dateien auf einmal ausgewählt werden;
* ein Klick auf :guilabel:`Neuer Soundfont` öffnet einen leeren Soundfont, in dem alles von Grund auf erstellt werden muss;
* ein Klick auf :guilabel:`Verzeichnis öffnen` öffnet den [Soundfont-Explorer](manual/soundfont-explorer.md), der ein ganzes Verzeichnis durchsucht und das Öffnen der gewünschten Datei ermöglicht;
* ein Doppelklick auf einen Soundfont im Verlauf öffnet ihn erneut (die ausgewählten Einträge können auch mit :kbd:`Enter` geöffnet und mit :kbd:`Del` oder mit dem rechts neben ihrem Pfad angezeigten Papierkorbsymbol aus dem Verlauf entfernt werden);
* eine Datei oder ein Verzeichnis, die bzw. das per Drag & Drop auf den Verlaufsbereich oder – unabhängig vom aktuellen Tab – auf die [Tab-Leiste](#doc_top) gezogen wird, wird geöffnet.

Wenn ein Soundfont geöffnet ist, erscheint ein neuer Tab mit einem [Soundfont-Editor](manual/soundfont-editor/index.md).


### Verknüpfungen


Am unteren Rand des linken Teils befinden sich drei Buttons:

* :guilabel:`Dokumentation`: öffnet einen Webbrowser, um die [Dokumentation](documentation) von Polyphone zu lesen;
* :guilabel:`Forum`: öffnet einen Webbrowser mit der Adresse des [Forums](forum) von Polyphone;
* :guilabel:`Einstellungen`: öffnet die [Einstellungen](manual/settings.md) der Software in einem neuen Tab (auch über :kbd:`Ctrl`+:kbd:`,` erreichbar).


## Rechter Teil {#doc_right}


### Online-Fundgrube


Ein Klick auf den Button :guilabel:`Online-Fundgrube` öffnet einen Tab, der den Inhalt der [Online-Fundgrube für Soundfonts](manual/online-repository.md) anzeigt.


### Tägliche Soundfonts


Hier werden jeden Tag 5 zufällig ausgewählte Soundfonts angezeigt, um zum Entdecken neuer Soundfonts anzuregen.
Ein Doppelklick auf einen von ihnen öffnet einen neuen Tab mit der zugehörigen [Soundfont-Beschreibung](manual/online-repository.md#doc_description).
Ein Klick auf den Namen eines Autors oder auf ein Attribut eines Soundfonts öffnet die [Online-Fundgrube](manual/online-repository.md) mit dem entsprechenden Filter.


### Suche


Hier kann ein Suchbegriff eingegeben werden, um in der Online-Fundgrube nach bestimmten Soundfonts zu suchen; die Ergebnisse werden im [dafür vorgesehenen Tab](manual/online-repository.md) angezeigt.


## Oberer Bereich {#doc_top}


### Tabs


Die Tabs ermöglichen einen schnellen Wechsel zwischen:

* dem [Startbildschirm](#doc_home) (erster Tab – immer geöffnet),
* dem [Editor](manual/soundfont-editor/index.md) eines geöffneten Soundfonts,
* dem [Soundfont-Explorer](manual/soundfont-explorer.md) (einer pro geöffnetem Verzeichnis),
* den [Einstellungen](manual/settings.md) der Software,
* der [Online-Fundgrube](manual/online-repository.md),
* einer [Soundfont-Beschreibung](manual/online-repository.md#doc_description) aus der Online-Fundgrube.

Zwischen den Tabs kann mit Tastenkombinationen oder dem Mausrad navigiert werden:

* :kbd:`Ctrl`+:kbd:`Tab`, :kbd:`Ctrl`+:kbd:`PageDown` oder :kbd:`Shift`+Mausrad nach unten über der Tab-Leiste, um zum nächsten Tab zu wechseln (ohne den Tab des Startbildschirms),
* :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`Tab`, :kbd:`Ctrl`+:kbd:`PageUp` oder :kbd:`Shift`+Mausrad nach oben über der Tab-Leiste, um zum vorherigen Tab zu wechseln (ohne den Tab des Startbildschirms),
* :kbd:`Ctrl`+:kbd:`Home` und :kbd:`Ctrl`+:kbd:`End`, um zum ersten bzw. letzten Tab zu wechseln (ohne den Tab des Startbildschirms),
* :kbd:`Ctrl`+:kbd:`W`, um den aktuellen Tab zu schließen,
* :kbd:`Ctrl`+:kbd:`H`, um zum Startbildschirm zu wechseln.

Die Tastenkombination :kbd:`Ctrl`+:kbd:`F` setzt den Fokus auf das Suchfeld des aktuellen Tabs: das Suchfeld des Startbildschirms, das Suchfeld der [Online-Fundgrube](manual/online-repository.md), die [Suchleiste der Baumansicht](manual/soundfont-editor/tree.md#doc_search) in einem Soundfont-Editor oder das Suchfeld des [Soundfont-Explorers](manual/soundfont-explorer.md).


### Menü


Das Menü in der oberen rechten Ecke enthält allgemeine Funktionen und wird [hier](manual/menu.md) beschrieben.


## Benachrichtigungen {#doc_notifications}


Beim Start liest Polyphone die auf der Website für seine Benutzer veröffentlichten Nachrichten und zeigt höchstens eine davon in einem Banner unter den Tabs an, über die gesamte Breite des Startbildschirms. Das Symbol des Banners hängt von der Art der Nachricht ab:

* eine Warnung, zum Beispiel zu einem bekannten Fehler der installierten Version;
* eine neue Version von Polyphone, die nur in den von der Website heruntergeladenen Versionen angezeigt wird (das Paket einer Linux-Distribution wird von der Distribution aktualisiert);
* eine Ankündigung zur Software oder zum Projekt;
* ein Aufruf zur Unterstützung, der nach mehreren Tagen der Nutzung angezeigt wird und nie, wenn Polyphone mit einem [Premium-Konto](premium) verbunden ist.

Die Schaltfläche rechts öffnet, falls vorhanden, die entsprechende Seite der Website. Die Nachricht verschwindet dann endgültig, wie mit dem Kreuz. Die nächste Nachricht wird frühestens eine Woche später angezeigt, außer einer Warnung, die sofort angezeigt wird.

Nur die Sprache der Oberfläche wird an die Website übermittelt, damit die Nachrichten in dieser Sprache verfasst sind, sofern sie verfügbar ist. Die Benachrichtigungen können in den [Einstellungen](manual/settings.md#doc_interface) deaktiviert werden.
