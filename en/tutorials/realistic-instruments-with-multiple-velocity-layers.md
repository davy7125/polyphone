Many soundfonts play a single sample per note: whatever the force with which a key is struck, the same recording is heard, only louder or softer.
Acoustic instruments don't behave this way.
A piano key struck softly gives a round and mellow sound, while the same key struck hard gives a brighter sound, richer in harmonics, with a more percussive attack.

Velocity layers bring this richness to a soundfont: each note is recorded at several dynamics, and the recording that is played is chosen according to the velocity of the note.
This tutorial takes the piano as an example, but the method applies to every instrument whose timbre changes with the playing force: guitars, drums, brass, strings…
It ends with other details that make an instrument even more realistic.


## Why one sample is not enough {#doc_why}


Listen to the key C4 struck four times, from soft to loud (velocities 30, 60, 90 and 120):

* with a <a href="files/velocity-layers-one-layer.mp3" download>single sample</a>, recorded at a medium dynamic,

![](files/velocity-layers-one-layer.mp3)

* with <a href="files/velocity-layers-four-layers.mp3" download>four layers</a>, the recording being chosen according to the velocity.

![](files/velocity-layers-four-layers.mp3)

With a single sample, the soft notes sound like loud notes heard from far away and the loudest note lacks brilliance.
The synthesizer slightly lowers the cutoff frequency of a filter for the soft notes (this is one of the default modulators), but this is far from the change of timbre of a real piano.
With four layers, the timbre follows the playing force.

The difference is also obvious in a musical context, here with arpeggios getting louder and louder:

* <a href="files/velocity-layers-phrase-one-layer.mp3" download>single sample</a>,

![](files/velocity-layers-phrase-one-layer.mp3)

* <a href="files/velocity-layers-phrase-four-layers.mp3" download>four layers</a>.

![](files/velocity-layers-phrase-four-layers.mp3)


## Preparing the samples {#doc_samples}


### Recording the layers


Each note has to be recorded at several dynamics, for example *pp* (pianissimo), *mp* (mezzo-piano), *mf* (mezzo-forte) and *ff* (fortissimo).
With 3 or 4 layers the improvement is already obvious, while the best piano libraries use 8 to 16 layers for smoother transitions.
All recordings must be made in the same conditions (microphones, distance, gain): only the playing force must change.

The example of this tutorial uses 4 layers, with one sample every 3 keys over the whole range of the piano.


### Importing


Name the files with the note and the dynamic, for instance :file:`C4 pp.wav`, :file:`C4 mp.wav`, :file:`C4 mf.wav` and :file:`C4 ff.wav`.
When the root key is not stored in an audio file, Polyphone finds it in the file name if it contains the name of the note (C4 being the key 60) or the number of the key.
The dynamic written in the name will be useful in the next step.

