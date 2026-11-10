Use these instructions if you cannot install Polyphone on your Linux distribution with the available installers in the section “[Software](software)”.
This method is the one used for building the Linux versions of Polyphone, on Ubuntu 24.04.


## Prerequisite


The following tools and libraries are required:

* a compiler (``build-essential``)
* cmake (``cmake``) and pkg-config (``pkgconf``)
* qt 6 (``qt6-base-dev``, ``qt6-svg-dev``, ``qt6-tools-dev``, ``qt6-tools-dev-tools`` and ``qt6-l10n-tools``)
* alsa (``libasound2-dev``)
* jack (``libjack-jackd2-dev``)
* rtaudio (``librtaudio-dev``)
* rtmidi (``librtmidi-dev``)
* vorbis (``libvorbis-dev``)
* ogg (``libogg-dev``)
* flac (``libflac-dev``)
* zlib (``zlib1g-dev``)
* ssl (``libssl-dev``)
* sndfile (``libsndfile1-dev``)

They can be installed with the following command:

```
sudo apt-get install build-essential cmake pkgconf qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools libasound2-dev libjack-jackd2-dev librtaudio-dev librtmidi-dev libvorbis-dev libogg-dev libflac-dev zlib1g-dev libssl-dev libsndfile1-dev
```

You also need the sources of Polyphone, available <a href="software" target="_blank">here</a> or on <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


## Build


Polyphone is built with :program:`CMake`, the project being described by the file :file:`CMakeLists.txt` of the directory :file:`sources`.
At the root of the project (the directory containing :file:`sources`), open a terminal and run the following commands:

```
cmake -S sources -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build --parallel 4
```

An executable file :file:`polyphone` in the directory :file:`build/bin` should then appear if everything went right.
It can be installed on the system with the command :command:`sudo cmake --install build`.

In the case where libraries such as :program:`RtAudio` or :program:`RtMidi` are missing or are not in a compatible version in your distribution, the copies included in the sources of Polyphone can be used instead, by adding these options to the first command:

```
-DPOLYPHONE_LOCAL_RTAUDIO=ON -DPOLYPHONE_LOCAL_RTMIDI=ON
```

The packages ``librtaudio-dev`` and ``librtmidi-dev`` are then not necessary.

Other options can be added to the first command:

-DPOLYPHONE_QT_VERSION=5
: Build with Qt 5 instead of Qt 6 (by default, the most recent version found is used).
  The packages of Qt 5 are then ``qtbase5-dev``, ``libqt5svg5-dev``, ``qttools5-dev`` and ``qttools5-dev-tools``.

-DPOLYPHONE_BUILD_PLUGIN=ON
: Also build the [plugin](development/building-the-polyphone-plugin.md).

-DPOLYPHONE_WOLFSSL=ON
: Use :program:`wolfSSL` instead of :program:`OpenSSL`.

**Note:** if you are using :program:`Qt Creator`, the project is opened via the file :file:`CMakeLists.txt` present in the directory :file:`sources` (see [Using Qt Creator to build Polyphone](development/using-qt-creator-to-build-polyphone.md)).
