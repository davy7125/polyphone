如果无法使用“[Software](software)”部分提供的安装程序在你的 Linux 发行版上安装 Polyphone，请参考以下说明。
Polyphone 的 Linux 版本就是在 Ubuntu 24.04 上用这种方法编译的。


## 前提条件


需要以下工具和库：

* 编译器（``build-essential``）
* qt 6（``qt6-base-dev``、``qt6-svg-dev``、``qt6-tools-dev``、``qt6-tools-dev-tools`` 和 ``qt6-l10n-tools``）
* alsa（``libasound2-dev``）
* jack（``libjack-jackd2-dev``）
* rtaudio（``librtaudio-dev``）
* rtmidi（``librtmidi-dev``）
* stk（``libstk-dev``）
* vorbis（``libvorbis-dev``）
* ogg（``libogg-dev``）
* flac（``libflac-dev``）
* zlib（``zlib1g-dev``）
* ssl（``libssl-dev``）
* sndfile（``libsndfile1-dev``）

可以用以下命令安装它们：

```
sudo apt-get install build-essential qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools libasound2-dev libjack-jackd2-dev librtaudio-dev librtmidi-dev libstk-dev libvorbis-dev libogg-dev libflac-dev zlib1g-dev libssl-dev libsndfile1-dev
```

还需要 Polyphone 的源代码，可以在<a href="software" target="_blank">这里</a>或 <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> 上获取。


## 编译


在项目的 :file:`sources` 目录（:file:`polyphone.pro` 文件所在的目录）中打开终端，运行以下命令编译 Polyphone：

```
qmake6 polyphone.pro PREFIX=/usr && make
```

如果一切顺利，:file:`bin` 目录中应该会出现可执行文件 :file:`polyphone`。
可以用 :command:`sudo make install` 命令将其安装到系统中。

默认情况下，会使用 :program:`RtAudio` 库的本地副本。
如果你的发行版中缺少 :program:`RtMidi` 或 :program:`Stk` 等库，或者其版本不兼容，可以编辑 :file:`.pro` 文件来使用它们的本地副本。
为此，取消相应行的注释。例如：

```
#DEFINES += USE_LOCAL_RTMIDI
```

将变为

```
DEFINES += USE_LOCAL_RTMIDI
```

（去掉开头的“#”）。

**注意：**如果使用 :program:`Qt Creator`，可以通过 :file:`sources` 目录中的 :file:`.pro` 文件打开该项目。
