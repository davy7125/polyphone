Polyphone kann über die Kommandozeile ausgeführt werden, um Soundfonts in das Format [sf2](#doc_sf2), [sf3](#doc_sf3), [sfz](#doc_sfz) oder [csv](#doc_csv) umzuwandeln. Für die Umwandlung werden die Dateiformate [sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) und [organ](manual/annexes/the-different-soundfont-formats.md#doc_organ) unterstützt.

Außerdem kann eine Option aufgerufen werden, um Polyphone im [Synth-Modus](#doc_synth) zu öffnen.

Dateien und Verzeichnisse können auch als Argumente übergeben werden, um sie in der grafischen Oberfläche zu öffnen, zum Beispiel :command:`polyphone file1.sf2 file2.sf3 /path/to/directory` (ein Verzeichnis wird im [Soundfont-Explorer](manual/soundfont-explorer.md) geöffnet).
Einige [weitere Optionen](#doc_other) stehen zur Verfügung, um die Konfiguration zurückzusetzen oder die Hilfe anzuzeigen.

Bei einer Umwandlung werden das Laden und das Schreiben in der Konsole protokolliert.
Der Rückgabewert ist 0, wenn die Umwandlung erfolgreich war, und ein anderer Wert im Fehlerfall (die Eingabedatei existiert nicht, die Datei kann nicht gelesen oder geschrieben werden, für die Formate sf2 und sf3 existiert bereits eine Ausgabedatei usw.).


## Umwandlung in sf2 {#doc_sf2}


### Kommandozeile


```
polyphone -1 -i <file/to/convert> -d <output/directory> -o <output_file_name>
```

**Hinweis:** Unter Windows müssen Sie statt :command:`polyphone` den vollständigen Pfad von Polyphone angeben, zum Beispiel :command:`C:/Program files/Polyphone/polyphone.exe`.


### Argumente


-1
: Modus „1“ bedeutet „in sf2 umwandeln“.

-d
: Ausgabeverzeichnis, in das die Eingabedatei umgewandelt wird.
  Dieses Argument ist optional; standardmäßig ist es dasselbe Verzeichnis wie das der Eingabedatei.

-o
: Name der umgewandelten Datei ohne Endung (die Endung „.sf2“ wird automatisch hinzugefügt).
  Dieses Argument ist optional; standardmäßig ist es derselbe Name wie der der Eingabedatei.


### Beispiel


```
polyphone -1 -i /path/to/file.sfArk
```


## Umwandlung in sf3 {#doc_sf3}


### Kommandozeile


```
polyphone -2 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**Hinweis:** Unter Windows müssen Sie statt :command:`polyphone` den vollständigen Pfad von Polyphone angeben, zum Beispiel :command:`C:/Program files/Polyphone/polyphone.exe`.


### Argumente


-2
: Modus „2“ bedeutet „in sf3 umwandeln“.

-d
: Ausgabeverzeichnis, in das die Eingabedatei umgewandelt wird.
  Dieses Argument ist optional; standardmäßig ist es dasselbe Verzeichnis wie das der Eingabedatei.

-o
: Name der umgewandelten Datei ohne Endung (die Endung „.sf3“ wird automatisch hinzugefügt).
  Dieses Argument ist optional; standardmäßig ist es derselbe Name wie der der Eingabedatei.

-c
: Konfiguration der Umwandlung.
  Die Kompressionsqualität kann angegeben werden: „0“ ist niedrig, „1“ mittel, „2“ hoch.
  Dieses Argument ist optional; standardmäßig ist es „1“ (mittlere Qualität).


### Beispiel


```
polyphone -2 -i /path/to/file.sf2 -c 2
```


## Umwandlung in sfz {#doc_sfz}


### Kommandozeile


```
polyphone -3 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**Hinweis:** Unter Windows müssen Sie statt :command:`polyphone` den vollständigen Pfad von Polyphone angeben, zum Beispiel :command:`C:/Program files/Polyphone/polyphone.exe`.


### Argumente


-3
: Modus „3“ bedeutet „in sfz umwandeln“.

-d
: Ausgabeverzeichnis, in das die Eingabedatei umgewandelt wird.
  Dieses Argument ist optional; standardmäßig ist es dasselbe Verzeichnis wie das der Eingabedatei.

-o
: Name der umgewandelten Datei ohne Endung (die Endung „.sfz“ wird automatisch hinzugefügt).
  Dieses Argument ist optional; standardmäßig ist es derselbe Name wie der der Eingabedatei.

-c
: Konfiguration der Umwandlung, bestehend aus 3 durch das Zeichen „|“ getrennten Werten. Dieses Argument ist optional; standardmäßig ist es „0|0|0“.
  
  * Der erste Wert ist „1“, wenn jedem Preset seine Presetnummer vorangestellt werden soll, andernfalls „0“.
  * Der zweite Wert ist „1“, wenn für jede Bank ein Verzeichnis erstellt werden soll, andernfalls „0“.
  * Der dritte Wert ist „1“, wenn die Presets nach der General-MIDI-Klassifikation sortiert werden sollen, andernfalls „0“.


### Beispiel


```
polyphone -3 -i /path/to/file.sf3 -c '0|1|1'
```

Die Umwandlung erstellt ein Verzeichnis (benannt nach dem Ausgabenamen) mit den sfz-Dateien und einem Unterverzeichnis :file:`samples`. Existiert dieses Verzeichnis bereits, wird seinem Namen eine Zahl angehängt.


## Umwandlung in .csv {#doc_csv}


### Kommandozeile


```
polyphone -4 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**Hinweis:** Unter Windows müssen Sie statt :command:`polyphone` den vollständigen Pfad von Polyphone angeben, zum Beispiel :command:`C:/Program files/Polyphone/polyphone.exe`.


### Argumente


-4
: Modus „4“ bedeutet „in .csv umwandeln“.

-d
: Ausgabeverzeichnis, in das die Eingabedatei umgewandelt wird.
  Dieses Argument ist optional; standardmäßig ist es dasselbe Verzeichnis wie das der Eingabedatei.

-o
: Name der umgewandelten Datei ohne Endung (die Endung „.csv“ wird automatisch hinzugefügt).
  Dieses Argument ist optional; standardmäßig ist es derselbe Name wie der der Eingabedatei.

-c
: Konfiguration der Umwandlung.
  Mit der Option „raw“ können Rohwerte (so, wie sie in der Soundfont-Datei gespeichert sind) statt Werten mit physikalischen, messbaren Einheiten exportiert werden. Zum Beispiel wird die exportierte Release-Zeit 0,491 (Sekunden) oder in Rohform -1229 betragen. Standardmäßig werden Werte mit physikalischen Einheiten exportiert.


### Beispiel


```
polyphone -4 -i /path/to/file.sf2 -c 'raw'
```

Die Umwandlung erstellt ein Verzeichnis (benannt nach dem Ausgabenamen) mit der Datei :file:`samples.csv` und einer Datei pro Instrument und pro Preset. Existiert dieses Verzeichnis bereits, wird seinem Namen eine Zahl angehängt.


## Synth-Modus {#doc_synth}


### Kommandozeile


```
polyphone -s -i <file/to/read> -c <configuration>
```

**Hinweis:** Unter Windows müssen Sie statt :command:`polyphone` den vollständigen Pfad von Polyphone angeben, zum Beispiel :command:`C:/Program files/Polyphone/polyphone.exe`.

![Polyphone, geöffnet mit der Option für den Synth-Modus](images/synth_mode.png "Polyphone, geöffnet mit der Option für den Synth-Modus")


### Argumente


-s
: Modus „s“ bedeutet „Polyphone im Synth-Modus öffnen“.

-c
: Konfiguration des Synthesizers, bestehend aus 3 durch das Zeichen „|“ getrennten Werten. Dieses Argument ist optional; standardmäßig ist es „0|off|off“. Diese Parameter können anschließend über die grafische Oberfläche geändert werden.
  
  * Der erste Wert ist die Nummer des MIDI-Kanals (zwischen 1 und 16), auf den zum Spielen gehört wird. Ein Wert von 0 oder „all“ bedeutet, dass alle Kanäle verwendet und wie ein einziger Kanal verarbeitet werden. Ein Wert von -1 oder „each“ hört jeden Kanal getrennt ab, was zum Beispiel Voraussetzung für das [Abspielen von MIDI-Dateien](tutorials/read-midi-file-with-soundfont.md) ist.
  * Der zweite Wert ist „off“ oder „on“ und deaktiviert bzw. aktiviert die Auswahl mehrerer Presets zum Spielen.
  * Der dritte Wert ist „off“, „on“ oder „toggle“ und deaktiviert bzw. aktiviert die Auswahl von Presets über die Tasten, die der Presetnummer entsprechen (Taste 0 aktiviert zum Beispiel Preset 0).


### Beispiel


```
polyphone -s -i /path/to/file.sf2 -c '1|on|toggle'
```


## Weitere Optionen {#doc_other}


-h
: Zeigt eine kurze Hilfemeldung an.

-r
: Setzt die Konfiguration von Polyphone zurück (Einstellungen, Fenstergeometrie, Verlauf usw.).
  Dies kann bei Problemen nützlich sein.

```
polyphone -r
```
