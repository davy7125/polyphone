Polyphone wurde so konzipiert, dass es Übersetzungen unterstützt, um die Zugänglichkeit zu verbessern.
Jeder kann dabei helfen: Eine Übersetzung kann von Grund auf neu erstellt oder eine bestehende Übersetzung vervollständigt oder verbessert werden.
Seit Version 2.6 wurden alle unvollständigen Übersetzungen mithilfe einer KI vervollständigt; sie verdienen eine Überprüfung durch Muttersprachler.


## Voraussetzungen


### Übersetzungsdatei


Laden Sie das Übersetzungspaket für die Sprache, die Sie übersetzen möchten, von dieser [Seite](software/translations) herunter.
Ist die Sprache noch nicht in der Liste, fragen Sie über das [Kontaktformular](contact) danach (Sie müssen nur die gewünschte neue Sprache angeben).
Sie erhalten dann schnell ein neues Übersetzungspaket.

Die Übersetzungsdateien sind auch im <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>-Repository im Verzeichnis :file:`sources` verfügbar (eine Datei :file:`polyphone_XX.ts` pro Sprache).


### Qt Linguist


Die einzige Software, die Sie zum Übersetzen brauchen, ist :program:`Qt Linguist`: ein Werkzeug zum Erstellen von Übersetzungsdateien.
Diese Software wird mit Qt installiert (siehe die [Entwicklungsseiten](development/index.md)) und ist für Windows-Nutzer auch <a href="https://github.com/thurask/Qt-Linguist/releases" target="_blank">hier</a> oder in den Paketquellen von Linux-Distributionen erhältlich (Paket :program:`linguist-qt6` unter Debian und Ubuntu, :program:`qt6-linguist` unter Fedora).


## Die Übersetzungsdatei öffnen


Entpacken Sie das soeben heruntergeladene Übersetzungspaket.
Sie finden darin eine .ts-Datei :file:`polyphone_XX.ts`, wobei :file:`XX` für Ihre Sprache steht.
Öffnen Sie :file:`polyphone_XX.ts` mit :program:`Qt Linguist`, und Sie können mit der Übersetzung beginnen.


## Übersetzen


Jeder Eintrag in jedem Kontext muss übersetzt werden.
Sie müssen lediglich die Elemente nacheinander durchgehen und Ihre Übersetzung eintragen.
Je nachdem, wie sicher Sie sich sind, gibt es drei Fälle.

* Wenn Sie nicht wissen, wie Sie etwas übersetzen sollen, überspringen Sie das Element ruhig.
  Das ist kein Problem: Andere können es ergänzen.
* Sie glauben, die richtige Übersetzung zu haben, sind sich aber unsicher: Tragen Sie Ihren Vorschlag ein und drücken Sie dann :kbd:`Ctrl`+:kbd:`J`.
  Der Eintrag wird nicht bestätigt, die Übersetzung wird in der übersetzten Oberfläche aber trotzdem verwendet.
  So wissen andere, die die Übersetzung verbessern, dass Sie sich unsicher waren.
* Sie haben die richtige Übersetzung: Tragen Sie sie ein und drücken Sie :kbd:`Ctrl`+:kbd:`Enter`.
  Dieser Eintrag wird bestätigt.

Eine Übersetzung erfordert Aufmerksamkeit (in einer Welt, in der alles darauf ausgelegt ist, <a href="https://medium.com/thrive-global/how-technology-hijacks-peoples-minds-from-a-magician-and-google-s-design-ethicist-56d62ef5edf3" target="_blank">sie zu vereinnahmen</a>).
Achten Sie bitte auf:

* die Zeichensetzung,
* Leerzeichen am Anfang und am Ende,
* die Groß- und Kleinschreibung,
* Singular- und Pluralformen.

Ein letzter Rat: **Nehmen Sie sich Zeit!**
Und vergessen Sie nicht, Ihre Arbeit mit :kbd:`Ctrl`+:kbd:`S` zu speichern.


## Ihre Arbeit testen


Sie können Ihre Arbeit jederzeit überprüfen.
Wählen Sie in :program:`Qt Linguist` den Menüpunkt :menuselection:`File --> Release As…` und erstellen Sie eine .qm-Datei für die Übersetzung, die Sie bereitstellen.
Folgen Sie den Anweisungen im Übersetzungspaket (Datei :file:`readme.txt`), um sie zu installieren, und öffnen Sie dann Polyphone.
Öffnen Sie die [Einstellungen](manual/settings.md#doc_interface), wählen Sie Ihre Übersetzung in der Liste der Sprachen aus und starten Sie Polyphone neu.
Nun können Sie Ihre Übersetzung überprüfen.

**Hinweis:** Der Vorgang :guilabel:`Release As…` übersetzt möglicherweise nur Text ohne Akzente. Tritt dieser Fehler auf, öffnen Sie ein Terminal und verwenden Sie folgenden Befehl: :command:`lrelease polyphone_XX.ts` (passen Sie ihn an Ihre aktuelle Datei an).
Eine Übersetzungsdatei :file:`polyphone_XX.qm` kann auch aus einem Verzeichnis namens :file:`translations` geladen werden (im aktuellen Verzeichnis oder im Konfigurationsverzeichnis von Polyphone), statt in die Software eingebettet zu sein.


## Die Übersetzung einreichen


Wenn Sie fertig sind (oder genug haben!), senden Sie die bearbeitete .ts-Datei (nicht die .qm-Datei!) an <contact@polyphone.io>.
Das Übersetzungspaket auf dieser [Seite](software/translations) wird dann aktualisiert.

Wenn Sie mit GitHub vertraut sind, können Sie Ihre .ts-Datei auch mit einem Pull Request im <a href="https://github.com/davy7125/polyphone" target="_blank">Repository</a> vorschlagen (siehe den Leitfaden für Beiträge).
