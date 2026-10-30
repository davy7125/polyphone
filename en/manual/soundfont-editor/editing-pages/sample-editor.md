When clicking on one or more sample(s) in the [tree](manual/soundfont-editor/tree.md), the editing page of the samples appears.

This page is divided into several parts:

* a [graph](#doc_graph) representing the sample,
* an [information](#doc_info) area,
* an analysis of the [frequencies](#doc_freq),
* an [equalizer](#doc_eq),
* a sample [player](#doc_player).


![Sample page](images/edit_sample.png "Sample page")


## The graph {#doc_graph}


### Zoom & drag


The graph allows the visualization of the sound wave.
It is possible to zoom vertically or horizontally by holding a right mouse click and dragging:

* dragging right expands the horizontal axis,
* dragging left contracts the horizontal axis,
* dragging up expands the vertical axis,
* dragging down contracts the vertical axis.

Holding the left mouse button allows a horizontal move of the graph, if the horizontal axis has been expanded.
The mouse wheel also scrolls the graph horizontally.


### Loop and playing positions


A green vertical bar indicates the start position of the loop (changeable by a left click on the graph), a red bar indicates the position of the end of the loop (changeable by a right click on the graph).
A dotted line representing a wave portion is associated with each vertical bar, useful for visualizing the superimposition of the beginning and the end of the loop.

During playback, a white vertical bar runs across the graph and indicates the playback position in real time.


### Cutting the sample


When holding the :kbd:`Alt` key, it is possible to define an area to cut:

* a left mouse button press defines the start of the area,
* the corresponding left mouse button release defines the end of the area.

Once an area is defined, a dialog appears to ask for a confirmation before cutting the sample.
The answer :guilabel:`Yes and create another sample` cuts the sample and also creates a new sample from the part that has been removed.


![Cutting sample](images/cutting_sample.png "Cutting sample")


### Colors of the graph


Graph colors are editable in the software [preferences](manual/settings.md#doc_interface).


## Section “Information” {#doc_info}


The :guilabel:`Information` section allows viewing — and in most cases editing — the following information:

Size
: The number of values and the corresponding duration in seconds are displayed.

Loop
: The loop start and end points can be modified here.
  A first button removes the loop by setting its start and end to 0.
  A second button edits the start and end positions so that the loop spans the entire sample.

Tuning
: Ability to adjust the root key of a sample and its correction in cents so that the sound is tuned to equal temperament.
  A button edits the root key and correction so that the values match the estimated [frequency](#doc_freq).

Sample rate
: Any modification will trigger a resampling so that the pitch of the sound is not altered.

Channel
: Select the type among :guilabel:`mono`, :guilabel:`left`, :guilabel:`right` or :guilabel:`link`.
  If the type is :guilabel:`left`, :guilabel:`right` or :guilabel:`link`, it is possible to cancel the stereo by selecting :guilabel:`mono`.
  When :guilabel:`mono` is selected, reverting to a stereo state will be done by selecting a linked sample.

Link
: Selection of the sample to link to make the sound stereo.
  If a sample is selected, its type will switch to :guilabel:`link` (this type can then be changed to :guilabel:`left` or :guilabel:`right`).

If the [compression](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md#doc_edit) is enabled for the soundfont ([sf3 format](manual/annexes/the-different-soundfont-formats.md#doc_sf3)), a compression button is displayed for the samples that are not compressed yet.
When compressing, the choice of the compression quality (:guilabel:`High`, :guilabel:`Medium` or :guilabel:`Low`) is requested in a dialog.
The loop of a sample can be altered by the compression.\
**Note:** decompression is performed automatically when editing the sample (applying a filter, trimming, amplifying, etc.).

![Compression quality selection](images/sample_compress_dialog.png "Compression quality selection")

Any change on a stereo sample may be applied on the linked sample if the option :guilabel:`Stereo editing` is ticked in the [preferences](manual/settings.md#doc_general) (or with the button located at the bottom right of the window).
Via a multiple selection in the [tree](manual/soundfont-editor/tree.md), several samples may be edited simultaneously.


## Frequency analysis {#doc_freq}


The frequency analysis includes a Fourier transform (graph showing the intensity of frequencies contained in the signal).

![Fourier graph](images/fourier_graph.png "Fourier graph")

On the right is shown the list of dominant peaks with the following information:

* ratio between the peak intensity and the intensity of the maximum peak,
* frequency,
* closest corresponding key,
* correction (distance to the closest corresponding key).

From these data an estimate of the pitch of the sound is made.
This estimate can be used for tuning.

A right click on the graph opens a contextual menu that allows you to:

* export the graph into a .png image,
* show the list of all peaks and a button for exporting this list into a .csv file.

![Show peak frequencies](images/tool_show_peak_frequencies.png "Show peak frequencies")


## Equalizer {#doc_eq}


The equalizer makes it possible to increase or decrease some frequencies, from -15 to 15 dB.
It is possible to listen to the effect with the option :guilabel:`preview` enabled and then definitely apply the effect by pressing the :guilabel:`Apply` button.
Pressing :guilabel:`Reset` resets all the sliders to 0 dB (but does not remove an effect that has already been applied to the sound: for that use the :guilabel:`Undo` button in the [toolbar](manual/soundfont-editor/toolbar.md#doc_edit)).

Any change on a stereo sample may be automatically applied on the linked sample if the option is checked in the [preferences](manual/settings.md#doc_general).


## Player {#doc_player}


The player plays the sound.
During playback, a white cursor runs across the graph to indicate the playback position.
Playback options are:

Loop
: Plays the sound in a loop if the beginning and the end of the loop are set.

Stereo
: Plays the sample in stereo, with its linked sample.
  If looping is also required, the two respective loops are used (they can be different).

Sine
: A sine calibration signal is added to the sound, allowing a sample tuning to the nearest pitch hundredth.
  At first the root key must be filled in.
  At this level, beats can be heard if the frequency of the sine differs slightly from the fundamental frequency of the sound.
  It remains to finely adjust the correction to make the beats as slow as possible, so that the sample is tuned at best.
  See the tutorial “[How to prepare a sample](tutorials/how-to-prepare-a-sample.md)” for more information.

A slider sets the volume of the playback (this volume is not stored in the soundfont).

If a playback is in progress when switching from one sample to another through the [tree](manual/soundfont-editor/tree.md), it automatically restarts.
Also, pressing the key :kbd:`Space` starts or stops the playback if the focus is on the tree.