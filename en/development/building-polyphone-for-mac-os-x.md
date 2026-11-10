Use these instructions if you cannot install Polyphone on your Mac with the available installers in the section “[Software](software)”.


## Prerequisite


The following applications are required:

* <a href="https://www.qt.io/download-open-source" target="_blank">Qt Creator with its framework</a> (version 6),
* Xcode or just the "Command Line Tools" (the version to use is specified on the Qt website — <a href="https://doc.qt.io/qt-6/supported-platforms.html" target="_blank">here</a> for instance).

You also need the sources of Polyphone, available <a href="software" target="_blank">here</a> or on <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.

For correctly opening a project with Qt Creator, it must first be given the permission to read all files from your computer (in "Privacy & Security" => "Full Disk Access" => add "Qt Creator.app" which is in its installation directory).

The build tool :program:`CMake` and the libraries used by Polyphone are easily installed with <a href="https://brew.sh/" target="_blank">Homebrew</a>. After installing this tool, run this command:

```
brew install cmake pkgconf libsndfile libogg libvorbis flac lame mpg123 opus openssl
```

The file :file:`CMakeLists.txt` of the directory :file:`sources` looks for the headers and the libraries in :file:`/opt/homebrew` (Apple Silicon), in :file:`/usr/local` (Intel) and in the directory :file:`macos` located next to the directory :file:`sources`.
The archive of static libraries available [here](files/macos.zip) can also be unzipped for obtaining the directory :file:`macos`, but its content may be out of date.

If some libraries are not up to date or not compatible with the version of your Mac, like SSL for example, it will be necessary to look for their sources on the internet and to recompile them by enabling a static linking:

```
configure --enable-static --disable-shared --prefix=...
make
make install
```

Then copy all .h (headers) in :file:`macos/include` and all .a (static libraries) in :file:`macos/lib`.


## Build


Open the file :file:`CMakeLists.txt` of the directory :file:`sources` with :program:`Qt Creator` (see [Using Qt Creator to build Polyphone](development/using-qt-creator-to-build-polyphone.md)) and build the project.
A bundle :file:`Polyphone.app` should appear in the subdirectory :file:`bin` of the build directory.

The build can also be done with the command line, at the root of the project (the directory containing :file:`sources`), by giving the path of Qt (adjust it first!):

```
cmake -S sources -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/Path/To/Qt/6.7.3/macos
cmake --build build --parallel 4
```

The bundle :file:`Polyphone.app` is then in the directory :file:`build/bin`.

If the compiler is complaining that some features are not available with the specified macOS target, you can increase the minimum version of macOS by adding this option to the first command (the default value is 12.0):

```
-DCMAKE_OSX_DEPLOYMENT_TARGET=13.0
```

This value is also to be written in file :file:`packaging/mac/polyphone.plist` below "LSMinimumSystemVersion".


## Complete the bundle


Run the following command to include libraries and frameworks inside the bundle (adjust the path first!):

```
/Path/To/Qt/6.7.3/macos/bin/macdeployqt /Path/To/build/bin/Polyphone.app
```

On a Mac with an Apple Silicon processor, the bundle must also be signed (even with an ad-hoc signature) to be able to run:

```
xattr -cr /Path/To/build/bin/Polyphone.app
codesign --force --deep --sign - /Path/To/build/bin/Polyphone.app
```

You can then execute the program or compress it as a .zip or .dmg file to share it.


## Troubleshooting


See this [topic](forum/support-bug-reports/8-success-build-polyphone-on-osx-10-11-6-qt-5-7) if you need more information or for getting some help.
