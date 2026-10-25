如果无法使用“[Software](software)”部分提供的安装程序在 Mac 上安装 Polyphone，请参考以下说明。


## 前提条件


需要以下应用程序：

* <a href="https://www.qt.io/download-open-source" target="_blank">Qt Creator 及其框架</a>（第 6 版），
* Xcode 或仅安装"Command Line Tools"（应使用的版本在 Qt 网站上有说明，例如<a href="https://doc.qt.io/qt-6/supported-platforms.html" target="_blank">这里</a>）。

还需要 Polyphone 的源代码，可以在<a href="software" target="_blank">这里</a>或 <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> 上获取。

要用 Qt Creator 正确打开项目，必须先授予它读取电脑上所有文件的权限（在“隐私与安全性” => “完全磁盘访问权限”中添加位于其安装目录中的“Qt Creator.app”）。

Polyphone 所用的库可以用 <a href="https://brew.sh/" target="_blank">Homebrew</a> 轻松安装。安装此工具后，运行以下命令：

```
brew install libsndfile libogg libvorbis flac lame mpg123 opus openssl
```

项目文件 :file:`polyphone.pro` 会在 :file:`/opt/homebrew`（Apple Silicon）、:file:`/usr/local`（Intel）以及位于 :file:`sources` 目录旁边的 :file:`macos` 目录中查找头文件和库。
也可以解压[这里](files/macos.zip)提供的静态库压缩包来获得 :file:`macos` 目录，但其内容可能已经过时。

如果某些库不是最新的，或者与你的 Mac 版本不兼容（例如 SSL），就需要在网上找到它们的源代码，并启用静态链接重新编译：

```
configure --enable-static --disable-shared --prefix=...
make
make install
```

然后将所有 .h 文件（头文件）复制到 :file:`macos/include`，将所有 .a 文件（静态库）复制到 :file:`macos/lib`。


## 编译


用 :program:`Qt Creator` 打开 :file:`polyphone.pro` 文件（参见[使用 Qt Creator 编译 Polyphone](development/using-qt-creator-to-build-polyphone.md)）并编译项目。
位于 :file:`sources` 目录旁边的 :file:`macos` 目录中应该会出现一个 :file:`Polyphone.app` 应用程序包。

也可以通过命令行进行编译：

```
cd sources
qmake polyphone.pro CONFIG+=release
make -j4
```

如果编译器提示某些功能在指定的 macOS 目标版本中不可用，可以在 :file:`polyphone.pro` 文件中提高以下变量的值（默认值为 12.0）：

```
QMAKE_MACOSX_DEPLOYMENT_TARGET = 12.0
```

该值还需要写入 :file:`packaging/mac/polyphone.plist` 文件中“LSMinimumSystemVersion”的下方。


## 完善应用程序包


运行以下命令，将库和框架打包进应用程序包中（请先修改路径！）：

```
/Path/To/Qt/6.7.3/macos/bin/macdeployqt /Path/To/macos/Polyphone.app
```

在搭载 Apple Silicon 处理器的 Mac 上，应用程序包还必须经过签名（即使是临时签名）才能运行：

```
xattr -cr /Path/To/macos/Polyphone.app
codesign --force --deep --sign - /Path/To/macos/Polyphone.app
```

然后就可以运行该程序，或者将其压缩为 .zip 或 .dmg 文件以便分享。


## 故障排除


如果需要更多信息或帮助，请参阅这个[主题](forum/support-bug-reports/8-success-build-polyphone-on-osx-10-11-6-qt-5-7)。
