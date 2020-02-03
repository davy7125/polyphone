To create a [soundfont](manual/annexes/the-different-soundfont-formats.md#doc_sf2), click on :guilabel:`Create a soundfont` in the [home screen](manual/index.md#doc_left).
Soundfonts are made from a 3-level structure, so the creation of a new soundfont involves three fundamental steps:

* the [sample preparation](#doc_sample),
* the [instrument creation](#doc_instrument), comprising samples,
* the [preset creation](#doc_preset), comprising instruments.


## Sample preparation {#doc_sample}


### Sample loading


The sample preparation begins by **loading** .wav files.
To do this first select the :guilabel:`Samples` category in the [tree](manual/soundfont-editor/tree.md) and select :guilabel:`Import samples` from the [toolbar](manual/soundfont-editor/toolbar.md#doc_edit).
Samples may be obtained:

* through Internet,
* by recording a real musical instrument,
* by synthesis using a specialized software.


### Sample looping


Next, a **loop** may be required in the sample, if you would like it to sound longer than its normal length.
An example of this would be if you only have a 1 second flute sample, but would like it to sound indefinitely.
This can be done by manually or automatically assigning loop points (start + end) within the sample boundaries, so that the loop area can be repeated and thus make the sample play longer.
While you can assign loop points manually, using the “[Auto loop](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop)“ function to assign them automatically usually produces better results and is a lot faster.

To assign loop points manually:

1. click in the [tree](manual/soundfont-editor/tree.md) on the sample to be looped,
2. in the [sample editor](manual/soundfont-editor/editing-pages/sample-editor.md), arbitrarily position the start and end of the loop in the [graphic](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph) (WAV display area) unless a loop is already defined.
   Use left-click for start and right-click for end.
   The right loop point should be entered first (since the left point it by default at the position 0 and that it's not possible to assign a right point before the left point).
3. press the :guilabel:`Play` button after having selected the loop function,
4. adjust the start and / or end of the loop, while the sample is playing, until the transition between the two positions is as smooth as possible.


**Note:** to hear looping in an instrument, you must enter select |loop on| in the :guilabel:`Loop playback` parameter row of either the Global or individual notes columns in the [Parameters Table](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table). ![loop on](images/loop_on.png "loop on"){.inline} turns it On, ![loop off](images/loop_off.png "loop off"){.inline} or a blank turns it Off.


![Loop illustration](images/loop_illustration.png "Loop illustration")


### Sample tuning


Finally, a **tuning** has to be done.
To do this, a calibration tool (Sinus) is available in the editing page of [samples](manual/soundfont-editor/editing-pages/sample-editor.md#doc_player).
For each sample the method is as follows:

1. begin the playback (press the :guilabel:`play` button), if possible with loop function selected,
2. select the sinus function,
3. adjust the volume slider to hear the two sounds as clearly as possible,
4. change the Root key until the two sounds most closely match,
5. adjust the Correction (cents) to tune the sample to the Rootkey.
   To do this, pay attention to any beats that occur and make sure they are as slow as possible.

The [frequency](manual/soundfont-editor/editing-pages/sample-editor.md#doc_freq) analysis can be a good indicator to start tuning.


### To go further


Several tools are available to edit samples like:

* [removing blank](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank) at start,
* [equalizing](manual/soundfont-editor/tools/sample-tools.md#doc_eq) the sound,
* [transposing](manual/soundfont-editor/tools/sample-tools.md#doc_transpose),
* [normalizing](manual/soundfont-editor/tools/sample-tools.md#doc_volume) the volume,
* [adjusting the balance](manual/soundfont-editor/tools/sample-tools.md#doc_balance) for stereo samples.


## Instrument creation {#doc_instrument}


For this step, it is assumed that one or more samples are available for the creation of an instrument.


### Creating an instrument and adding samples


First, click on :guilabel:`New instrument` in the [toolbar](manual/soundfont-editor/toolbar.md#doc_edit).
A name must now be entered.

Then add samples to the instrument using a “[drag & drop](manual/soundfont-editor/tree.md#doc_dragdrop)”.
In the tree you will notice that divisions appear in the instrument.

**Note:** When samples are created the divisions are not copied but linked. There are no constraints on the number of divisions.


### Distribution of divisions


When samples are added to instruments, they appear as divisions (columns) in the [instrument editor table](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).
Each division must then be positioned on the keyboard by changing the :guilabel:`Key range` in the table.
Generally, it is a good practice to have the range of a division include the root key of its represented sample.
The entire surface of the keyboard must be covered (eg, key 36 to key 96 for a classic synthesizer keyboard).

The tool “[Sample auto-positioning](manual/soundfont-editor/tools/instrument-tools.md#doc_position)” automatically distributes the samples over the keyboard.


### Setting the divisions


If the sample(s) are looped, to hear the looping in an instrument, select ![loop on](images/loop_on.png "loop on"){.inline} in the :guilabel:`Loop playback` parameter row of either the :guilabel:`Global` or individual notes columns in the :guilabel:`Parameters Table`.

* ![loop on](images/loop_on.png "loop on"){.inline} turns it On,
* ![loop off](images/loop_off.png "loop off"){.inline} or a blank turns it Off.
* ![loop on and end](images/loop_on_end.png "loop on and end"){.inline} turns the looping On and the rest of the sample is read after the key is released.

The global division, as its name suggests, allows you to enter a parameter for the entire instrument.
A global parameter applies only to the divisions whose same parameter is not specified.
This means that the parameters of the standard divisions take precedence over the settings of the global division.

In the context of this simple tutorial, no other parameters have to be edited for the instrument to work.
It should be already possible to play with the [virtual keyboard](manual/soundfont-editor/toolbar.md#doc_keyboard).

However, to improve the instrument it is advisable to enter a value in the :guilabel:`Vol env release (s)` row, which prevents the sound from stopping abruptly, as soon as a key is released.
The other parameters are described in the description of [tables](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).


## Preset creation {#doc_preset}


For this step, it is assumed that one or more instruments are available for the creation of a preset.


### What is a preset?


A preset is the equivalent of a sound patch name found in hardware synths.
It is visible from outside a soundfont and is identified by a bank number and a preset number.
A soundfont can have one or many presets.
Each preset contains one or more instruments, just as instruments contain one or more samples.


### Creating a preset and adding instruments


First, click on :guilabel:`New preset` in the [toolbar](manual/soundfont-editor/toolbar.md#doc_edit).
A name must now be entered.

Then add instruments to the preset using a “[drag & drop](manual/soundfont-editor/tree.md#doc_dragdrop)”.
In the tree you will notice that one or more divisions appear in the preset.
Often there may only be one, as in the case of a preset containing a single intrument.

It is possible to create as many or more presets as instruments, and each preset can contain one or more instruments.


### Setting up a preset


Changing the preset parameters may not be required since:

* when creating a preset, the bank number and the preset number are automatically assigned,
* when adding an instrument to a preset the key range is automatically calculated according to the instrument.

For the scope of this tutorial, the creation of a soundfont is finished!