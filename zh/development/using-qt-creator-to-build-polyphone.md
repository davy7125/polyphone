按照以下说明，可以了解如何使用 Qt Creator 编译 Polyphone。
这里假定你的电脑上已经安装了带有 Qt 框架的 Qt Creator、编译器以及编译 Polyphone 所需的依赖项（相关内容请参阅对应页面）。


## 前提条件


需要以下组件：

* C++ 编译器（VC++、Clang、GCC……），
* Qt Creator 及其框架（Qt 6），
* Polyphone 的依赖项。

它们的安装方法取决于你所使用的操作系统，请参阅相应的文档页面。


## 编译 Polyphone


### 复制源代码


在<a href="software" target="_blank">这里</a>或 <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> 上获取 Polyphone 的源代码。


### 打开并配置项目


首先打开 :file:`polyphone.pro` 文件并进行编辑：如果你的操作系统没有提供 :program:`rtmidi` 和 :program:`stk`，可以改用随附的版本（默认使用随附的 :program:`rtaudio`）。
去掉以下各行开头的‘#’以取消注释：

```
DEFINES += USE_LOCAL_RTMIDI
DEFINES += USE_LOCAL_STK
```

如果想使用电脑上已经安装的库，可以跳过这些修改（请确保这些库已经安装！）。
在 Windows 和 macOS 上无需执行这一步，因为始终使用随附的版本。

启动 Qt Creator 并打开 :file:`polyphone.pro` 文件。


![打开 Qt Creator](images/open-Qt-Creator.png "打开 Qt Creator")


然后配置项目。

![配置项目](images/configure-project.png "配置项目")


### 运行 qmake 并编译项目


进入 :guilabel:`Build` 菜单，选择 :guilabel:`Run qmake` 菜单项。


![运行 qmake](images/run-qmake.png "运行 qmake")


当右下角的进度条变为绿色并达到 100% 时，:command:`qmake` 运行完毕。


![qmake 运行完毕](images/qmake-finished.png "qmake 运行完毕")


在左侧栏的 :guilabel:`Project` 部分，再进入 :guilabel:`Build&Run` 部分，选择构建配置（建议选择 :guilabel:`Release`）。

最后，单击 :guilabel:`Build` 菜单中的 :guilabel:`Build project Polyphone` 操作。编译项目时可能会出现一些警告，但不应出现错误。

现在，根据你在 Qt Creator 构建配置中的选择，可执行文件应该位于 Qt Creator 创建的构建目录中（例如源代码旁边的 :file:`build-XXX`）：

* 在 Linux 上，:file:`polyphone` 文件位于 :file:`bin` 子目录中，
* 在 Windows 上，:file:`polyphone.exe` 文件位于 :file:`release` 或 :file:`debug` 子目录中，
* 在 macOS 上，:file:`Polyphone.app` 应用程序包位于 :file:`sources` 目录旁边的 :file:`macos` 目录中。


## 故障排除


如果需要更多信息或帮助，请参阅这个[分类](forum/support-bug-reports)。


<p class="endpage">特别感谢 <a href="dashboard/820-tomotello">Tomotello</a> 撰写本文。</p>