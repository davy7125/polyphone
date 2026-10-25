Ein Raspberry Pi ist ein Computer im Scheckkartenformat, auf dem Polyphone installiert werden kann. Baut man ihn in ein Gehäuse mit Lautsprechern ein und verbindet ihn mit einem Masterkeyboard, entsteht ein echter Synthesizer!

![Selbstgebauter Synthesizer – Vorderseite](images/synth_front.jpg "Selbstgebauter Synthesizer – Vorderseite")

![Selbstgebauter Synthesizer – Rückseite](images/synth_back.jpg "Selbstgebauter Synthesizer – Rückseite")

## Benötigtes Material

Diese Liste kann an Ihr Projekt angepasst werden:

- ein Raspberry Pi,
- eine microSD-Karte mit 32 GB oder mehr,
- ein Netzteil für den Raspberry Pi,
- ein Digital-Analog-Wandler (DAC), der einen USB-Ausgang in eine Audio-Klinkenbuchse umwandelt,
- Computerlautsprecher, die den Vorteil haben, sowohl die Lautsprecher als auch die Signalverstärkung zu enthalten,
- ein Hauptschalter (Ein/Aus),
- ein doppeltes USB-Einbaukabel (Stecker auf Buchse),
- ein Gehäuse, in dem alles Platz findet,
- und natürlich ein Masterkeyboard zum Spielen.

Um den Raspberry Pi bequem einzurichten, empfiehlt es sich, außerdem Folgendes bereitzuhalten:

- einen Bildschirm mit einem Micro-HDMI-Kabel zum Anschluss an den Raspberry Pi,
- eine Tastatur,
- eine Maus.

Im Beispiel auf dieser Seite wurden gebrauchte Lautsprecher vom Typ Edifier R1280T verwendet. Sie sind preisgünstig, geben tiefe Töne relativ gut wieder, und wie Sie unten sehen werden, lassen sich Schwächen im Frequenzgang korrigieren.

Als Masterkeyboard dient ein Akai MPK Mini Plus, das per USB angeschlossen werden kann. Der USB-/Klinkenadapter ist ein Einstiegsmodell von UGREEN, und der Raspberry Pi ist die Version 5.

(Diese Angaben sollen keine Werbung für eine bestimmte Marke sein, sondern eine funktionierende Kombination vorstellen.)

## Das Gehäuse vorbereiten

Dieses Tutorial konzentriert sich nicht auf eine bestimmte Bauweise des Gehäuses. Holz bringt Ihr handwerkliches Geschick zur Geltung, wie in diesem Beispiel, aber Karton funktioniert genauso gut. Lassen Sie Ihrer Kreativität freien Lauf! Insgesamt brauchen Sie einen Deckel, um an die Bauteile zu gelangen, sowie Öffnungen für:

- die Lautsprecher,
- das Netzkabel,
- den Ein/Aus-Schalter,
- den Lautstärkeregler und – falls nötig – die Klangregler,
- die Helmholtz-Resonatoren für die Bassreflexöffnung usw.

Eine mögliche Reihenfolge beim Einbau in das Gehäuse:

- zuerst den Ein/Aus-Schalter, die USB-Anschlüsse an der Rückseite und alle weiteren Bedienelemente der Lautsprecher einbauen,
- das Netzkabel verlegen und an den Eingang des Ein/Aus-Schalters anschließen,
- das Netzteil der Lautsprecher einsetzen und an den Ausgang des Ein/Aus-Schalters anschließen,
- die Platine der Lautsprecher einsetzen und an das Netzteil anschließen,
- die Lautsprecher befestigen und an die Platine anschließen,
- den Raspberry Pi befestigen,
- das Netzteil des Raspberry Pi an den Ausgang des Ein/Aus-Schalters anschließen (der Ausgang des Schalters versorgt sowohl die Lautsprecher als auch den Raspberry Pi mit Strom),
- das doppelte USB-Kabel an den Raspberry Pi anschließen,
- den USB-/Klinkenwandler an eine weitere USB-Buchse anschließen,
- die Platine der Lautsprecher über ein Klinkenkabel anschließen.

Im folgenden Bild sind das Netzteil und die Elektronikplatine der Lautsprecher orange eingekreist. Der Raspberry Pi ist blau eingekreist.

![Das Innere des Synthesizers](images/synth_inside.jpg "Das Innere des Synthesizers")

