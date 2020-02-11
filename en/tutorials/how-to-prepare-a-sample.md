An instrument built in a soundfont is made of samples, coming for example from the recordings of a real instrument.
This tutorial describes how to add and prepare a sample in a soundfont.


## Import


You must first have samples on your computer.
A lot of free samples can be found on internet, being either recorded from real instruments (from a flute, guitar, cello, …) or created with different kinds of audio synthesis.

For this tutorial this flute sample is provided: <a href="downloads/tutorials/flute.wav" download>flute.wav</a>.

Once you have it, create a new soundfont with Polyphone (:menuselection:`Menu --> New`) and import the sample (:guilabel:`Add a sample`).


![Import sample](images/import-sample.png "Import sample")


## Loop


### Introduction


A loop is an area within a sample that can be repeated to extend the duration of the sample.
Let's say a sample lasts only 0.5 second, with a nice loop the sound can be extended up to 10 seconds or more.
This is useful only for samples of non-percussive instruments such as flutes, trumpets, organs, violins.
To the opposite, drums require no loops.
This might also be the case for a piano or a guitar, depending on how these sounds are synthesized.

Thus, this section is necessary if and only if you feel the need of extending the original length of a sample.
If not, go directly to the next section “Tuning”.


### Configuring a loop


In the tree, click on the sample you want to loop and focus on the loop parameters.


![Loop sample: loop parameters](images/loop-sample-1.png "Loop sample: loop parameters")


If you are lucky, these values were packaged in the .wav sample and these fields are already filled.
If not, or if it's been badly configured (this can happen…), arbitrarily click left and right on the graphic (WAV display area) to define the beginning and the end of the loop.
This is generally done on the steady phase of the sound, not within the attack or the release.
Then, zoom around the beginning of the loop and try to superimpose the green (or red) and blue curves.


![Loop sample: wave alignment](images/loop-sample-2.png "Loop sample: wave alignment")


### Listening to the result


When the loop is configured, press :guilabel:`play` on the player with the :guilabel:`loop` enabled to check the result.


![Loop sample: check the result](images/loop-sample-3.png "Loop sample: check the result")


If you don't hear the transition once the end of the loop is read, this means that the loop is good.
Otherwise, keep the player on and slightly move the end and / or the beginning of the loop until you find points with a transition as smooth as possible.
Try the next records to see the difference between a bad and a good loop:

* <a href="downloads/tutorials/bad-loop.mp3" download>bad loop</a>

![](downloads/tutorials/bad-loop.mp3)

* <a href="downloads/tutorials/good-loop.mp3" download>good loop</a>

![](downloads/tutorials/good-loop.mp3)

<!-- note begin -->
**Note:** A tool exists to “cheat” if you cannot find a loop easily: the [auto-loop][s-tool-autoloop] tool.
It will automatically create a loop by cutting the sample and superimposing two sections, so that the end of the section will exactly match its beginning.
See the related documentation for more information.
<!-- note end -->


## Tuning


All samples have to be tuned if you don't want an out-of-tune instrument.
By indicating in the soundfont the exact pitch of all imported samples, the soundfont will be able to adapt them to keys.
Some samples may already have these information, in which case you shouldn't have to change the values in :guilabel:`Root Key` and :guilabel:`Correction (cents)`.
But as a precaution it is always good to check.


![Tune sample: pitch information](images/tune-sample-1.png "Tune sample: pitch information")


Enable the loop if the sample has one and press :guilabel:`play` on the player.
Then add the calibration sine and adjust its volume so that you can hear both sounds as clearly as possible.


![Tune sample: play sample along with calibration signal](images/tune-sample-2.png "Tune sample: play sample along with calibration signal")


Start the tuning by changing the root key until the two sounds most closely match.
Continue by adjusting the correction.
For this step, pay attention to any beats that occur and make sure they are as slow as possible.
Here are examples containing the sound of a flute and a calibration sine:

* <a href="downloads/tutorials/very-bad-tune.mp3" download>very bad tune</a> (the root key doesn't match)

![](downloads/tutorials/very-bad-tune.mp3)

* <a href="downloads/tutorials/bad-tune.mp3" download>bad tune</a> (the correction is not correct and you can hear beats)

![](downloads/tutorials/bad-tune.mp3)

* <a href="downloads/tutorials/good-tune.mp3" download>good tune</a> (both values are good)

![](downloads/tutorials/good-tune.mp3)

<!-- note begin -->
**Note:** The [frequency analysis][s-editor-freq] can be a good indicator to start tuning. But keep in mind that no algorithm is perfect for this task and only your ears will find the right tune.
<!-- note end -->


![Tune sample: frequency analysis](images/tune-sample-3.png "Tune sample: frequency analysis")


## To go further


Several tools are available in the menu :guilabel:`Tools`.
You can edit samples by:

* [removing blank][s-tool-removeblank] at start,
* [equalizing][s-editor-eq] the sound,
* [transposing][s-tool-transpose],
* [normalizing][s-tool-volume] the volume,
* [adjusting the balance][s-tool-balance] for stereo samples.


## Note on the stereo samples


Stereo samples are made of two mono samples.
The link is stored in the information of a sample.


![Stereo sample](images/stereo-sample.png "Stereo sample")


Managing two mono samples may add difficulty to loop them because a loop can sound fine for one side and not for the other.
In that case, don't hesitate to cheat by configuring two different loops on the right and left side.
The quality of the sound will not be impaired.


[s-editor-eq]:        manual/soundfont-editor/editing-pages/sample-editor#doc_eq
[s-editor-freq]:      manual/soundfont-editor/editing-pages/sample-editor.md#doc_freq
[s-tool-autoloop]:    manual/soundfont-editor/tools/sample-tools.md#doc_autoloop
[s-tool-balance]:     manual/soundfont-editor/tools/sample-tools.md#doc_balance
[s-tool-removeblank]: manual/soundfont-editor/tools/sample-tools.md#doc_removeblank
[s-tool-transpose]:   manual/soundfont-editor/tools/sample-tools.md#doc_transpose
[s-tool-volume]:      manual/soundfont-editor/tools/sample-tools.md#doc_volume
