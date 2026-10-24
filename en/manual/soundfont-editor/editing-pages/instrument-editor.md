When clicking on one or more instrument(s) in the [tree](manual/soundfont-editor/tree.md), the editing page of instruments appears.

This page comprises several parts:

* the [table](#doc_table),
* the [range editor](#doc_range) when the button :guilabel:`Edit ranges` from the [toolbar](manual/soundfont-editor/toolbar.md#doc_view) is clicked,
* the [envelope editor](#doc_envelope) when the button :guilabel:`Edit envelope` from the [toolbar](manual/soundfont-editor/toolbar.md#doc_view) is clicked,
* the [modulator section](#doc_modulator).


![Instrument page](images/edit_instrument.png "Instrument page")


If only one instrument is selected, the page allows the editing of the global parameters of the instrument and the parameters of all its divisions.

In the case where several instruments are selected, the global parameters of each of these instruments can be edited.


![Instrument page with multiple instruments selected](images/edit_multiple_instruments.png "Instrument page with multiple instruments selected")


## Table {#doc_table}


### Presentation


The table allows a global view and the editing of an instrument configuration.
A change of a parameter in the :guilabel:`Global` division applies to all divisions, except for divisions whose same parameter is already specified (local override).

The rows written in italic (:guilabel:`Sample root key`, :guilabel:`Sample length` and :guilabel:`Loop`) cannot be edited: they only remind the properties of the sample linked to each division, for information.

The editable parameters are:

Key range
: Key range for which the sample will be heard.

Velocity range
: Velocity range for which the sample will be heard.
  It is possible to set different sounds for the same note, with different velocity ranges, to distinguish different nuances of a piano key for example.

Attenuation (dB)
: Attenuation in decibels applied to the sample.\
  Note: the sf2 format doesn't allow this value to be negative at the instrument level, no amplification is thus possible.

Pan
: Balance applied to the sound, to hear it to the left or to the right in stereo.

Loop playback
: ![Loop off](images/loop_off.png "loop off"){.inline} indicates a sound reproduced with no loop.
  ![Loop on](images/loop_on.png "loop on"){.inline} indicates a sound which loops continuously.
  ![Release](images/sample_mode_release.png "release"){.inline} is for triggering a sample playback when a key is released. The volume envelope is still used for defining the volume and a short attack (0.2 ms) is added for smoothing the start of the sample on the release.
  ![Loop on + release](images/loop_on_end.png "loop on and release"){.inline} indicates a sound which loops for the duration of key depression and then proceeds to play the remainder of the sample.\
  Note: the playback mode “release” is NOT documented by the sf2 format, so other players such as FluidSynth may decide not to implement it.
  The other way to achieve this behavior is to double each sample with a sample containing a short silent loop followed by the sound to emit on the release.
  The added sample will be read with the playback parameter “loop on + release”.
  See the tutorial “[Using custom releases in an instrument](tutorials/using-custom-releases-in-an-instrument.md)”.

Root key
: Allows you to change the normal root key of a sample to a different note from that usually used.
  For example, if a sample has a root key of 50 and is played over the key range [49 — 51], it will be initially transposed by -1, 0, and +1 semitone(s) on keys 49, 50, and 51 respectively.
  If the root key is then overridden to 51 at the instrument level, the same sample will be initially transposed by -2, -1, and 0 semitone(s) on keys 49, 50, and 51 respectively.

Tuning (semitones and cents)
: Adjusts the tuning in semitones and hundredths of a semitone.
  This transposition is added on top of the initial transposition related to the triggered note (for example, the initial transposition of a sample with a root key of 60 will be +2 semitones if the key pressed on the keyboard is 62).

Tuning (scale)
: This parameter modifies the interval between two consecutive notes and will therefore affect the initial transposition linked to the triggered note.
  A value of 100 represents the equal temperament scale: 100 hundredths of a semitone are used to transpose a sample from one key to the next.
  A value of 0 disables this initial transposition, meaning the sample will be played identically across all keys on the keyboard.

Filter
: Apply a low-pass filter, by specifying the cutoff frequency and the resonance.

Volume envelope
: Change the volume envelope, and possibly how it changes with the note.

Modulation
: Set a modulation (envelope, delay, frequency, changes with the note) and its effect on the tone, filter and volume.

Vibrato
: Set a vibrato (delay, frequency) and its effect on the tone.

Exclusive class
: This parameter can define one or more exclusive classes, by assigning to a set of divisions within a class the same parameter value other than 0.
  When an exclusive class is defined, any note triggered from one of the divisions of the exclusive class ends all the other sounds of the same class.
  The scope of an exclusive class is the preset in which the class is defined (a sound triggered in an instrument will end a sound of another instrument if the two instruments are in the same preset).
  This effect can be used for percussion instruments like a cymbal.
  Basically playing one note automatically terminates another in the same group.

Chorus and reverb
: Indicates the intensity of the chorus and reverb effects.
  The effects are configurable in the software [settings](manual/settings.md#doc_sound).

Fixed key and velocity
: Force the value of the key and of the velocity on the full extent of the division.

Offsets
: Changes the positions of the start and end of a sound during playback, as well as the start and end of the loop.
  The unit of an offset is samples (number of values).


### Editing facilities in the table


To facilitate and speed up editing in the table, the following features have been implemented:

* the mouse wheel may be used to change a value (the cell has to be in edit mode first),
* an external MIDI keyboard (or the [virtual keyboard](manual/soundfont-editor/toolbar.md#doc_keyboard)) can be used to set the root key, key range and velocity range (the cell has to be in edit mode first),
* a multiple selection, followed by the editing of the last cell, will edit all cells simultaneously,
* the keys :kbd:`Del` and :kbd:`Backspace` clear the content of all selected cells,
* stereo divisions may be edited simultaneously if the option is enabled in the software [preferences](manual/settings.md#doc_general),
* key names may be numeric or as a text (having C3, C4 or C5 as middle C), depending on the option chosen in the [preferences](manual/settings.md#doc_interface),
* copy / paste / cut of a set of cells (not necessarily adjacent) may be done via :kbd:`Ctrl`+:kbd:`C` / :kbd:`Ctrl`+:kbd:`V` / :kbd:`Ctrl`+:kbd:`X`.

Do not forget the [tools](manual/soundfont-editor/tools/instrument-tools.md) available!
In particular the tool allowing a [key-based configuration](manual/soundfont-editor/tools/instrument-tools.md#doc_global).


### Possibility to mute divisions {#doc_solo}


For testing purposes, it is possible to mute a division or to mute all divisions but one in the tables.
The options are accessible with a right click on the header of a division, and also in the contextual menu of the [tree](manual/soundfont-editor/tree.md#doc_menu).


![Mute divisions in a table](images/table_mute_divisions.png "Mute divisions in a table")


## Range editor {#doc_range}


The range editor allows you to quickly and graphically arrange samples according to their key and velocity ranges.
This graph further highlights configuration problems, such as zones free of samples or unwanted superpositions.


![Range editor](images/edit_range.png "Range editor")


Each sample is represented by a semitransparent rectangle, so that the superposition of several elements is identifiable.


### Rectangle selection


A simple left click on a rectangle will select it and remove all other selections.
If several rectangles are superimposed, consecutive clicks change the selection under the mouse cursor.
A double-click on a rectangle opens the corresponding [sample](manual/soundfont-editor/editing-pages/sample-editor.md) (or the corresponding instrument in the [preset editor](manual/soundfont-editor/editing-pages/preset-editor.md)).

A multiple selection is possible:

* by maintaining the key :kbd:`Ctrl` and then clicking on the different rectangles to select them one by one,
* by using the key :kbd:`Shift`: all rectangles between the first rectangle being clicked and the next one will be selected.


### Updating the rectangles


After having selected one or more rectangle(s), it is possible with the left button to:

* change the size, when you grab one of the edges of a rectangle;
* change the position, by pointing to the center of a rectangle.

If several rectangles are selected, all changes are made synchronously.
The key :kbd:`Esc` cancels a change in progress.


### Drag & zoom


Maintaining the right click allows you to change the zoom around the selected point.

Maintaining the left click allows you to move the graphic if the initial point is not on a rectangle.

A click on the middle button plays a note: the key is given by the horizontal position of the mouse and the velocity by its vertical position.


## Envelope editor {#doc_envelope}


The envelope editor provides a graphical overview of the 8 parameters defining the volume envelope and the 8 parameters defining the modulation envelope.
This is thus a tool for better understanding the impact of each of these parameters.


![Envelope editor](images/edit_envelope.png "Envelope editor")


The buttons above the graph select alternatively the volume envelope (displayed in green) or the modulation envelope (displayed in red) for the editing.
During the editing of the modulation envelope, the volume envelope is displayed with a thin line.

The left part of the graph represents the evolution of the envelope when a key is **pressed**.
7 out of the 8 parameters are involved in this representation:

Delay
: Duration during which the envelope remains at 0 just after a key is pressed. In the majority of cases we want to hear audio immediately after pressing the keys and then this parameter is left at zero. However **delay** comes in handy for example when we want to create richer pad sounds constructed with several layers. Suppose we want to introduce a second sound layer 0.5 seconds after the key is pressed, then we can set it by entering “0.5” in this field. A delay does not hold the playback of the sound — it mutes the audio. If we have set the delay to 0.5 seconds, we will start hearing the sample as it is after 0.5 seconds of playback, the first 0.5 seconds having been played silently.

Attack
: Duration after **delay** during which the envelope grows from 0 to the maximum value. For example a bowed string sound like a violin has an attack which is quite long, while a plucked string sound such as a guitar is very short. Even when we want to use an attack as short as possible, it is advisable to enter at least a value of 0.001, otherwise a “click” might be heard after pressing a key.

Hold
: Duration after **attack** during which the envelope keeps its maximum value.

Decay
: Duration after **hold** during which the envelope progressively goes from the maximum value to the value specified by **sustain**. When no reduction is specified in **sustain** (0 dB, the default value), the volume during the **decay** will stay at its maximum no matter which value is specified.

Sustain
: Value after **decay** sustained as long as the pressed key is not released. Just like the **attenuation**, the sustain value is a reduction expressed in decibels. In the case where we want to reduce the volume by half after the decay time, a value of 6 dB must be set. The number “144” can be specified if we want to completely mute the sound.

Key → Hold
: Change of the **hold** duration depending on the key (see below).

Key → Decay
: Change of the **decay** duration depending on the key (see below).

A positive value in **Key → XXX** gradually decreases the duration of XXX toward the trebles, whereas a negative value does the opposite.
For instance:

* value 100 halves the duration of XXX per octave above middle C (60) and doubles the duration of XXX per octave below middle C,
* value -100 doubles the duration of XXX per octave above middle C (60) and halves the duration of XXX per octave below middle C.

These parameters can be useful when designing a guitar or a piano with the sustain-pedal on: the lower the tone the longer it lasts until we hear no sound at all.

The right part represents the evolution of the envelope when a key is **released**.
Only the **release** duration is involved in this representation.
The envelope gradually returns to 0 during this duration.
Sounds of violins or synth-pads have a relatively long release time, while that of other sounds such as organs are very short. In practice, even for short release times, it is advisable to set a duration of at least 0.2 seconds, because a release time which is too short doesn't sound pleasant or natural (however it also depends on what we have in mind for the instrument we want to create).

When only one division of an instrument is selected in the tree, the corresponding sound [sample](manual/soundfont-editor/editing-pages/sample-editor.md) is displayed on the background.
It is thus possible to better visualize the evolution of the envelope relative to the sample being played.


## Modulators {#doc_modulator}


The lower section of the editing page of instruments is intended for the creation of modulators, globally for an instrument or for a particular division.
When you play an instrument, a modulator listens to one or two MIDI signal(s) and modifies one of the parameters in the table according to the mathematical rule configured.
The output of a modulator can also be used as input to another modulator (this function appeared in version 2.04 of the [sf2 format](manual/annexes/the-different-soundfont-formats.md#doc_sf2) and may not be supported by some synthesizers).


![Modulator editor](images/modulator_editor_2.png "Modulator editor")


Buttons on the left are made for:

* expanding / collapsing the modulator area,
* adding / removing a modulator,
* copying / pasting / duplicating one or more modulators.

When a modulator is added, the border of the corresponding cell in the table is thicker (see the tuning in the :guilabel:`Global` column above).

The modulators of one division (or of the global division) are displayed only if a single division is selected.
When the section is collapsed, a summary lists the number of modulators and the parameters they modify.
Selecting cells in the table selects the modulators acting on the corresponding parameters, and conversely.

Each modulator is made of the following elements, from left to right:

Sources
: The first source is a MIDI signal: the velocity or the key of the played note, the poly pressure, the channel pressure, the pitch wheel, the sensitivity of the pitch wheel or a MIDI controller (:guilabel:`CC`).
  It can also be the output of one or more other modulators (:guilabel:`Link`), those having this modulator as destination.
  The second source is optional and modifies the effect of the first one.

Curves
: Next to each source, a button gives access to a grid of curves for choosing how the signal is converted: linear, concave, convex or switch, ascending or descending, unipolar (from 0 to 1) or bipolar (from -1 to 1).

Amount
: Maximum amplitude of the modulation.

Transform
: :guilabel:`no operation` or :guilabel:`absolute value` (the absolute value of the result of the modulation is used).

Destination
: The parameter modified by the modulator, or another modulator (:guilabel:`Modulator`).
  The text under the destination indicates the range of the modulation: the values added to the parameter (:guilabel:`Add from` … :guilabel:`To`) or the factors applied to it (:guilabel:`Multiply from` … :guilabel:`To`).

When a modulator is identical to one that is located above in the list, the label :guilabel:`overwritten by` is displayed.
At the instrument level, the label :guilabel:`overriding default mod.` indicates a modulator that replaces one of the [default modulators](manual/soundfont-editor/tools/instrument-tools.md#doc_default_mod) of the sf2 format, and :guilabel:`disabling default mod.` indicates that its amount is 0 (the default modulator is then turned off).
