Polyphone enthält einen Server nach dem Model Context Protocol (MCP), einem Standard, mit dem KI-Assistenten andere Programme verwenden können.
Sobald er aktiviert ist, kann ein Assistent wie Claude mit den in Polyphone geöffneten Soundfonts arbeiten: Sie beschreiben in Ihren eigenen Worten, was Sie möchten, und der Assistent durchsucht die Soundfonts, bearbeitet sie, verwendet die Werkzeuge von Polyphone und misst das Ergebnis.

Jede Änderung erscheint sofort im Editor und kann rückgängig gemacht werden, als hätten Sie sie selbst vorgenommen.
Dieses Tutorial zeigt, wie Claude mit Polyphone verbunden wird, und gibt dann Beispiele für mögliche Anfragen, mit Audiodateien, um die Ergebnisse zu hören.


## Was der Assistent tun kann {#doc_principles}


Der Assistent hat Zugriff auf alles, was in Polyphone geöffnet ist:

* **Durchsuchen**: die Soundfonts, ihre Samples, Instrumente und Presets, die Parameter jedes Bereichs sowie die in der [Baumansicht](manual/soundfont-editor/tree.md) ausgewählten Elemente (sodass „dieses Sample“ oder „die ausgewählten Instrumente“ verstanden werden),
* **Bearbeitung**: Elemente erstellen, umbenennen, duplizieren oder löschen, Parameter, Tasten- und Anschlagsbereiche, Loop-Punkte, Modulatoren ändern…
* **Werkzeuge**: die meisten Werkzeuge aus dem [Werkzeugmenü](manual/soundfont-editor/tools/index.md), etwa der Auto-Loop, die Lautstärkeänderung, die Sample-Autopositionierung oder das natürliche Release,
* **Klanganalyse**: Messungen von Lautheit, Tonhöhe, Obertönen, Attack, Loop-Qualität… sowie das Rendern einer vom Synthesizer gespielten Note.

Die Analysefunktionen sind entscheidend: **Der Assistent kann nicht hören**.
Stattdessen misst er den Klang vor und nach einer Änderung, so wie ein Toningenieur einen Spektrumanalysator abliest.
Deshalb kann er prüfen, ob ein Loop knackt, ob die Noten richtig gestimmt sind oder ob die Lautstärke über die ganze Tastatur gleichmäßig ist – aber Ihre Ohren haben das letzte Wort.

Jede Änderung des Assistenten ist ein Schritt im Verlauf des Soundfonts: :kbd:`Ctrl`+:kbd:`Z` in Polyphone macht sie rückgängig.
Nichts wird auf die Festplatte geschrieben, solange Sie den Assistenten nicht bitten, den Soundfont zu speichern, oder ihn nicht selbst speichern.


## Einrichtung {#doc_setup}


### Den Server in Polyphone aktivieren


