The configuration summaries provide an overview of all elements contained in a soundfont.
They are accessible via the headers :guilabel:`Samples`, :guilabel:`Instruments` and :guilabel:`Presets` in the [tree](manual/soundfont-editor/tree.md), and allow the visualization of their corresponding elements.

Information gathered in these pages can be used to support inspections within large soundfonts.

Each element is displayed on a line of a table.
A click on the header of a column sorts the table according to this column, and a double-click on a line opens the corresponding element in its [editing page](manual/soundfont-editor/editing-pages/index.md).
Values that deserve attention are written in bold, in yellow (warning) or in red (error).


## Samples {#doc_sample}


When clicking on the :guilabel:`Samples` header of a soundfont in the [tree](manual/soundfont-editor/tree.md), the sample configuration summary appears.


![Sample configuration summary](images/conf_summary_sample.png "Sample configuration summary")


For each sample, the columns are: :guilabel:`Used`, :guilabel:`Total duration`, :guilabel:`Loop duration`, :guilabel:`Root key`, :guilabel:`Correction`, :guilabel:`Sample type`, :guilabel:`Linked sample`, :guilabel:`Sample rate` and :guilabel:`Compression`.
This page allows for example to perform the following checks:

* Is it used?
* Is its total duration (and possibly its loop duration) long enough?
* Is the loop correct? (a loop that is likely to produce a click is highlighted)
* Has the tuning been done?
* Is it properly linked in stereo? (the two samples of a stereo pair should have the same root key)
* Is the sample rate high enough for a good audio quality?
* Has it been compressed?


## Instruments {#doc_instrument}


When clicking on the :guilabel:`Instruments` header of a soundfont in the [tree](manual/soundfont-editor/tree.md), the instrument configuration summary appears.


![Instrument configuration summary](images/conf_summary_instrument.png "Instrument configuration summary")


For each instrument, the columns are: :guilabel:`Used`, :guilabel:`Sample number`, :guilabel:`Parameter number`, :guilabel:`Modulator number`, :guilabel:`Max key range`, :guilabel:`Max velocity range`, :guilabel:`Attenuation`, :guilabel:`Loop playback`, :guilabel:`Chorus` and :guilabel:`Reverb`.
This page allows for example to perform the following checks:

* Is it used?
* Are the numbers of linked samples, parameters and modulators consistent?
* Are the key and velocity ranges correct?
* Have the samples been properly attenuated?
* Is the loop playback on?
* Have the chorus and reverb settings been forgotten?


## Presets {#doc_preset}


When clicking on the :guilabel:`Presets` header of a soundfont in the [tree](manual/soundfont-editor/tree.md), the preset configuration summary appears.


![Preset configuration summary](images/conf_summary_preset.png "Preset configuration summary")


For each preset, the columns are: :guilabel:`Bank - Preset`, :guilabel:`Instrument number`, :guilabel:`Parameter number`, :guilabel:`Modulator number`, :guilabel:`Max key range`, :guilabel:`Max velocity range`, :guilabel:`Attenuation`, :guilabel:`Chorus` and :guilabel:`Reverb`.
This page allows for example to perform the following checks:

* Have the bank and preset numbers been properly filled?
* Are the numbers of linked instruments, parameters and modulators consistent?
* Are the key and velocity ranges correct?
* Have the instruments been accurately attenuated?
* Have the chorus and reverb settings (as offsets) been forgotten?