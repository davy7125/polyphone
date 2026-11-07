Polyphone includes a server following the Model Context Protocol (MCP), a standard allowing AI assistants to use other programs.
Once it is enabled, an assistant such as Claude can work on the soundfonts open in Polyphone: you describe what you want in plain language, and the assistant browses the soundfonts, edits them, uses the tools of Polyphone and measures the result.

Every change appears immediately in the editor and can be undone, as if you had made it yourself.
This tutorial shows how to connect Claude to Polyphone, and then gives examples of what can be asked, with audio files to hear the results.


## What the assistant can do {#doc_principles}


The assistant has access to everything that is open in Polyphone:

* **browsing**: the soundfonts, their samples, instruments and presets, the parameters of each division, and the elements selected in the [tree](manual/soundfont-editor/tree.md) (so that “this sample” or “the selected instruments” can be understood),
* **editing**: creating, renaming, duplicating or removing elements, changing parameters, key and velocity ranges, loop points, modulators…
* **tools**: most of the [tools](manual/soundfont-editor/tools/index.md) of Polyphone, such as the auto loop, the volume change, the auto-positioning of the samples or the natural release,
* **sound analysis**: measures of loudness, pitch, harmonics, attack, loop quality… and the rendering of a note played by the synthesizer.

The analysis functions are essential: **the assistant cannot hear**.
Instead, it measures the sound before and after a change, the way an engineer would read a spectrum analyzer.
This is why it can check that a loop doesn't click, that the notes are in tune or that the volume is even across the keyboard — but your ears remain the final judge.

Each change made by the assistant is one step in the history of the soundfont: :kbd:`Ctrl`+:kbd:`Z` in Polyphone undoes it.
Nothing is written to disk until you ask the assistant to save the soundfont, or until you save it yourself.


## Setting up {#doc_setup}


### Enabling the server in Polyphone


