MIDI files contain MIDI messages which, when put together, form a melody. They can be combined with a synthesizer using a soundfont, and this tutorial shows how to use a MIDI file to test the rendering of a soundfont.


## A bit of background


<div class="too_complicated">
<p>When playing on a synthesizer keyboard, <a href="https://en.wikipedia.org/wiki/MIDI" target="_blank">MIDI</a> signals are emitted, containing information such as the notes played and their velocity (a term indicating the force with which the key is struck). A MIDI file, in simple terms, contains a chronological record of the signals emitted by such a keyboard. Until then, no sound has yet been created.</p>

<p>For a sound to be emitted by the loudspeaker, a conversion step is necessary, and this is where the synthesizer comes in: it triggers sounds from the MIDI signals received. Several types of synthesizer exist, and here we will be talking about sample-based synthesizers, and more specifically synthesizers using sound banks in the SoundFont format.</p>

<p>A MIDI file can control up to 16 different channels, allowing 16 different instruments to be used in parallel. As well as indicating which notes to play, the MIDI file contains instructions telling the synthesizer to use instrument X from bank Y for the upcoming notes on channel Z. To ensure a correct instrument selection, the sound bank used by the synthesizer must comply with the <a href="https://en.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> standard, which assigns a specific instrument to each X and Y number. For example, instrument <strong>001</strong> must be mapped to a piano in the soundfont.</p>

<p>To recap, we are going to talk here about the playback of a MIDI file driving a synthesizer that triggers sounds contained in a soundfont that complies with the General MIDI standard. What follows is simpler!</p>
</div>


## Prerequisites


To play a MIDI file with the method described below, you will need:

- the latest version of Polyphone, available on this [page](software),
- a MIDI file, such as <a href="files/ff7choco.mid" download>this one</a>,
- a soundfont that you want to test and that complies with the <a href="https://en.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> standard (a filter exists in the [repository](soundfonts/midi-standard/gm)).

A soundfont that doesn't comply with this standard can also be tested, by [choosing its presets](#doc_presets) yourself.


## Reading a MIDI file


Open your soundfont in Polyphone, then click on the icon ![MIDI](images/toolbar_midi.png "MIDI"){.inline} on the right of the [toolbar](manual/soundfont-editor/toolbar.md) to display the [MIDI player](manual/soundfont-editor/toolbar.md#doc_midi_player).

Click on the folder button at the top left of the window, select your MIDI file, then click on the play button (or press :kbd:`Space`), and you are ready to go.

![The MIDI player of Polyphone](images/midi_player.png "The MIDI player of Polyphone")

Each channel of the file containing notes has its own row in the table. The column :guilabel:`Preset` shows the preset requested by the file, which can change during the piece, and the led of the column :guilabel:`Volume` lights up each time a note starts. The slider below the name of the file moves to another passage of the piece.

The MIDI player is also available when Polyphone is opened in [synth mode](tutorials/use-polyphone-as-a-synthesizer.md).


## Testing the soundfont


The soundfont is played as it is in the editor, unsaved changes included: you can edit an instrument while the piece is playing and hear the result immediately, for instance when adjusting its volume, its envelope or its filter.

The player also helps you to listen to the details:

- the buttons :guilabel:`S` (solo) and :guilabel:`M` (mute) of a channel isolate an instrument or remove it from the mix, and the slider of the column :guilabel:`Volume` balances it with the others;
- a warning icon in the column :guilabel:`Preset` means that the soundfont doesn't have the preset requested by the file: the notes of this channel are not played, which reveals a soundfont that doesn't fully comply with the General MIDI standard;
- the MIDI button of a channel lists the controllers currently used by the file (modulation wheel, expression, reverb…), which is useful for checking the [modulators](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) of the soundfont;
- the [playback settings](manual/soundfont-editor/toolbar.md#doc_midi_player_settings), on the right of the banner, play the piece in a loop, slow it down for listening to a fast passage, or transpose it for testing other keys.


## Choosing the soundfonts and the presets {#doc_presets}


Each channel can play a different soundfont: open several soundfonts in Polyphone, then double click on a cell of the column :guilabel:`Soundfont` to choose the soundfont played by the channel. You can for instance compare the pianos of two soundfonts on the same part, or keep a General MIDI soundfont for the accompaniment while your own soundfont plays the melody.

By default, the channels are set to :guilabel:`Selection`: they play the soundfont of the current tab, or the file selected in a [soundfont explorer](manual/soundfont-explorer.md#doc_play). To compare all the soundfonts of a directory, open it in Polyphone and select its files one after the other while the piece is playing: each soundfont is loaded in the background and heard as soon as it is ready, without opening a tab.

![Choosing the soundfont of a channel](images/midi_player_soundfonts.png "Choosing the soundfont of a channel")

In the same way, a double click on a cell of the column :guilabel:`Preset` forces a preset of the soundfont instead of the one requested by the file. This is the way to test a soundfont that doesn't comply with the General MIDI standard, a soundfont containing a single instrument for instance: choose its preset for the channel of the melody, the program changes of the file being then ignored on this channel.


## Exporting the result


When the result suits you, :guilabel:`Export audio...` in the playback settings writes the piece in a wav file as it is heard in the player, with the same soundfonts, presets, volumes, tempo and pitch.
A MIDI file can also be converted into a wav file with the [command line](manual/annexes/command-line.md#doc_render), for instance in a script.
