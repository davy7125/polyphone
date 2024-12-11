Use these instructions if you cannot install Polyphone on your Mac with the available installers in the section “[Download](download)”.


## Prerequisite


The following applications are required:

* <a href="https://www.qt.io/download-open-source" target="_blank">Qt Creator with its framework</a>,
* Xcode or just the "Command Line Tools" (the version to use is specified on the Qt website - <a href="https://doc.qt.io/qt-6/supported-platforms.html" target="_blank">here</a> for instance).

You also need the sources of Polyphone, available <a href="download" target="_blank">here</a> or on <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>, along with the necessary libraries you can download [here](files/macos.zip).

For correctly opening a project with Qt Creator, it must first be given the permission to read all files from your computer (in "Security and privacy" => "Full disk access" => add "Qt creator.app" which is in its installation directory).

If some of the previously provided libraries are not up to date or not compatible with the version of Mac, like SSL for example, it will be necessary to look for their sources on the internet and to recompile them by enabling a static linking:
```
configure --enable-static --disable-shared --prefix=...
make
make install
```
Then copy all .h (headers) and .a (static libraries).

Another way to get the libraries already built is to use <a href="https://brew.sh/" target="_blank">Homebrew</a>. After installing this tool, it is possible to get "libsndfile" along with its dependencies with this command:
```
arch -x86_64 brew install libsndfile
```
The .h and .a files will then be located in "/usr/local/Cellar".


## Build


Unzip the libraries and place the directory :file:`macos` just next to the directory :file:`sources`.

Open the file :file:`polyphone.pro` with :program:`Qt Creator`.
Build the project, a bundle :file:`polyphone.app` should appear in the directory :file:`lib_mac`.

If the SDK path cannot be resolved, try modifying this variable in file :file:`polyphone.pro` for matching your SDK version (linked to the XCode version):
```
QMAKE_MAC_SDK = macosx10.15
```

If the compiler is now complaining that some features are not available with the specified MacOS target, you can increase this variable in the .pro file:
```
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15
```
This value is also to be written in file "sources/contrib/mac/polyphone.plist" below "LSMinimumSystemVersion".


## Complete the bundle


Run the following command to include libraries and frameworks inside the bundle (adjust the path first!):

```
/Path/To/Qt/5.2.0/clang_64/bin/macdeployqt /Path/To/lib_mac/polyphone.app
```

You can then execute the program or compress it as a .zip or .dmg file to share it.


## Troubleshooting


See this [topic](forum/support-bug-reports/8-success-build-polyphone-on-osx-10-11-6-qt-5-7) if you need more information or for getting some help.
