The purpose of this tutorial is to create expressive instruments using looped samples whose releases have been recorded.
You will learn how to fully use the capabilities of such a sample, by using the attack and sustain of the sample but also its release.


## Traditional release vs custom release


When you trigger a note you will hear first the beginning of the sample (the attack) and then, as long as you don't release the key, the loop within the sample will be played again and again (the sustained phase).
When you release the key, the loop will still be used and the sound will progressively be attenuated until you cannot hear it anymore.
This is the traditional way to do a release with soundfonts: easy to set but ignoring transient harmonics that could occur during the real release of an acoustic instrument.
This type of release can be viewed as a simple extension of the sustained phase.

A more realistic release would be to use the real release provided by the recorded sample.
The attack and sustained phase will be done the same way but when the key is released the sustained phase will suddenly drop, leaving room for the recorded release that will begin to sound.
A custom release is used instead of extending the sound of the sustained phase, adding details to the sound.

Creating such an instrument is made by splitting the problem in two:

* first defining the attack and sustained phase,
* then triggering the release.

The example that will be taken here is the sound of a trumpet in an organ, for its high harmonic content and audible progression.

**Note:** since version 2.4, Polyphone can also trigger a sample when a key is released, thanks to the loop playback mode ![release](images/sample_mode_release.png "release"){.inline} (see the [instrument editor](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)), which makes the short silent loop described below unnecessary.
However, this mode is not documented by the sf2 format and may not be supported by other players, while the method described in this tutorial works with any player.


## Sample preparation


Two cases may appear: you have a file comprising the full sample from the attack to the release, or you have an additional file containing only the release that has been already extracted.


### Full sample with release


Load the sample twice in Polyphone and then, in the second instance of the sample, remove everything in the sample except the release.
The removal can be done by using the key :kbd:`Alt` and then defining an area on the [sample graph](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph).
To find the point where the release begins, play the sample several times and try to define the point where you think no more power was inducing the sound.
This last part will be the release.
Then follow the instructions below.


### Separate sample and release


Samples for the example: <a href="files/trumpet.wav" download>sample 1</a>, <a href="files/trumpet-release.wav" download>sample 2</a>.

If you have 2 samples, the first one containing the beginning and the sustained phase of the sound and the second one containing the release, you only need to make sure that the release begins with a very short blank (a couple of points) being close to 0.
A short inaudible loop will thus be made with this portion.

Tune and loop the first sample as explained in the tutorial “[how to prepare a sample](tutorials/how-to-prepare-a-sample.md)”.

Loop the second sample on a very short part at the beginning, for example from the point 1 to the point 2 or 3.
Specify the same root key and correction as the first sample.


## First instrument: attack and sustained phase


Create a first instrument with the samples comprising the attack and the sustained phase of the sound.
Configure the global division this way:

* select ![loop on](images/loop_on.png "loop on"){.inline} in the :guilabel:`Loop playback` parameter,
* specify 0.01 s in the :guilabel:`Vol env release (s)` parameter (very short release).

Now you have an instrument comprising no releases.


## Second instrument: release


Create a second instrument for the release comprising the release samples with a very short blank loop at the beginning.
Configure it this way:

* select ![loop on and end](images/loop_on_end.png "loop on and release"){.inline} (loop on + release) in the :guilabel:`Loop playback` parameter,
* select a high number such as 100 seconds in the :guilabel:`Vol env release (s)` parameter so that the release is not attenuated,
* determine approximately the duration of the attack in the first sample and use this value in the :guilabel:`Vol env attack (s)` parameter.

Now you have an instrument remaining silent as long as you don't release a key, and then you can hear the release phase.
You may also notice that a very fast tap on a key will trigger the release in an attenuated way.
This is barely noticeable here with the example since the attack of a trumpet is very fast.
The duration of the attack has been slightly increased in the example.


## Merging both instruments


Create a preset using these two instruments.
The first instrument is heard when a key is triggered and the second instrument is triggered when the key is released.
You can then listen to the result and try to make a smoother transition between the sustained phase and the release:

* by attenuating the first or second instrument,
* by increasing the release duration of the first instrument.

The result with the example files is <a href="files/custom%20release.sf2" download>here</a>.

You can listen here to the differences between:

* a <a href="files/classic-release.mp3" download>classical release</a>

![](files/classic-release.mp3)

* and a <a href="files/custom-release.mp3" download>custom release</a>

![](files/custom-release.mp3)