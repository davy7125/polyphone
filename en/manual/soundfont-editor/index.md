When a soundfont is created or when an existing one is opened, a new tab containing a soundfont editor appears.


![Soundfont editor](images/soundfont_editor.png "Soundfont editor")


A soundfont editor is made of:

* a [tree](manual/soundfont-editor/tree.md) on the left, representing the architecture of the soundfont;
* a [toolbar](manual/soundfont-editor/toolbar.md) on the top (in blue in the picture);
* a main area that can either contain an [editing page](manual/soundfont-editor/editing-pages/index.md) or a [configuration summary](manual/soundfont-editor/configuration-summaries.md), depending on what is selected in the [tree](manual/soundfont-editor/tree.md),
* a bar at the bottom of the main area, whose content depends on the selection:
  * for the general information, the name and the size of the file,
  * for a sample or an instrument, the instruments or the presets in which it is used (a click on one of them selects it in the tree), and a button for enabling or disabling the stereo editing (see the option :guilabel:`Stereo editing` in the [settings](manual/settings.md#doc_general)),
  * for a preset, its bank and preset numbers,
  * for a configuration summary, the number of elements displayed.