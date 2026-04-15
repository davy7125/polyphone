When clicking on one or more sample(s) in the [tree](manual/soundfont-editor/tree.md), the editing page of the samples appears.

This page is divided into several parts:

* a [graph](#doc_graph) representing the sample,
* an [information](#doc_info) area,
* an analyze of [frequencies](#doc_freq),
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


### Loop and playing positions


A green vertical bar indicates the start position of the loop (changeable by left click), a red bar indicates the position of the end of the loop (changeable by right click).
A dotted line representing a wave portion is associated with each vertical bar, useful for visualizing the superimposition of the beginning and the end of the loop.

During playback, a white vertical bar runs across the graph and indicates the playback position in real time.


### Cutting the sample


When holding the :kbd:`Alt` keyboard button, it is possible to define an area to cut:

* a left mouse button press defines the start of the area,
* the corresponding left mouse button release defines the end of the area.

Once an area is defined, a dialog appears to ask for a confirmation before cutting the sample.


![Cutting sample](images/cutting_sample.png "Cutting sample")


### Colors of the graph


Graph colors are editable in the software [preferences](manual/settings.md#doc_interface).


## Section “Information” {#doc_info}


The :guilabel:`Information` section allows viewing - and in most cases editing - the following information:

Sample size
: The number of values and the corresponding duration in seconds are displayed.

Loop
: The loop start and end points can be modified here.
  A button edits the start and end positions so that the loop spans the entire sample.

Tuning
: Ability to adjust the root note of a sample and its correction in cents so that the sound is tuned to equal temperament.
  A button edits the root note and correction so that the values match the estimated [frequency](#doc_freq).

Sample rate
: Any modification will trigger a resampling so that the pitch of the sound is not altered.

Sound type
: Select the type among :guilabel:`mono`, :guilabel:`left`, :guilabel:`right` or :guilabel:`linked`.
  If the type is :guilabel:`left`, :guilabel:`right` or :guilabel:`linked`, it is possible to cancel the stereo by selecting :guilabel:`mono`.
  When :guilabel:`mono` is selected, reverting to a stereo state will be done by selecting a linked sample.
  If compression is possible ([sf3 format](manual/annexes/the-different-soundfont-formats.md#doc_sf3)), a compression button is displayed for all uncompressed samples. When compressing, the choice of compression quality is requested in a dialog.
  **Note:** decompression is performed automatically when editing the sample (applying a filter, trimming, amplifying, etc.).

![Compression quality selection](images/sample_compress_dialog.png "Compression quality selection")

: Linked sample
  Selection of the sample to link to make the sound stereo.
  If a sample is selected, their type will switch to :guilabel:`linked` (this type can then be changed to :guilabel:`left` or :guilabel:`right`).

Any change on a stereo sample may be applied on the linked sample if the option :guilabel:`Stereo editing` is ticked in the [preferences](manual/settings.md#doc_general).
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
Pressing :guilabel:`reset` resets all the buttons to 0 dB (but does not remove the effect on the sound: for that use the :guilabel:`undo` button in the [toolbar](manual/soundfont-editor/toolbar.md#doc_edit)).

Any change on a stereo sample may be automatically applied on the linked sample if the option is checked in the [preferences](manual/settings.md#doc_general).


## Player {#doc_player}


The player plays the sound.
During playback, a white cursor runs across the graph to indicate the playback position.
Playback options are:

Loop
: Plays the sound loop if the beginning and end of the loop are completed.

Stereo
: Plays the sample in stereo, with its linked sample.
  If looping is also required, the two respective loops are used (they can be different).

Sine
: A sine calibration signal is added to the sound, allowing a sample tuning to the nearest pitch hundredth.
  At first the root key must be filled.
  At this level, beats can be heard if the frequency of the sine differs slightly from the fundamental frequency of the sound.
  It remains to finely adjust the correction to make the beats as slow as possible, so that the sample is tuned at best.
  See the tutorial “[How to prepare a sample](tutorials/how-to-prepare-a-sample.md)” for more information.

If a playback is in progress when switching from one sample to another through the [tree](manual/soundfont-editor/tree.md), it automatically restarts.
Also, pressing the key :kbd:`space` enables or disables playback if you are navigating in the tree.