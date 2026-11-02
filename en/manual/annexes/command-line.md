Polyphone can be run with a command line to convert soundfonts in the format [sf2](#doc_sf2), [sf3](#doc_sf3), [sfz](#doc_sfz), [csv](#doc_csv) or [dls](#doc_dls). Supported file formats for the conversion are [sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [dls](manual/annexes/the-different-soundfont-formats.md#doc_dls), [sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark), and [organ](manual/annexes/the-different-soundfont-formats.md#doc_organ).

A [MIDI file can be rendered](#doc_render) into a wav file with a soundfont.
An option can also be called for opening Polyphone in [synth mode](#doc_synth).

Files and directories can also be passed as arguments for opening them in the graphical interface, for instance :command:`polyphone file1.sf2 file2.sf3 /path/to/directory` (a directory is opened in the [soundfont explorer](manual/soundfont-explorer.md)).
A few [other options](#doc_other) are available for resetting the configuration or for displaying the help.

In a conversion, the loading and the writing are logged in the console.
The returned code is 0 if the conversion succeeded, and another value in case of failure (the input file does not exist, the file cannot be read or written, an output file already exists for the formats sf2, sf3 and dls, etc.).


## Conversion to sf2 {#doc_sf2}


### Command line


```
polyphone -1 -i <file/to/convert> -d <output/directory> -o <output_file_name>
```

**Note:** with Windows you need to write the full path of Polyphone instead of just :command:`polyphone`, which is for example :command:`C:/Program files/Polyphone/polyphone.exe`.


### Arguments


-1
: Mode “1” is “convert to sf2”.

-d
: Output directory in which the input file will be converted.
  This argument is optional, by default this is the same directory as the input file.

-o
: Output name of the converted file without the extension (the extension “.sf2” will be automatically added).
  This argument is optional, by default this is the same name as the input file.


### Example


```
polyphone -1 -i /path/to/file.sfArk
```


## Conversion to sf3 {#doc_sf3}


### Command line


```
polyphone -2 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**Note:** with Windows you need to write the full path of Polyphone instead of just :command:`polyphone`, which is for example :command:`C:/Program files/Polyphone/polyphone.exe`.


### Arguments


-2
: Mode “2” is “convert to sf3”.

-d
: Output directory in which the input file will be converted.
  This argument is optional, by default this is the same directory as the input file.

-o
: Output name of the converted file without the extension (the extension “.sf3” will be automatically added).
  This argument is optional, by default this is the same name as the input file.

-c
: Conversion configuration.
  It is possible to specify the compression quality: “0” is low, “1” is medium, “2” is high.
  This argument is optional, by default this is “1” (medium quality).


### Example


```
polyphone -2 -i /path/to/file.sf2 -c 2
```


## Conversion to sfz {#doc_sfz}


### Command line


```
polyphone -3 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**Note:** with Windows you need to write the full path of Polyphone instead of just :command:`polyphone`, which is for example :command:`C:/Program files/Polyphone/polyphone.exe`.


### Arguments


-3
: Mode “3” is “convert to sfz”.

-d
: Output directory in which the input file will be converted.
  This argument is optional, by default this is the same directory as the input file.

-o
: Output name of the converted file without the extension (the extension “.sfz” will be automatically added).
  This argument is optional, by default this is the same name as the input file.

-c
: Conversion configuration made of 3 values separated by the “|” character. This argument is optional, by default this is “0|0|0”.
  
  * The first value is “1” if each preset must be prefixed by its preset number, “0” otherwise.
  * The second value is “1” if a directory per bank must be created, “0” otherwise.
  * The third value is “1” if the General MIDI classification must be used to sort presets, “0” otherwise.


### Example


```
polyphone -3 -i /path/to/file.sf3 -c '0|1|1'
```

The conversion creates a directory (named after the output name) containing the sfz files and a sub-directory :file:`samples`. If this directory already exists, a number is added to its name.


## Conversion to .csv {#doc_csv}


### Command line


```
polyphone -4 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**Note:** with Windows you need to write the full path of Polyphone instead of just :command:`polyphone`, which is for example :command:`C:/Program files/Polyphone/polyphone.exe`.


### Arguments


-4
: Mode “4” is “convert to .csv”.

-d
: Output directory in which the input file will be converted.
  This argument is optional, by default this is the same directory as the input file.

-o
: Output name of the converted file without the extension (the extension “.csv” will be automatically added).
  This argument is optional, by default this is the same name as the input file.

-c
: Conversion configuration.
  The option “raw” can be used to export raw values (as they are stored in the soundfont file) rather than values with physical, measurable units. For example, the exported release time will be 0.491 (seconds) or -1229 in its raw form. By default, values with physical units are exported.


### Example


```
polyphone -4 -i /path/to/file.sf2 -c 'raw'
```

The conversion creates a directory (named after the output name) containing the file :file:`samples.csv` and one file per instrument and per preset. If this directory already exists, a number is added to its name.


## Rendering of a MIDI file {#doc_render}


### Command line


```
polyphone -5 -i <soundfont> -c <MIDI/file> -d <output/directory> -o <output_file_name>
```

**Note:** with Windows you need to write the full path of Polyphone instead of just :command:`polyphone`, which is for example :command:`C:/Program files/Polyphone/polyphone.exe`.


### Arguments


-5
: Mode “5” is “render a MIDI file”: the MIDI file is played with the soundfont and the result is written in a wav file.

-d
: Output directory in which the wav file will be written.
  This argument is optional, by default this is the same directory as the MIDI file.

-o
: Output name of the wav file without the extension (the extension “.wav” will be automatically added).
  This argument is optional, by default this is the same name as the MIDI file.

-c
: Path of the MIDI file to play (extension .mid, .midi, .kar or .rmi).


### Example


```
polyphone -5 -i /path/to/file.sf2 -c /path/to/song.mid
```

All channels of the MIDI file play the soundfont, the presets being chosen by the program changes of the file (bank 128 on channel 10, for the percussions).
The settings of the synthesizer are used: [sound](manual/settings.md#doc_sound) (reverb, chorus, general volume), [polyphony](manual/settings.md#doc_general) and [tuning](manual/settings.md#doc_keyboard).
The result is a stereo 32-bit float wav file at 48,000 Hz, ending once the sound of the last notes has faded out. An existing wav file is replaced.
The notes of a preset that the soundfont doesn't have are not played, and a warning is displayed for this preset.


## Conversion to dls {#doc_dls}


### Command line


```
polyphone -6 -i <file/to/convert> -d <output/directory> -o <output_file_name>
```

**Note:** with Windows you need to write the full path of Polyphone instead of just :command:`polyphone`, which is for example :command:`C:/Program files/Polyphone/polyphone.exe`.


### Arguments


-6
: Mode “6” is “convert to dls”.

-d
: Output directory in which the input file will be converted.
  This argument is optional, by default this is the same directory as the input file.

-o
: Output name of the converted file without the extension (the extension “.dls” will be automatically added).
  This argument is optional, by default this is the same name as the input file.


### Example


```
polyphone -6 -i /path/to/file.sf2
```

The file is written in the DLS level 2 format, the limitations of the conversion being described [here](manual/annexes/the-different-soundfont-formats.md#doc_dls).


## Synth mode {#doc_synth}


### Command line


```
polyphone -s -i <file/to/read> -c <configuration>
```

**Note:** with Windows you need to write the full path of Polyphone instead of just :command:`polyphone`, which is for example :command:`C:/Program files/Polyphone/polyphone.exe`.

![Polyphone opened with the synth mode option](images/synth_mode.png "Polyphone opened with the synth mode option")


### Arguments


-s
: Mode “s” is “open Polyphone in synth mode”.

-c
: Synth configuration made of 3 values separated by the “|” character. This argument is optional, by default this is “0|off|off”. These parameters can then be changed via the graphical interface.
  
  * The first value is the MIDI channel number (between 1 and 16) to be listened to for playing. A value of 0 or “all” indicates that all channels will be used and processed as one channel. A value of -1 or “each” listens to each channel separately, which is a prerequisite for [playing MIDI files](tutorials/read-midi-file-with-soundfont.md) for instance.
  * The second value is “off” or “on”, enabling or disabling the selection of multiple presets for playing.
  * The third value is “off”, “on”, or “toggle”, enabling or disabling the selection of presets by the keys corresponding to the preset number (key 0 activates preset 0, for example).


### Example


```
polyphone -s -i /path/to/file.sf2 -c '1|on|toggle'
```


## Other options {#doc_other}


-h
: Displays a short help message.

-r
: Resets the configuration of Polyphone (settings, window geometry, history, etc.).
  This can be useful in case of trouble.

```
polyphone -r
```
