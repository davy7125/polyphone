With Polyphone, it is possible to open a directory from the [home screen](manual/index.md#doc_left) by clicking the :guilabel:`Open directory` button. The soundfont explorer then opens in a new tab to display the files of that directory (its sub-directories are not scanned), presenting, for each soundfont, a preview of its content.


![Soundfont explorer](images/soundfont_explorer.png "Soundfont explorer")


## Soundfont preview {#doc_preview}


The main list, located on the left side of the interface, contains the list of files, and Polyphone distinguishes them as follows:

* compatible files that can be scanned quickly ([.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz) and [.dls](manual/annexes/the-different-soundfont-formats.md#doc_dls) formats, for example) are displayed with details about their content,
* compatible files that cannot be scanned quickly ([.sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) format, in particular) are displayed with basic information only,
* files that are not compatible are grayed out and cannot be opened.

Clicking on a scannable file displays, in the lists on the right side of the interface, the samples, instruments and presets it contains. Each item in these lists also includes information to help assess the level of detail of the soundfont and the amount of work put into it.

### File-level details

* the size and the last update date are displayed for each file,
* for soundfonts that can be scanned, the following information is added:
  * number of samples, instruments and presets,
  * names of the first presets.

These details are aligned in columns under a header. The :guilabel:`Display` button, to the right of the text field, opens a menu for choosing the columns shown (all of them can be hidden except the name) and whether the :guilabel:`Preset summary` is displayed, that is the names of the first presets on a second line. Without this summary, each file takes up a single line, as in a table.

### Information for each sample

* type (mono / stereo),
* total duration,
* loop duration, if any,
* sample rate.

### Information for each instrument or preset

* total number of divisions, parameters and generators,
* number of distinct key ranges,
* number of distinct velocity ranges.


## Sort and search functions {#doc_filter}


The upper section features a text field (the shortcut :kbd:`Ctrl`+:kbd:`F` puts the focus on it). When it is filled in, the explorer only displays the files matching at least one of the following criteria:

* the name contains the searched text,
* the name of one of the samples, instruments or presets contains the searched text.

The lists of samples, instruments and presets are also filtered to highlight the matching elements.

Clicking on a column header sorts the files by name, size, date, or number of samples, instruments or presets, and clicking on it again reverses the order. The first click sorts the names from A to Z and the other columns starting with the largest value or the newest file. When sorting by a number of elements, the files whose content is unknown are always placed at the end of the list.

The display options, the sort, and the width and expanded or collapsed state of the lists on the right are kept for the next time.


## File operations {#doc_file_operations}


When clicking on a file in the main list, 2 clickable icons appear before the file name:

* a pencil icon to rename the file (also accessible with :kbd:`F2`),
* a trash icon to move the file to the recycle bin, after confirmation (also accessible with :kbd:`Del`).

These operations allow you to quickly manage and clean up a local soundfont collection.


## Playing the elements {#doc_play}


The ![piano](images/toolbar_piano.png "piano"){.inline} button, on the right of the :guilabel:`Display` button, shows or hides the [virtual keyboard](manual/soundfont-editor/toolbar.md#doc_keyboard), and the ![recorder](images/toolbar_recorder.png "recorder"){.inline} button next to it shows or hides the [recorder](manual/soundfont-editor/toolbar.md#doc_recorder), like the buttons of the editor toolbar.

The sample, instrument or preset selected in the lists on the right can be played with the virtual keyboard, or with an external MIDI keyboard even if the virtual keyboard is hidden. As in the editor, the virtual keyboard shows the range of the element, or the root key of a sample. A sample is played in a loop if it has one, and in stereo if the :guilabel:`stereo` option of the sample [player](manual/soundfont-editor/editing-pages/sample-editor.md#doc_player) is ticked.

The first time an element of a file is selected, the soundfont is loaded in the background without opening a tab. The soundfont is then kept as long as elements of the same file are selected, and released when another file is selected in the main list.

A soundfont already open in a tab is played as it is in the editor, with its unsaved changes.


## Opening a soundfont {#doc_open_soundfont}


Double-clicking on an item in the main list opens the file — provided its format is compatible with Polyphone — at the location configured in the [settings](manual/settings.md#doc_interface) with the option :guilabel:`Startup view` ([general information](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md) is displayed by default).

Double-clicking on a [sample](manual/soundfont-editor/editing-pages/sample-editor.md), [instrument](manual/soundfont-editor/editing-pages/instrument-editor.md) or [preset](manual/soundfont-editor/editing-pages/preset-editor.md) opens the file at the selected element.

If the soundfont has already been loaded for playing one of its elements, the tab opens immediately.