Open the [settings](manual/settings.md#doc_ai_assistant) and go to the section :guilabel:`AI assistant`.
Check :guilabel:`Allow AI assistants to access the open soundfonts`: the line below then shows the address of the server, http://127.0.0.1:8642/mcp by default.
If the port 8642 is already used by another program, choose another one with :guilabel:`Port`.

The server only accepts connections from your own computer.
The same page lists all the functions offered to the assistant, grouped in four categories: :guilabel:`Browsing`, :guilabel:`Editing`, :guilabel:`Tools` and :guilabel:`Sound analysis`.


### Connecting Claude Code


<a href="https://claude.com/claude-code" target="_blank">Claude Code</a> is an assistant made by Anthropic that runs in a terminal (it also exists as an extension of VS Code and JetBrains editors).
Once it is installed, copy the command shown in the settings of Polyphone with the button :guilabel:`Copy`, and run it in a terminal:

```
claude mcp add --transport http polyphone http://127.0.0.1:8642/mcp
```

By default, Claude Code uses the server only when it is started from the folder where this command was run.
To use Polyphone from any folder, add the option `--scope user`:

```
claude mcp add --scope user --transport http polyphone http://127.0.0.1:8642/mcp
```

Then start a new session of Claude Code: the functions of Polyphone are loaded when a session starts, and Polyphone must already be open with the server enabled.
The command `/mcp` in Claude Code shows whether the server “polyphone” is connected.
The first time the assistant wants to use a function of Polyphone, Claude Code asks for your permission, and you can choose not to be asked again for this function.

**Note:** other assistants supporting MCP with the HTTP transport can connect to the same address.


### First questions


Start with simple questions to see how the assistant explores the soundfonts:

* *“Which soundfonts are open in Polyphone?”*
* *“What does this soundfont contain? Are some samples used by no instrument?”*
* *“Give me the list of the presets with their bank and preset numbers.”*

When you select an element in the tree of Polyphone, the assistant can refer to it: *“Lower the volume of the selected sample by 6 dB”* or *“Rename these instruments with the prefix ‘Organ’”*.
For small edits, the result is immediate and visible in the editor.


## Example: checking and fixing an instrument {#doc_check}


Mistakes are easily made when building an instrument with many samples, and some of them are hard to notice: a note slightly out of tune, a sample a bit louder than its neighbors, a loop clicking only after a few seconds…
The assistant can check a whole instrument in a few seconds.

To try it yourself, download <a href="files/flute-with-mistakes.sf2" download>flute-with-mistakes.sf2</a> and open it in Polyphone.
It contains a flute instrument in which a few mistakes have been made on purpose.
Listen to a scale from C4 to F6 followed by a long G6:

![](files/ai-assistant-flute-before.mp3)

Then ask:

*“Check the instrument ‘Flute’: are there notes out of tune, too loud or too soft, keys without sound or bad loops?”*

The assistant analyzes the instrument (a single function measures the loudness, the pitch, the timbre and the loop of every division), then examines the suspicious samples in detail.
Its answer lists the problems:

* the key E4 plays nothing: the division of “Flute 63” stops at D♯4 and the next one starts at F4,
* “Flute 69” sounds a semitone too high: it plays an A4 (440 Hz) but its root key is G♯4,
* “Flute 78” is 30 cents too high: its pitch correction is +30 cents whereas the recording is in tune,
* the division of “Flute 84” is 6 dB louder than the others (keys B5 to C♯6),
* the loop of “Flute 90” clicks: the waveforms don't match at the loop point and the length of the loop is not a whole number of periods.

Then ask: *“Fix these problems.”*

The assistant extends the key range of “Flute 63” to E4, sets the root key of “Flute 69” to A4 and the correction of “Flute 78” to 0, gives an attenuation of 6 dB to the division of “Flute 84” (the sample itself is not changed), and uses the tool [Auto loop](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop) on “Flute 90”.
A new analysis confirms the result: no missing key, no note out of tune, the same volume everywhere and a loop rated “good”.
The same scale after the corrections:

![](files/ai-assistant-flute-after.mp3)

The assistant explains what it does, and it often has to make choices: here it attenuated a division rather than changing the audio data of the sample.
If you prefer another solution, just say it — and you can review each change in the editor, or undo it.


## Example: creating an instrument from scratch {#doc_create}


An assistant like Claude Code can also run programs on your computer, for instance to compute samples.
Combined with the functions of Polyphone, this allows building a whole instrument from a description.
The flute of the previous example was made this way, starting with two requests:

*“Can you make me a flute sound for the key 60?”*

*“Now make a complete flute with several levels of intensity, in a new soundfont.”*

Here is what the assistant did, step by step:

1. it measured a recorded flute in a General MIDI soundfont to know the timbre of the instrument in each register (harmonics, breath noise, attack, vibrato),
2. it wrote a program synthesizing 39 samples: 13 notes from C4 to C7, one every 3 keys, each at three dynamics — *p* is darker and breathier with a soft attack, *f* is richer in harmonics with a sharper attack,
3. it imported the samples in a new soundfont, set their root keys and their loops, and created one instrument per dynamic with a release of 0.4 s,
4. it stacked the three instruments in a preset (bank 0, preset 73, the flute of the General MIDI standard) with the velocity ranges 0-49, 50-89 and 90-127, as described in the tutorial “[Realistic instruments with multiple velocity layers](tutorials/realistic-instruments-with-multiple-velocity-layers.md)”,
5. it rendered notes on both sides of each velocity boundary: the volume only changes by 0.1 to 0.4 dB, so that only the timbre changes from one layer to the next.

The last step revealed a weakness: in the highest octave, the three layers had almost the same timbre.
The assistant changed its program so that the loud notes also become brighter in the high register, then generated the samples and built the instrument again.

Listen to the key C4 played with the velocities 35, 70 and 110 — one note per layer:

![](files/ai-assistant-flute-layers.mp3)

And a phrase getting louder and louder, where the timbre follows the dynamics:

![](files/ai-assistant-flute-phrase.mp3)

A synthesized flute will never equal a good recording: this example shows the method more than the sound.
With your own recordings, the same kind of request applies: *“Import the wav files of this folder, find their root key, loop them and build an instrument with one layer per dynamic.”*


## More ideas {#doc_ideas}


Here are other requests that work well:

* *“Set a natural release for all the instruments, longer in the bass.”*
* *“The high notes of this instrument are too bright: make them softer, gradually from C6.”*
* *“Create a celeste stop from this organ instrument, with 3 beats per second at C4.”*
* *“Convert all the stereo samples to mono and update the instruments.”*
* *“Compare these two samples: why does the second one sound duller?”*
* *“Explain the modulators of this preset in simple words.”*
* *“Remove the unused samples and the duplicates, then save the soundfont as sf3.”*


## Tips {#doc_tips}


* **Be precise about the scope**: “only the instrument ‘Piano’”, “don't change the samples, only the instruments”…
* **Ask for measures**: “check the result” or “compare before and after” makes the assistant verify its work with the analysis functions.
* **Go step by step** for large changes, and listen between the steps.
* **Keep a copy** of your soundfont, and save only when you are satisfied: ask the assistant to save, or use :kbd:`Ctrl`+:kbd:`S`.
* **Large soundfonts** take more time to analyze: target an instrument or a preset rather than the whole soundfont.
* **Privacy**: the assistant receives the names, the parameters, the measures and pictures of waveforms, not the audio itself; but these data are sent to the service of the assistant, like the rest of the conversation.
