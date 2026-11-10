Use these instructions to know how to build Polyphone with Qt Creator.
It is assumed you already have on your computer Qt Creator installed with the Qt framework, a compiler and the necessary dependencies to build Polyphone (see related pages for this).


## Prerequisite


The following elements are required:

* a C++ compiler (GCC, Clang, MinGW…),
* Qt Creator with its framework (Qt 6, or Qt 5),
* :program:`CMake` (version 3.16 or more), which can be installed with Qt Creator,
* Polyphone dependencies.

The instructions for installing them are dependent on the OS you are using, see the related documentation pages for this.


## Build Polyphone


### Copy the sources


Get Polyphone sources from <a href="software" target="_blank">here</a> or from <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


### Open and configure the project


Start Qt Creator and open the file :file:`CMakeLists.txt` located in the directory :file:`sources`.


![Open Qt Creator](images/open-Qt-Creator.png "Open Qt Creator")


Then configure the project by choosing a kit, that is to say a version of Qt and a compiler.

![Configure project](images/configure-project.png "Configure project")


Qt Creator then runs :program:`CMake`, which checks that the dependencies are found.
The messages are displayed in the panel :guilabel:`General Messages`: if a library is missing, install it and run :program:`CMake` again with the item :guilabel:`Run CMake` of the menu :guilabel:`Build`.

The options of the project are in the section :guilabel:`Projects` of the left column, under :guilabel:`Build` → :guilabel:`CMake`.
On Linux, the copies of :program:`RtAudio` and :program:`RtMidi` included in the sources can be used instead of the libraries of the system: enable the options ``POLYPHONE_LOCAL_RTAUDIO`` and ``POLYPHONE_LOCAL_RTMIDI``, then click on :guilabel:`Run CMake`.
This is not necessary with Windows and macOS, the distributed versions being always used.
In the same section, select the build configuration (:guilabel:`Release` is preferred).


### Build the project


Finally, click on the action :guilabel:`Build Project "Polyphone"` in the menu :guilabel:`Build`. During the project build you may see warnings but there should be no errors.

Now you should have the executable in the subdirectory :file:`bin` of the build directory created by Qt Creator (:file:`build/Desktop_Qt_6_…-Release` in the directory :file:`sources`, for instance):

* with Linux, the file :file:`polyphone`,
* with Windows, the file :file:`polyphone.exe`,
* with macOS, the bundle :file:`Polyphone.app`.


## Troubleshooting


See this [category](forum/support-bug-reports) if you need more information or for getting some help.


<p class="endpage">Special thanks to <a href="dashboard/820-tomotello">Tomotello</a> for this article.</p>
