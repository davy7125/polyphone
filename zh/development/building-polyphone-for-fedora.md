如果无法使用“[Software](software)”部分提供的安装程序在 Fedora 发行版上安装 Polyphone，请参考以下说明。


## 前提条件


### 安装 Qt

在 Fedora 基础安装完成后，安装以下软件包（包括编译器），作为 Qt 开发的前提条件：

```
sudo dnf update
sudo dnf install @development-tools
```

然后用以下命令安装 Qt 6 框架：

```
sudo dnf install qt-creator qt6-qtbase-devel qt6-qtsvg-devel qt6-qttools-devel
```


### Polyphone 的依赖项

安装以下依赖项（如果安装了图形化的软件包管理器，也可以用它来安装）：

```
sudo dnf install alsa-lib-devel jack-audio-connection-kit-devel rtaudio-devel zlib-devel libogg-devel flac-devel libvorbis-devel glib2-devel openssl-devel rtmidi-devel stk-devel libsndfile-devel
```

**注意：**在较新版本的 Fedora 中，:program:`jack-audio-connection-kit-devel` 软件包可能会被 :program:`pipewire-jack-audio-connection-kit-devel` 取代。


## 编译


### 源代码


首先在<a href="software" target="_blank">这里</a>或 <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> 上获取 Polyphone 的源代码。

如果想用 Qt Creator 继续编译，请按照这篇教程操作：[使用 Qt Creator 编译](development/using-qt-creator-to-build-polyphone.md)。否则，请继续阅读本文档。


### 生成可执行文件


进入源代码目录（:file:`polyphone.pro` 文件所在的目录），打开终端，用以下命令编译 Polyphone：

```
qmake6 && make
```

如果一切顺利，:file:`bin` 子目录中会生成编译好的 :file:`polyphone` 文件。
如果是用 Qt Creator 编译的，该文件则位于 :file:`sources` 旁边新建的 :file:`build-XXX` 目录中。


## 故障排除


如有需要，可以在论坛的这个[分类](forum/support-bug-reports)中获得帮助。