Öffnen Sie die [Einstellungen](manual/settings.md#doc_ai_assistant) und gehen Sie zum Abschnitt :guilabel:`KI-Assistent`.
Aktivieren Sie :guilabel:`KI-Assistenten den Zugriff auf die geöffneten Soundfonts erlauben`: Die Zeile darunter zeigt dann die Adresse des Servers an, standardmäßig http://127.0.0.1:8642/mcp.
Wenn der Port 8642 bereits von einem anderen Programm belegt ist, wählen Sie mit :guilabel:`Port` einen anderen.

Der Server nimmt nur Verbindungen von Ihrem eigenen Computer an.
Dieselbe Seite listet alle Funktionen auf, die dem Assistenten angeboten werden, gruppiert in vier Kategorien: :guilabel:`Durchsuchen`, :guilabel:`Bearbeitung`, :guilabel:`Werkzeuge` und :guilabel:`Klanganalyse`.


### Claude Code verbinden


<a href="https://claude.com/claude-code" target="_blank">Claude Code</a> ist ein Assistent von Anthropic, der in einem Terminal läuft (er ist auch als Erweiterung für die Editoren VS Code und JetBrains erhältlich).
Kopieren Sie nach der Installation den in den Einstellungen von Polyphone angezeigten Befehl mit der Schaltfläche :guilabel:`Kopieren` und führen Sie ihn in einem Terminal aus:

```
claude mcp add --transport http polyphone http://127.0.0.1:8642/mcp
```

Standardmäßig verwendet Claude Code den Server nur, wenn es aus dem Ordner gestartet wird, in dem dieser Befehl ausgeführt wurde.
Um Polyphone aus jedem Ordner heraus zu verwenden, fügen Sie die Option `--scope user` hinzu:

```
claude mcp add --scope user --transport http polyphone http://127.0.0.1:8642/mcp
```

Starten Sie dann eine neue Sitzung von Claude Code: Die Funktionen von Polyphone werden beim Start einer Sitzung geladen, und Polyphone muss bereits mit aktiviertem Server geöffnet sein.
Der Befehl `/mcp` in Claude Code zeigt an, ob der Server „polyphone“ verbunden ist.
Wenn der Assistent zum ersten Mal eine Funktion von Polyphone verwenden möchte, fragt Claude Code Sie um Erlaubnis, und Sie können festlegen, für diese Funktion nicht mehr gefragt zu werden.

**Hinweis:** Andere Assistenten, die MCP über den HTTP-Transport unterstützen, können sich mit derselben Adresse verbinden.


### Erste Fragen


Beginnen Sie mit einfachen Fragen, um zu sehen, wie der Assistent die Soundfonts erkundet:

* *„Welche Soundfonts sind in Polyphone geöffnet?“*
* *„Was enthält dieser Soundfont? Gibt es Samples, die von keinem Instrument verwendet werden?“*
* *„Gib mir die Liste der Presets mit ihrer Bank- und Presetnummer.“*

Wenn Sie in der Baumansicht von Polyphone ein Element auswählen, kann sich der Assistent darauf beziehen: *„Senke die Lautstärke des ausgewählten Samples um 6 dB“* oder *„Benenne diese Instrumente mit dem Präfix ‚Orgel‘ um“*.
Bei kleinen Änderungen ist das Ergebnis sofort im Editor sichtbar.


## Beispiel: ein Instrument prüfen und korrigieren {#doc_check}


Beim Aufbau eines Instruments mit vielen Samples schleichen sich leicht Fehler ein, und manche sind schwer zu bemerken: eine leicht verstimmte Note, ein Sample, das etwas lauter ist als seine Nachbarn, ein Loop, der erst nach einigen Sekunden knackt…
Der Assistent kann ein ganzes Instrument in wenigen Sekunden prüfen.

Um es selbst auszuprobieren, laden Sie <a href="files/flute-with-mistakes.sf2" download>flute-with-mistakes.sf2</a> herunter und öffnen Sie die Datei in Polyphone.
Sie enthält ein Flöteninstrument, in das absichtlich einige Fehler eingebaut wurden.
Hören Sie eine Tonleiter von C4 bis F6, gefolgt von einem langen G6:

![](files/ai-assistant-flute-before.mp3)

Fragen Sie dann:

*„Prüfe das Instrument ‚Flute‘: Gibt es verstimmte, zu laute oder zu leise Noten, Tasten ohne Klang oder schlechte Loops?“*

Der Assistent analysiert das Instrument (eine einzige Funktion misst die Lautheit, die Tonhöhe, die Klangfarbe und den Loop jedes Bereichs) und untersucht dann die verdächtigen Samples im Detail.
Seine Antwort listet die Probleme auf:

* die Taste E4 spielt nichts: Der Bereich von „Flute 63“ endet bei D♯4, und der nächste beginnt bei F4,
* „Flute 69“ klingt einen Halbton zu hoch: Es spielt ein A4 (440 Hz), aber seine Basisnote ist G♯4,
* „Flute 78“ ist 30 Cent zu hoch: Seine Korrektur beträgt +30 Cent, obwohl die Aufnahme richtig gestimmt ist,
* der Bereich von „Flute 84“ ist 6 dB lauter als die anderen (Tasten H5 bis C♯6),
* der Loop von „Flute 90“ knackt: Die Wellenformen stimmen am Loop-Punkt nicht überein, und die Länge des Loops ist kein ganzzahliges Vielfaches der Periode.

Fragen Sie dann: *„Behebe diese Probleme.“*

Der Assistent erweitert den Tastenbereich von „Flute 63“ bis E4, setzt die Basisnote von „Flute 69“ auf A4 und die Korrektur von „Flute 78“ auf 0, gibt dem Bereich von „Flute 84“ eine Dämpfung von 6 dB (das Sample selbst wird nicht verändert) und wendet das Werkzeug [Auto-Loop](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop) auf „Flute 90“ an.
Eine neue Analyse bestätigt das Ergebnis: keine fehlende Taste, keine verstimmte Note, überall dieselbe Lautstärke und ein Loop, dessen Wellenformen am Loop-Punkt jetzt übereinstimmen.
Dieselbe Tonleiter nach den Korrekturen:

![](files/ai-assistant-flute-after.mp3)

Der Assistent erklärt, was er tut, und muss oft Entscheidungen treffen: Hier hat er einen Bereich gedämpft, statt die Audiodaten des Samples zu verändern.
Wenn Sie eine andere Lösung bevorzugen, sagen Sie es einfach – und Sie können jede Änderung im Editor überprüfen oder rückgängig machen.


## Beispiel: ein Instrument von Grund auf erstellen {#doc_create}


Ein Assistent wie Claude Code kann auch Programme auf Ihrem Computer ausführen, zum Beispiel um Samples zu berechnen.
Zusammen mit den Funktionen von Polyphone lässt sich so ein ganzes Instrument aus einer Beschreibung bauen.
Die Flöte des vorigen Beispiels ist auf diese Weise entstanden, ausgehend von zwei Anfragen:

*„Kannst du mir einen Flötenklang für die Taste 60 machen?“*

*„Mach jetzt eine vollständige Flöte mit mehreren Dynamikstufen, in einem neuen Soundfont.“*

Das hat der Assistent Schritt für Schritt getan:

1. Er hat eine aufgenommene Flöte in einem General-MIDI-Soundfont vermessen, um die Klangfarbe des Instruments in jedem Register zu kennen (Obertöne, Atemgeräusch, Attack, Vibrato),
2. er hat ein Programm geschrieben, das 39 Samples synthetisiert: 13 Noten von C4 bis C7, eine auf jeder dritten Taste, jeweils in drei Dynamikstufen – *p* ist dunkler und hauchiger mit einem weichen Attack, *f* ist obertonreicher mit einem schärferen Attack,
3. er hat die Samples in einen neuen Soundfont importiert, ihre Basisnoten und Loops eingestellt und pro Dynamikstufe ein Instrument mit einem Release von 0,4 s erstellt,
4. er hat die drei Instrumente in einem Preset gestapelt (Bank 0, Preset 73, die Flöte des General-MIDI-Standards), mit den Anschlagsbereichen 0-49, 50-89 und 90-127, wie im Tutorial „[Realistische Instrumente mit mehreren Anschlagsebenen](tutorials/realistic-instruments-with-multiple-velocity-layers.md)“ beschrieben,
5. er hat Noten auf beiden Seiten jeder Anschlagsgrenze gerendert: Die Lautstärke ändert sich nur um 0,1 bis 0,4 dB, sodass sich von einer Ebene zur nächsten nur die Klangfarbe ändert.

Der letzte Schritt hat eine Schwäche aufgedeckt: In der höchsten Oktave hatten die drei Ebenen fast dieselbe Klangfarbe.
Der Assistent hat sein Programm so geändert, dass die lauten Noten auch im hohen Register heller werden, und dann die Samples neu erzeugt und das Instrument neu aufgebaut.

Hören Sie die Taste C4, gespielt mit den Anschlagsstärken 35, 70 und 110 – eine Note pro Ebene:

![](files/ai-assistant-flute-layers.mp3)

Und eine immer lauter werdende Phrase, in der die Klangfarbe der Dynamik folgt:

![](files/ai-assistant-flute-phrase.mp3)

Eine synthetisierte Flöte wird nie an eine gute Aufnahme heranreichen: Dieses Beispiel zeigt eher die Methode als den Klang.
Mit Ihren eigenen Aufnahmen funktioniert dieselbe Art von Anfrage: *„Importiere die WAV-Dateien dieses Ordners, finde ihre Basisnote, loope sie und baue ein Instrument mit einer Ebene pro Dynamikstufe.“*


## Weitere Ideen {#doc_ideas}


Hier sind weitere Anfragen, die gut funktionieren:

* *„Stelle für alle Instrumente ein natürliches Release ein, länger im Bass.“*
* *„Die hohen Noten dieses Instruments sind zu hell: Mach sie ab C6 allmählich weicher.“*
* *„Erstelle aus diesem Orgelinstrument ein Schwebungsregister (Celeste) mit 3 Schwebungen pro Sekunde bei C4.“*
* *„Wandle alle Stereo-Samples in Mono um und aktualisiere die Instrumente.“*
* *„Vergleiche diese beiden Samples: Warum klingt das zweite dumpfer?“*
* *„Erkläre die Modulatoren dieses Presets in einfachen Worten.“*
* *„Entferne die unbenutzten Samples und die Duplikate und speichere den Soundfont dann als sf3.“*


## Tipps {#doc_tips}


* **Grenzen Sie den Umfang ein**: „nur das Instrument ‚Piano‘“, „ändere nicht die Samples, nur die Instrumente“…
* **Bitten Sie um Messungen**: „prüfe das Ergebnis“ oder „vergleiche vorher und nachher“ veranlasst den Assistenten, seine Arbeit mit den Analysefunktionen zu überprüfen.
* **Gehen Sie schrittweise vor** bei größeren Änderungen, und hören Sie zwischen den Schritten hin.
* **Behalten Sie eine Kopie** Ihres Soundfonts, und speichern Sie erst, wenn Sie zufrieden sind: Bitten Sie den Assistenten zu speichern, oder verwenden Sie :kbd:`Ctrl`+:kbd:`S`.
* **Große Soundfonts** brauchen länger für die Analyse: Wählen Sie lieber ein Instrument oder ein Preset als den ganzen Soundfont.
* **Datenschutz**: Der Assistent erhält die Namen, die Parameter, die Messwerte und Bilder von Wellenformen, nicht das Audio selbst; diese Daten werden jedoch wie der Rest der Unterhaltung an den Dienst des Assistenten gesendet.
