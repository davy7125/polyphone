Polyphone can deal with several soundfont formats:

* [sf2](#doc_sf2),
* [sf3](#doc_sf3),
* [sfz](#doc_sfz),
* [sfArk](#doc_sfark).


## Sf2 soundfonts {#doc_sf2}


Sf2 soundfonts are the main files edited by Polyphone.
Each sf2 file comprises one or more musical virtual instruments, made of audio samples and a lot of parameters.
Parameters define how the samples should be played throughout the keyboard, possibly modulated by predefined signals ([modulators][i-editor-mod]).
Sf2 files are built according to a 3-level structure:

* [samples][s-editor], coming from a trumpet or a piano for instance,
* [instruments][i-editor], made of samples,
* [presets][p-editor], made of instruments.

Further to the definition and setting of these elements, a sf2 file contains also [general information][edit-general] (the author, copyright or edit time for instance).

Soundfonts are used by software synthesizers, such as [Fluidsynth], using wave tables and driven by MIDI signals.
It is also possible to use sf2 soundfonts to listen MIDI files with [TiMidity], [WildMIDI] or QuickTime and listen scores with [MuseScore].
Hardware such as samplers, soundcards or even synthesizers may also support the .sf2 format.

Further information on soundfonts may be found for example on [Wikipedia][SoundFont].
The sf2 specifications are available for [version 2.01][sf-201] and for [version 2.04][sf-204].


## Sf3 soundfonts {#doc_sf3}


The sf3 format, developed by [MuseScore], is similar in all respects to the sf2 format except that the [samples][s-editor] are stored in the OGG format (like the MP3 format but open source) instead of being stored as raw data.
The consequence is that the sf3 format is about 10 times lighter than the sf2 format for a comparable quality.

A soundfont exported in this format is not intended to be edited later because successive compressions would result in a lower sound quality.
This format should be seen as a final product and is very interesting for all end users of soundfonts in that:

* downloads are made easier on internet,
* space is saved in computers,
* the combination midi + sf3 provides a lightweight and powerful solution to play soundtracks (video games, mobile applications, …).

Unlike the [sf2pack] format which answers the same issues, the sf3 format is entirely open-source.
The source code is available, thus ensuring its sustainability (let's avoid the mistakes done with sfArk or sfPack!).


## Sfz soundfonts {#doc_sfz}


The sfz format has the same goal than the sf2 format: create a musical instrument by disposing and configuring samples over the keyboard.
The main difference is that while the sf2 format is only 1 file that contains everything, the sfz format is a text file delivered with a set of .wav samples.
Since it was meant to be editable by a human, the main advantage was to edit the file without the need of a complex editor.
But an editor is still highly recommended for big instruments, the quantity of parameters can indeed be quickly discouraging.
The sfz format is also not defined as strictly as the sf2 format: differences may appear in the way to edit and in the way to play an sfz instrument.


### Limitations on the sfz import


* Only opcodes having a counterpart in :ref:`tables <instrument editor table>` are imported.
* The envelope applied to pitch and filter being the same in sf2 files (Mod env), importing different envelopes for the pitch and filter is not possible (``fileg_*`` and ``pitcheg_*`` opcodes).
* The LFO applied to volume and filter being the same in sf2 files (Mod LFO), importing different LFOs for the volume and filter is not possible (``amplfo_*`` and ``fillfo_*`` opcodes).
* The only filter that can be imported is the second order low pass filter (``fil_type=lpf_2p``).
* To be imported correctly, ``group`` and ``off_by`` opcodes must contain the same value within the same group.

<!-- warning begin -->
**Warning:** ``width`` and ``position`` opcodes may not be interpreted correctly.
<!-- warning end -->


### Limitations on the sfz export


* [Modulators][i-editor-mod] are currently not exported.
* The sfz format (version 1) allowing only one pitch modulation, the two parameters :guilabel:`Vib LFO → pitch (c)` and :guilabel:`Mod LFO → pitch (c)` cannot be exported simultaneously.
* A loop 0-1 can be exported but is ignored by most of the sfz players.
* 24-bit may not be supported in some synthesizers, please refer to their specification manual.


### In general


The matching of sound levels (in dB), during the sfz / sf2 conversion process, has been tested with Sforzando software.
There may however remain some differences.

The way parameters change according to the key (via :guilabel:`key → Vol env hold / decay` and :guilabel:`key → Mod env hold / decay`) have no exact match.
The sf2 format uses an exponential law to define the decay and hold durations according to the key, while the sfz format uses a linear law.


## SfArk archives {#doc_sfark}


SfArk archives, like the sf3 format, is a compressed format.
This means that the quality of the sounds is a bit lowered (but often imperceptible).
This format tends to be obsolete, that's why Polyphone only allows importing sfArk archives (not the export).



*[MIDI]: Musical Instrument Digital Interface


[Fluidsynth]: http://www.fluidsynth.org/                  {target=_blank rel=noopener}
[MuseScore]:  https://musescore.org/                      {target=_blank rel=noopener}
[sf2pack]:    http://www.fmjsoft.com/fmt/sf2pack.htm      {target=_blank rel=noopener}
[SoundFont]:  https://en.wikipedia.org/wiki/SoundFont     {target=_blank rel=noopener}
[TiMidity]:   https://sourceforge.net/projects/timidity   {target=_blank rel=noopener}
[WildMIDI]:   https://www.mindwerks.net/projects/wildmidi {target=_blank rel=noopener}

[edit-general]: manual/soundfont-editor/editing-pages/editing-of-the-general-information.md
[i-editor-mod]: manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator
[i-editor]:     manual/soundfont-editor/editing-pages/instrument-editor.md
[p-editor]:     manual/soundfont-editor/editing-pages/preset-editor.md
[s-editor]:     manual/soundfont-editor/editing-pages/sample-editor.md

[sf-201]: downloads/sf_specifications_v2.01.pdf {.icon-file-pdf}
[sf-204]: downloads/sf_specifications_v2.04.pdf {.icon-file-pdf}
