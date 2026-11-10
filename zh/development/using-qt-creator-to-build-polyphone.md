按照以下说明，可以了解如何使用 Qt Creator 编译 Polyphone。
这里假定你的电脑上已经安装了带有 Qt 框架的 Qt Creator、编译器以及编译 Polyphone 所需的依赖项（相关内容请参阅对应页面）。


## 前提条件


需要以下组件：

* C++ 编译器（GCC、Clang、MinGW……），
* Qt Creator 及其框架（Qt 6 或 Qt 5），
* :program:`CMake`（3.16 或更高版本），可以随 Qt Creator 一起安装，
* Polyphone 的依赖项。

它们的安装方法取决于你所使用的操作系统，请参阅相应的文档页面。


## 编译 Polyphone


### 复制源代码


在<a href="software" target="_blank">这里</a>或 <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> 上获取 Polyphone 的源代码。


### 打开并配置项目


启动 Qt Creator 并打开位于 :file:`sources` 目录中的 :file:`CMakeLists.txt` 文件。


![打开 Qt Creator](images/open-Qt-Creator.png "打开 Qt Creator")


然后选择一个构建套件（即一个 Qt 版本和一个编译器）来配置项目。

![配置项目](images/configure-project.png "配置项目")


随后 Qt Creator 会运行 :program:`CMake`，检查能否找到各个依赖项。
消息显示在 :guilabel:`General Messages` 面板中：如果缺少某个库，请安装它，然后通过 :guilabel:`Build` 菜单中的 :guilabel:`Run CMake` 菜单项再次运行 :program:`CMake`。

项目的选项位于左侧栏的 :guilabel:`Projects` 部分，在 :guilabel:`Build` → :guilabel:`CMake` 下。
在 Linux 上，可以使用源代码中附带的 :program:`RtAudio` 和 :program:`RtMidi` 副本来代替系统中的库：启用 ``POLYPHONE_LOCAL_RTAUDIO`` 和 ``POLYPHONE_LOCAL_RTMIDI`` 选项，然后单击 :guilabel:`Run CMake`。
在 Windows 和 macOS 上无需这样做，因为始终使用随附的版本。
在同一部分中选择构建配置（建议选择 :guilabel:`Release`）。


### 编译项目


最后，单击 :guilabel:`Build` 菜单中的 :guilabel:`Build Project "Polyphone"` 操作。编译项目时可能会出现一些警告，但不应出现错误。

现在，可执行文件应该位于 Qt Creator 创建的构建目录的 :file:`bin` 子目录中（例如 :file:`sources` 目录中的 :file:`build/Desktop_Qt_6_…-Release`）：

* 在 Linux 上为 :file:`polyphone` 文件，
* 在 Windows 上为 :file:`polyphone.exe` 文件，
* 在 macOS 上为 :file:`Polyphone.app` 应用程序包。


## 故障排除


如果需要更多信息或帮助，请参阅这个[分类](forum/support-bug-reports)。


<p class="endpage">特别感谢 <a href="dashboard/820-tomotello">Tomotello</a> 撰写本文。</p>