When clicking on one or more instrument(s) in the [tree](manual/soundfont-editor/tree.md), the editing page of instruments appears.

This page comprises several parts:

* the [table](#doc_table),
* the [range editor](#doc_range) when the button :guilabel:`Edit ranges` from the [toolbar](manual/soundfont-editor/toolbar.md#doc_view) is clicked,
* the [envelope editor](#doc_envelope) when the button :guilabel:`Edit envelope` from the [toolbar](manual/soundfont-editor/toolbar.md#doc_view) is clicked,
* the [timbre view](#doc_timbre) when the button :guilabel:`Timbre` from the [toolbar](manual/soundfont-editor/toolbar.md#doc_view) is clicked,
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


### Moving the points


Each point of the curve can be moved with the left button:

* the ends of **delay**, **attack**, **hold** and **release** move horizontally;
* the end of **decay** moves in both directions: horizontally for the **decay**, vertically for the **sustain**;
* the point at which the key is released moves vertically, for the **sustain**.

The parameters of the point under the mouse cursor and their values are displayed in a top corner of the graph.
When several points are at the same place, the circle is divided into parts, from the first stage on the left to the last one on the right.

The **Key → Hold** and **Key → Decay** parameters are edited with a small ruler crossing the end of **hold** or **decay**: its top end shows the duration for the highest key of the division, its bottom end the duration for the lowest key.
Moving one end changes the duration of its key only, the other end staying in place; with the key :kbd:`Ctrl` held down, both ends move around the middle of the ruler.
The ruler appears when the mouse gets close to the point and remains visible when the parameter is not 0.

The key :kbd:`Shift` gives a finer adjustment.
Pushing a point against the right side of the graph scrolls the view or zooms out, to reach longer durations.
The key :kbd:`Esc` cancels a change in progress.
A double-click on a point resets its parameters (only **Key → Hold** or **Key → Decay** for an end of the ruler): the value of the :guilabel:`Global` division or the default value applies again.

If several divisions or instruments are selected, all their envelopes are changed together: durations in the same proportion, the other values by the same amount.


### Drag & zoom


Maintaining the right click allows you to change the zoom around the selected point.

Maintaining the left click allows you to move the graphic if the click does not start on a point of the curve.


## Timbre view {#doc_timbre}


The timbre view shows, all along the keyboard, the timbre of an instrument **as it is heard**: the level of each harmonic, once each sample is transposed to the keys it plays and once the attenuation, the filter and the modulators of its division are applied.
Irregularities become visible, in particular the breaks at the junctions between samples.
From this view, computed notes can be created, redundant samples removed, and the timbre or the level of a sample corrected.
The view measures and proposes, nothing is applied automatically.


![Timbre view](images/edit_timbre.png "Timbre view")


### Divisions taken into account


The view shows what is played at the velocity chosen at the top left (100 by default, then the last one used).
The bar next to it shows the velocity layers of the instrument: a click on a layer selects the velocity in its middle.

The divisions taken into account are those playing at this velocity, that are not [muted](#doc_solo) and that are not played only when the key is released (sample mode “release”).
Muting a division is thus the way to exclude it from the view, for instance a noise or a release sound.
When the [solo on selection](manual/soundfont-editor/tree.md#doc_menu) is enabled, the mutes follow the selection and are not taken into account.
The divisions playing the same key (stereo pair, layers) are combined: their powers are added.

A **root key** gathers all the divisions having this root key, possibly with several key ranges.
The samples are analyzed in the background (the number of samples remaining is shown at the top right): the harmonics are measured on the loop, or on the steady part of the sound if there is no loop.
These analyses are kept until the data, the loop, the root key or the pitch correction of a sample changes.


### Map


Each column is a key, aligned on the keyboard displayed below, and each row a harmonic rank (H1: fundamental, H2: second harmonic...), the rank 1 being at the bottom as in a spectrogram.
The first ranks are displayed one by one, the next ones being grouped in bands (see [Partials](#doc_timbre_partials)).

Above the map, summary rows show for each key:

* :guilabel:`Level`: total level,
* :guilabel:`Timbre`: timbre deviation of the root key from its reference (hatched when the reference is less reliable, at the ends of the keyboard),
* :guilabel:`Transposition`: blue when the sample is transposed down, red when it is transposed up,
* and, chosen in the menu :guilabel:`Options` > :guilabel:`Summaries`: :guilabel:`Brightness` (center of gravity of the ranks), :guilabel:`Odd / even` (odd ranks compared to even ranks), :guilabel:`Noise` (harmonics to noise ratio), :guilabel:`Tuning` (cents from the closest note of the equal temperament) and :guilabel:`Loop` (quality of the loop).

On the map, vertical lines mark the junctions between divisions (thicker when the sample changes), a dot marks each root key, hatching shows the ranks that are absent (outside the band of the sample or of the output, above the upper limit) or not reliably measured, and the keys that play nothing are grayed.
A tooltip gives the values of the cell under the mouse cursor: key, rank, level, deviation, samples, transposition and duration factor (the attack, the tremolo and the beats are faster when a sample is transposed up).

The list :guilabel:`Display` selects what the colors represent:

Absolute levels
: Level of each rank in dB.

Relative levels
: Level of each rank relative to the strongest one of the key: the timbre regardless of the level.

Deviation from the reference
: Difference between the root key and its reference for each rank, over all the keys played by the root key: too strong (red) or too weak (blue).

Jumps
: Difference with the previous key: the breaks at the junctions.

Transposition
: Semitones between the key and the root key of its sample.

Confidence
: Part of the power that is reliably measured.

The menu :guilabel:`Options` also allows displaying the whole keyboard or only one column per root key, coloring by steps of the thresholds (the deviations within the tolerance being neutral), and playing the clicked keys.
The keyboard below the map plays the instrument at the velocity of the view.


### Reference and decisions


The **reference** of a root key is what its neighbors predict: the neighboring root keys are played at its key (with the parameters of their divisions) and their levels are interpolated rank by rank, in dB.
At both ends of the keyboard, the reference is extrapolated from the two neighbors on the same side, which is less reliable.
With :guilabel:`Local regression`, a robust regression on several neighbors on each side is used instead, so that a defective neighbor weighs less.

The deviation of a root key from its reference is summed up by two numbers:

* the **level** deviation, difference of the total levels, which can be corrected by the attenuation,
* the **timbre** deviation, mean of the differences of the rows once the level deviation is removed, each row weighing according to its loudness (a weak harmonic counts, without dominating).

The strip :guilabel:`Decisions` gives a label to each root key and to each junction, with a shape and a color:

Consistent ✓ (green)
: The timbre deviation is within the tolerance.

Redundant − (blue)
: The root key is consistent, and removing it would change its keys by less than the threshold :guilabel:`Redundancy`, without a break at the new junction: it can be removed.

To correct ~ (orange)
: The timbre deviation is between the tolerance and the maximum: the timbre can be corrected.

Too different × (red)
: The timbre deviation is beyond the maximum: the root key can be removed or replaced with a computed note.

Break + (purple)
: At a change of sample, the timbre jump is beyond the threshold :guilabel:`Break` and a key without sample exists between both root keys: a computed note can be created there.

Level ▲ ▼ (yellow)
: The level deviation is beyond the threshold :guilabel:`Level` (▲ too loud, ▼ too soft): the attenuation can be corrected.

The thresholds are set in the menu :guilabel:`Thresholds`.
Their default values (:guilabel:`Tolerance` 4.5 dB, :guilabel:`Maximum` 8 dB, :guilabel:`Redundancy` 2.5 dB, :guilabel:`Break` 6 dB, :guilabel:`Level` 2.5 dB) come from organ sample sets, where 70% of the root keys deviate by less than 4.5 dB from their neighbors: they can be adapted to the instrument.
A junction or a deviation can be wanted (an instrument becoming another one, a mixture breaking back): the labels are only proposals.


### Partials {#doc_timbre_partials}


The menu :guilabel:`Partials` sets the rows of the map, shared by the display, the deviations and the corrections:

Ranks displayed one by one
: Number of ranks displayed and compared one by one (10 by default). The ear distinguishes the first harmonics individually, the next ones being perceived together as brightness.

Next ranks
: The next ranks are grouped in bands of a third of an octave (11-12, 13-16, 17-20...) or of an octave, so that many weak high ranks don't weigh more than the first ones.

Upper limit
: The partials above this frequency are ignored (16 kHz by default, or the half of the sample rate).

Dynamic range
: The ranks further below the strongest one of the key are not compared (60 dB by default).


### Details and actions


A click on a key shows below the map what it plays: the samples with their transposition, attenuation and filter, the duration factor, the pitch, the deviations of the root key and what its removal would change.
The bar chart compares the levels heard with the reference, or with the computed note that a key without sample could receive (a dashed column on the map).

A click on a root key selects its divisions in the [tree](manual/soundfont-editor/tree.md), and selecting divisions in the tree selects their root keys.
Several root keys can be selected with :kbd:`Ctrl` or :kbd:`Shift`, or by dragging the mouse over keys.

The buttons on the right and the contextual menu of the map apply to the selected root keys:

Create a note here...
: On a key without sample: the closest root key is transposed and the levels of its harmonics are interpolated (or extrapolated) from a second root key. The root keys used can be chosen, and a warning is shown when the key is far from them. The key ranges around are adjusted; the divisions entirely within the range of the new note are removed.

Remove...
: The divisions of the root key are removed and its neighbors take its keys. The samples that are not used anymore can be removed too.

Replace with a computed note...
: The root key is removed, then created again from its neighbors.

Correct the timbre...
: Opens the [correction](#doc_timbre_correction) window.

Correct the level
: The attenuation of the divisions is changed so that the level deviation becomes zero (an attenuation cannot be negative).

Check by rendering
: The key is played by the sound engine and the harmonics of the result are measured: they are shown on the bar chart for checking the computation of the view.

Mute these divisions
: In the contextual menu: excludes the divisions from the view.

The menu :guilabel:`Propose` selects all root keys (or junctions) having a label and applies the corresponding action to them, after a confirmation: correct the root keys to correct, replace those too different, remove the redundant ones, create computed notes at the breaks, correct the deviating levels.
They are processed from the worst to the least bad, the references being computed again after each action: a defective root key also distorts the reference of its neighbors, which look half defective until it is corrected.

Each action, or series of actions, can be undone in one step (:kbd:`Ctrl`+:kbd:`Z`).


### Correction of the timbre {#doc_timbre_correction}


![Correction of the timbre](images/timbre_correction.png "Correction of the timbre")


The window shows the levels heard, the target and the expected result for each row, and offers the following parameters:

Reference
: Target of the correction: :guilabel:`Interpolation between the neighbors`, :guilabel:`Local regression` on several neighbors on each side, or :guilabel:`Aligned on a root key` (another root key played at the key).

Correct the timbre (sample data)
: The gain of each row is applied to the data of the samples with a minimum phase curve following the pitch: the correction stays valid on all the keys of the division, whatever the transposition, and the loop is kept. The same correction is applied to both samples of a stereo pair.

Correct the level (attenuation)
: The level deviation is corrected through the attenuation of the divisions, which doesn't change the data. If the attenuation would become negative, the rest goes in the data, within the full scale.

Tolerance
: A smaller deviation of a row is not corrected, only the part beyond is.

Strength
: Part of the deviation corrected.

Maximum correction
: Largest gain of a row, on each side.

Ranks corrected
: All rows, or only the ranks from 1 to a given rank.

Brightness, Odd ranks
: Slope added to the target (positive for a brighter sound), and level added to the odd ranks 3, 5, 7... of the target.

The section :guilabel:`Listening` compares the original sound (A) and the corrected one (B) with the button :guilabel:`Play`.
If the correction would exceed the full scale, the sample is lowered and the attenuation compensates.
When a sample is also used by other divisions or instruments, Polyphone asks whether to duplicate it for the corrected root keys or to modify it everywhere (the answer can be remembered until Polyphone is closed).


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
