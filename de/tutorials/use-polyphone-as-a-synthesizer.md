Polyphone ist nicht nur ein Soundfont-Editor, sondern auch ein vollwertiger Synthesizer, auf dem Sie mit einer MIDI-Tastatur spielen können.
Dieses Tutorial zeigt, wie man Polyphone im Synth-Modus startet.


## Voraussetzungen

Zunächst benötigen Sie ein **Masterkeyboard** oder einen anderen MIDI-Controller, den Sie über einen **MIDI-USB-Adapter** mit dem Computer verbinden.

Laden Sie die neueste Version von Polyphone [hier](software) herunter, installieren und öffnen Sie das Programm. In den [Einstellungen](manual/settings.md#doc_general) sollte Ihr MIDI-Gerät unter den MIDI-Eingängen zu finden sein. Ist das nicht der Fall, ist wahrscheinlich der Treiber des MIDI-Adapters nicht auf dem Computer installiert (beheben Sie dies, bevor Sie fortfahren).

Prüfen Sie, ebenfalls in den Einstellungen, ob der Audioausgang korrekt ist.

## Polyphone im Synth-Modus öffnen


### Windows

Erstellen Sie eine Verknüpfung zu :file:`polyphone.exe` mit dem Argument :command:`-s`, wie im Abschnitt [Startoptionen von Polyphone](#doc_launch) unten erklärt.

### Linux

Öffnen Sie ein Terminal und geben Sie die folgende Kommandozeile ein:

```
polyphone -s
```

### Mac

Öffnen Sie ein Terminal und geben Sie die folgende Kommandozeile ein:

```
/Applications/Polyphone.app/Contents/MacOS/Polyphone -s
```

**Hinweis:** Wenn Sie die Anwendung Polyphone in einen anderen Ordner als :file:`Applications` verschoben haben, müssen Sie den obigen Pfad anpassen.


## Ein Instrument spielen


Um ein Instrument zu spielen, öffnen Sie einfach einen Soundfont, den Sie zum Beispiel aus der [Fundgrube](soundfonts) herunterladen können, und wählen Sie ein darin enthaltenes Instrument aus. Die Instrumente sind nach Banken geordnet: Wählen Sie eine Bank aus, um darunter die Liste der Instrumente anzuzeigen.

![Polyphone, geöffnet mit der Option für den Synth-Modus](images/synth_mode.png "Polyphone, geöffnet mit der Option für den Synth-Modus")

Sobald Sie ein Instrument ausgewählt haben, wird sein Tastenbereich auf der virtuellen Tastatur angezeigt, und Sie können es mit den entsprechenden Tasten Ihres MIDI-Controllers spielen.
Der Button :guilabel:`MIDI-Panik` stoppt alle gerade klingenden Töne (nützlich, wenn eine Note hängt), und der Button :guilabel:`Rekorder` öffnet den [Rekorder](manual/soundfont-editor/toolbar.md#doc_recorder).


## Optionen


### Mehrere Instrumente gleichzeitig spielen

Um beim Drücken einer Taste mehrere Instrumente zu spielen, setzen Sie einfach die Option :guilabel:`Mehrfachauswahl` auf :guilabel:`Ein`. In der Instrumentenliste können nun mehrere Instrumente ausgewählt werden.

### Mehrere Instrumente getrennt spielen

Sind mehrere MIDI-Controller mit dem Computer verbunden, können Sie mehrere Soundfonts öffnen und für jeden geöffneten Soundfont einen bestimmten Kanal wählen. So wird zum Beispiel mit einer ersten Tastatur ein Instrument und mit einer zweiten Tastatur ein anderes gespielt. Derselbe Soundfont kann mehrmals geöffnet werden, um mehrere seiner Instrumente gleichzeitig zu spielen.

Die Kanalzuordnung erfolgt durch Ändern des Werts von :guilabel:`Kanal`. Zum Beispiel hört der zuerst geöffnete Tab auf Kanal 1, der zweite auf Kanal 2.
Der Wert :guilabel:`alle` (Standard) hört auf alle Kanäle, und der Wert :guilabel:`erkennen...` wählt den Kanal der nächsten Taste, die auf Ihrer Tastatur gedrückt wird.

![Synthesizer-Konfiguration mit 2 Kanälen](images/synth_2_channels.png "Synthesizer-Konfiguration mit 2 Kanälen")

**Hinweis:** Manche Tastaturen lassen sich in zwei Bereiche aufteilen (Split), sodass mit einer einzigen Tastatur Tasten an 2 verschiedene Kanäle gesendet werden können.

### Instrumente mit einer Taste wechseln

Das Wechseln des Instruments erfolgt in der Regel über eine MIDI-Nachricht vom Typ „Program Change“, und Polyphone kann diese auswerten.

Es kann jedoch auch ein anderer Mechanismus verwendet werden: Tasten, die so programmiert sind, dass sie Nachrichten vom Typ „Note On“ / „Note Off“ senden, wobei die Notennummer der Instrumentnummer entspricht. Dieser Mechanismus kann nützlich sein, wenn der Soundfont nur wenige Instrumente enthält und sich Tasten so programmieren lassen, dass sie bestimmte Noten senden (zum Beispiel die Pads eines Masterkeyboards).

Ist die Option :guilabel:`Auswahl nach Tasten` auf :guilabel:`Ein` gesetzt:

* wird ein Instrument aktiviert, wenn eine MIDI-Nachricht „Note On“ empfangen wird,
* wird ein Instrument deaktiviert, wenn eine MIDI-Nachricht „Note Off“ empfangen wird.

Diese Option eignet sich, wenn das Pad eines Masterkeyboards als Umschalter konfiguriert ist (ein Druck aktiviert die Note, ein weiterer deaktiviert sie).

Ist :guilabel:`umschalten` ausgewählt:

* wird ein Instrument aktiviert, wenn eine MIDI-Nachricht „Note On“ empfangen wird,
* wird ein Instrument deaktiviert, wenn eine weitere MIDI-Nachricht „Note On“ empfangen wird.

In diesem Fall erfolgt das Umschalten in Polyphone.


## Startoptionen von Polyphone {#doc_launch}


Alle oben beschriebenen Optionen können beim Öffnen von Polyphone aufgerufen werden, indem der Kommandozeile Parameter hinzugefügt werden. Es kann auch der zu öffnende Soundfont angegeben werden.

Unter **Linux** oder **macOS** ergänzen Sie einfach die Kommandozeile. Unter **Windows** erstellen Sie eine Verknüpfung, indem Sie mit der rechten Maustaste auf die ausführbare Datei :file:`polyphone.exe` klicken und dann :guilabel:`Senden an` → :guilabel:`Desktop (Verknüpfung erstellen)` wählen (unter Windows 11 über :guilabel:`Weitere Optionen anzeigen` im Kontextmenü). Anschließend können Sie in den Eigenschaften dieser Verknüpfung dem Ziel Parameter hinzufügen, zum Beispiel:

```
"C:\Program Files\Polyphone\polyphone.exe" -s -c "all|on|toggle"
```

Die verschiedenen Optionen werden auf dieser [Seite](manual/annexes/command-line.md#doc_synth) beschrieben.


## Einsatzmöglichkeiten des Synthesizers


Jetzt, da Sie wissen, wie man Polyphone in einen Synthesizer verwandelt, warum nicht [mit einem Raspberry Pi ein Musikinstrument bauen](tutorials/create-a-synthesizer-with-raspberry-pi.md)?

Der Synth-Modus von Polyphone kann auch zum [Abspielen von MIDI-Dateien](tutorials/read-midi-file-with-soundfont.md) verwendet werden.
