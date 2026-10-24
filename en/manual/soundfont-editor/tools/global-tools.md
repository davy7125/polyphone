When the header :guilabel:`General` is selected in the [tree](manual/soundfont-editor/tree.md), the [tool menu](manual/soundfont-editor/tools/index.md) provides tools sorted in 3 categories:

* [clean up](#doc_cat_clean),
* [modulators](#doc_cat_mod),
* [utility](#doc_cat_util).


## Clean up {#doc_cat_clean}


### Remove unused elements {#doc_unused}


All samples that are not used by any instrument and all instruments that are not used by any preset are automatically deleted.
A message indicates how many elements have been deleted.


## Modulators {#doc_cat_mod}


### Remove all modulators {#doc_removemods}


All [modulators](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) present in all [instruments](manual/soundfont-editor/editing-pages/instrument-editor.md) and [presets](manual/soundfont-editor/editing-pages/preset-editor.md) will be removed.


## Utility {#doc_cat_util}


### Export preset list {#doc_export}


With this tool, the list of all [presets](manual/soundfont-editor/editing-pages/preset-editor.md) is written as a text, with their bank and preset numbers.
This text can be formatted as csv, to be integrated in a spreadsheet, or as an html table, to be integrated in a website.
A button copies the text to the clipboard.


![Preset list](images/en_preset_list.png "Preset list")