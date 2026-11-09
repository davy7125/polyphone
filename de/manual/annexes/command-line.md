Polyphone kann über die Kommandozeile ausgeführt werden, um Soundfonts in das Format [sf2](#doc_sf2), [sf3](#doc_sf3), [sfz](#doc_sfz), [csv](#doc_csv) oder [dls](#doc_dls) umzuwandeln. Für die Umwandlung werden die Dateiformate [sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [dls](manual/annexes/the-different-soundfont-formats.md#doc_dls), [sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) und [organ](manual/annexes/the-different-soundfont-formats.md#doc_organ) unterstützt.

Eine [MIDI-Datei kann](#doc_render) mit einem Soundfont abgespielt und als wav-Datei gespeichert werden.
Außerdem können Optionen aufgerufen werden, um Polyphone im [Synth-Modus](#doc_synth) oder mit gestartetem [MCP-Server](#doc_mcp) zu öffnen.

Dateien und Verzeichnisse können auch als Argumente übergeben werden, um sie in der grafischen Oberfläche zu öffnen, zum Beispiel :command:`polyphone file1.sf2 file2.sf3 /path/to/directory` (ein Verzeichnis wird im [Soundfont-Explorer](manual/soundfont-explorer.md) geöffnet).
Wenn Polyphone bereits geöffnet ist, werden die Dateien in der laufenden Instanz geöffnet.
Einige [weitere Optionen](#doc_other) stehen zur Verfügung, um die Konfiguration zurückzusetzen oder die Hilfe und die Version anzuzeigen.

Die Liste aller Optionen wird mit :command:`polyphone --help` angezeigt.
Die Reihenfolge der Argumente spielt keine Rolle.
Die Optionen der [früheren Versionen](#doc_legacy) werden weiterhin akzeptiert.

**Hinweis:** Unter Windows müssen Sie statt :command:`polyphone` den vollständigen Pfad von Polyphone angeben, zum Beispiel :command:`C:/Program files/Polyphone/polyphone.exe`.


## Umwandlung {#doc_conversion}


### Kommandozeile


```
polyphone --convert <format> <umzuwandelnde/dateien> -d <ausgabe/verzeichnis> -o <ausgabe/datei> -f
```


### Argumente


--convert
: Format der Umwandlung: „sf2“, „sf3“, „sfz“, „csv“ oder „dls“; die für jedes Format spezifischen Optionen werden weiter unten beschrieben.
  Dieses Argument ist optional, wenn eine Ausgabedatei mit ihrer Endung angegeben wird, zum Beispiel :command:`polyphone file.sf2 -o file.sf3`.

-d, --output-dir
: Ausgabeverzeichnis, das existieren muss.
  Dieses Argument ist optional; standardmäßig ist es das Verzeichnis jeder Eingabedatei.

-o, --output
: Ausgabedatei, mit oder ohne Endung (die Endung des Formats wird automatisch hinzugefügt).
  Dieses Argument ist optional; standardmäßig hat die Ausgabedatei den Namen der Eingabedatei.
  Ein Name allein wird in das Ausgabeverzeichnis geschrieben, ein relativer Pfad beginnt im aktuellen Verzeichnis (oder im Ausgabeverzeichnis, wenn -d verwendet wird).
  Dieses Argument kann nicht verwendet werden, wenn mehrere Dateien umgewandelt werden.

-f, --force
: Ersetzt eine vorhandene Ausgabedatei.
  Ohne dieses Argument schlägt die Umwandlung fehl, wenn die Ausgabedatei bereits existiert.
  Die Eingabedatei kann durch ihre Umwandlung ersetzt werden, zum Beispiel schreibt :command:`polyphone --convert sf2 file.sf2 -f` die Datei :file:`file.sf2` neu.

Mehrere Dateien können mit einem einzigen Befehl umgewandelt werden, wobei jede Ausgabedatei den Namen ihrer Eingabedatei erhält.
Bei einem Fehler werden die übrigen Dateien trotzdem umgewandelt.


### Beispiele


```
polyphone --convert sf2 /path/to/file.sfArk
polyphone /path/to/file.sf2 -o /path/to/converted.sf3
polyphone --convert sf3 *.sf2 -d /path/to/directory
```


## Umwandlung in sf2 {#doc_sf2}


Für diese Umwandlung wird keine besondere Option verwendet.

```
polyphone --convert sf2 /path/to/file.dls
```


## Umwandlung in sf3 {#doc_sf3}


--quality
: Kompressionsqualität: „low“ (niedrig), „medium“ (mittel) oder „high“ (hoch).
  Dieses Argument ist optional; standardmäßig ist es „medium“.

```
polyphone --convert sf3 /path/to/file.sf2 --quality high
```


## Umwandlung in sfz {#doc_sfz}


--preset-prefix
: Jedem Preset wird seine Presetnummer vorangestellt.

--bank-dir
: Für jede Bank wird ein Verzeichnis erstellt.

--gm-sort
: Die General-MIDI-Klassifikation wird verwendet, um die Presets zu sortieren.

```
polyphone --convert sfz /path/to/file.sf3 --bank-dir --gm-sort
```

Die Umwandlung erstellt ein Verzeichnis (benannt nach der Ausgabedatei), das die sfz-Dateien und ein Unterverzeichnis :file:`samples` enthält.
Wenn dieses Verzeichnis bereits existiert, wird seinem Namen eine Nummer angehängt: Es wird nichts ersetzt, auch nicht mit -f.


## Umwandlung in .csv {#doc_csv}


--raw
: Es werden Rohwerte exportiert (so, wie sie in der Soundfont-Datei gespeichert sind) statt Werten mit physikalischen, messbaren Einheiten.
  Zum Beispiel wird die exportierte Release-Zeit 0,491 (Sekunden) oder in Rohform -1229 sein.
  Standardmäßig werden Werte mit physikalischen Einheiten exportiert.

```
polyphone --convert csv /path/to/file.sf2 --raw
```

Die Umwandlung erstellt ein Verzeichnis (benannt nach der Ausgabedatei), das die Datei :file:`samples.csv` sowie eine Datei pro Instrument und pro Preset enthält.
Wenn dieses Verzeichnis bereits existiert, wird seinem Namen eine Nummer angehängt: Es wird nichts ersetzt, auch nicht mit -f.


## Umwandlung in dls {#doc_dls}


Für diese Umwandlung wird keine besondere Option verwendet.

```
polyphone --convert dls /path/to/file.sf2
```

Die Datei wird im Format DLS Level 2 geschrieben; die Einschränkungen der Umwandlung werden [hier](manual/annexes/the-different-soundfont-formats.md#doc_dls) beschrieben.


## Rendern einer MIDI-Datei {#doc_render}


### Kommandozeile


```
polyphone --render <MIDI/datei> <soundfont> -d <ausgabe/verzeichnis> -o <ausgabe/datei> -f
```


### Argumente


--render
: Pfad der abzuspielenden MIDI-Datei (Endung .mid, .midi, .kar oder .rmi): Die MIDI-Datei wird mit dem Soundfont abgespielt und das Ergebnis in eine wav-Datei geschrieben.

-d, --output-dir
: Ausgabeverzeichnis, in das die wav-Datei geschrieben wird.
  Dieses Argument ist optional; standardmäßig ist es das Verzeichnis der MIDI-Datei.

-o, --output
: Ausgabe-wav-Datei, mit oder ohne Endung.
  Dieses Argument ist optional; standardmäßig hat die wav-Datei den Namen der MIDI-Datei.

-f, --force
: Ersetzt eine vorhandene wav-Datei.


### Beispiel


```
polyphone --render /path/to/song.mid /path/to/file.sf2
```

Alle Kanäle der MIDI-Datei spielen den Soundfont, wobei die Presets durch die Programmwechsel der Datei gewählt werden (Bank 128 auf Kanal 10, für das Schlagzeug).
Die Einstellungen des Synthesizers werden verwendet: [Sound](manual/settings.md#doc_sound) (Hall, Chorus, Gesamtlautstärke), [Polyphonie](manual/settings.md#doc_general) und [Stimmung](manual/settings.md#doc_keyboard).
Das Ergebnis ist eine Stereo-wav-Datei im 32-Bit-Gleitkommaformat mit 48.000 Hz, die endet, sobald der Klang der letzten Noten verklungen ist.
Die Noten eines Presets, das der Soundfont nicht enthält, werden nicht gespielt, und für dieses Preset wird eine Warnung angezeigt.
In der grafischen Oberfläche kann eine MIDI-Datei auch aus dem [MIDI-Player](manual/soundfont-editor/toolbar.md#doc_midi_player_settings) exportiert werden, wobei jeder Kanal einen anderen Soundfont oder ein anderes Preset spielen kann.


## Synth-Modus {#doc_synth}


### Kommandozeile


```
polyphone --synth <zu/lesende/datei> --channel <kanal> --multiple-selection --key-selection <modus>
```

![Polyphone, geöffnet mit der Option für den Synth-Modus](images/synth_mode.png "Polyphone, geöffnet mit der Option für den Synth-Modus")


### Argumente


-s, --synth
: Polyphone wird im Synth-Modus geöffnet.

--channel
: Nummer des MIDI-Kanals (zwischen 1 und 16), auf den zum Spielen gehört wird.
  Der Wert „all“ bedeutet, dass alle Kanäle verwendet und wie ein einziger Kanal verarbeitet werden (Standardwert).
  Der Wert „each“ hört jeden Kanal getrennt ab, was zum Beispiel Voraussetzung dafür ist, die MIDI-Ausgabe eines Sequenzers zu spielen (eine MIDI-Datei kann auch direkt mit dem [MIDI-Player](manual/soundfont-editor/toolbar.md#doc_midi_player) abgespielt werden).

--multiple-selection
: Mehrere Presets können zum Spielen ausgewählt werden.

--key-selection
: Mit dem Wert „on“ werden die Presets über die Tasten ausgewählt, die der Presetnummer entsprechen (Taste 0 aktiviert zum Beispiel Preset 0).
  Mit dem Wert „toggle“ wählt eine Taste ihr Preset auch wieder ab.

Diese Argumente sind optional, und die Einstellungen können anschließend über die grafische Oberfläche geändert werden.


### Beispiel


```
polyphone --synth /path/to/file.sf2 --channel 1 --multiple-selection --key-selection toggle
```


## MCP-Server {#doc_mcp}


### Kommandozeile


```
polyphone --mcp <zu/öffnende/dateien> --mcp-port <port>
```


### Argumente


--mcp
: Der MCP-Server, mit dem ein KI-Assistent die geöffneten Soundfonts durchsuchen, bearbeiten und analysieren kann, wird für diese Sitzung gestartet, auch wenn er in den [Einstellungen](manual/settings.md) deaktiviert ist.
  Die Adresse des Servers wird in der Konsole angezeigt.
  Eine neue Instanz von Polyphone wird geöffnet, auch wenn Polyphone bereits geöffnet ist.

--mcp-port
: Port des MCP-Servers (dieses Argument impliziert --mcp).
  Dieses Argument ist optional; standardmäßig ist es der Port der Einstellungen (8642).

Eine Änderung der Einstellungen des MCP-Servers in der grafischen Oberfläche ersetzt diese Optionen.


### Beispiel


```
polyphone --mcp-port 8700 /path/to/file.sf2
```


## Weitere Optionen {#doc_other}


-h, --help
: Zeigt die Liste aller Optionen an.

-v, --version
: Zeigt die Version von Polyphone an.

-r, --reset-config
: Setzt die Konfiguration von Polyphone zurück (Einstellungen, Fenstergeometrie, Verlauf usw.).
  Dies kann bei Problemen nützlich sein.

```
polyphone --reset-config
```


## Meldungen und Rückgabewerte {#doc_exit}


Das Laden und das Schreiben werden in der Konsole protokolliert, Fehler werden in die Fehlerausgabe geschrieben.
Die Meldungen und die Hilfe erscheinen in der in den [Einstellungen](manual/settings.md) gewählten Sprache.

Der Rückgabewert ist:

0
: Erfolg.

1
: Eine Eingabedatei oder das Ausgabeverzeichnis existiert nicht, oder die Ausgabedatei existiert bereits (ohne -f).

2
: Die Argumente sind ungültig: unbekannte Option, falscher Wert, Option im gewählten Modus nicht verwendet, nicht unterstütztes Format der Eingabedatei usw.

3
: Eine Eingabedatei kann nicht gelesen werden (Soundfont, MIDI-Datei).

4
: Die Ausgabe kann nicht geschrieben werden.

Wenn mehrere Dateien umgewandelt werden, ist der Rückgabewert der des ersten Fehlers.


## Optionen früherer Versionen {#doc_legacy}


Die Optionen früherer Versionen von Polyphone werden weiterhin akzeptiert:

-1, -2, -3, -4, -6
: Umwandlung in sf2, sf3, sfz, csv und dls, wie mit --convert.

-5
: Rendern einer MIDI-Datei, deren Pfad mit -c angegeben wird.

-i
: Eingabedatei, die auch ohne diese Option angegeben werden kann.

-c
: Konfiguration aus Werten, die durch das Zeichen „|“ getrennt sind:
  
  * Umwandlung in sf3: Kompressionsqualität, „0“ für niedrig, „1“ für mittel, „2“ für hoch;
  * Umwandlung in sfz: 3 Werte, „1“ oder „0“, für --preset-prefix, --bank-dir und --gm-sort, zum Beispiel „0|1|1“;
  * Umwandlung in csv: „raw“ für --raw;
  * Rendern einer MIDI-Datei (-5): Pfad der MIDI-Datei;
  * Synth-Modus: Kanal („all“, „each“ oder 1 bis 16, wobei 0 „all“ und -1 „each“ entspricht), Mehrfachauswahl („on“ oder „off“) und Auswahl über die Tasten („on“, „off“ oder „toggle“), zum Beispiel „all|on|toggle“.

```
polyphone -2 -i /path/to/file.sf2 -c 2
polyphone -s -i /path/to/file.sf2 -c 'all|on|toggle'
```
