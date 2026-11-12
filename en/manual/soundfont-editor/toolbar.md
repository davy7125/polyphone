The toolbar, located on the top of the editor, contains icons for:

* common [editing functions](#doc_edit),
* switching the [view](#doc_view) in editors,
* displaying the [recorder](#doc_recorder),
* displaying the [virtual keyboard](#doc_keyboard),
* displaying the [MIDI player](#doc_midi_player).


![Toolbar](images/toolbar.png "Toolbar")


## Editing functions {#doc_edit}


The elements in the left part of the toolbar are for common editing functions:

![new sample](images/toolbar_sample.png "new sample"){.inline} Add a sample
: Add new [samples](manual/soundfont-editor/editing-pages/sample-editor.md) to the soundfont.
  A dialog will be displayed for selecting audio files (:file:`.wav`, :file:`.flac`, :file:`.mp3`, :file:`.ogg`, :file:`.aif`, :file:`.aiff`, :file:`.aifc` and :file:`.snd` files can be imported), several files being selectable at once.

![new instrument](images/toolbar_instrument.png "new instrument"){.inline} Add an instrument
: Create a new [instrument](manual/soundfont-editor/editing-pages/instrument-editor.md).
  A dialog asks for the name of the instrument and, if samples are selected in the [tree](manual/soundfont-editor/tree.md), offers to link them to the new instrument.
  In this case, stereo samples are panned to the left and to the right, the loop is enabled if all samples are looped and, when the samples have different root keys, the key ranges are automatically defined (see the tool [Sample auto-positioning](manual/soundfont-editor/tools/instrument-tools.md#doc_position)).

![new preset](images/toolbar_preset.png "new preset"){.inline} Add a preset
: Create a new [preset](manual/soundfont-editor/editing-pages/preset-editor.md).
  A dialog asks for the name of the preset and, if instruments are selected in the tree, offers to link them to the new preset.
  The first available bank and preset numbers are used.

![tools](images/toolbar_toolbox.png "tools"){.inline} Toolbox
: Clicking on this button will show a menu containing all available [tools](manual/soundfont-editor/tools/index.md) for the selected element(s) in the [tree](manual/soundfont-editor/tree.md).

![undo](images/toolbar_undo.png "undo"){.inline} Undo
: Undo the last modification, also accessible with :kbd:`Ctrl`+:kbd:`Z`.

![redo](images/toolbar_redo.png "redo"){.inline} Redo
: Redo the last undone modification, also accessible with :kbd:`Ctrl`+:kbd:`Y` or :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`Z`.

![save](images/toolbar_save.png "save"){.inline} Save
: Save the soundfont, also accessible with :kbd:`Ctrl`+:kbd:`S`.


## Switch view {#doc_view}


In the right part of the toolbar, some icons may appear depending on which [editor](manual/soundfont-editor/editing-pages/index.md) is displayed:

* switching between the ![table](images/toolbar_table.png "table"){.inline} [table view](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table), the ![range](images/toolbar_range.png "range"){.inline} [range editor](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range), the ![envelope](images/toolbar_adsr.png "envelope"){.inline} [envelope editor](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope) and the ![timbre](images/toolbar_timbre.png "timbre"){.inline} [timbre view](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_timbre) is possible when editing an instrument,
* switching between the ![table](images/toolbar_table.png "table"){.inline} table view and the ![range](images/toolbar_range.png "range"){.inline} range editor is possible when editing a preset,
* nothing displayed otherwise.

The selected view is kept when another instrument or preset is selected. The view displayed for the first instrument or preset when a soundfont is opened can be chosen with the option :guilabel:`Startup view` in the [settings](manual/settings.md#doc_interface).


## Recorder {#doc_recorder}


When clicking on the icon ![recorder](images/toolbar_recorder.png "recorder"){.inline}, a window opens showing the recorder, which can record in a file what Polyphone plays, the audio input or the MIDI signals. The elapsed time is displayed in the window.


![Recorder](images/recorder.png "Recorder")


The list in the banner selects the recording mode:

Polyphone → wav
: Record the output of Polyphone in a :file:`.wav` file.

Microphone → wav
: Record the audio input chosen in the [settings](manual/settings.md#doc_general) in a :file:`.wav` file, in stereo or in mono depending on the device.
  The input is not sent to the audio output.

MIDI signals
: Record in a :file:`.mid` file what is played with a MIDI keyboard or the [virtual keyboard](#doc_keyboard).

The left button can have two possible states (shortcut :kbd:`Ctrl`+:kbd:`R`, available in all windows, the recorder being opened if needed):

Record
: Start a new record if the acquisition is off.
  The file is created in the folder of the last record, with a number added to its name if a file with this name already exists.

Stop
: Stop the current acquisition.
  The acquisition is also stopped when the window is closed, after a confirmation.

The right button can also have two possible states (only accessible if the acquisition is on, shortcut :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`R`):

Pause
: Pause the current acquisition.

Resume
: Resume the current acquisition.

Below these buttons, the meter shows the level of the channels, even when nothing is recorded: the volume can be adjusted before a record. The value on its right is the highest level reached since the beginning of the record, displayed on a red background if the maximum level has been reached, the sound being then clipped. A click on the meter resets this value. In the mode :guilabel:`MIDI signals`, the meter shows the velocity of the notes played, which confirms that the signals are received.

The bottom of the window lists the last 5 records, audio or MIDI depending on the mode, with their size or their number of notes. A file that no longer exists is removed from the list when the window opens. A click selects a line, which then shows four buttons:

Listen
: Play the audio file in Polyphone, on the audio output (a second click stops it, and selecting another line during the playback plays its file instead), or open the MIDI file in the [MIDI player](#doc_midi_player).

Folder
: Open the folder of the file.

Delete
: Move the file to the trash, after a confirmation.

Import
: Import the audio file as a sample in the soundfont displayed.
  This button is only available if a soundfont editor is displayed.

The menu on the right of the banner contains the following options:

Start automatically
: The record button waits for the first sound or MIDI signal before starting the record.
  After an automatic stop, the next record is awaited in the same way, until a click on the stop button.

Stop after a silence
: Number of seconds without sound or MIDI signal stopping a record, from 1 to 10 (:guilabel:`disabled` for no automatic stop).
  The silence is removed from the file, and a key still held is not a silence.

Sound level
: Only for the audio input: level from which the input is considered as a sound, above its background noise.
  This level is fixed at -90 dB for the output of Polyphone.

Ask for a file name
: A dialog asks for the name of the file at the beginning of a record.

Change the folder...
: Select the folder of the next records, common to the audio and MIDI files.

A message is displayed if a record cannot continue (disk full or too slow, file reaching 4 GB, change of the audio input or of the sample rate of the audio output): what has been recorded until then stays in the file.


## Virtual keyboard {#doc_keyboard}


When clicking on the icon ![piano](images/toolbar_piano.png "piano"){.inline}, a window opens showing the virtual keyboard that has been made for testing samples, instruments and presets.
This window triggers MIDI signals:

* **note on** / **note off** / **aftertouch** with the piano keyboard,
* other MIDI signals if the **MIDI controller part** is expanded.

The banner at the top of the window contains a list for choosing the type of keyboard (5 octaves, 6 octaves, 88 keys or 128 keys), and displays the key, the velocity and the aftertouch of the note being played.
The menu on the right of the banner shows or hides the [MIDI controllers](#doc_keyboard_controllers) and the [audio effects](#doc_keyboard_effects), displayed above the keyboard.

The window keeps the proportions of the keyboard: changing its width changes its height.
Showing or hiding an area only changes the height of the window, which grows or shrinks from its top so that the keyboard stays at the same place on the screen (this is not possible with Wayland on Linux).

The virtual keyboard can also be displayed from the [soundfont explorer](manual/soundfont-explorer.md#doc_play), for playing the elements of the files of a directory without opening them.


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
  They play as soon as the window of the virtual keyboard is active, even if another element of this window has the focus (a knob, the drop-down list…).

using an external MIDI keyboard
: The MIDI input is defined in the [settings](manual/settings.md#doc_general).
  If the piano keyboard is visible, it will show what you are playing.


#### Markers and colors


* When an [instrument](manual/soundfont-editor/editing-pages/instrument-editor.md) or a [preset](manual/soundfont-editor/editing-pages/preset-editor.md) is selected, the area not covered by the divisions is **grayed**.
  It is thus possible to visualize the full range of an instrument or a preset.
* When a division of an instrument or a preset is selected, its range is displayed the same way as above.
  Moreover, its root key is identified by an **orange marker**.
* When a key is selected on the keyboard, it appears in **blue** (or the selection color selected in the software [settings](manual/settings.md#doc_interface)).
  Moreover, the range of its corresponding division appears in **light blue**.
  This applies only at the instrument level.
* Middle C (key 60) is identified by a **black marker**.


#### Useful shortcuts


:kbd:`Ctrl`+:kbd:`K`
: This shortcut puts the focus on the virtual keyboard at any time (the keyboard is displayed if it was hidden).
  This allows, while editing in a table for instance, to use the computer keyboard to play instead of editing a cell.
  A glow effect on the virtual keyboard confirms that the focus is on it.

:kbd:`Ctrl`+:kbd:`1`, :kbd:`Ctrl`+:kbd:`2`, … :kbd:`Ctrl`+:kbd:`8`
: These shortcuts change the octaves played with the computer keyboard.
  After the use of :kbd:`Ctrl`+:kbd:`1`, the lowest octaves can be played.
  The highest octaves are reached using :kbd:`Ctrl`+:kbd:`8`.
  They also work as soon as the window of the virtual keyboard is active.

:kbd:`→` / :kbd:`←`
: The right or left arrow will play the pitch just above or below the last key that has been played, with the same velocity.
  Combined with the key :kbd:`Shift`, 6 keys are skipped instead of 1.

:kbd:`↑` / :kbd:`↓`
: The up or down arrow will play the same pitch as the last key played, but with a stronger or weaker velocity.
  Combined with the key :kbd:`Shift`, the velocity is changed by 10 instead of 1.

The arrows play notes only if the piano keyboard has the focus: otherwise they act on the element having the focus, a knob for instance.


### MIDI controllers {#doc_keyboard_controllers}


When the option :guilabel:`MIDI controllers` is checked in the menu on the right of the banner, the MIDI controller area is displayed above the keyboard.
If you are playing with modulators, it is advised to use these features to test your soundfont.


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

All these elements can be synchronized with a MIDI keyboard connected to the computer if the MIDI input is correctly set in the [settings](manual/settings.md#doc_general). Relative MIDI controller values are supported for driving the knobs.


### Audio effects {#doc_keyboard_effects}


When the option :guilabel:`Effects` is checked in the menu on the right of the banner, an area is displayed at the top of the window for adjusting the main settings of the synthesizer while playing:

Reverb
: A led switches the reverb on or off, then 4 knobs change its level, room size, width and damping.
  The name of a knob is displayed when the mouse is over it.

Chorus
: A led switches the chorus on or off, then 3 knobs change its level, depth and frequency.

Volume
: The slider changes the general volume.

These settings are the same as in the [sound settings](manual/settings.md#doc_sound): a change made in one place is displayed in the other one.
A change is progressive and doesn't interrupt the notes being played.


## MIDI player {#doc_midi_player}


When clicking on the icon ![MIDI](images/toolbar_midi.png "MIDI"){.inline}, a window opens showing the MIDI player, which plays a MIDI file (:file:`.mid`, :file:`.midi`, :file:`.kar` or :file:`.rmi`) with the soundfonts that are open or with the files selected in a [soundfont explorer](manual/soundfont-explorer.md#doc_play), for testing them in a piece of music.
The soundfonts are played as they are in the editor, with their unsaved changes.
The MIDI player is also available in [synth mode](tutorials/use-polyphone-as-a-synthesizer.md).


![MIDI player](images/midi_player.png "MIDI player")


The banner at the top of the window contains, from left to right:

* a button for opening a MIDI file (when the window is opened, the last file opened is loaded again if it still exists),
* the name of the file, the elapsed time and the total duration, above a slider for moving to another position in the piece,
* a button for starting or pausing the playback, also accessible with :kbd:`Space`,
* a button for stopping the playback and going back to the beginning,
* a button opening the [playback settings](#doc_midi_player_settings).

If the file cannot be read, the reason is displayed instead of the channels.
The playback stops when the window is closed.

During the playback, an external MIDI keyboard or the [virtual keyboard](#doc_keyboard) can still play the element selected in the editor along with the file (the notes of the file are not displayed on the virtual keyboard), and the [recorder](#doc_recorder) can record the result.


### Channels


The table lists the channels of the file that contain notes, each channel having its own settings:

Channel
: The number of the channel, the name of its track being displayed in a tooltip.
  The buttons :guilabel:`M` (mute) and :guilabel:`S` (solo) silence the channel or play only the channels in solo.
  The MIDI button displays the current MIDI values of the channel that differ from their default value: controllers, pitch wheel (in semitones), its sensitivity and channel pressure.

Soundfont
: The soundfont played by the channel: :guilabel:`Selection`, one of the soundfonts open in tabs, or :guilabel:`None` for silence.
  :guilabel:`Selection`, chosen for all channels when a file is opened, plays what is selected in the main window: the soundfont of the current tab, or the file selected in a [soundfont explorer](manual/soundfont-explorer.md#doc_play), whose name is displayed in brackets.
  The other tabs (home, settings, online repository…) keep the soundfont played.
  When the selection changes, a spinner turns in front of the name while the soundfont is loading, the previous one being played meanwhile, then the presets of these channels go back to :guilabel:`Auto`.

Preset
: With :guilabel:`Auto`, the presets are chosen by the program changes of the file (bank 128 on channel 10, for the percussions) and the current one is displayed in brackets.
  Otherwise, the preset chosen in the list is played whatever the file requests.
  If the soundfont doesn't have the preset requested by the file, a warning icon is displayed and the notes of the channel are not played.

Volume
: The led lights up when notes start, and the slider changes the volume of the channel from -40 to +10 dB (a double click resets it to 0 dB).

The list of the soundfonts or presets opens with a double click on a cell, or with a click on a cell already selected.
These settings are reset when another file is opened.


### Playback settings {#doc_midi_player_settings}


The button on the right of the banner opens a menu with the following settings, which are kept when another file is opened:

Loop playback
: The piece starts again from the beginning when it ends.

Tempo
: Playback speed, from 25% to 200% of the tempo of the file (its tempo changes being kept).
  The tempo of the file is displayed in a tooltip, and the elapsed time and the duration displayed in the banner follow the playback speed.

Pitch
: Transposition from -12 to +12 semitones, except for the percussions (bank 128).

A double click on a slider resets it to its default value (100% and 0).


![Playback settings](images/midi_player_settings.png "Playback settings")


The menu ends with :guilabel:`Export audio...`, which writes the piece in a :file:`.wav` file as it is heard in the player: same soundfonts, presets, volumes, mute and solo, tempo and pitch.
The playback is paused during the export, which ends once the sound of the last notes has faded out.
If the export is canceled, the incomplete file is removed.
A MIDI file can also be rendered from the [command line](manual/annexes/command-line.md#doc_render).

*[MIDI]: Musical Instrument Digital Interface