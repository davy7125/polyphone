The tree is the first element in the editor you should focus on.
It represents the architecture of the open soundfont and pilots the right part of the software when you click on one or more elements.

The tree is always displayed with the same [structure](#doc_structure), provides a [contextual menu](#doc_menu) and has [drag & drop](#doc_dragdrop), [copy / paste](#doc_copypaste) and [search](#doc_search) features.


## Structure {#doc_structure}


The structure of the tree is made of 4 headers and sub-content for the headers :guilabel:`Samples`, :guilabel:`Instruments` and :guilabel:`Presets`.


![Tree: structure](images/tree_1.png "Tree: structure")


### Header “General”


A click on the header :guilabel:`General` opens the editor of the [general information](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md).


### Section “Samples”


This section lists all samples contained in the soundfont, which are the raw material upon which all instruments are built.
A click on one or more samples leads to the [sample editor](manual/soundfont-editor/editing-pages/sample-editor.md).
A click on the header :guilabel:`Samples` displays the [configuration summary](manual/soundfont-editor/configuration-summaries.md#doc_sample) of all samples.


### Section “Instruments”


This section lists all instruments contained in the soundfont, an instrument being an intermediate element made of samples and used by presets.
Instruments contain links to samples called “divisions”, displayed as a list below each instrument.
A click on an instrument or on one of its divisions leads to the [instrument editor](manual/soundfont-editor/editing-pages/instrument-editor.md).
A click on the header :guilabel:`Instruments` displays the [configuration summary](manual/soundfont-editor/configuration-summaries.md#doc_instrument) of all instruments.


### Section “Presets”


This section lists all presets contained in the soundfont, a preset being a final elements made of instruments and that are accessible outside the soundfont.
Presets contain links to instruments called “divisions”, displayed as a list below each preset.
A click on a preset or on one of its divisions shows the [preset editor](manual/soundfont-editor/editing-pages/preset-editor.md).
A click on the header :guilabel:`Presets` displays the [configuration summary](manual/soundfont-editor/configuration-summaries.md#doc_preset) of all presets.


## Contextual menu {#doc_menu}


A right click on an element shows a menu comprising the following actions:

* **Bind to…**\
  Bind the selected sample(s) to an instrument, or the selected instrument(s) to a preset.
* **Replace by…**\
  Replace a sample by another sample in an instrument, or replace an instrument by another instrument in a preset.
  The configuration of the sample or instrument is kept.
* **Copy**\
  Copy the selected element(s), also accessible with :kbd:`Ctrl`+:kbd:`C`.
* **Paste**\
  Paste the previously copied element(s), also accessible with :kbd:`Ctrl`+:kbd:`V`.
* **Duplicate**\
  Duplicate the selected element(s) that can be samples, instruments, presets or divisions.
  Also accessible with :kbd:`Ctrl`+:kbd:`D`.
* **Delete**\
  Delete the selected element(s), also accessible with :kbd:`Del`.
* **Rename or bulk rename**\
  Rename the selected element(s), also accessible with :kbd:`F2`.
  When several elements are selected, different options are provided:
  * overwrite existing name with key name as suffix (for samples only),
  * overwrite existing name with numerical ascending suffix,
  * replace characters,
  * insert text at a specific position,
  * delete character range.


![Tree: context menu](images/tree_2.png "Tree: context menu")


## Drag & drop {#doc_dragdrop}


The operation “drag & drop” in the tree enables quick associations or copies.
Before a drag & drop you need to select elements.
This can be a single element or a multiple selection made possible with the buttons :kbd:`Ctrl` and :kbd:`Shift`.

Then, the result of a drag & drop depends on the kind of elements that are dragged and the kind of element that is targeted.

* **Sample → instrument**\
  Association of the sample to the instrument, creation of an instrument division.
* **Sample → header :guilabel:`Instruments`**\
  Creation of an instrument that contains all samples dragged or creation of one instrument per sample dragged.
* **Instrument → preset**\
  Association of the instrument to the preset, creation of a preset division.
* **Instrument → instrument**\
  Copy of all divisions of the source instrument into the destination instrument (it can be the same instrument).
* **Instrument division → instrument or instrument division**\
  Copy of the division of the source instrument into the destination instrument (it can be the same instrument).
* **Instrument → header :guilabel:`Presets`**\
  Creation of a preset that contains all instruments dragged or creation of one preset per instrument dragged.
* **Preset → preset**\
  Copy of all divisions of the source preset into the destination preset (it can be the same preset).
* **Preset division → preset or preset division**\
  Copy of the division of the source preset into the destination preset (it can be the same preset).


## Copy / paste {#doc_copypaste}


Everything that is possible with a drag & drop as seen previously is also possible with a copy / paste.
You can expect the same result with :kbd:`Ctrl`+:kbd:`C` and :kbd:`Ctrl`+:kbd:`V` in a tree.

Copy / Paste is also allowed within different soundfonts.
It is possible to copy one element in a first soundfont, and paste it in a second soundfont.
A multiple selection is allowed.

* **Sample(s) selected**\
  Copy of the samples.
* **Instrument(s) selected**\
  Copy of the instruments including all samples linked.
* **Preset(s) selected**\
  Copy of the presets including all instruments and samples linked.

During a copy of elements between different soundfonts, if an element having the same name already exists several options are provided:

* ignore the operation, in which case the copy is canceled for the element having the same name,
* replace the existing element, in which case the existing element is overwritten by the copied element,
* duplicate the element, in which case the element is copied next to the existing element with a different name.


## Search {#doc_search}


To facilitate the search for items in the tree, a search bar is available to filter.


![Tree: search bar](images/tree_3.png "Tree: search bar")


When a string is inserted into the search bar, the elements containing it are shown.
Moreover:

* if the string is included in the name of sample, the instruments using the sample and the presets using the instruments using the sample are displayed,
* if the string is included in the name of an instrument, the samples used by the instrument and the presets using the instrument are displayed,
* if the string is included in the name of a preset, the instruments used by the preset and the samples used by the instruments used by the preset are displayed.

A click on the cross to the right of the editable field cancels the filter.