Use these instructions for building Polyphone for Windows with the latest version of the sources on <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>. You will have the software at its most advanced level of development, including the latest features, but perhaps also some instabilities.


## Prerequisite


### Qt installation

Polyphone is written in C++ with the Qt development environment (version 6). To install Qt, go to this <a href="https://www.qt.io/download-qt-installer-oss" target="_blank">page</a> and download the installer. You will need to create an account first.

Then launch the installer and select the following components&nbsp;:

![Components to select](images/select-qt-components.png "Components to select")

Take the latest version available if these choices are not exactly the same.
The component “MinGW 64-bit” of the Qt version provides the Qt libraries built for MinGW.


### Compiler and dependencies installation

Polyphone is built with MinGW for the following reasons:

* there's above all a practical reason, since the dependencies below are compatible with MinGW,
* but also to enable correct import / export of files in sfArk format (the compilation option ``-ffloat-store`` is required).

The compiler, the build tools :program:`CMake` and :program:`Ninja` and the libraries used by Polyphone are installed with <a href="https://www.msys2.org/" target="_blank">MSYS2</a>.
Install it in the default directory :file:`C:/msys64`, open the terminal :program:`MSYS2 MINGW64` and run this command:

```
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-zlib mingw-w64-x86_64-libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-openssl mingw-w64-x86_64-flac mingw-w64-x86_64-libsndfile
```

Then go to Windows system settings to add the folder :file:`C:/msys64/mingw64/bin` to the Path&nbsp;:

* open the Windows menu,
* type "Environment variables",
* open the system properties and click on "Environment variables",
* double-click on "Path" in the system variables,
* and add :file:`C:/msys64/mingw64/bin` to the directories listed in variable "Path".

In doing so, the compiler, the tools and the libraries of MSYS2 can be found by Qt Creator and by Polyphone.


## Build


### Sources

Start by downloading the Polyphone sources <a href="software" target="_blank">here</a> or from <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> and unarchive the files on your Windows desktop, for example.

### Build with Qt Creator

Continue now with this tutorial for building Polyphone with Qt Creator: [Using Qt Creator to build Polyphone](development/using-qt-creator-to-build-polyphone.md).
The kit used must have the compiler of MSYS2 (:file:`C:\msys64\mingw64\bin\g++.exe`), which finds the libraries installed with MSYS2, and the version of Qt for MinGW 64-bit.

### Build with the command line

This method is the one used for building the Windows versions of Polyphone.
Open the terminal :program:`MSYS2 MINGW64`, go to the root of the project (the directory containing :file:`sources`) and run the following commands (adapt the path of Qt beforehand):

```
export PATH="/mingw64/bin:/c/Qt/6.6.2/mingw_64/bin:$PATH"
cmake -S sources -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:/Qt/6.6.2/mingw_64
cmake --build build --parallel 4
```

If compilation went well, you should see the executable file :file:`polyphone.exe` in the directory :file:`build/bin`.


### Launch Polyphone

To enable the executable to run independently, its folder must be completed with the Qt libraries.
Run the following command in a terminal (adapt the paths beforehand):

```
C:\Qt\6.6.2\mingw_64\bin\windeployqt.exe --release "C:\Users\User\Desktop\polyphone\build\bin\polyphone.exe"
```

The Qt libraries will appear (:file:`Qt6Core` for example) and a double-click on file :file:`polyphone.exe` will launch the software.
To enable Polyphone to be launched on another Windows computer, these files from :file:`C:\msys64\mingw64\bin` are also needed to complete the folder:

* libgcc_s_seh-1.dll
* libstdc++-6.dll
* libwinpthread-1.dll
* libssp-0.dll
* libsndfile-1.dll
* libFLAC.dll
* libogg-0.dll
* libvorbis-0.dll
* libvorbisenc-2.dll
* libvorbisfile-3.dll
* libopus-0.dll
* libmp3lame-0.dll
* libmpg123-0.dll
* libcrypto-3-x64.dll
* libssl-3-x64.dll
* zlib1.dll


## Troubleshooting


This forum [category](forum/support-bug-reports) will give you some help if needed.
