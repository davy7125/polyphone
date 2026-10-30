Polyphone supports several soundfont formats:

* [sf2](#doc_sf2),
* [sf3](#doc_sf3),
* [sfz](#doc_sfz),
* [dls](#doc_dls),
* [sfArk](#doc_sfark),
* [organ](#doc_organ).


## Sf2 soundfonts {#doc_sf2}


Sf2 soundfonts are the main files edited by Polyphone.
Each sf2 file comprises one or more musical virtual instruments, made of audio samples and a lot of parameters.
Parameters define how the samples should be played throughout the keyboard, possibly modulated by predefined signals ([modulators](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator)).
Sf2 files are built according to a 3-level structure:

* [samples](manual/soundfont-editor/editing-pages/sample-editor.md), coming from a trumpet or a piano for instance,
* [instruments](manual/soundfont-editor/editing-pages/instrument-editor.md), made of samples,
* [presets](manual/soundfont-editor/editing-pages/preset-editor.md), made of instruments.

Further to the definition and setting of these elements, a sf2 file contains also [general information](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md) (the author, copyright or edit time for instance).

Soundfonts are used by software synthesizers, such as <a href="http://www.fluidsynth.org/" target="_blank">FluidSynth</a>, driven by MIDI signals.
It is also possible to use sf2 soundfonts to listen to MIDI files with <a href="https://sourceforge.net/projects/timidity" target="_blank">TiMidity</a>, <a href="https://www.mindwerks.net/projects/wildmidi" target="_blank">WildMIDI</a> or QuickTime and to listen to scores with <a href="https://musescore.org/" target="_blank">MuseScore</a>.
Hardware such as samplers, soundcards or even synthesizers may also support the .sf2 format.

Further information on soundfonts may be found for example on <a href="https://en.wikipedia.org/wiki/SoundFont" target="_blank">Wikipedia</a>.
The sf2 specifications are available [here](files/sf_specifications_v2.01.pdf) for version 2.01 and [there](files/sf_specifications_v2.04.pdf) for version 2.04.


## Sf3 soundfonts {#doc_sf3}


The sf3 format, developed by <a href="https://musescore.org/" target="_blank">MuseScore</a>, is similar in all respects to the sf2 format except that the [samples](manual/soundfont-editor/editing-pages/sample-editor.md) can be stored in the OGG format (like the MP3 format but open-source) instead of being stored as raw data.
The consequence is that the sf3 format is about 10 times lighter than the sf2 format for a comparable quality.
In Polyphone, the compression is enabled in the [general information](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md#doc_edit) of the soundfont and each sample can then be compressed in the [sample editor](manual/soundfont-editor/editing-pages/sample-editor.md#doc_info) (the quality of the compression can be chosen).

The compressed samples in a soundfont exported in this format are not intended to be edited later because successive compressions would result in a lower sound quality.
This format should be seen as a final product and is very interesting for all end users of soundfonts in that:

* downloads are made easier on internet,
* space is saved in computers,
* the combination MIDI + sf3 provides a lightweight and powerful solution to play soundtracks (video games, mobile applications, …).

Unlike the <a href="http://www.fmjsoft.com/fmt/sf2pack.htm" target="_blank">sf2pack</a> format which answers the same issues, the sf3 format is entirely open-source.
The source code is available, thus ensuring its sustainability (let's avoid the mistakes made with sfArk or sfPack!).


## Sfz soundfonts {#doc_sfz}


The sfz format has the same goal as the sf2 format: create a musical instrument by arranging and configuring samples over the keyboard.
The main difference is that while the sf2 format is only 1 file that contains everything, the sfz format is a text file delivered with a set of .wav samples.
Since it was meant to be editable by a human, the main advantage was to edit the file without the need of a complex editor.
But an editor is still highly recommended for big instruments, the quantity of parameters can indeed be quickly discouraging.
The sfz format is also not defined as strictly as the sf2 format: differences may appear in the way to edit and in the way to play an sfz instrument. But at the same time, the capabilities of the sfz instruments have been increased compared to the sf2 standard.


### Limitations on the sfz import


* Only opcodes having a counterpart in [tables](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) are imported.
* The envelope applied to pitch and filter being the same in sf2 files (Mod env), importing different envelopes for the pitch and filter is not possible (``fileg_*`` and ``pitcheg_*`` opcodes).
* The LFO applied to volume and filter being the same in sf2 files (Mod LFO), importing different LFOs for the volume and filter is not possible (``amplfo_*`` and ``fillfo_*`` opcodes).
* The only filter that can be imported is the second order low pass filter (``fil_type=lpf_2p``).
* To be imported correctly, ``group`` and ``off_by`` opcodes must contain the same value within the same group.

**Warning:** ``width`` and ``position`` opcodes may not be interpreted correctly.


### Limitations on the sfz export


* [Modulators](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) are currently not exported.
* The sfz format (version 1) allowing only one pitch modulation, the two parameters :guilabel:`Vib LFO → pitch (c)` and :guilabel:`Mod LFO → pitch (c)` cannot be exported simultaneously.
* A loop 0-1 can be exported but is ignored by most of the sfz players.
* 24-bit samples may not be supported in some synthesizers, please refer to their specification manual.


### In general


The matching of sound levels (in dB), during the sfz / sf2 conversion process, has been tested with Sforzando software.
There may however remain some differences.

Moreover, the way parameters change according to the key (via :guilabel:`key → Vol env hold / decay` and :guilabel:`key → Mod env hold / decay`) have no exact match.
The sf2 format uses an exponential law to define the decay and hold durations according to the key, while the sfz format uses a linear law.


## DLS files {#doc_dls}


The DLS format (DownLoadable Sounds), published by the MIDI Manufacturers Association, has the same goal as the sf2 format: a single file contains samples and the parameters needed to play them as musical instruments.
It exists in two levels: DLS level 1 (1997) and DLS level 2 (the latest version being 2.2).
DLS files have been used by the software synthesizer of Windows (the file :file:`gm.dls`), by DirectMusic, by video games and by mobile phones.

The structure differs from the sf2 format: a DLS file contains waves (the samples) and instruments, each instrument having a bank and a program number like a preset.
An instrument is made of regions, each one playing a wave over a key and velocity range.
The parameters (envelopes, LFOs, filter, modulations, …) are written as connections between a source (for example a MIDI controller or a LFO), an optional control and a destination.
They can be defined for the whole instrument or for a single region, the parameters of a region then replacing those of the instrument.

Polyphone imports DLS files of both levels (.dls files only, DLS banks embedded in .rmi or .xmf files are not supported) and [exports](manual/menu.md#doc_save) soundfonts in the DLS level 2.2 format.
A DLS file imported is not modified when the soundfont is saved: the soundfont is saved in the sf2 format.


### Limitations on the DLS import


* Some parameters have no equivalent in the sf2 format and are ignored: the shutdown time of the volume envelope, the outputs other than the stereo pan (center, rear and LFE channels), the vibrato LFO sent to the volume or the filter, and the modulation envelope sent to the volume.
* A note played again stops the previous one in a DLS synthesizer, whereas both notes are heard in a sf2 synthesizer.
* Only the bank MSB (CC0) is kept, or the bank LSB (CC32) if the MSB is 0. If several instruments have the same bank and program number, the program number of one of them is changed.


### Limitations on the DLS export


* A DLS level 1 synthesizer plays the samples but ignores the envelopes, LFOs, filter and modulations.
* 24-bit samples are rounded to 16 bits.
* Not exported:
  the parameters :guilabel:`Fixed key` and :guilabel:`Fixed velocity`,
  the divisions using the [loop playback](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) "release",
  the modulators with the transform "absolute value" or linked to other modulators,
  the modulators of a parameter that does not exist in the DLS format (sample offsets, key to envelope hold or decay, …),
  the modulators changing the depth of a LFO or of the modulation envelope with 2 sources.
* Two modulators with the same sources and destination but different curves are merged.
* Only 15 exclusive classes per preset can be exported.
* :guilabel:`Scale tuning` and the modulations from MIDI controllers other than CC1, CC7, CC10, CC11, CC91 and CC93 may be ignored by some DLS synthesizers.


### In general


The default modulations of the sf2 and DLS formats are different (velocity to filter cutoff, vibrato with the modulation wheel and the channel pressure, levels of reverb and chorus): modulators are added during the conversions so that the sound is the same.

The conversions have been tested by comparing the renderings of the original files and of the converted files, and with <a href="https://github.com/spessasus/spessasynth_core" target="_blank">SpessaSynth</a>, which reads and writes DLS files.
Since few synthesizers support the DLS level 2 format, the way some parameters are played may differ from one synthesizer to another.


## SfArk archives {#doc_sfark}


SfArk archives, like the sf3 format, are a compressed format.
This means that the quality of the sounds is a bit lowered (but often imperceptible).
This format tends to be obsolete, that's why Polyphone only allows importing sfArk archives (not the export).


## GrandOrgue dispositions {#doc_organ}


GrandOrgue is a program whose main purpose is to emulate instruments like pipe organs. An organ is defined by a series of wav files, each containing the sound of a pipe, and a file with the extension ".organ" which contains all the parameters needed to create an organ based on all wav files. Basically, all samples are associated with specific keys and are grouped by stops, which can be activated or deactivated when playing the organ. Polyphone is able to create soundfonts based on this mapping, but does not support the conversion of soundfonts to the GrandOrgue format.

### Import limitation

Transpositions are currently not supported.


*[MIDI]: Musical Instrument Digital Interface
