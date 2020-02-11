When one or more samples are selected in the [tree], [tool menu][tools] contains several tools sorted in 4 categories:

* [analyze](#doc_cat_analyze),
* [files](#doc_cat_files),
* [sample processing](#doc_cat_processing),
* [stereo samples](#doc_cat_stereo).

Using the tools simultaneously on several samples is possible (with a multi selection).


## Analyze {#doc_cat_analyze}


### Show peak frequencies {#doc_peakfrequencies}


This tool gathers the information on the peak frequencies of all selected samples, and the results can be exported in a .csv file.
The peak frequencies are visible in the editing page of [samples][s-editor-freq].


![Show peak frequencies](images/tool_show_peak_frequencies.png "Show peak frequencies")


## Files {#doc_cat_files}


### Wav export {#doc_wavexport}


After having selected a destination directory, all selected samples will be exported as .wav files.


## Sample processing {#doc_cat_processing}


### Auto loop {#doc_autoloop}


The software seeks a stable area of the sample and then artificially creates a loop.
A sound segment is copied with a cross-fade, so that the transition at the join location is as smooth as possible.

To assign loop points automatically:

1. click in the [tree] on the sample to be looped,
2. in the editing page of [samples][s-editor]:
  * position the start and end of the loop in the graphic (WAV display area) around a stable area, or
  * let the tool determine automatically a stable area by defining 0 as loop start and loop end,
3. in the :guilabel:`Tools` menu select :guilabel:`Sample`, :guilabel:`Auto loop`,
4. press the :guilabel:`Play` button with the loop option checked,
5. if the loop points found are not suitable you can [undo][toolbar-edit] the operation, try repositioning them manually around a stable area and then use the :guilabel:`Auto loop` function again.

The tool needs to find a stable area of more than 0.25 seconds.
If it fails, a warning message appears indicating that no loop can be found.
In this case it is possible to specify manually a stable area of more than 0.25 seconds and retry the tool, or try to find the loop points manually.

<!-- warning begin -->
**Warning:** This tool modifies the original sample.
Modifications can be undone via the “[undo][toolbar-edit]” function, but if the file is saved and Polyphone is closed there is no way to step back.
<!-- warning end -->

<!-- note begin -->
**Note:** By default, the looping is disabled in an instrument.
To hear it, you must select ![loop on][loop_on] in the :guilabel:`Loop playback` parameter row of either the :guilabel:`Global` or individual notes columns in the :guilabel:`Parameters Table`.
![loop on][loop_on] turns it On, ![loop off][loop_off] turns it Off.
![loop on and end][loop_on_end] turns the looping On and the sample will play to the end after a key is released.
<!-- note end -->


### Change volume {#doc_volume}


You can change the volume in three different ways:

* in adding a certain amount of dB (positive or negative amount),
* in multiplying the signal by a number,
* in normalizing the signal so that the maximum of this signal represents some percentage of the maximum intensity.


![Change volume tool](images/tool_change_volume.png "Change volume tool")


### External command {#doc_externalcommand}


This tool allows you to use an external command or program to edit a sample.
You don't need to export and re-import a sample into Polyphone anymore, everything is done automatically:

1. download an editing software for wav files such as :program:`Audacity` or :program:`Sweep` (Linux),
2. use the tool :guilabel:`External command` for one or more samples,
3. fill the command line in the right field to call the external software (you may need to read the documentation of this software),
4. edit the sample(s) with the external tool,
5. save the modified version of the wav file(s) with the external tool and close it,
6. the sample(s) will be automatically loaded into the soundfont.


![External command tool](images/tool_external_command.png "External command tool")


### Filter frequencies {#doc_filter}


When you use this tool, a window will be displayed showing the frequencies contained in the sample(s) you selected.
You can then use the mouse to draw a red area.
Everything that is in this area will be removed: this means that you can manually decrease of remove some frequencies.


![Filter frequency tool](images/tool_filter_frequencies.png "Filter frequency tool")


<!-- note begin -->
**Note:** This filter is not common since it cuts frequencies in a hard way.
This tool can be useful to design a brick-wall filter but in most cases you will probably want to use the [equalizer][s-editor-eq].
<!-- note end -->


### Remove blank at start {#doc_removeblank}


Any blank area before the sample begins is eliminated.
This reduces the time before the sound is heard.


![Remove blank at start](images/remove_blank.png "Remove blank at start")


<!-- note begin -->
**Note:** this tool may be applied automatically each time a sample is imported, according to the software [settings][settings-general].
<!-- note end -->


### Transpose {#doc_transpose}


The tool asks for a shift in semitones and then re-samples the sound to change the pitch.


![Transpose tool](images/tool_transpose_smpl.png "Transpose tool")


### Trim to end of loop {#doc_trimloop}


If a loop is defined for the selected sample, the software will cut the sound after the end of the loop (leaving a small margin at the end).


![Trim to loop](images/trim_to_loop.png "Trim to loop")


<!-- note begin -->
**Note:** this tool may be applied automatically each time a sample is imported, according to the software [settings][settings-general].
<!-- note end -->


## Stereo samples {#doc_cat_stereo}


### Balance adjustment {#doc_balance}


If the sample is linked to another one (stereo), the software can equalize the mean intensity of the two samples to adjust the balance.
This will then be easier to accurately pan a sample in an [instrument][i-editor].


### Link all stereo samples {#doc_link}


Among the selected samples, samples whose names only differ by the last letter (R / L) are automatically linked.
For example, if in a soundfont two samples are called ``bassoon45L`` and ``bassoon45R``, ``bassoon45L`` will be recognized as the left part of ``bassoon45R`` and ``bassoon45R`` will be recognized as the right part of ``bassoon45L``.

With this information the right panning will be used when these samples are added in an instrument.
You can also enjoy the simultaneous editing of left and right parts in the instrument [table][i-editor-table] if the stereo editing is activated in the [settings][settings-general].

<!-- warning begin -->
**Warning:** This tool can break existing links if the names don't match the rule above.
<!-- warning end -->


### Unlink all stereo samples {#doc_unlink}


All selected samples that are linked (stereo) are unlinked.

Thus, all samples will be processed as mono sounds and they will all have the same panning if they are added in an instrument.
The stereo editing, if activated in the [settings][settings-general], will not be applicable anymore.


[loop_off]:    images/loop_off.png    "loop off"        {.inline}
[loop_on]:     images/loop_on.png     "loop on"         {.inline}
[loop_on_end]: images/loop_on_end.png "loop on and end" {.inline}

[i-editor-table]:   manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table
[i-editor]:         manual/soundfont-editor/editing-pages/instrument-editor.md
[s-editor-eq]:      manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq
[s-editor-freq]:    manual/soundfont-editor/editing-pages/sample-editor.md#doc_freq
[s-editor]:         manual/soundfont-editor/editing-pages/sample-editor.md
[settings-general]: manual/settings.md#doc_general
[toolbar-edit]:     manual/soundfont-editor/toolbar.md#doc_edit
[tools]:            manual/soundfont-editor/tools/index.md
[tree]:             manual/soundfont-editor/tree.md
