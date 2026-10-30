The menu, located at the top right corner of the software, contains several items allowing you to access different features.


![Menu](images/menu.png "Menu")


## Opening a soundfont editor {#doc_open}


New
: Create a new empty soundfont, everything having to be done from scratch.
  Also accessible with the shortcut :kbd:`Ctrl`+:kbd:`N`.

Open…
: Open an [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [.dls](manual/annexes/the-different-soundfont-formats.md#doc_dls), [.sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) or [.organ](manual/annexes/the-different-soundfont-formats.md#doc_organ) file.
  Also accessible with the shortcut :kbd:`Ctrl`+:kbd:`O`.


## Saving a soundfont {#doc_save}


Save
: Save the current file.
  Also accessible with the shortcut :kbd:`Ctrl`+:kbd:`S`.

Save as…
: Save the current file in another file.
  Also accessible with the shortcut :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`S`.

Export soundfonts…
: Merge and export one or several presets from the open soundfonts in the [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz) or [.dls](manual/annexes/the-different-soundfont-formats.md#doc_dls) format. The additional .csv format exports all parameters for a detailed analysis of the soundfont content.
  Also accessible with the shortcut :kbd:`Ctrl`+:kbd:`E`.


![Export window](images/export.png "Export window")


In the export window, the presets of all open soundfonts are listed and the ones to export must be ticked (the buttons :guilabel:`Select all` and :guilabel:`Unselect all` help to quickly change the selection).
The other parameters are:

Destination
: The directory in which the files are written.
  A file is never overwritten: if a file with the same name already exists, a number is added at the end of the name of the new file.

Format
: One of :guilabel:`.sf2`, :guilabel:`.sf3`, :guilabel:`.sfz`, :guilabel:`.csv` or :guilabel:`.dls`.
  If the presets come from several soundfonts, they are merged into a single soundfont in which each source soundfont becomes a bank.
  This is why a maximum of 127 soundfonts, each of them with a maximum of 127 selected presets, can be exported at the same time.

Quality
: For the .sf3 format only, quality of the compression of the samples.

One file per preset
: For the .sf2, .sf3 and .dls formats only, creation of one file per selected preset instead of a single file.

One directory per bank, preset number as prefix, General MIDI classification
: For the .sfz format only. The export creates a directory containing the sfz files and a sub-directory :file:`samples`.
  The sfz files can be put in one directory per bank, their name can be prefixed with the preset number, and they can be sorted into directories corresponding to the General MIDI categories.

Raw values
: For the .csv format only. The values are written as they are stored in the sf2 format, instead of being converted into usual units (seconds, decibels, cents, etc.).
  The export creates a directory containing the file :file:`samples.csv` and one file per instrument and per preset.


## View options {#doc_view}


Full screen
: Toggle the full screen mode.
  Also accessible with the shortcut :kbd:`F11`.


## Shortcuts {#doc_shortcuts}


Settings
: Open the software [settings](manual/settings.md).
  Also accessible with the shortcut :kbd:`Ctrl`+:kbd:`,`.

Online manual
: Show this user manual in a web browser.
  Also accessible with the shortcut :kbd:`F1`.

About Polyphone…
: Show the software version and the copyright. The button :guilabel:`Credit` shows the list of people who contributed to the project.


## Closing {#doc_close}


Close tab
: Close the current tab (a soundfont editor, a soundfont explorer, the settings, etc.).
  Also accessible with the shortcut :kbd:`Ctrl`+:kbd:`W`.

Quit
: Quit the software.
  Also accessible with the shortcut :kbd:`Ctrl`+:kbd:`Q`.