Hinweis: Die Lautsprecher müssen so angeordnet sein, dass kein Schall vom Lautsprecherausgang ins Innere des Gehäuses zurückgelangen kann, sonst entsteht eine Resonanz. **Also nicht wie in diesem Beispiel!** Manchmal muss man experimentieren, um die Fallstricke zu entdecken …

![Der Raspberry Pi](images/synth_raspberry_pi.jpg "Der Raspberry Pi")

3 USB-Anschlüsse werden verwendet:

- einer, um einen Klinkenausgang zu erhalten,
- die anderen 2 werden zur Rückseite des Gehäuses geführt und dienen zum Anschluss des Masterkeyboards bzw. beim Einrichten des Raspberry Pi einer Tastatur und einer Maus.

## Den Raspberry Pi vorbereiten

### Debian installieren

Einer der ersten Schritte nach dem Erhalt eines Raspberry Pi ist die Installation von **Debian** auf einer microSD-Karte. Anleitungen dazu finden sich leicht im Internet, zum Beispiel <a href="https://www.raspberrypi.com/documentation/computers/getting-started.html#installing-the-operating-system" target="_blank">hier</a>.

Sobald die microSD-Karte vorbereitet ist, stecken Sie sie in den dafür vorgesehenen Steckplatz des Raspberry Pi. Schließen Sie dann Netzteil, Bildschirm, Maus und Tastatur an, und Ihr Computer ist einsatzbereit!

### Polyphone installieren und einrichten

Aktualisieren Sie zunächst das System mit:

```
sudo apt-get update
sudo apt-get upgrade
```

Installieren Sie dann **Polyphone** mit dem .deb-Paket, das auf dieser [Seite](software) verfügbar ist. Schließen Sie die MIDI-Tastatur per USB an den Raspberry Pi an (ziehen Sie die Computertastatur ab, falls kein USB-Anschluss frei ist), öffnen Sie dann Polyphone und wählen Sie in den [Einstellungen](manual/settings.md#doc_general) die MIDI-Tastatur in der Dropdown-Liste der MIDI-Eingänge aus. Laden Sie einen Soundfont herunter, zum Beispiel aus der [Fundgrube](soundfonts), und schon können Sie mit der am Raspberry Pi angeschlossenen MIDI-Tastatur ein Instrument spielen!

### Polyphone automatisch starten

Dieser Schritt ist nötig, um auf den Bildschirm verzichten zu können. Polyphone kann beim Hochfahren des Raspberry Pi automatisch gestartet werden, indem eine Datei :file:`synthesizer.desktop` mit folgendem Inhalt erstellt wird:

```
[Desktop Entry]
Name=Synthesizer
Exec=polyphone -s /home/pi/Desktop/my-soundfont.sf2
Terminal=false
Type=Application
StartupNotify=true
Icon=/home/pi/Desktop/my-icon.png
Categories=AudioVideo;
```

Diese Datei wird anschließend im Ordner :file:`/home/pi/.config/autostart` abgelegt.

Die Kommandozeile kann um Optionen ergänzt werden, um auf einen bestimmten MIDI-Kanal zu hören, mehrere Instrumente gleichzeitig zu spielen oder Instrumente über Tasten zu aktivieren / deaktivieren. Diese [Seite](tutorials/use-polyphone-as-a-synthesizer.md) beschreibt die Optionen für die Verwendung von Polyphone im Synth-Modus im Detail.

## Weiterführendes

### Klangentzerrung

Jeder Lautsprecher reagiert aufgrund seiner Bauform mehr oder weniger unterschiedlich auf die Frequenzen, die er wiedergeben soll. Um einen linearen Frequenzgang zu erhalten und den Klang so originalgetreu wiederzugeben, kann eine Entzerrung erstellt werden, die die Schwächen des Lautsprechers ausgleicht. Das Werkzeug <a href="https://github.com/erdewit/HiFiScan" target="_blank">HiFiScan</a> berechnet die Parameter der Entzerrung automatisch; das Vorgehen ist auf der Projektseite beschrieben.

Nach der Berechnung der Entzerrungskurve kann **HiFiScan** Koeffizienten zur Einbindung in Equalizer exportieren. Für uns ist die 12-Band-Entzerrung interessant, die mit **Calf** verwendet werden kann.

### Fernzugriff

Bisher erforderte die Einrichtung des Raspberry Pi einen Bildschirm zur Anzeige des Desktops. Das System kann jedoch so eingerichtet werden, dass ein Fernzugriff über das **VNC-Protokoll** möglich ist, und im Internet finden sich viele Anleitungen dazu.
