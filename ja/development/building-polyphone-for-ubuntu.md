「[ソフトウェア](software)」セクションで提供されているインストーラーでLinuxディストリビューションにPolyphoneをインストールできない場合は、この手順に従ってください。
この方法は、Ubuntu 24.04でLinux版のPolyphoneをビルドするときに実際に使っているものです。


## 前提条件


次のツールとライブラリが必要です。

* コンパイラー（``build-essential``）
* cmake（``cmake``）とpkg-config（``pkgconf``）
* qt 6（``qt6-base-dev``、``qt6-svg-dev``、``qt6-tools-dev``、``qt6-tools-dev-tools``、``qt6-l10n-tools``）
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

これらは次のコマンドでインストールできます。

```
sudo apt-get install build-essential cmake pkgconf qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools libasound2-dev libjack-jackd2-dev librtaudio-dev librtmidi-dev libvorbis-dev libogg-dev libflac-dev zlib1g-dev libssl-dev libsndfile1-dev
```

また、<a href="software" target="_blank">こちら</a>または<a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>で入手できるPolyphoneのソースも必要です。


## ビルド


Polyphoneは :program:`CMake` でビルドします。プロジェクトはディレクトリ :file:`sources` のファイル :file:`CMakeLists.txt` に記述されています。
プロジェクトのルート（:file:`sources` を含むディレクトリ）でターミナルを開き、次のコマンドを実行します。

```
cmake -S sources -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build --parallel 4
```

すべてうまくいけば、ディレクトリ :file:`build/bin` に実行ファイル :file:`polyphone` ができるはずです。
コマンド :command:`sudo cmake --install build` でシステムにインストールできます。

:program:`RtAudio` や :program:`RtMidi` などのライブラリがお使いのディストリビューションにない場合や、互換性のあるバージョンでない場合は、最初のコマンドに次のオプションを追加すると、代わりにPolyphoneのソースに含まれているコピーを使えます。

```
-DPOLYPHONE_LOCAL_RTAUDIO=ON -DPOLYPHONE_LOCAL_RTMIDI=ON
```

この場合、パッケージ ``librtaudio-dev`` と ``librtmidi-dev`` は必要ありません。

最初のコマンドには、ほかにも次のオプションを追加できます。

-DPOLYPHONE_QT_VERSION=5
: Qt 6の代わりにQt 5でビルドします（デフォルトでは、見つかった最新のバージョンが使われます）。
  この場合、Qt 5のパッケージは ``qtbase5-dev``、``libqt5svg5-dev``、``qttools5-dev``、``qttools5-dev-tools`` です。

-DPOLYPHONE_BUILD_PLUGIN=ON
: [プラグイン](development/building-the-polyphone-plugin.md)もビルドします。

-DPOLYPHONE_WOLFSSL=ON
: :program:`OpenSSL` の代わりに :program:`wolfSSL` を使います。

**注意：**:program:`Qt Creator` を使っている場合は、ディレクトリ :file:`sources` にあるファイル :file:`CMakeLists.txt` からプロジェクトを開きます（「[Qt CreatorでPolyphoneをビルドする](development/using-qt-creator-to-build-polyphone.md)」を参照）。
