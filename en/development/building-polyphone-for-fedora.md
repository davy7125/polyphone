Use these instructions if you cannot install Polyphone on your Fedora distribution with the available installers in the section “[Software](software)”.


## Prerequisite


### Qt installation

After a base installation of Fedora, install the following packages (including a compiler) as pre-requisites for Qt development:

```
sudo dnf update
sudo dnf install @development-tools
```

Then install the Qt 6 framework with the following command:

```
sudo dnf install qt-creator qt6-qtbase-devel qt6-qtsvg-devel qt6-qttools-devel
```


### Polyphone dependencies

Install the following dependencies (if you have a graphical package manager installed you could alternatively use it for installation):

```
sudo dnf install alsa-lib-devel jack-audio-connection-kit-devel rtaudio-devel zlib-devel libogg-devel flac-devel libvorbis-devel glib2-devel openssl-devel rtmidi-devel stk-devel libsndfile-devel
```

**Note:** with recent versions of Fedora, the package :program:`jack-audio-connection-kit-devel` may be replaced by :program:`pipewire-jack-audio-connection-kit-devel`.


## Build


### Sources


First, get Polyphone sources from <a href="software" target="_blank">here</a> or from <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.

If you want to continue the build with Qt Creator, follow now this tutorial: [build with Qt Creator](development/using-qt-creator-to-build-polyphone.md). Otherwise, continue the reading of this document.


### Getting the executable


Go into source directory (where the file :file:`polyphone.pro` is located), open a terminal and build Polyphone with this command:

```
qmake6 && make
```

If everything goes well you will have the compiled file :file:`polyphone` in the subdirectory :file:`bin`.
When the build is done with Qt Creator, this file is rather located in the directory :file:`build-XXX` created next to :file:`sources`.


## Troubleshooting


This forum [category](forum/support-bug-reports) will give you some help if needed.
