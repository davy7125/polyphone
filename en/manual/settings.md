The software preferences are accessible either from the [home screen](manual/index.md) or from the [menu](manual/menu.md#doc_shortcuts).
They are divided into five sections:

* [General](#doc_general)
* [Interface](#doc_interface)
* [Sound](#doc_sound)
* [Virtual keyboard](#doc_keyboard)
* [Online repository](#doc_repository)


## General {#doc_general}


The :guilabel:`General` section allows the modification of the following parameters:

* **Audio backend**\
  Specifies the audio backend used to play samples.
  JACK and ASIO drivers (the latter for Windows only) may be chosen.
* **Buffer size**\
  Specifies the buffer size used for JACK and ASIO drivers.
  A small buffer results in a small latency, but the sound may become scratchy.
* **MIDI input**\
  Specifies the MIDI input controlling the virtual keyboard.

Moreover, it is possible to check the following options:

* **Sample import: trim to loop**\
  When importing a sample, data located after the end of loop are automatically removed (same effect as the tool “[Trim to end of loop](manual/soundfont-editor/tools/sample-tools.md#doc_trimloop)”).
* **Sample import: remove blank at start**\
  When importing a sample, any blank area located before the beginning of the attack is automatically removed (same effect as the tool “[Remove blank at start](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)”).
* **Stereo editing: change linked sample**\
  When editing one side of a stereo sample, the other side is automatically edited the same.
  The same applies when editing a stereo instrument division: the division corresponding to the other channel (with the same note and velocity ranges) is edited the same.


![Settings, general](images/settings_general.png "Settings, general")


## Interface {#doc_interface}


This section shows options related to the interface of the software:

* **Language**\
  The language may be changed here (a restart of the software is required).
* **Key names**\
  Keys may be named depending on their number (according to MIDI specification), or depending on the name of middle C (C3, C4 or C5).
  Key names may comprise sharps or flats.
* **Sort divisions**\
  The division order within an instrument or a preset can be changed here.
* **Decorations**\
  Background decorations in some lists can be deactivated for readability.
* **Color theme**\
  Different predefined color themes are available for customizing the appearance of Polyphone.
  A manual selection of colors is also possible.


![Settings, interface](images/settings_interface.png "Settings, interface")


## Sound {#doc_sound}


The :guilabel:`Sound` section makes it possible to modify the chorus and reverb of the synth, along with its global volume.
The global volume may be changed with the volume MIDI controller if an external keyboard is connected.


![Settings, sound](images/settings_sound.png "Settings, sound")


## Virtual keyboard {#doc_keyboard}


The keys of the computer keyboard used to control the [virtual keyboard](manual/soundfont-editor/toolbar.md#doc_keyboard) are defined in the :guilabel:`Keyboard` section.
The pitch may be increased or decreased octave by octave.


![Settings, keyboard](images/settings_keyboard.png "Settings, keyboard")


## Online repository {#doc_repository}


Identifiers can be entered here to link Polyphone with an account, a Premium account unlocking the [soundfont browser](manual/soundfont-browser.md).

The download directory used by the [soundfont browser](manual/soundfont-browser.md) can be changed here.


![Settings, repository](images/settings_repository.png "Settings, repository")



*[JACK]: JACK Audio Connection Kit
*[ASIO]: Audio Stream Input/Output
*[MIDI]: Musical Instrument Digital Interface