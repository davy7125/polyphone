Polyphone also exists as a plugin for digital audio workstations (DAW) and the other hosts of audio plugins.
The plugin plays soundfonts in the formats sf2, sf3, sfz and dls with the synthesizer of Polyphone: a soundfont sounds the same in a host and in the editor.
It plays the soundfonts but doesn't edit them: the changes are made in Polyphone, and the soundfont is then opened again in the plugin.

The plugin is available in the formats VST3, CLAP and LV2 on Linux, Windows and macOS, and also Audio Unit (AU) on macOS.


![Polyphone plugin](images/plugin.png "Polyphone plugin")


## Installation {#doc_install}


The plugin is distributed in an archive for each operating system, available in the section “[Software](software)”.
Copy the formats used by your host in the folders where it looks for the plugins, then rescan the plugins in the host.

Linux
: :file:`Polyphone.vst3` in :file:`~/.vst3/`,\
  :file:`Polyphone.clap` in :file:`~/.clap/`,\
  :file:`Polyphone.lv2` in :file:`~/.lv2/`.

macOS
: :file:`Polyphone.vst3` in :file:`~/Library/Audio/Plug-Ins/VST3/`,\
  :file:`Polyphone.clap` in :file:`~/Library/Audio/Plug-Ins/CLAP/`,\
  :file:`Polyphone.component` in :file:`~/Library/Audio/Plug-Ins/Components/` (Audio Unit format, used by Logic Pro and GarageBand),\
  :file:`Polyphone.lv2` in :file:`~/Library/Audio/Plug-Ins/LV2/`.

Windows
: :file:`Polyphone.vst3` in :file:`C:\Program Files\Common Files\VST3`,\
  :file:`Polyphone.clap` in :file:`C:\Program Files\Common Files\CLAP`,\
  :file:`Polyphone.lv2` in :file:`%APPDATA%\LV2`.

On macOS, the plugins are not notarized: if the host refuses to load them, remove the quarantine flag of the downloaded files, for instance:

```
xattr -dr com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/Polyphone.vst3
```


## Soundfont {#doc_soundfont}


The top of the window shows the name of the soundfont and its path.
A soundfont is opened:

* with the button :guilabel:`Open...`,
* with the arrow on its right, listing the last 10 soundfonts opened,
* by dropping a file on the window.

The sfz and dls files are converted as Polyphone imports them, the samples of an sfz file being the audio files it references.
When a soundfont is opened, its structure is read at once: the instruments are listed and can be played.
Its samples are then read in the background, with a spinner in front of its name (its tooltip gives the progress): a note whose samples are not read yet is silent.
The samples of the notes played and of the instruments chosen are read first, the samples of an sf3 file being decompressed.
Once everything is read, nothing is read on the disk while playing; an offline export waits until all samples are read.
When several instances of the plugin play the same file (one per track for instance), the file is read and kept in memory only once.
If the file has changed, for instance after being saved again in Polyphone, it is read again.


## Playing modes {#doc_modes}


Three buttons above the list of instruments (the presets of the soundfont) choose how the instruments are played:

:guilabel:`One instrument`
: The instrument selected in the list is played on the 16 MIDI channels.
  The instruments are displayed with their bank and program numbers, for instance “000:024 Guitar”.

:guilabel:`16 channels`
: Each MIDI channel plays its own instrument, chosen in its row.
  The choice :guilabel:`Program change` plays the instrument chosen by the bank select and program change messages received by the channel, as a General MIDI synthesizer does: the bank 128 is then used on the channel 10, for the drums.
  The instrument played is displayed on the right of each row.
  This mode is convenient for playing a MIDI file arranged for several instruments.

:guilabel:`MPE`
: The selected instrument is played by an MPE controller (MIDI Polyphonic Expression): each note has its own channel, and thus its own pitch bend, pressure and timbre (CC 74).
  A lower zone of 15 channels is used until the controller sends its configuration (RPN 6).
  The master channel of a zone acts on all its notes: its pitch bend is added to the one of each note and its controllers apply to all of them.
  The pitch bend range is 48 semitones for the notes and 2 semitones for the master channel, and can be changed by the controller (RPN 0).


![Plugin in the mode “16 channels”](images/plugin_channels.png "Plugin in the mode “16 channels”")


## Parameters {#doc_parameters}


