The page for editing the general information is displayed when the header :guilabel:`General` is selected in the [tree](manual/soundfont-editor/tree.md).
This page is made of three kinds of elements:

* [editable fields](#doc_edit),
* [general information](#doc_info) that is not editable,
* [statistics](#doc_stats) about the soundfont.

The button :guilabel:`Default modulators` of the [toolbar](manual/soundfont-editor/toolbar.md#doc_view) displays the [default modulators](#doc_default_mod) of the soundfont, and the button :guilabel:`General` displays this page again.


![Global information page](images/edit_general.png "Global information page")


## Editable fields {#doc_edit}


The editing page of the general information provides fields for changing the following attributes:

Title
: This is the internal name of the soundfont, which does not necessarily have to do with the file name.

Author
: Relates to the author name of the soundfont.

Date
: Creation date of the file (a calendar helps to choose it).

Product
: Product for which the soundfont is designed.

Copyright
: Mention to fill in if the file is subject to copyright.

Comments
: Information not included in any of the previous categories (the text can contain any UTF-8 character).

Bit depth
: Indicates whether the samples are stored with a 16-bit or 24-bit resolution (in which case the compression is not allowed).
  Please note that 24-bit samples may not be supported in some synthesizers.

Compression
: Enables the sample compression (in which case the bit depth is set to 16 bits). If this option is enabled, a compress button will be displayed in the [sample editor](manual/soundfont-editor/editing-pages/sample-editor.md).
  The compression is a feature provided by the [sf3 format](manual/annexes/the-different-soundfont-formats.md#doc_sf3) and might not be supported by all synthesizers.
  Enabling or disabling this option will determine the file format when saving, and Polyphone will change the file extension .sf2 / .sf3 if necessary.


## General information {#doc_info}


The editing page of the general information provides the following non-editable information:

Soundfont format
: Version of the [sf2 format](manual/annexes/the-different-soundfont-formats.md#doc_sf2) used by the file.

ROM name and version
: Name and version of a sample ROM if used.

Sound engine
: Name of a sound engine.

Editing software
: Editing software used for the soundfont creation.

These pieces of information are displayed in a tooltip when the mouse is over the information icon, at the end of the line showing the author and the date.

File name and size
: Address and name of the file, and its size on the disk, displayed at the bottom of the window (the size is the one of the file as it was last saved).


## Statistics {#doc_stats}


A count is made on:

* the number of [samples](manual/soundfont-editor/editing-pages/sample-editor.md), unused samples and duplicate samples,
* the number of [instruments](manual/soundfont-editor/editing-pages/instrument-editor.md) and unused instruments,
* the number of [presets](manual/soundfont-editor/editing-pages/preset-editor.md),
* the number of parameters and modulators edited for the instruments,
* the number of parameters and modulators edited for the presets.

The button :guilabel:`Details` next to the samples, the instruments and the presets displays the corresponding [configuration summary](manual/soundfont-editor/configuration-summaries.md).

Unused samples and instruments can be easily removed with the tool “[Remove unused elements](manual/soundfont-editor/tools/global-tools.md#doc_unused)”.
Duplicate samples can be removed with the tool “[Clean duplicates](manual/soundfont-editor/tools/global-tools.md#doc_duplicates)”.

**Note:** The number of 65536 parameters in the instruments and the presets should not be exceeded.
Some synthesizers may not be able to read them all.



## Default modulators {#doc_default_mod}


Default modulators are applied to every instrument of the soundfont, before the [modulators](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) of the instrument: a modulator of an instrument having the same sources, destination and transform replaces a default modulator (label :guilabel:`overriding default mod.`).

Without its own list, a soundfont uses the default modulators of the SoundFont format, displayed without being editable: velocity to attenuation and to filter cutoff, channel pressure and modulation wheel to vibrato, volume, pan, expression, reverb, chorus and pitch wheel.
The velocity to filter cutoff modulator depends on the [sound settings](manual/settings.md#doc_sound).

The button :guilabel:`Customize` gives the soundfont its own list, starting with these modulators, which is then edited like the modulators of an instrument (add, remove, copy, paste, change).
This list replaces the default modulators of the SoundFont format: a modulator removed from the list no longer applies to any instrument, a modulator added to the list applies to all of them.
The button :guilabel:`Reset to the SoundFont defaults` removes the list and restores the default modulators of the format.

The list is saved in the DMOD chunk of the sf2 and sf3 files, an [extension of the format](manual/annexes/the-different-soundfont-formats.md#doc_sf2) read by FluidSynth 2.5 and later, SpessaSynth and Polyphone 2.7 and later (the [plugin](manual/plugin.md) included).
Other synthesizers ignore it and apply the default modulators of the SoundFont format.

When an instrument is copied into another soundfont whose default modulators differ, global modulators are added to the instrument so that it sounds the same.
The [DLS export](manual/annexes/the-different-soundfont-formats.md#doc_dls) converts the default modulators of the soundfont; the sfz export only keeps their effect of the velocity on the volume and on the filter cutoff.