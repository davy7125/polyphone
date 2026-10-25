「[ソフトウェア](software)」セクションで提供されているインストーラーでFedoraディストリビューションにPolyphoneをインストールできない場合は、この手順に従ってください。


## 前提条件


### Qtのインストール

Fedoraの基本インストールの後、Qt開発の前提条件として、次のパッケージ（コンパイラーを含む）をインストールします。

```
sudo dnf update
sudo dnf install @development-tools
```

次に、以下のコマンドでQt 6フレームワークをインストールします。

```
sudo dnf install qt-creator qt6-qtbase-devel qt6-qtsvg-devel qt6-qttools-devel
```


### Polyphoneの依存関係

次の依存関係をインストールします（グラフィカルなパッケージマネージャーがインストールされている場合は、それを使ってインストールしてもかまいません）。

```
sudo dnf install alsa-lib-devel jack-audio-connection-kit-devel rtaudio-devel zlib-devel libogg-devel flac-devel libvorbis-devel glib2-devel openssl-devel rtmidi-devel stk-devel libsndfile-devel
```

**注意：**最近のバージョンのFedoraでは、パッケージ :program:`jack-audio-connection-kit-devel` が :program:`pipewire-jack-audio-connection-kit-devel` に置き換えられている場合があります。


## ビルド


### ソース


まず、Polyphoneのソースを<a href="software" target="_blank">こちら</a>または<a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>から入手します。

Qt Creatorでビルドを続ける場合は、ここでチュートリアル「[Qt Creatorでビルドする](development/using-qt-creator-to-build-polyphone.md)」に進んでください。そうでない場合は、このまま読み進めてください。


### 実行ファイルの作成


ソースディレクトリ（ファイル :file:`polyphone.pro` がある場所）に移動してターミナルを開き、次のコマンドでPolyphoneをビルドします。

```
qmake6 && make
```

すべてうまくいけば、サブディレクトリ :file:`bin` にコンパイル済みのファイル :file:`polyphone` ができます。
Qt Creatorでビルドした場合、このファイルは :file:`sources` の隣に作成されるディレクトリ :file:`build-XXX` にあります。


## トラブルシューティング


必要な場合は、フォーラムのこの[カテゴリ](forum/support-bug-reports)が役に立つでしょう。
