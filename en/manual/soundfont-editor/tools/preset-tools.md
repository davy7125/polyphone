When a preset or one of its division is selected in the [tree](manual/soundfont-editor/tree.md), the [tool menu](manual/soundfont-editor/tools/index.md) contains several tools sorted in 4 categories:

* [analyze](#doc_cat_analyze),
* [fast editing](#doc_cat_fast),
* [modulators](#doc_cat_mod),
* [transformation](#doc_cat_transformation).

Using the tools simultaneously on several presets is possible (with a multi selection), except for the visualizer.
All tools are also available for the [instruments](manual/soundfont-editor/tools/instrument-tools.md).


## Analyze {#doc_cat_analyze}


### Display parameters {#doc_display}

Same tool than “[Display parameters](manual/soundfont-editor/tools/instrument-tools.md#doc_display)” for instruments.


## Fast editing {#doc_cat_fast}


### Change attenuations {#doc_attenuation}

Same tool than “[Change attenuations](manual/soundfont-editor/tools/instrument-tools.md#doc_attenuation)” for instruments.


### Key-based configuration {#doc_global}

Same tool than “[Key-based configuration](manual/soundfont-editor/tools/instrument-tools.md#doc_global)” for instruments.


### Sound spatialization {#doc_spatialization}

Same tool than “[Sound spatialization](manual/soundfont-editor/tools/instrument-tools.md#doc_spatialization)” for instruments.


## Modulators {#doc_cat_mod}


### Remove modulators {#doc_remove_mod}

Same tool than “[Remove modulators](manual/soundfont-editor/tools/instrument-tools.md#doc_remove_mod)” for instruments.


## Transformation {#doc_cat_transformation}


### Division duplication {#doc_division}

Same tool than “[Division duplication](manual/soundfont-editor/tools/instrument-tools.md#doc_division)” for instruments.


### Merge samples for each note {#doc_mergesamples}

In the case where a preset calls several instruments and samples simultaneously when a key is struck, this tool will merge, for each note, all the samples played. The result is a new preset that plays only one sample per key, thus reducing the CPU load required to run the preset.

The options defined in the image below will make the tool simulate every 3 notes a 5-second key press. The audio recording will be in stereo and the release will not be recorded. The samples thus created will then be looped using the [auto-loop tool](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop) and the instrument containing all these samples will be named “Cymbale III”.

**Note:** using the auto loop option will remove the release part and slighly decrease the sustained duration of each sample.

![Merge samples for each note](images/tool_merge_samples.png "Merge samples for each note")
