This page lists typical flaws found in soundfonts (which may be pointed out in reviews) and, for each of them, provides techniques and documentation links to fix them:

* [poor quality or badly prepared samples](#doc_sample),
* [incorrect tuning](#doc_tuning),
* [inappropriate ADSR envelope](#doc_adsr),
* [missing or inaccurate loops](#doc_loop),
* [MIDI standard not met](#doc_standard).


## Poor quality or badly prepared samples {#doc_sample}


It is possible that a soundfont provides no convincing instruments, and if the cause lies in poor-quality samples, the only solution is to replace them:

* search for other samples online,
* record with a higher-quality microphone (or use external signal processing tools),
* create samples by other means.

Sometimes, the issue can be resolved by better preparing or using the existing samples.

### Poor sound quality, especially in the high range

Typically, a single audio sample is not sufficient to create a high-quality instrument. A sample has its own pitch (except for non-melodic percussion), and using it over a wide note range will distort the sound, especially in the higher notes. To fix this issue, multiple samples should be used, or if only one sample is available, you can:

* duplicate the sample,
* transpose the duplicated sample by an octave, for example, using this [tool](manual/soundfont-editor/tools/sample-tools.md#doc_transpose),
* filter out high harmonics using this [tool](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq) or [this one](manual/soundfont-editor/tools/sample-tools.md#doc_filter).

The samples should then be mapped to note ranges where their playback does not alter the original timbre.

### The sound contains regular “clicks”

The problem likely comes from a poorly placed loop point, which is discussed in another [section](#doc_loop).

### Poorly trimmed samples

Samples are often only partially used, and removing the excess can reduce the file size:

* the useful part of a sample is sometimes located before the final loop point: it is therefore possible to delete the portion of the sample after the loop end, and this [tool](manual/soundfont-editor/tools/sample-tools.md#doc_trimloop) lets you do that in a few clicks;
* samples may begin with silence, which should be removed to reduce latency when triggering a note (there is also a [tool](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank) to do this quickly);
* finally, some samples may simply not be used: this [configuration summary](manual/soundfont-editor/configuration-summaries.md#doc_sample) lists them, and this [tool](manual/soundfont-editor/tools/global-tools.md#doc_unused) removes them automatically.


## Incorrect tuning {#doc_tuning}


Tuning a virtual instrument involves several steps, and it's important to go through all of them, in order.

### Tuning at the sample level

This is the most important tuning step and is described on this [page](tutorials/how-to-prepare-a-sample.md#doc_tuning).

### Tuning at the instrument level

4 parameters, described on this [page](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table), are used to define the transposition with which a sample is played: **root key**, **tuning (semitones)**, **tuning (cents)**, and **tuning scale**. These values can be modified during instrument playback by [modulators](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator), which should also be reviewed carefully.

If no parameters or modulators are defined for the instrument division linked to a sample, it's important to check whether any parameters or modulators are defined in the global division of the instrument (this division sets the default configuration for all other divisions).

### Tuning at the preset level

At the preset level, 3 parameters affect the transposition of a sample: **tuning (semitones)**, **tuning (cents)**, and **tuning (scale)**. These values are added to the configuration of the instruments used in the preset divisions.

Just like instruments, presets can include modulators, and their global division acts as a default configuration that should not be overlooked.


## Inappropriate ADSR envelope {#doc_adsr}


The ADSR envelope represents how the volume evolves when a note is played, but also when that same note is released. Each of the four characters in the term ADSR corresponds to a specific phase, and the volume envelope used in Soundfont actually includes 6 phases:

* delay,
* attack (A),
* hold,
* decay (D),
* sustain (S),
* release (R).

These phases are described in more detail on this [page](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope), and below are listed some possible issues related to the envelope.

### A sound starts too abruptly

To soften the start of a note, specify or increase the **attack duration** of the note at the [instrument](manual/soundfont-editor/editing-pages/instrument-editor.md) or [preset](manual/soundfont-editor/editing-pages/preset-editor.md) level.

### A sound starts too softly

Conversely to the previous section, decrease or remove the **attack duration** of the note.

### A delay is felt before the sound starts

Three possible and complementary solutions:

* remove the **delay duration** if it's specified at the [instrument](manual/soundfont-editor/editing-pages/instrument-editor.md) or [preset](manual/soundfont-editor/editing-pages/preset-editor.md) level,
* remove the silence at the beginning of the audio sample (see sample trimming on this [page](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph) or the silence removal tool [here](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)),
* finally, the perceived latency may be caused by the audio server used on your computer (in this case it's not a flaw in the soundfont itself, and you can change the audio output in the [settings](manual/settings.md#doc_general)).

### The sound stops suddenly

If the sound stops suddenly while it should continue as long as the note is held, see the [loop](#doc_loop) section.

If the sound stops abruptly when a note is released, specify or increase the **release duration** at the [instrument](manual/soundfont-editor/editing-pages/instrument-editor.md) or [preset](manual/soundfont-editor/editing-pages/preset-editor.md) level. This [tool](manual/soundfont-editor/tools/instrument-tools.md#doc_release) allows you to configure this duration so that it sounds natural across the entire pitch range, from low to high.


## Missing or inaccurate loops {#doc_loop}


Loops make it possible to produce a sound of infinite duration from a sample with a fixed length, without the ear noticing the repetitions. Loops are essential for certain types of instruments such as flutes, but not for others like percussion. Loops can also be used in piano samples to simulate a sustain pedal using a short sample.

### The sound stops before the note is released

If the sound stops when it should continue playing, the most likely causes are:

* the sample loop is not defined — in that case, you can refer to this [page](tutorials/how-to-prepare-a-sample.md#doc_loop) to set up a loop,
* loop playback is not enabled — in this case, you need to change the **playback mode** at the [instrument](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) level.

### The sound contains regular “clicks”

The loop is most likely incorrect, and this [page](tutorials/how-to-prepare-a-sample.md#doc_loop) explains how to create proper loops.


## MIDI standard not met {#doc_standard}


MIDI standards impose a specific numbering for the instruments contained in soundfonts. This allows different soundfonts that follow the same standard to be interchangeable, without causing significant differences in the timbres played: for example, a piano and a flute will be called in the same way. If a review indicates that a standard is not being followed, there are two possible solutions:

* either remove the information stating that the soundfont complies with a standard,
* or verify that all presets in the soundfont are correctly assigned to the appropriate kind of instrument (see this [page](manual/annexes/midi-standards.md) for the list of instruments) and that all instruments required by the standard are present.