The parameters are the ones of the synthesizer of Polyphone, which can be found in its [sound](manual/settings.md#doc_sound) and [keyboard](manual/settings.md#doc_keyboard) settings.
:guilabel:`Gain` (from -24 to +24 dB) is in the header, the other parameters are grouped on the right:

:guilabel:`Tuning`
: :guilabel:`Reference pitch`: a note from A#3 to A4 and its frequency (from 400 to 480 Hz for A4, 440 Hz by default), this note sounding at this frequency whatever the temperament.
  :guilabel:`Transposition`: from -24 to +24 semitones, the drums being not transposed.

:guilabel:`Velocity`
: The response to the velocity of the notes, each setting being drawn in a graph (velocity from 0 to 127 from left to right):
  
  * :guilabel:`Filter cutoff`: default modulator lowering the filter cutoff according to the velocity, by up to 2 octaves, as in the [sound settings](manual/settings.md#doc_sound) of Polyphone. The version 2.04 of the SoundFont format (default) lowers it all the more as the velocity is low. The version 2.01 had a jump at the velocity 64, an error fixed by the version 2.04: it is a compatibility mode for the old soundfonts made to compensate this jump. With :guilabel:`Disabled`, the cutoff does not depend on the velocity. It has no effect on a soundfont having its own [default modulators](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md#doc_default_mod).
  * :guilabel:`Curve`: curve applied to the velocity of the notes received, linear in the middle (default, restored by a double click), concave to the left (softer velocity: the high velocities need a stronger touch) and convex to the right (the velocity rises quickly).

:guilabel:`Temperaments and scales`
: The temperaments and scales of Polyphone in their categories, then the custom temperament, transposed to the chosen :guilabel:`Tonality`.
  The keyboard on the right shows the fine tune of each note in cents and is edited as in the [keyboard settings](manual/settings.md#doc_keyboard) of Polyphone: mouse wheel or vertical drag, double click to set a value back to 0, context menu to change the use of a note or to move the tonality to it. A temperament edited becomes the custom temperament.
  The notes not used by a scale are not played, unless :guilabel:`Play all` is checked.

:guilabel:`Reverb`
: :guilabel:`Level`, :guilabel:`Size`, :guilabel:`Width` and :guilabel:`Damping`.

:guilabel:`Chorus`
: :guilabel:`Level`, :guilabel:`Depth` and :guilabel:`Frequency`.

The groups :guilabel:`Temperaments and scales`, :guilabel:`Reverb` and :guilabel:`Chorus` have a switch on the right of their title: switched off, the group is collapsed and its temperament or effect is not applied (equal temperament with all the notes, no effect), its settings being kept.
The height of the window follows the groups expanded.

The host can automate all these parameters, except the note of the reference pitch (the frequency of A4 being automated) and the fine tunes of the custom temperament, which are saved with the project.


## Keyboard and status {#doc_keyboard}


The keyboard at the bottom of the window shows the notes received, and can be played with the mouse.
Below, the number of voices playing and the share of the processor taken by the plugin are displayed.

The button :guilabel:`Panic` stops all the sounds and resets the controllers, which is useful if a note is stuck.


## Settings {#doc_settings}


The button :guilabel:`Settings` opens a menu with the following options:

:guilabel:`Polyphony`
: Maximum number of voices, from 64 to 4096 (1024 by default).
  Above this number, the quietest notes fade out.

:guilabel:`Keyboard`
: MIDI channel of the notes played with the keyboard of the window.

:guilabel:`Language`
: Language of the plugin, by default the one of the system.
  This choice is shared by all instances of the plugin.
  The names of the parameters and of the outputs given to the host are the ones of the language used when the plugin is loaded (always English with LV2).

:guilabel:`Fixed latency (notes played at their exact position)`
: When this option is disabled (default), the MIDI messages of an audio block are moved together so that the first note of the block starts at its beginning: the notes played live sound sooner, by half the duration of a buffer on average, but their latency varies with their position in the buffer of the host.
  When it is enabled, each message is processed at its exact position: the latency of the notes played live is constant.
  During an offline export, the messages are always processed at their exact position.

:guilabel:`Clipping to [-1; 1]`
: The outputs are limited to the interval [-1; 1] (default), as in Polyphone.
  Without clipping, the higher values are given to the host, which mixes them as floating-point numbers: the gain can then be raised without saturation, the level being adjusted before the output of the host.

:guilabel:`Store the soundfont in the project`
: The soundfont is saved in the project of the host, which can then be opened on another computer without the file.
  This is not possible with an sfz file, whose samples are other files.

:guilabel:`Channels and effects on their own outputs`
: In addition to the main output, the outputs “Channel 1” to “Channel 16” receive the notes of their channel without effect, and the output “Effects” receives the part sent to the reverb and to the chorus.
  A channel whose output is not enabled by the host stays in the main output.
  When this option is disabled (default), everything is mixed in the main output.


## Use in a project {#doc_project}


The project of the host saves the parameters, the instruments played, the polyphony and the path of the soundfont (or its content, if it is stored in the project).
When a project is opened, only the structure of the soundfont is read before the rest is restored: the host does not wait for the samples, and an offline export can start at once (it waits for the samples not read yet).

A note starts exactly at the position of its MIDI message, whatever the buffer size of the host, during an offline export and in real time with a [fixed latency](manual/plugin.md#doc_settings).
During an offline export, the voices are never limited by the processor load.
