如果无法使用“[Software](software)”部分提供的安装程序在你的 Linux 发行版上安装 Polyphone，请参考以下说明。
Polyphone 的 Linux 版本就是在 Ubuntu 24.04 上用这种方法编译的。


## 前提条件


需要以下工具和库：

* 编译器（``build-essential``）
* cmake（``cmake``）和 pkg-config（``pkgconf``）
* qt 6（``qt6-base-dev``、``qt6-svg-dev``、``qt6-tools-dev``、``qt6-tools-dev-tools`` 和 ``qt6-l10n-tools``）
* alsa（``libasound2-dev``）
* jack（``libjack-jackd2-dev``）
* rtaudio（``librtaudio-dev``）
* rtmidi（``librtmidi-dev``）
* vorbis（``libvorbis-dev``）
* ogg（``libogg-dev``）
* flac（``libflac-dev``）
* zlib（``zlib1g-dev``）
* ssl（``libssl-dev``）
* sndfile（``libsndfile1-dev``）

可以用以下命令安装它们：

```
sudo apt-get install build-essential cmake pkgconf qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools libasound2-dev libjack-jackd2-dev librtaudio-dev librtmidi-dev libvorbis-dev libogg-dev libflac-dev zlib1g-dev libssl-dev libsndfile1-dev
```

还需要 Polyphone 的源代码，可以在<a href="software" target="_blank">这里</a>或 <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> 上获取。


## 编译


Polyphone 使用 :program:`CMake` 编译，项目由 :file:`sources` 目录中的 :file:`CMakeLists.txt` 文件描述。
在项目的根目录（包含 :file:`sources` 的目录）中打开终端，运行以下命令：

```
cmake -S sources -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build --parallel 4
```

如果一切顺利，:file:`build/bin` 目录中应该会出现可执行文件 :file:`polyphone`。
可以用 :command:`sudo cmake --install build` 命令将其安装到系统中。

如果你的发行版中缺少 :program:`RtAudio` 或 :program:`RtMidi` 等库，或者其版本不兼容，可以在第一条命令中添加以下选项，改用 Polyphone 源代码中附带的副本：

```
-DPOLYPHONE_LOCAL_RTAUDIO=ON -DPOLYPHONE_LOCAL_RTMIDI=ON
```

这样就不再需要 ``librtaudio-dev`` 和 ``librtmidi-dev`` 软件包。

还可以在第一条命令中添加其他选项：

-DPOLYPHONE_QT_VERSION=5
: 使用 Qt 5 而不是 Qt 6 编译（默认使用找到的最新版本）。
  此时 Qt 5 的软件包为 ``qtbase5-dev``、``libqt5svg5-dev``、``qttools5-dev`` 和 ``qttools5-dev-tools``。

-DPOLYPHONE_BUILD_PLUGIN=ON
: 同时编译[插件](development/building-the-polyphone-plugin.md)。

-DPOLYPHONE_WOLFSSL=ON
: 使用 :program:`wolfSSL` 代替 :program:`OpenSSL`。

**注意：**如果使用 :program:`Qt Creator`，需要通过 :file:`sources` 目录中的 :file:`CMakeLists.txt` 文件打开该项目（参见[使用 Qt Creator 编译 Polyphone](development/using-qt-creator-to-build-polyphone.md)）。
