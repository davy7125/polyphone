Polyphone can be run with a command line to convert soundfonts in the format [sf2](#doc_sf2), [sf3](#doc_sf3) or [sfz](#doc_sfz).

Supported file formats for the conversion are [sf2], [sf3], [sfz] and [sfArk].


## Conversion to sf2 {#doc_sf2}


### Command line


```
polyphone -1 -i <file/to/convert> -d <output/directory> -o <output/file/name>
```

<!-- note begin -->
**Note:** with Windows you need to write the full path of Polyphone instead of just :command:`polyphone`, which is for example :command:`C:/Program files/Polyphone/polyphone.exe`.
<!-- note end -->


### Arguments


* **-1**\
  Mode “1” is “convert to sf2”.
* **-d**\
  Output directory in which the input file will be converted.
  This argument is optional, by default this is the same directory than the input file.
* **-o**\
  Output name of the converted file without the extension (the extension “.sf2” will be automatically added).
  This argument is optional, by default this is the same name than the input file.


### Example


```
polyphone -1 -i /path/to/file.sfArk
```


## Conversion to sf3 {#doc_sf3}


### Command line


```
polyphone -2 -i <file/to/convert> -d <output/directory> -o <output/file/name> -c [config]
```

<!-- note begin -->
**Note:** with Windows you need to write the full path of Polyphone instead of just :command:`polyphone`, which is for example :command:`C:/Program files/Polyphone/polyphone.exe`.
<!-- note end -->


### Arguments


* **-2**\
  Mode “2” is “convert to sf3”.
* **-d**
  Output directory in which the input file will be converted.
  This argument is optional, by default this is the same directory than the input file.
* **-o**\
  Output name of the converted file without the extension (the extension “.sf3” will be automatically added).
  This argument is optional, by default this is the same name than the input file.
* **-c**\
  Conversion configuration.
  It is possible to specify the compression quality: “0” is low, “1” is medium, “2” is high.
  This argument is optional, by default this is “1” (medium quality).


### Example


```
polyphone -2 -i /path/to/file.sf2 -c 2
```


## Conversion to sfz {#doc_sfz}


### Command line


```
polyphone -3 -i <file/to/convert> -d <output/directory> -o <output/file/name> -c [config]
```

<!-- note begin -->
**Note:** with Windows you need to write the full path of Polyphone instead of just :command:`polyphone`, which is for example :command:`C:/Program files/Polyphone/polyphone.exe`.
<!-- note end -->


### Arguments


* **-3**\
  Mode “3” is “convert to sfz”.
* **-d**\
  Output directory in which the input file will be converted.
  This argument is optional, by default this is the same directory than the input file.
* **-o**\
  Output name of the converted file without the extension (the extension “.sfz” will be automatically added).
  This argument is optional, by default this is the same name than the input file.
* **-c**\
  Conversion configuration made of 3 characters. The first character is “1” if each preset must be prefixed by its preset number, “0” otherwise. The second character is “1” if a directory per bank must be created, “0” otherwise. The third character is “1” if the General MIDI classification must be used to sort presets, “0” otherwise.
  This argument is optional, by default this is “000”.


### Example


```
polyphone -3 -i /path/to/file.sf3 -c 011
```


[sf2]:   manual/annexes/the-different-soundfont-formats.md#doc_sf2
[sf3]:   manual/annexes/the-different-soundfont-formats.md#doc_sf3
[sfArk]: manual/annexes/the-different-soundfont-formats.md#doc_sfark
[sfz]:   manual/annexes/the-different-soundfont-formats.md#doc_sfz
