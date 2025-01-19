MIDI files contain MIDI messages which, when put together, form a melody. They can be combined with a synthesizer using a soundfont, and this tutorial shows how to use a MIDI file to test the rendering of a soundfont.

## A bit of background

<div class="too_complicated">
<p>When playing on a synthesizer keyboard, <a href="https://en.wikipedia.org/wiki/MIDI" target="_blank">MIDI</a> signals are emitted, containing information such as the notes played and their velocity (a term indicating the force with which the key is struck). A MIDI file, in simple terms, contains a chronological record of the signals emitted by such a keyboard. Until then, no sound has yet been created.</p>

<p>For a sound to be emitted by the loudspeaker, a conversion step is necessary, and this is where the synthesizer comes in: it triggers sounds from the MIDI signals received. Several types of synthesizer exist, and here we will be talking about sample based synthesizers, and more specifically synthesizers using sound banks in the SoundFont format.</p>

<p>A MIDI file can control up to 16 different channels, allowing 16 different instruments to be used in parallel. As well as indicating which notes to play, the MIDI file contains instructions telling the synthesizer to use instrument X from bank Y for the upcoming notes on channel Z. To ensure a correct instrument selection, the sound bank used by the synthesizer must comply with the <a href="https://en.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> standard, which assigns a specific instrument to each X and Y number. For example, instrument <strong>001</strong> must be mapped to a piano in the soundfont.</p>

<p>To recap, we are going to talk here about the playback of a MIDI file driving a synthesizer that triggers sounds contained in a soundfont that complies with the General MIDI standard. What follows is simpler!</p>
</div>

## Prerequisites

To play a MIDI file using the method described below, you will need:
- an internet access,
- a web browser,
- a MIDI file, such as <a href="files/ff7choco.mid" download>this one</a>,
- optionally, a soundfont that you want to test and that complies with the <a href="https://en.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> standard (a filter exists in the [repository](soundfonts/midi-standard/gm)),
- optionally, the latest version of Polyphone available on this [page](software).

## Reading a MIDI file

A simple method is to use the <a href="https://spessasus.github.io/SpessaSynth/" target="_blank">SpessaSynth</a> synthesizer, which is both a MIDI file reader and a soundfont synthesizer. It has the advantage of being available online and requires no special installation.

Click on the :guilabel:`Load MIDI file` button, select your MIDI file, and you are ready to go.

![The SpessaSynth MIDI player and synthesizer](images/SpessaSynth.jpg "The SpessaSynth MIDI player and synthesizer")

## Selecting a soundfont

To use a soundfont of your choice, click on the :guilabel:`Load soundfont` button and select your file.

## Connecting the MIDI output to Polyphone

As mentioned above, **SpessaSynth** is both a MIDI file reader and a synthesizer. It is possible to use only the MIDI file reader feature and send the signals to Polyphone, which will take over for sound synthesis. This can be useful if the soundfont is too large to be sent over the Internet.

To connect SpessaSynth to Polyphone (method tested with Linux):
- launch the Polyphone software in [synth mode](tutorials/use-polyphone-as-a-synthesizer.md) and open the configurations,
- select “MIDI through” as [MIDI input](manual/settings.md#doc_general) (remember the name),
- open SpessaSynth in your web browser and click on the :guilabel:`Configurations` button,
- select the MIDI output “MIDI through” (identical name to Polyphone's MIDI input).

As a MIDI file can send messages to 16 channels, you will need to open the soundfont 16 times and specify a different channel from 1 to 16 for each tab. For convenience, when opening Polyphone, you can specify the soundfont to be opened and the option “each” to open all 16 tabs. The command would be, for example:

```
polyphone -s -i "/home/davy/Downloads/gm.sf2" -c "each/off/off"
```

See this [page](manual/annexes/command-line.md) for more information about command lines.
