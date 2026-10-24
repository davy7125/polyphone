Use these instructions to know how to build Polyphone with Qt Creator.
It is assumed you already have on your computer Qt Creator installed with the Qt framework, a compiler and the necessary dependencies to build Polyphone (see related pages for this).


## Prerequisite


The following elements are required:

* a C++ compiler (VC++, Clang, GCC, …),
* Qt Creator with its framework (Qt 6),
* Polyphone dependencies.

The instructions for installing them are dependent on the OS you are using, see the related documentation pages for this.


## Build Polyphone


### Copy the sources


Get Polyphone sources from <a href="software" target="_blank">here</a> or from <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


### Open and configure the project


First, open the file :file:`polyphone.pro` and edit it to possibly use the distributed versions of :program:`rtmidi` and :program:`stk` if your OS doesn't provide them (the distributed version of :program:`rtaudio` is used by default).
Uncomment the following lines by removing the leading ‘#’:

```
DEFINES += USE_LOCAL_RTMIDI
DEFINES += USE_LOCAL_STK
```

You can skip these modifications if you want to use libraries already installed in your computer (make sure you have them!).
This step is not necessary with Windows and macOS, the distributed versions being always used.

Start Qt Creator and open the file :file:`polyphone.pro`.


![Open Qt Creator](images/open-Qt-Creator.png "Open Qt Creator")


Then configure the project.

![Configure project](images/configure-project.png "Configure project")


### Run qmake and build the project


Go in the menu :guilabel:`Build` and choose the item :guilabel:`Run qmake`.


![Run qmake](images/run-qmake.png "Run qmake")


:command:`qmake` run is finished when the progress bar on the bottom right corner has reached 100% in green.


![qmake finished](images/qmake-finished.png "qmake finished")


In the section :guilabel:`Project` in the left column, then in the section :guilabel:`Build&Run`, select the build configuration (:guilabel:`Release` is preferred).

Finally, click on the action :guilabel:`Build project Polyphone` in the menu :guilabel:`Build`. During the project build you may see warnings but there should be no errors.

Now you should have the executable in the build directory created by Qt Creator (:file:`build-XXX` next to the sources, for instance), depending on your choice in Qt Creator build configuration:

* with Linux, the file :file:`polyphone` is in the subdirectory :file:`bin`,
* with Windows, the file :file:`polyphone.exe` is in the subdirectory :file:`release` or :file:`debug`,
* with macOS, the bundle :file:`Polyphone.app` is in the directory :file:`macos` located next to the directory :file:`sources`.


## Troubleshooting


See this [category](forum/support-bug-reports) if you need more information or for getting some help.


<p class="endpage">Special thanks to <a href="dashboard/820-tomotello">Tomotello</a> for this article.</p>