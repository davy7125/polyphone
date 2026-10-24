Use these instructions if you cannot install Polyphone on your Linux distribution with the available installers in the section “[Software](software)”.
This method is the one used for building the Linux versions of Polyphone, on Ubuntu 24.04.


## Prerequisite


The following tools and libraries are required:

* a compiler (``build-essential``)
* qt 6 (``qt6-base-dev``, ``qt6-svg-dev``, ``qt6-tools-dev``, ``qt6-tools-dev-tools`` and ``qt6-l10n-tools``)
* alsa (``libasound2-dev``)
* jack (``libjack-jackd2-dev``)
* rtaudio (``librtaudio-dev``)
* rtmidi (``librtmidi-dev``)
* stk (``libstk-dev``)
* vorbis (``libvorbis-dev``)
* ogg (``libogg-dev``)
* flac (``libflac-dev``)
* zlib (``zlib1g-dev``)
* ssl (``libssl-dev``)
* sndfile (``libsndfile1-dev``)

They can be installed with the following command:

```
sudo apt-get install build-essential qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools libasound2-dev libjack-jackd2-dev librtaudio-dev librtmidi-dev libstk-dev libvorbis-dev libogg-dev libflac-dev zlib1g-dev libssl-dev libsndfile1-dev
```

You also need the sources of Polyphone, available <a href="software" target="_blank">here</a> or on <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


## Build


In the directory :file:`sources` of the project (where the file :file:`polyphone.pro` is located), open a terminal and build Polyphone by running the following command:

```
qmake6 polyphone.pro PREFIX=/usr && make
```

An executable file :file:`polyphone` in the directory :file:`bin` should then appear if everything went right.
It can be installed on the system with the command :command:`sudo make install`.

By default, a local copy of the library :program:`RtAudio` is used.
In the case where libraries such as :program:`RtMidi` or :program:`Stk` are missing or are not in a compatible version in your distribution, you can edit the :file:`.pro` file to use local copies of them.
To do this, uncomment the corresponding lines. For instance

```
#DEFINES += USE_LOCAL_RTMIDI
```

will become

```
DEFINES += USE_LOCAL_RTMIDI
```

(with no leading “#”).

**Note:** if you are using :program:`Qt Creator`, the project may be opened via its :file:`.pro` file present in the directory :file:`sources`.
