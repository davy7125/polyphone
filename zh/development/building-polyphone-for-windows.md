按照以下说明，可以使用 <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> 上最新版本的源代码在 Windows 上编译 Polyphone。这样你将得到处于最新开发阶段的软件，包含最新的功能，但也可能存在一些不稳定之处。


## 前提条件


### 安装 Qt

Polyphone 使用 C++ 和 Qt 开发环境（第 6 版）编写。要安装 Qt，请访问这个<a href="https://www.qt.io/download-qt-installer-oss" target="_blank">页面</a>并下载安装程序。需要先创建一个账户。

然后运行安装程序，并选择以下组件：

![要选择的组件](images/select-qt-components.png "要选择的组件")

如果这些选项与图中不完全一致，请选择可用的最新版本。


### 安装编译器和依赖项

Qt 需要一个编译器来编译 Polyphone 的源代码。必须使用 MinGW，原因如下：

* 首先是出于实际考虑，因为下面列出的依赖项都与 MinGW 兼容，
* 其次是为了能够正确导入 / 导出 sfArk 格式的文件（需要编译选项 ``-ffloat-store``）。

编译器以及 Polyphone 所用的库通过 <a href="https://www.msys2.org/" target="_blank">MSYS2</a> 安装。
请将其安装在默认目录 :file:`C:/msys64` 中（项目文件 :file:`polyphone.pro` 会在 :file:`C:/msys64/mingw64` 中查找库），打开 :program:`MSYS2 MINGW64` 终端并运行以下命令：

```
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-zlib mingw-w64-x86_64-libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-openssl mingw-w64-x86_64-flac mingw-w64-x86_64-libsndfile
```

然后进入 Windows 系统设置，将 :file:`C:/msys64/mingw64/bin` 文件夹添加到 Path 中：

* 打开 Windows 菜单，
* 输入“环境变量”，
* 打开系统属性并单击“环境变量”，
* 在系统变量中双击“Path”，
* 将 :file:`C:/msys64/mingw64/bin` 添加到变量“Path”所列的目录中。

这样 Qt 就能找到 MinGW 的可执行文件和库。


## 编译


### 源代码

首先在<a href="software" target="_blank">这里</a>或 <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> 上下载 Polyphone 的源代码，并将文件解压，例如解压到 Windows 桌面上。

### 使用 Qt Creator 编译

接下来请按照这篇教程使用 Qt Creator 编译 Polyphone：[使用 Qt Creator 编译 Polyphone](development/using-qt-creator-to-build-polyphone.md)。无需修改 :file:`polyphone.pro` 文件。

### 使用命令行编译

打开 :program:`MSYS2 MINGW64` 终端，将 Qt 添加到 Path，然后在一个新目录中编译项目（请先修改为你的路径）：

```
export PATH="/c/Qt/6.6.2/mingw_64/bin:$PATH"
mkdir build
cd build
qmake ../sources/polyphone.pro CONFIG+=release
mingw32-make -j4
```

如果编译顺利，你应该能在 :file:`build/release` 目录中看到可执行文件 :file:`polyphone.exe`。


### 启动 Polyphone

为了让可执行文件能够独立运行，需要在其所在文件夹中补充 Qt 库。
在终端中运行以下命令（请先修改为你的路径）：

```
C:\Qt\6.6.2\mingw_64\bin\windeployqt.exe "C:\Users\User\Desktop\build\release\polyphone.exe"
```

Qt 库会出现在文件夹中（例如 :file:`Qt6Core`），双击 :file:`polyphone.exe` 文件即可启动软件。
为了让 Polyphone 能够在另一台 Windows 电脑上启动，还需要将 :file:`C:\msys64\mingw64\bin` 中的以下文件补充到该文件夹中：

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


## 故障排除


如有需要，可以在论坛的这个[分类](forum/support-bug-reports)中获得帮助。
