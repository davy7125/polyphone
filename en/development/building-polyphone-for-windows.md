Use these instructions for building Polyphone for Windows with the latest version of the sources on <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>. You will have the software at its most advanced level of development, including the latest features, but perhaps also some instabilities.


## Prerequisite


### Qt installation

Polyphone is written in C++ with the Qt development environment. To install Qt, go to this <a href="https://www.qt.io/download-qt-installer-oss" target="_blank">page</a> and download the installer. You will need to create an account first.

Then launch the installer and select the following components&nbsp;:

![Components to select](images/select-qt-components.png "Components to select")

Take the latest version available if these choices are not exactly the same.


### Compiler installation

Qt needs a compiler to compile Polyphone sources. It is necessary to use MinGW for the following reasons:

* there's above all a practical reason, since the dependencies below are compatible with MinGW
* but also to enable correct import/export of files in sfArk format.

You can download MinGW on this <a href="https://wiki.qt.io/MinGW" target="_blank">page</a>, taking the version corresponding to what has been selected in the previous paragraph.

Unarchive the whole in directory :file:`C:/mingw64` for instance.

Then go to Windows system settings to add the folder :file:`C:/mingw64/bin` to the Path&nbsp;:

* open the Windows menu,
* type "Environment variables",
* open the system properties and click on "Environment variables",
* double-click on "Path" in the system variables,
* and add :file:`C:/mingw64/bin` to the directories listed in variable "Path".

In doing so, mingw64 executables can be called by Qt.


### Polyphone dependencies

Download the project dependencies available [here](files/lib_windows.zip). These are additional elements used by Polyphone that are not supplied by Qt.


## Build


### Sources

Start by downloading the Polyphone sources <a href="download" target="_blank">here</a> or from <a href="https://github.com/davy7125/polyphone" target="_blank">Github</a> and unarchive the files on your Windows desktop, for example.

Then, unarchive the additional Polyphone dependencies in directory "lib_windows" that must be next to the "sources" folder.

Continue now with this tutorial for building Polyphone with Qt Creator: [Using Qt Creator to build Polyphone](development/using-qt-creator-to-build-polyphone.md). No modification of the file :file:`polyphone.pro` will be necessary.

### Launch Polyphone without Qt Creator

Go to the directory :file:`lib_windows/64bits`. If compilation went well, you should see the executable file :file:`polyphone.exe`. To enable it to run independently, this folder must be completed. Run the following command in a terminal (adapt the paths beforehand):

```
C:\Qt\6.7.2\mingw_64\bin\windeployqt.exe "C:\Users\User\Desktop\polyphone\lib_windows\64bits\polyphone.exe"
```

The Qt libraries will appear (:file:`Qt6Core` for example) and a double-click on file :file:`polyphone.exe` will launch the software.
To enable Polyphone to be launched on another Windows computer, these files from :file:`C:\mingw64\bin` are also needed to complete the folder:

* libgcc_s_seh-1.dll
* libssp-0.dll
* libstdc++-6.dll
* libwinpthread-1.dll


## Troubleshooting


This forum [category](forum/support-bug-reports) will give you some help if needed.