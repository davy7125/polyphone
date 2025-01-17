Polyphone isn't just a soundfont editor, it's also a full-featured synthesizer that lets you play along with a MIDI keyboard.
This tutorial shows you how to launch Polyphone in synth mode.

## Prerequisites

First you need a **master keyboard** or other MIDI controller, which you connect to the computer via a **MIDI-to-USB adapter**.

Download the latest version of Polyphone [here](software), then install and open the program. In the settings, you should find your MIDI device among the [MIDI inputs](manual/settings.md#doc_general). If this is not the case, it's likely that the MIDI adapter driver is not installed on the computer (resolve this before continuing).

Still in the settings, check that the [audio output](manual/settings.md#doc_general) is correct.

## Open Polyphone in synth mode

### Windows

After installing Polyphone, click on the :guilabel:`Polyphone (synth mode)` shortcut in the Windows start menu.

### Linux

Open a terminal and write the following command line:

```
polyphone -s
```

### Mac

Open a terminal and write the following command line:

```
/Applications/Polyphone.app/Contents/MacOS/polyphone -s
```

**Note:** if you have moved the Polyphone application to a folder other than :file:`Applications`, you will need to adapt the above path.

## Playing an instrument

To play an instrument, simply open a soundfont that you can download from the [repository](soundfonts), for example, and select an instrument contained in that soundfont. Instruments are arranged by bank: select a bank to display the instrument list below.

![Polyphone opened with the synth mode option](images/synth_mode.png "Polyphone opened with the synth mode option")

Once you have selected an instrument, its key range is displayed on the virtual keyboard, and you can play it using the corresponding keys on your MIDI controller.

## Options

### Playing several instruments at the same time

To play several instruments when a key is pressed on the keyboard, simply set the :guilabel:`Multiple selection` option to :guilabel:`on`. Multiple instruments can now be selected in the instrument list.

### Playing several instruments separately

If you have several MIDI controllers connected to the computer, you can open several soundfonts and choose a specific channel for each of the opened soundfonts. In this way, for example, a first keyboard will be used to play one instrument, and a second keyboard will be used to play another one. The same soundfont can be opened several times to play several of its instruments simultaneously.

Channel assignment is done by modifying the value of :guilabel:`channel`. For example, the first tab opened will listen to channel 1, while the second will listen to channel 2.

![2-channel synth configuration](images/synth_2_channels.png "2-channel synth configuration")

**Note:** some keyboards can be split into two parts, so that a single keyboard can be used to send keys to 2 different channels.

### Switching instruments with a key

Switching the instrument is generally made via a MIDI message of type “Program change” and Polyphone is able to read these.

However, another mechanism can be used: the use of keys programmed to send “Note ON” / “Note OFF” messages, with the note number corresponding to the instrument number. This mechanism can be useful when the soundfont features only a few instruments, and when keys can be programmed to send specific notes (e.g. master keyboard pads).

If the :guilabel:`Selection by keys` option is set to :guilabel:`on`:
- an instrument is activated when a “Note ON” MIDI message is received,
- an instrument is deactivated when a “Note OFF” MIDI message is received.

This option can be used when the pad on a master keyboard is configured as a toggle (one press activates the note, another press deactivates it).

If :guilabel:`toggle` is selected:
- an instrument is activated when a “Note ON” MIDI message is received,
- an instrument is deactivated when another “Note ON” MIDI message is received.

This time, the switchover takes place at Polyphone level.

## Polyphone launch options

All the options seen above can be called up when opening Polyphone by adding parameters to the command line. It is also possible to specify the soundfont to be opened.

For **Linux** or **macOS**, simply complete the command line. For **Windows**, create a shortcut by right-clicking on the executable file :file:`polyphone.exe`, then select :guilabel:`Send to desktop (create shortcut)` from the :guilabel:`Other options` of the context menu. Then, by displaying the properties of this shortcut, it is possible to add parameters to the target, which could be:

```
"C:\Program Files\Polyphone\polyphone.exe" -s -c "all|on|toggle"
```

The various options are described on this [page](manual/annexes/command-line.md#doc_synth).

## Synth uses

Now that you know how to turn Polyphone into a synthesizer, why not [use a Raspberry Pi to create a musical instrument](tutorials/create-a-synthesizer-with-raspberry-pi.md)?

Polyphone's synth mode can also be used to [read MIDI files](tutorials/read-midi-file-with-soundfont.md).
