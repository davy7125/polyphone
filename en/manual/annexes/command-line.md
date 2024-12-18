Polyphone can be run with a command line to convert soundfonts in the format [sf2](#doc_sf2), [sf3](#doc_sf3) or [sfz](#doc_sfz). Supported file formats for the conversion are [sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) and [organ](manual/annexes/the-different-soundfont-formats.md#doc_organ).

An option can also be called for opening Polyphone in [synth mode](#doc_synth).


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
  This argument is optional, by default this is the same directory than the input file.

-o
: Output name of the converted file without the extension (the extension “.sf2” will be automatically added).
  This argument is optional, by default this is the same name than the input file.


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
  This argument is optional, by default this is the same directory than the input file.

-o
: Output name of the converted file without the extension (the extension “.sf3” will be automatically added).
  This argument is optional, by default this is the same name than the input file.

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
  This argument is optional, by default this is the same directory than the input file.

-o
: Output name of the converted file without the extension (the extension “.sfz” will be automatically added).
  This argument is optional, by default this is the same name than the input file.

-c
: Conversion configuration made of 3 characters. This argument is optional, by default this is “000”.
  
  * The first character is “1” if each preset must be prefixed by its preset number, “0” otherwise.
  * The second character is “1” if a directory per bank must be created, “0” otherwise.
  * The third character is “1” if the General MIDI classification must be used to sort presets, “0” otherwise.


### Example


```
polyphone -3 -i /path/to/file.sf3 -c 011
```


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
: Synth configuration. The field consists of 3 fields separated by the “|” character. This argument is optional, by default this is “0|off|off”. These parameters can then be changed via the graphical interface.
  
  * The first field is the MIDI channel number (between 1 and 16) to be listened to for playing. A value of 0 or “all” to indicates that all channels will be used and processed as one channel. A value of -1 or “each” listens to each channel separately, which is a prerequisite for [playing MIDI files](tutorials/read-midi-file-with-polyphone.md) for instance.
  * The second field is “off” or “on”, enabling or disabling the selection of multiple presets for playing.
  * The third field is “off”, “on”, or “toggle”, enabling or disabling the selection of presets by the keys corresponding to the preset number (key 0 activates preset 0, for example).


### Example


```
polyphone -s -i /path/to/file.sf2 -c '1|on|toggle'
```
