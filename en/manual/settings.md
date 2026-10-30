The software preferences are accessible either from the [home screen](manual/index.md) or from the [menu](manual/menu.md#doc_shortcuts) (shortcut :kbd:`Ctrl`+:kbd:`,`).
They are divided into five sections:

* [General](#doc_general)
* [Interface](#doc_interface)
* [Sound](#doc_sound)
* [Keyboard](#doc_keyboard)
* [Online repository](#doc_repository)

All modifications are saved immediately.


## General {#doc_general}


The :guilabel:`General` section allows the modification of the following parameters:

Audio backend
: Specifies the audio system and the device used to play sounds.
  The available systems depend on the operating system (for example WASAPI and ASIO on Windows, ALSA and JACK on Linux, Core Audio on macOS).
  Choosing “-” turns off the audio output.

Buffer size
: Specifies the size of the audio buffer.
  A small buffer results in a small latency, but the sound may become scratchy.
  With the value :guilabel:`Default`, the size depends on the audio system.

Polyphony
: Maximum number of voices played at the same time, from 64 to 4096 (1024 by default).
  A voice is a sample being played: a note can use several voices, two with a stereo sample for instance.
  Beyond this number, notes fade out: first the released ones, then the quietest ones.
  The notes that have just started, as well as the highest and the lowest keys held, are kept as long as possible.

Processor load
: Maximum share of the processor time used for computing the voices, from 20 to 100% (60% by default).
  Beyond this share, notes fade out in the same way.
  A high value allows more notes to be played at the same time, but the sound may become scratchy.

MIDI input
: Specifies the MIDI input controlling the synthesizer and the virtual keyboard.

Moreover, it is possible to tick the following options:

Stereo editing: change linked sample
: When editing one side of a stereo sample, the other side is automatically edited the same.
  The same applies when editing a stereo instrument division: the division corresponding to the other channel (with the same note and velocity ranges) is edited the same.

Sample import: trim to loop
: When importing a sample, data located after the end of loop are automatically removed (same effect as the tool “[Trim to end of loop](manual/soundfont-editor/tools/sample-tools.md#doc_trimloop)”).

Sample import: remove blank at start
: When importing a sample, any blank area located before the beginning of the attack is automatically removed (same effect as the tool “[Remove blank at start](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)”).


![Settings, general](images/settings_general.png "Settings, general")


## Interface {#doc_interface}


This section shows options related to the interface of the software:

Language
: The language may be changed here.

Key names
: Keys may be named depending on their number (according to the MIDI specification), or depending on the name of middle C (C3, C4 or C5).
  Key names may comprise sharps or flats.

Unique instance
: Open all files in a single window.

Startup view
: Select the view to display when a soundfont is opened among the following: :guilabel:`General` (default), :guilabel:`Samples`, :guilabel:`Samples - first sample`, :guilabel:`Instruments`, :guilabel:`Instruments - first instrument - Table`, :guilabel:`Instruments - first instrument - Ranges`, :guilabel:`Instruments - first instrument - Envelopes`, :guilabel:`Presets`, :guilabel:`Presets - first preset - Table`, :guilabel:`Presets - first preset - Ranges`. For the first instrument or preset, the last part sets the page displayed: [table](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table), [range editor](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range) or [envelope editor](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope).

Sort divisions
: The order of the divisions within an instrument or a preset can be changed here: :guilabel:`by key range`, :guilabel:`by velocity range`, :guilabel:`in alphabetical order` or :guilabel:`none` (the order of creation).

Style
: The style of the interface elements.
  The style :guilabel:`Fusion` is the default one, and others may be proposed depending on the operating system (Adwaita, Windows, macOS…).

Colors
: Different predefined color sets are available for customizing the appearance of Polyphone.
  A manual selection of the colors of the text, the window, the buttons, the selection and the lists is also possible.
  Colors can be changed only with the styles :guilabel:`Fusion` and :guilabel:`Windows`.

Window borders
: Display or hide the window borders, for saving space on the screen (not available on macOS).

Decorations
: Background decorations in some lists can be deactivated for readability.

A modification of the language, the key names, the style, the colors, the window borders or the decorations is applied during the next start of the software.


![Settings, interface](images/settings_interface.png "Settings, interface")


## Sound {#doc_sound}


The :guilabel:`Sound` section makes it possible to modify the reverb (level, room size, damping and width) and the chorus (level, depth and frequency) of the synthesizer, along with its general volume.
The general volume may be changed with the volume MIDI controller if an external keyboard is connected.
A led on the left of :guilabel:`Reverb` and :guilabel:`Chorus` switches each effect on or off without losing its settings.
These settings can also be changed below the [virtual keyboard](manual/soundfont-editor/toolbar.md#doc_keyboard_effects) while playing: a change is progressive and doesn't interrupt the notes being played.

Since the default modulator modifying the filter cutoff frequency has been changed between versions 2.01 and 2.04 of the sf2 specifications, it is possible to define here which version to use:

* :guilabel:`SoundFont 2.01`: use the rule “lower the filter frequency taking into account the velocity IF the velocity is less than 64” (a clear and problematic gap can be heard between velocities 63 and 64);
* :guilabel:`SoundFont 2.04` (default): use the rule “lower the filter frequency taking into account the velocity” (no more gap);
* :guilabel:`disabled`: this default modulator is not set.


![Settings, sound](images/settings_sound.png "Settings, sound")


## Keyboard {#doc_keyboard}


The keys of the computer keyboard used to control the [virtual keyboard](manual/soundfont-editor/toolbar.md#doc_keyboard) are defined in the :guilabel:`Keyboard` section.
The pitch may be increased or decreased octave by octave with the setting :guilabel:`First C`.

For more advanced musicians, the tuning can be changed here:

Reference pitch
: The frequency of the A located above middle C (key 69), between 390 and 470 Hz (440 Hz by default).

Temperament
: A temperament other than the equal temperament can be applied when playing.
  When a temperament is selected, the key that is the reference of this temperament (its tonic) can also be chosen.

The reference pitch and the temperament can also be changed with MIDI RPN messages (master fine tuning, tuning program select and tuning bank select).


![Settings, keyboard](images/settings_keyboard.png "Settings, keyboard")


## Online repository {#doc_repository}


The email address and the password of an account can be entered here to link Polyphone with this account, a [Premium account](premium) unlocking the whole [online repository](manual/online-repository.md).
After a successful connection, Polyphone is automatically connected to the account at each start, until the button :guilabel:`Log out` is used.

The directory used when downloading soundfonts from the [online repository](manual/online-repository.md) can also be changed here.


![Settings, repository](images/settings_repository.png "Settings, repository")



*[JACK]: JACK Audio Connection Kit
*[ASIO]: Audio Stream Input/Output
*[MIDI]: Musical Instrument Digital Interface