Import all the files with :guilabel:`Add a sample` in the [toolbar](manual/soundfont-editor/toolbar.md#doc_edit), or with a [drag & drop](manual/soundfont-editor/tree.md#doc_dragdrop) onto the tree.
A stereo file gives two samples, whose names end with “L” and “R”.
Then check the tuning of each sample as explained in the tutorial “[How to prepare a sample](tutorials/how-to-prepare-a-sample.md#doc_tuning)”.
Piano samples don't need any loop: the sound fades out naturally.


## One instrument per layer {#doc_instruments}


Each layer becomes an instrument containing the samples of a single dynamic.
For each dynamic:

1. type its name (“pp” for instance) in the [search bar](manual/soundfont-editor/tree.md#doc_search) of the tree, so that only the samples of this layer are displayed,
2. select all these samples: click on the first one and then on the last one while holding :kbd:`Shift`,
3. click on :guilabel:`Add an instrument` in the [toolbar](manual/soundfont-editor/toolbar.md#doc_edit), name the instrument (“Piano pp” for instance) and keep the option :guilabel:`Link selected samples` checked.

The stereo samples are automatically panned to the left and to the right, and the key ranges are distributed over the keyboard according to the root keys (see the tool “[Sample auto-positioning](manual/soundfont-editor/tools/instrument-tools.md#doc_position)”).
Each instrument can already be played alone with the [virtual keyboard](manual/soundfont-editor/toolbar.md#doc_keyboard).

The parameters common to all layers can be edited at once: when the four instruments are selected in the tree, the [table](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) shows their global divisions side by side, and a value typed after selecting the four cells of a row is written in all of them.
In the example, the parameter :guilabel:`Vol env release (s)` is set to 1 s so that the sound doesn't stop abruptly when a key is released.


## Stacking the layers in a preset {#doc_preset}


Select the four instruments in the tree and click on :guilabel:`Add a preset`, the option :guilabel:`Link selected instruments` being checked.
The preset then contains four divisions, all of them covering the whole keyboard.
The layers are stacked by giving each division of the preset a different :guilabel:`Velocity range` in the [preset editor](manual/soundfont-editor/editing-pages/preset-editor.md):

* 0-39 for the layer *pp*,
* 40-69 for the layer *mp*,
* 70-99 for the layer *mf*,
* 100-127 for the layer *ff*.

The ranges must cover all velocities from 0 to 127, without gaps (some velocities would remain silent) or overlaps (two layers would be heard at the same time).
The boundaries don't have to be regular: they can be moved according to the recordings.
The values of the :guilabel:`Attenuation (dB)` row are explained in the next section.


![Velocity layers in the preset table](images/velocity_layers_table.png "Velocity layers in the preset table")


In the [range editor](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range), each layer is a horizontal band: the selected division (here *mp*) is hatched.
This view quickly shows a gap or an overlap.


![Velocity layers in the range editor](images/velocity_layers_ranges.png "Velocity layers in the range editor")


**Note:** the velocity ranges can also be defined in a single instrument containing all the samples, each division having its own velocity range.
In this case, the choice :guilabel:`by velocity range` of the option :guilabel:`Sort divisions` in the [settings](manual/settings.md#doc_interface) keeps the divisions of a same layer together.


## Balancing the layers {#doc_balance}


Within a layer, the volume already changes with the velocity: this is done by another default modulator, defined by the sf2 format.
But at each boundary, the volume suddenly increases because the recordings of the upper layer are louder: about 5 to 7 dB in the example.
The goal is that only the timbre changes when going from one layer to the next one, the volume increasing smoothly.

Use the [virtual keyboard](manual/soundfont-editor/toolbar.md#doc_keyboard) to compare the two sides of a boundary: the velocity of a note depends on the vertical position of the click, and the arrows :kbd:`↑` and :kbd:`↓` play the last key again with a velocity increased or decreased by 1.
Then, for each boundary from the bottom to the top:

1. play a key with the last velocity of a layer (39 for instance) and then with the first velocity of the next layer (40),
2. increase the attenuation of the upper layer in the preset table until both notes have about the same volume,
3. go on with the next boundary: the attenuations add up, each layer being at least as attenuated as the layer below.

In the example, the attenuations are 0, 6, 11 and 16 dB, as shown in the table above.
Check several keys across the keyboard: if a register remains unbalanced, the attenuation of the divisions concerned can be refined in the instrument of the layer, for instance with the tool “[Key-based configuration](manual/soundfont-editor/tools/instrument-tools.md#doc_global)”.

The overall dynamic range of the instrument, from the softest to the loudest note, is then given by the default modulator.
If the soft notes are too quiet or not quiet enough, this modulator can be changed with the tool “[Override a default modulator](manual/soundfont-editor/tools/instrument-tools.md#doc_default_mod)”.

Finally, nothing beats a real piece of music for testing the result: play the soundfont with a MIDI keyboard or with the [MIDI player](manual/soundfont-editor/toolbar.md#doc_midi_player) (see the tutorial “[Read a MIDI file with a soundfont](tutorials/read-midi-file-with-soundfont.md)”).


## More details for more realism {#doc_details}


Velocity layers are a big step, but a real instrument has other characteristics that a soundfont can reproduce.
Here are two of them for the piano.


### Release noises


When a key is released, the damper falls back onto the strings and the mechanism makes a small noise.
Some sample libraries provide these noises: the one used in the example has one per key.

They are placed in a fifth instrument, “Piano release”, added to the preset over the whole velocity range:

* select the playback mode ![release](images/sample_mode_release.png "release"){.inline} (release) in the :guilabel:`Loop playback` row, so that a sample is played when the key is released,
* use a long :guilabel:`Vol env release (s)` (5 s in the example) so that the noise is played entirely,
* attenuate the instrument a lot (40 dB in the example): the noise must remain discreet.

Thanks to the default modulator linking the velocity to the volume, the noise is louder when the note was struck harder.

**Note:** the playback mode “release” is not documented by the sf2 format and may not be supported by other players.
The tutorial “[Using custom releases in an instrument](tutorials/using-custom-releases-in-an-instrument.md)” describes a method that works with any player.


### Strings without dampers


On a piano, the highest strings have no dampers (the last octave and a half, roughly): they keep ringing after the key is released.
In each layer instrument, the divisions of these keys (from F6, key 89, in the example) are thus given a longer :guilabel:`Vol env release (s)`: 3 s instead of 1 s.

Listen to short notes in the middle of the keyboard, then in the highest octave and finally a chord in the bass:

* <a href="files/velocity-layers-without-details.mp3" download>without these details</a>,

![](files/velocity-layers-without-details.mp3)

* <a href="files/velocity-layers-with-details.mp3" download>with the release noises and the strings without dampers</a>.

![](files/velocity-layers-with-details.mp3)


### Other ideas


Other improvements are worth trying:

* recording every key instead of one key out of three: on a piano each key has its own strings, and a transposed sample is never exactly the neighboring note,
* using stereo recordings, which give width to the sound,
* computing release times that change across the keyboard with the tool “[Natural release](manual/soundfont-editor/tools/instrument-tools.md#doc_release)”,
* adjusting the volume or the filter by register with the tool “[Key-based configuration](manual/soundfont-editor/tools/instrument-tools.md#doc_global)”.

And the same approach works for other instruments: a guitar string plucked softly or hard, a snare drum between ghost notes and accents, a trumpet going from a round to a brassy sound…
Each detail taken from the real instrument makes the soundfont more alive.
