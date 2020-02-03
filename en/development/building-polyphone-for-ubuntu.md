Use these instructions if you cannot install Polyphone on your Linux distribution with the available installers in the section “[Download](https://www.polyphone-soundfonts.com/en/download)”.
This method has been tested with Ubuntu 16.04 (Xenial).


## Prerequisite


The following libraries are required:

* qt (``qt5-default`` + ``libqt5svg5-dev``)
* alsa (``libasound2-dev``)
* jack (``libjack-jackd2-dev``)
* portaudio (``portaudio19-dev``)
* rtmidi (``librtmidi-dev``)
* stk (``libstk0-dev``)
* qcustomplot (``libqcustomplot-dev``)
* vorbis (``libvorbis-dev``)
* ogg (``libogg-dev``)
* flac (``libflac-dev``)
* ssl (``libssl-dev``)

Use for example :program:`synaptic` to install them.


## Build


In the root directory of the project, open a terminal and build Polyphone by running the following command:

```
qmake && make
```

An executable file :file:`polyphone` in the directory :file:`RELEASE` should then appear if everything went right.

In the case where libraries such as RtMidi, Stk, QCustomPlot are missing or are not in a compatible version in your distribution, you can edit the :file:`.pro` file to use local copies of them.
To do this, uncomment the corresponding lines. For instance

```
#DEFINES += USE_LOCAL_RTMIDI
```

will become

```
DEFINES += USE_LOCAL_RTMIDI
```

(with no leading “#”).

**Note:** if you are using :program:`Qt Creator`, the project may be opened via its :file:`.pro` file present in the root directory.