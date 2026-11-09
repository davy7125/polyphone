Polyphone can be run with a command line to convert soundfonts in the format [sf2](#doc_sf2), [sf3](#doc_sf3), [sfz](#doc_sfz), [csv](#doc_csv) or [dls](#doc_dls). Supported file formats for the conversion are [sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [dls](manual/annexes/the-different-soundfont-formats.md#doc_dls), [sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark), and [organ](manual/annexes/the-different-soundfont-formats.md#doc_organ).

A [MIDI file can be rendered](#doc_render) into a wav file with a soundfont.
Options can also be called for opening Polyphone in [synth mode](#doc_synth) or with the [MCP server](#doc_mcp) started.

Files and directories can also be passed as arguments for opening them in the graphical interface, for instance :command:`polyphone file1.sf2 file2.sf3 /path/to/directory` (a directory is opened in the [soundfont explorer](manual/soundfont-explorer.md)).
If Polyphone is already open, the files are opened in the running instance.
A few [other options](#doc_other) are available for resetting the configuration or for displaying the help and the version.

The list of all options is displayed with :command:`polyphone --help`.
The order of the arguments doesn't matter.
The options of the [previous versions](#doc_legacy) are still accepted.

**Note:** with Windows you need to write the full path of Polyphone instead of just :command:`polyphone`, which is for example :command:`C:/Program files/Polyphone/polyphone.exe`.


## Conversion {#doc_conversion}


### Command line


```
polyphone --convert <format> <files/to/convert> -d <output/directory> -o <output/file> -f
```


### Arguments


--convert
: Format of the conversion: “sf2”, “sf3”, “sfz”, “csv” or “dls”, the options specific to each format being described below.
  This argument is optional if an output file is given with its extension, for instance :command:`polyphone file.sf2 -o file.sf3`.

-d, --output-dir
: Output directory, which must exist.
  This argument is optional, by default this is the directory of each input file.

-o, --output
: Output file, with or without its extension (the extension of the format is automatically added).
  This argument is optional, by default the output file has the name of the input file.
  A name alone is written in the output directory, a relative path starts from the current directory (or from the output directory if -d is used).
  This argument cannot be used if several files are converted.

-f, --force
: Replace an existing output file.
  Without this argument, the conversion fails if the output file already exists.
  The input file can be replaced by its conversion, for instance :command:`polyphone --convert sf2 file.sf2 -f` rewrites :file:`file.sf2`.

Several files can be converted with a single command, each output file having the name of its input file.
In case of failure the other files are still converted.


### Examples


```
polyphone --convert sf2 /path/to/file.sfArk
polyphone /path/to/file.sf2 -o /path/to/converted.sf3
polyphone --convert sf3 *.sf2 -d /path/to/directory
```


## Conversion to sf2 {#doc_sf2}


No specific option is used for this conversion.

```
polyphone --convert sf2 /path/to/file.dls
```


## Conversion to sf3 {#doc_sf3}


--quality
: Compression quality: “low”, “medium” or “high”.
  This argument is optional, by default this is “medium”.

```
polyphone --convert sf3 /path/to/file.sf2 --quality high
```


## Conversion to sfz {#doc_sfz}


--preset-prefix
: Each preset is prefixed by its preset number.

--bank-dir
: A directory per bank is created.

--gm-sort
: The General MIDI classification is used to sort presets.

```
polyphone --convert sfz /path/to/file.sf3 --bank-dir --gm-sort
```

The conversion creates a directory (named after the output file) containing the sfz files and a sub-directory :file:`samples`.
If this directory already exists, a number is added to its name: nothing is replaced, even with -f.


## Conversion to .csv {#doc_csv}


--raw
: Raw values are exported (as they are stored in the soundfont file) rather than values with physical, measurable units.
  For example, the exported release time will be 0.491 (seconds) or -1229 in its raw form.
  By default, values with physical units are exported.

```
polyphone --convert csv /path/to/file.sf2 --raw
```

The conversion creates a directory (named after the output file) containing the file :file:`samples.csv` and one file per instrument and per preset.
If this directory already exists, a number is added to its name: nothing is replaced, even with -f.


## Conversion to dls {#doc_dls}


No specific option is used for this conversion.

```
polyphone --convert dls /path/to/file.sf2
```

The file is written in the DLS level 2 format, the limitations of the conversion being described [here](manual/annexes/the-different-soundfont-formats.md#doc_dls).


## Rendering of a MIDI file {#doc_render}


### Command line


```
polyphone --render <MIDI/file> <soundfont> -d <output/directory> -o <output/file> -f
```


### Arguments


--render
: Path of the MIDI file to play (extension .mid, .midi, .kar or .rmi): the MIDI file is played with the soundfont and the result is written in a wav file.

-d, --output-dir
: Output directory in which the wav file will be written.
  This argument is optional, by default this is the same directory as the MIDI file.

-o, --output
: Output wav file, with or without its extension.
  This argument is optional, by default the wav file has the name of the MIDI file.

-f, --force
: Replace an existing wav file.


### Example


```
polyphone --render /path/to/song.mid /path/to/file.sf2
```

All channels of the MIDI file play the soundfont, the presets being chosen by the program changes of the file (bank 128 on channel 10, for the percussions).
The settings of the synthesizer are used: [sound](manual/settings.md#doc_sound) (reverb, chorus, general volume), [polyphony](manual/settings.md#doc_general) and [tuning](manual/settings.md#doc_keyboard).
The result is a stereo 32-bit float wav file at 48,000 Hz, ending once the sound of the last notes has faded out.
The notes of a preset that the soundfont doesn't have are not played, and a warning is displayed for this preset.
In the graphical interface, a MIDI file can also be exported from the [MIDI player](manual/soundfont-editor/toolbar.md#doc_midi_player_settings), each channel being able to play a different soundfont or preset.


## Synth mode {#doc_synth}


### Command line


```
polyphone --synth <file/to/read> --channel <channel> --multiple-selection --key-selection <mode>
```

![Polyphone opened with the synth mode option](images/synth_mode.png "Polyphone opened with the synth mode option")


### Arguments


-s, --synth
: Polyphone is opened in synth mode.

--channel
: MIDI channel number (between 1 and 16) to be listened to for playing.
  The value “all” indicates that all channels will be used and processed as one channel (default value).
  The value “each” listens to each channel separately, which is a prerequisite for playing the MIDI output of a sequencer for instance (a MIDI file can also be played directly with the [MIDI player](manual/soundfont-editor/toolbar.md#doc_midi_player)).

--multiple-selection
: Several presets can be selected for playing.

--key-selection
: Presets are selected by the keys corresponding to the preset number (key 0 activates preset 0, for example), with the value “on”.
  With the value “toggle”, a key also deselects its preset.

These arguments are optional, and the settings can then be changed via the graphical interface.


### Example


```
polyphone --synth /path/to/file.sf2 --channel 1 --multiple-selection --key-selection toggle
```


## MCP server {#doc_mcp}


### Command line


```
polyphone --mcp <files/to/open> --mcp-port <port>
```


### Arguments


--mcp
: The MCP server, allowing an AI assistant to browse, edit and analyze the open soundfonts, is started for this session even if it is disabled in the [settings](manual/settings.md).
  The address of the server is displayed in the console.
  A new instance of Polyphone is opened, even if Polyphone is already open.

--mcp-port
: Port of the MCP server (this argument implies --mcp).
  This argument is optional, by default this is the port of the settings (8642).

Changing the MCP server settings in the graphical interface replaces these options.


### Example


```
polyphone --mcp-port 8700 /path/to/file.sf2
```


## Other options {#doc_other}


-h, --help
: Displays the list of all options.

-v, --version
: Displays the version of Polyphone.

-r, --reset-config
: Resets the configuration of Polyphone (settings, window geometry, history, etc.).
  This can be useful in case of trouble.

```
polyphone --reset-config
```


## Messages and returned codes {#doc_exit}


The loading and the writing are logged in the console, the errors being written in the error output.
The messages and the help are in the language chosen in the [settings](manual/settings.md).

The returned code is:

0
: Success.

1
: An input file or the output directory doesn't exist, or the output file already exists (without -f).

2
: The arguments are not valid: unknown option, wrong value, option not used with the chosen mode, unsupported input file format, etc.

3
: An input file cannot be read (soundfont, MIDI file).

4
: The output cannot be written.

When several files are converted, the returned code is the one of the first failure.


## Options of the previous versions {#doc_legacy}


The options of the previous versions of Polyphone are still accepted:

-1, -2, -3, -4, -6
: Conversion to sf2, sf3, sfz, csv and dls, the same as --convert.

-5
: Rendering of a MIDI file, whose path is given with -c.

-i
: Input file, which can also be written without this option.

-c
: Configuration, made of values separated by the character “|”:
  
  * conversion to sf3: compression quality, “0” for low, “1” for medium, “2” for high;
  * conversion to sfz: 3 values, “1” or “0”, for --preset-prefix, --bank-dir and --gm-sort, for instance “0|1|1”;
  * conversion to csv: “raw” for --raw;
  * rendering of a MIDI file (-5): path of the MIDI file;
  * synth mode: channel (“all”, “each” or 1 to 16, 0 being the same as “all” and -1 the same as “each”), multiple selection (“on” or “off”) and selection by keys (“on”, “off” or “toggle”), for instance “all|on|toggle”.

```
polyphone -2 -i /path/to/file.sf2 -c 2
polyphone -s -i /path/to/file.sf2 -c 'all|on|toggle'
```
