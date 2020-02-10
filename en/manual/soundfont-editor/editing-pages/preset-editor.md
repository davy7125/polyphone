When clicking on one or more preset(s) in the [tree](manual/soundfont-editor/tree.md), the editing page of presets appears.
A preset is a combination of one or more instruments and is the element visible from outside a [soundfont](manual/annexes/the-different-soundfont-formats.md#doc_sf2).

Similarly to the [instrument editor](manual/soundfont-editor/editing-pages/instrument-editor.md), several parts are present:

* the table,
* the range editor when the button :guilabel:`Edit ranges` of the [toolbar](manual/soundfont-editor/toolbar.md#doc_view) is clicked,
* the modulator section.


![Preset page](images/edit_preset.png "Preset page")


If only one preset is selected, the page allows the editing of the global parameters of the preset and the parameters of all its divisions.
In the case where several presets are selected, the global parameters of each of these presets can be edited.


## Differences with the instruments {#doc_diff}


The parameters edited here are said relative unlike parameters of an instrument which, except in special cases, are said absolute.
Values of a preset parameters are added or multiplied to the instrument parameters that compose it.
Some parameters are not editable at the preset level:

* loop playback,
* root key,
* exclusive class,
* fixed key,
* fixed velocity,
* the four offsets.

A preset is identified by a preset number and bank number, this combination being unique and adjustable below the table.
Each bank comprises 127 presets.

The last bank (128) is usually used for percussion kits.