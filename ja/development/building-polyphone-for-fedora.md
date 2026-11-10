「[ソフトウェア](software)」セクションで提供されているインストーラーでFedoraディストリビューションにPolyphoneをインストールできない場合は、この手順に従ってください。


## 前提条件


### Qtのインストール

Fedoraの基本インストールの後、Qt開発の前提条件として、次のパッケージ（コンパイラーと :program:`CMake` を含む）をインストールします。

```
sudo dnf update
sudo dnf install @development-tools cmake pkgconf
```

次に、以下のコマンドでQt 6フレームワークをインストールします。

```
sudo dnf install qt-creator qt6-qtbase-devel qt6-qtsvg-devel qt6-qttools-devel
```


### Polyphoneの依存関係

次の依存関係をインストールします（グラフィカルなパッケージマネージャーがインストールされている場合は、それを使ってインストールしてもかまいません）。

```
sudo dnf install alsa-lib-devel jack-audio-connection-kit-devel rtaudio-devel zlib-devel libogg-devel flac-devel libvorbis-devel glib2-devel openssl-devel rtmidi-devel libsndfile-devel
```

**注意：**最近のバージョンのFedoraでは、パッケージ :program:`jack-audio-connection-kit-devel` が :program:`pipewire-jack-audio-connection-kit-devel` に置き換えられている場合があります。


## ビルド


### ソース


まず、Polyphoneのソースを<a href="software" target="_blank">こちら</a>または<a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>から入手します。

Qt Creatorでビルドを続ける場合は、ここでチュートリアル「[Qt Creatorでビルドする](development/using-qt-creator-to-build-polyphone.md)」に進んでください。そうでない場合は、このまま読み進めてください。


### 実行ファイルの作成


Polyphoneは :program:`CMake` でビルドします。プロジェクトはディレクトリ :file:`sources` のファイル :file:`CMakeLists.txt` に記述されています。
プロジェクトのルート（:file:`sources` を含むディレクトリ）でターミナルを開き、次のコマンドでPolyphoneをビルドします。

```
cmake -S sources -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 4
```

すべてうまくいけば、ディレクトリ :file:`build/bin` にコンパイル済みのファイル :file:`polyphone` ができます。
Qt Creatorでビルドした場合、このファイルはQt Creatorで選んだビルドディレクトリのサブディレクトリ :file:`bin` にあります。

:program:`RtAudio` や :program:`RtMidi` の互換性のあるバージョンが利用できない場合は、最初のコマンドにオプション ``-DPOLYPHONE_LOCAL_RTAUDIO=ON -DPOLYPHONE_LOCAL_RTMIDI=ON`` を追加すると、Polyphoneのソースに含まれているコピーを使えます。
その他のオプションについては、ページ「[Ubuntu向けにPolyphoneをビルドする](development/building-polyphone-for-ubuntu.md)」で説明しています。


## トラブルシューティング


必要な場合は、フォーラムのこの[カテゴリ](forum/support-bug-reports)が役に立つでしょう。
