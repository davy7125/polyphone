「[ソフトウェア](software)」セクションで提供されているインストーラーでLinuxディストリビューションにPolyphoneをインストールできない場合は、この手順に従ってください。
この方法は、Ubuntu 24.04でLinux版のPolyphoneをビルドするときに実際に使っているものです。


## 前提条件


次のツールとライブラリが必要です。

* コンパイラー（``build-essential``）
* qt 6（``qt6-base-dev``、``qt6-svg-dev``、``qt6-tools-dev``、``qt6-tools-dev-tools``、``qt6-l10n-tools``）
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

これらは次のコマンドでインストールできます。

```
sudo apt-get install build-essential qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools libasound2-dev libjack-jackd2-dev librtaudio-dev librtmidi-dev libstk-dev libvorbis-dev libogg-dev libflac-dev zlib1g-dev libssl-dev libsndfile1-dev
```

また、<a href="software" target="_blank">こちら</a>または<a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>で入手できるPolyphoneのソースも必要です。


## ビルド


プロジェクトのディレクトリ :file:`sources`（ファイル :file:`polyphone.pro` がある場所）でターミナルを開き、次のコマンドを実行してPolyphoneをビルドします。

```
qmake6 polyphone.pro PREFIX=/usr && make
```

すべてうまくいけば、ディレクトリ :file:`bin` に実行ファイル :file:`polyphone` ができるはずです。
コマンド :command:`sudo make install` でシステムにインストールできます。

デフォルトでは、ライブラリ :program:`RtAudio` のローカルコピーが使われます。
:program:`RtMidi` や :program:`Stk` などのライブラリがお使いのディストリビューションにない場合や、互換性のあるバージョンでない場合は、:file:`.pro` ファイルを編集して、それらのローカルコピーを使うようにできます。
そのためには、対応する行のコメントを解除します。たとえば

```
#DEFINES += USE_LOCAL_RTMIDI
```

は、次のようになります。

```
DEFINES += USE_LOCAL_RTMIDI
```

（先頭の「#」を削除します）

**注意：**:program:`Qt Creator` を使っている場合は、ディレクトリ :file:`sources` にある :file:`.pro` ファイルからプロジェクトを開けます。
