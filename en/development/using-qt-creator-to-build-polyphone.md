Use these instructions to know how to build Polyphone with Qt Creator.
It is assumed you already have on your computer Qt Creator installed with the Qt framework, a compiler and the necessary dependencies to build Polyphone (see related pages for this).


## Prerequisite


The following elements are required:

* a C++ compiler (VC++, Clang, GCC, …),
* Qt Creator with its framework,
* Polyphone dependencies.

The instructions for installing them are dependent on the OS you are using, see the related documentation pages for this.


## Build Polyphone


### Copy the sources


Get Polyphone sources from [here][download] or from [Github][polyphone].


### Open and configure the project


First, open the file :file:`polyphone.pro` and edit it to possibly use the distributed versions for :program:`rtmidi`, :program:`stk` and :program:`qcustomplot` if your OS doesn't provide them.
Uncomment the following lines by removing the leading ‘#’:

```
DEFINES += USE_LOCAL_RTMIDI
DEFINES += USE_LOCAL_STK
DEFINES += USE_LOCAL_QCUSTOMPLOT
```

You can skip these modifications if you want to use libraries already installed in your computer (make sure you have them!).

Start Qt Creator and open the file :file:`polyphone.pro`.


![Open Qt Creator](images/open-Qt-Creator.png "Open Qt Creator")


Then configure the project.

![Configure project](images/configure-project.png "Configure project")


### Run qmake and build the project


Go in the menu :guilabel:`Build` and choose the item :guilabel:`Run qmake`.


![Run qmake](images/run-qmake.png "Run qmake")


:command:`qmake` run is finished when the proceeding bar on the bottom right corner has reached the 100% green.


![qmake finished](images/qmake-finished.png "qmake finished")


In the section :guilabel:`Project` in the left column, then in the section :guilabel:`Build&Run`, select the build configuration (:guilabel:`Release` is preferred).

Finally, click on the action :guilabel:`Build project Polyphone` in the menu :guilabel:`Build`.


![Build project](images/build-project.png "Build project")


During project build you should see warnings but no errors.

Now you should have the executable :file:`polyphone` either in the :file:`RELEASE` or :file:`DEBUG` subfolder of the source code, depending on your choice in QtCreator build configuration.


## Troubleshooting


See this [topic](forum/polyphone/support-bug-reports) if you need more information or for getting some help.


<p class="endpage">Special thanks to <a href="dashboard/820-tomotello">Tomotello</a> for this article.</p>


[download]:  download                              {target=_blank}
[polyphone]: https://github.com/davy7125/polyphone {target=_blank}
