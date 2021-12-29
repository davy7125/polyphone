When an instrument or one of its divisions is selected in the [tree](manual/soundfont-editor/tree.md), the [tool menu](manual/soundfont-editor/tools/index.md) contains several tools sorted in 4 categories:

* [analyze](#doc_cat_analyze),
* [fast editing](#doc_cat_fast),
* [modulators](#doc_cat_mod),
* [transformation](#doc_cat_transformation).

Using the tools simultaneously on several instruments is possible (with a multi selection), except for creating chords, mixtures and for displaying parameters.


## Analyze {#doc_cat_analyze} 


### Display parameters {#doc_display}


This tool displays the evolution of a parameter according to the key.
A logarithmic scale is available (take care of values equal to or less than 0!).


![Display parameter tool](images/tool_display_parameters.png "Display parameter tool")


## Fast editing {#doc_cat_fast}


### Change attenuations {#doc_attenuation}


The tool computes the minimum and the maximum attenuations in all selected instruments, and gives you the possibility to add an offset to all attenuation values without exceeding the limits.

For example, if the lowest attenuation of the divisions within the selected instruments is 16 dB and if the desired minimal attenuation is 5 dB (to increase the general volume), the attenuation of all divisions of all instruments can be decreased by 11 dB.


![Change attenuation tool](images/tool_change_attenuation.png "Change attenuation tool")


### Detune {#doc_detune}


This feature slightly detunes the instrument notes, so as to create an undulating effect in conjunction with a tuned instrument.
The intensity of the effect is expressed in number of beats per second and is adjustable:

* number of beats per second at key 60, which is "C" in the middle of the keyboard;
* evolution of the number of beats according to the note, dividing or multiplying the number of beats by a constant factor from one octave to another.


![Detuning tool](images/tool_detune.png "Detuning tool")


Here is an example of an instrument that is <a href="downloads/tutorials/mixture_base.mp3" download>tuned</a>:

![](downloads/tutorials/celeste_tuning_off.mp3)

And this instrument has been combined with itself in a version that is <a href="downloads/tutorials/mixture_done.mp3" download>detuned</a>:

![](downloads/tutorials/celeste_tuning_on.mp3)

The corresponding .sf2 file can be downloaded <a href="downloads/tutorials/celeste_tuning.sf2" download>here</a>.


### Key-based configuration {#doc_global}


This tool allows the editing of a parameter for all divisions of an instrument simultaneously.


![Key-based configuration tool](images/tool_keybased_configuration.png "Key-based configuration tool")


The graph represents the modification intensity, the first value on the left corresponding to the change applied to the division comprising the key 0, and the last value on the right corresponding to the change applied to the division comprising the key 127.
By changing the pattern, it is possible to edit the curve:

* manually using the mouse,
* by using a linear law,
* by using an exponential law,
* by using a random generation.

In the case where a linear or exponential law is used, a left click in the graph defines the beginning of the linear or exponential area, a right click defines the end.

The parameter to be modified is chosen in :guilabel:`Parameter`, and the modification type is chosen in :guilabel:`Modification`:

Addition
: The modifier is added to the value of the selected parameter.

Multiplication
: The modifier is multiplied with the value of the selected parameter.

Replacement
: The modifier replaces the value of the selected parameter.

It is possible to apply the changes only on divisions whose velocity range is comprised in a specific range, useful for example in the case where a piano has several velocity layers.


### Natural release {#doc_release}


This tool creates a release for each division of the instrument.
Release times are computed taking into account the first two parameters and a pitch modification may be added by using the third one.


![Natural release tool](images/tool_natural_release.png "Natural release tool")


### Sample auto-positioning {#doc_position}


The software determines the key ranges of all divisions of an instrument, in order to optimize their use all over the keyboard.


### Sound spatialization {#doc_spatialization}


This feature spatializes sounds according to their pitch, which can be useful for a piano for example (bass and treble left to right).


![Sound spatialization tool](images/tool_sound_spatialization.png "Sound spatialization tool")


The bars in the graph represent each a string or pipe, having a length in relation to the pitch.
The shortest bar corresponds to the highest note and the longest bar corresponds to the lowest note.

Each note is positioned in the space, the far left corresponding to a sound coming from the left and the extreme right corresponding to a sound coming from the right.
Several patterns can be selected:

Ascending
: Trebles are in the left side, basses are in the right side.

Descending
: Basses are in the left side, trebles are in the right side.

Hollow
: Basses are at both extremities, trebles are centered.

Spike
: Trebles are at both extremities, basses are centered.

Random
: Trebles and basses are positioned randomly.

Other options are available:

* number of pattern repetitions (:guilabel:`Divisions number`),
* intensity of the effect (:guilabel:`Spreading`),
* filling of a division (:guilabel:`Filling`),
* offset left — right if the spreading is not 100% (:guilabel:`Offset`),
* :guilabel:`Inversions`.

The graph is updated every time a setting changes for a better visualization.
On mouse over, the correspondence note number / panning is displayed.


### Transpose {#doc_transpose}


The transposition tool allows the transposition of an entire instrument.
The sounds of the instrument are thus the same, but at a different position over the keyboard.

As input, a shift in terms of semitones is required.
For the pitch to be higher, the shift must be positive.
Conversely, for the pitch to be lower the shift must be negative.
The operations performed by the tool, for each division of the instrument, are then as follows:

* the root key is adjusted,
* modification of the fine tuning if the transposition is performed by a number of semitones including decimals,
* shift of the key range according to the number of semitones if :guilabel:`adapt key range` is checked.


![Transposition tool](images/tool_transpose_inst.png "Transposition tool")


## Modulators {#doc_cat_mod}


### Override a default modulator {#doc_default_mod}


Default modulators are automatically assigned to every instrument.
This tool can create modulators that will override or disable them.


![Overriding tool](images/tool_default_mod.png "Overriding tool")


### Remove modulators {#doc_remove_mod}


All modulators of all selected instruments will be removed.

**Note:** Default modulators will still apply.
The only way to disable a default modulator is to override it with the tool “[Override a default modulator](#doc_default_mod)”.


## Transformation {#doc_cat_transformation}


### Create chords {#doc_chords}


Based on an instrument, this tool allows you to create all possible chords.
This could be interesting for example for the creation of a choir or pads.


![Tool for creating chords](images/tool_create_chords.png "Tool for creating chords")


Here is an example, <a href="downloads/tutorials/chord_base.mp3" download>before</a>:

![](downloads/tutorials/chord_base.mp3)

And <a href="downloads/tutorials/chord_done.mp3" download>after</a>:

![](downloads/tutorials/chord_done.mp3)

An example .sf2 file, based on a flute, can be downloaded <a href="downloads/tutorials/chords.sf2" download>here</a>.


### Division duplication {#doc_division}


This action has for effect the duplication of all divisions of an instrument.
For example, if a sample linked to an instrument has a key range of 36-38, the software duplicates this division so as to have this linked sample for the key ranges 36-36, 37-37 and 38-38.
It makes then possible to thoroughly edit each sample parameter for each different note, instead of editing whole divisions with the same parameters.

It is also possible to duplicate the divisions according to velocity ranges.


![Division duplication tool](images/tool_division_duplication.png "Division duplication tool")


### Mixture creation {#doc_mixture}


Specific to the organ, this tool allows the creation of a mixture from an existing instrument.
An interface appears for specifying the different mixture divisions, and for each the list of the ranks.
Types of possible ranks are very diverse, ranging from the octave to the 27th harmonic.


![Mixture creation tool](images/tool_mixture.png "Mixture creation tool")


To work, this tool relies on all samples associated to the selected instrument, the range defined for each sample and the corresponding attenuation.
Samples have to be tuned regardless the definition of the instrument (via the [sample editor](manual/soundfont-editor/editing-pages/sample-editor.md)), and the root key specified must not take into account membership in a stop (no transposition if the sound is a principal 4' for example).

An option allows the automatic loop of created samples, another sets the creation interval of samples.
Finally, it is possible to choose the type of the created sounds: mono or stereo.

Here is an example, <a href="downloads/tutorials/mixture_base.mp3" download>before</a>:

![](downloads/tutorials/mixture_base.mp3)

And <a href="downloads/tutorials/mixture_done.mp3" download>after</a>:

![](downloads/tutorials/mixture_done.mp3)

An example .sf2 file, based on an organ stop, can be downloaded <a href="downloads/tutorials/mixture.sf2" download>here</a>.
