The toolbar, located on the top of the editor, contains icons for:

* common [editing functions](#doc_edit),
* switching the [view](#doc_view) in editors,
* displaying the [recorder](#doc_recorder),
* displaying the [virtual keyboard](#doc_keyboard).


![Toolbar](images/toolbar.png "Toolbar")


## Editing functions {#doc_edit}


The elements in the left part of the toolbar are for common editing functions:


![new sample][sample] New sample
: Add new [samples](manual/soundfont-editor/editing-pages/sample-editor.md) to the soundfont.
  A dialog will be displayed for selecting audio files (:file:`.wav` and :file:`.flac` files can be imported).

![new instrument][instrument] New instrument
: Create a new [instrument](manual/soundfont-editor/editing-pages/instrument-editor.md).

![new preset][preset] New preset
: Create a new [preset](manual/soundfont-editor/editing-pages/preset-editor.md).

![tools][toolbox] Tools
: Clicking on this button will show a menu containing all available [tools](manual/soundfont-editor/tools/index.md) for the selected element(s) in the [tree](manual/soundfont-editor/tree.md).

![undo][undo] Undo
: Undo the last modification.

![redo][redo] Redo
: Redo the last undone modification.

![save][save] Save file
: Save the soundfont.


## Switch view {#doc_view}


In the right part of the toolbar, some icons may appear depending on which [editor](manual/soundfont-editor/editing-pages/index.md) is displayed:

* switching between the ![table][table] [table view](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table), the ![range][range] [range editor](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range) and the ![envelope][adsr] [envelope editor](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope) is possible when editing an instrument,
* switching between the ![table][table] table view and the ![range][range] range editor is possible when editing a preset,
* nothing displayed otherwise.


## Recorder {#doc_recorder}


When clicking on the icon ![recorder][recorder], a window opens showing the recorder that can record the output of Polyphone.


![Recorder](images/recorder.png "Recorder")


The left button can have two possible states:


Record
: Start a new record if the acquisition is off.
  It is possible here to specify the file name in which the audio signal is to be recorded.

Stop
: Stop the current acquisition.


The right button can also have two possible states (only accessible if the acquisition is on):


Pause
: Pause the current acquisition.

Resume
: Resume the current acquisition.


## Virtual keyboard {#doc_keyboard}


When clicking on the icon ![piano][piano], a window opens showing the virtual keyboard that has been made for testing samples, instruments and presets.
This window triggers MIDI signals:

* **note on** / **note off** / **after touch** with the piano keyboard,
* other MIDI signals if the **MIDI controller part** is expanded.


![Virtual keyboard](images/virtual_keyboard.png "Virtual keyboard")


### Piano keyboard


The piano keyboard triggers signals that contains key and velocity numbers.
It can be controlled in three ways:


with the mouse
: The desired key may be clicked by the mouse.
  The velocity is defined according to the vertical position of the click.
  After a click, following moves within the same key will trigger aftertouch signals.

using the computer keyboard
: The keys used are defined in the [settings](manual/settings.md#doc_keyboard).

using an external MIDI keyboard
: The MIDI input is defined in the [settings](manual/settings.md#doc_general).
  If the piano keyboard is visible, it will show what you are playing.


#### Markers and colors


* When an [instrument](manual/soundfont-editor/editing-pages/instrument-editor.md) or a [preset](manual/soundfont-editor/editing-pages/preset-editor.md) is selected, the area not covered by the divisions is **grayed**.
  It is thus possible to visualize the full range of an instrument or a preset.
* When a division of an instrument or a preset is selected, its range is displayed the same way than above.
  Moreover, its root key is identified by an **orange marker**.
* When a key is selected on the keyboard, it appears in **blue** (or the selection color selected in the software [settings](manual/settings.md#doc_interface).
  Moreover, the range of its corresponding range(s) appears in **light blue**.
  This applies only at the instrument level.
* Middle C (key 60) is identified by a **black marker**.


#### Useful shortcuts


:kbd:`Ctrl`+:kbd:`K`
: This shortcut can put at any time the focus on the virtual keyboard.
  This allows, while editing in a table for instance, to use the computer to play instead of edit a cell.
  A glow effect on the virtual keyboard confirms that the focus is on it.

:kbd:`Ctrl`+:kbd:`1`, :kbd:`Ctrl`+:kbd:`2`, … :kbd:`Ctrl`+:kbd:`8`
: These shortcuts change the octaves played with the computer keyboard.
  After the use of :kbd:`Ctrl`+:kbd:`1`, the lowest octave can be played.
  The highest octave is reached using :kbd:`Ctrl`+:kbd:`8`.

:kbd:`→` / :kbd:`←`
: The right or left arrow will play the pitch just next or below the last key that has been played, with the same velocity.
  The association of the key :kbd:`Shift` skips keys.

:kbd:`↑` / :kbd:`↓`
: The up or down arrow will play the same pitch than the last key played, but with a stronger or weaker velocity.
  The association of the key :kbd:`Shift` skips velocities.


### MIDI controllers


When clicking on the bottom right arrow, the MIDI controller area can be displayed or hidden.
If you are playing with modulators, it is advised that you use these features to test your soundfont.


![Virtual keyboard with MIDI controllers](images/virtual_keyboard_2.png "Virtual keyboard with MIDI controllers")


This area is made of different buttons:


Pitch wheel
: The pitch wheel increases or decreases the pitch.
  It goes back automatically to the original position 0.

Sensitivity
: The sensitivity defines the modification range in semi-tones of the pitch wheel.
  By default this is 2 (one full tone).

Pressure
: The pressure is the amount of pressure on the keyboard.
  It can be compared to the aftertouch but while the aftertouch is key-specific, the pressure is a global signal for all keys.

Knobs
: 3 knobs are here so that MIDI controller values can be changed.
  For each knob you can specify a controller number and then its value.

On / off led
: This led is to set a MIDI controller value to 0 or 127 (useful for the sustain pedal for instance).
  It is like a knob but with only two possible values.



*[MIDI]: Musical Instrument Digital Interface


[sample]:     images/toolbar_sample.png     "new sample"     {.inline}
[instrument]: images/toolbar_instrument.png "new instrument" {.inline}
[preset]:     images/toolbar_preset.png     "new preset"     {.inline}
[toolbox]:    images/toolbar_toolbox.png    "tools"          {.inline}
[undo]:       images/toolbar_undo.png       "undo"           {.inline}
[redo]:       images/toolbar_redo.png       "redo"           {.inline}
[save]:       images/toolbar_save.png       "save"           {.inline}
[table]:      images/toolbar_table.png      "table"          {.inline}
[range]:      images/toolbar_range.png      "range"          {.inline}
[adsr]:       images/toolbar_adsr.png       "envelope"       {.inline}
[recorder]:   images/toolbar_recorder.png   "recorder"       {.inline}
[piano]:      images/toolbar_piano.png      "piano"          {.inline}
