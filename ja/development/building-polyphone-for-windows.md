ここでは、<a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>にある最新のソースを使って、Windows向けにPolyphoneをビルドする手順を説明します。最新の機能を含む、開発の最先端の状態のソフトウェアを使えますが、不安定な部分があるかもしれません。


## 前提条件


### Qtのインストール

PolyphoneはC++とQt開発環境（バージョン6）で書かれています。Qtをインストールするには、この<a href="https://www.qt.io/download-qt-installer-oss" target="_blank">ページ</a>に移動してインストーラーをダウンロードします。先にアカウントを作成する必要があります。

次にインストーラーを起動し、次のコンポーネントを選択します。

![選択するコンポーネント](images/select-qt-components.png "選択するコンポーネント")

選択肢がこれとまったく同じでない場合は、利用できる最新のバージョンを選んでください。


### コンパイラーと依存関係のインストール

QtでPolyphoneのソースをコンパイルするには、コンパイラーが必要です。次の理由から、MinGWを使う必要があります。

* 何よりも実用上の理由で、以下の依存ライブラリがMinGWに対応しているためです。
* また、sfArk形式のファイルを正しくインポート/エクスポートできるようにするためでもあります（コンパイルオプション ``-ffloat-store`` が必要です）。

Polyphoneが使うコンパイラーとライブラリは、<a href="https://www.msys2.org/" target="_blank">MSYS2</a>でインストールします。
MSYS2をデフォルトのディレクトリ :file:`C:/msys64` にインストールし（プロジェクトファイル :file:`polyphone.pro` は :file:`C:/msys64/mingw64` でライブラリを探します）、ターミナル :program:`MSYS2 MINGW64` を開いて次のコマンドを実行します。

```
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-zlib mingw-w64-x86_64-libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-openssl mingw-w64-x86_64-flac mingw-w64-x86_64-libsndfile
```

次に、Windowsのシステム設定で、フォルダー :file:`C:/msys64/mingw64/bin` をPathに追加します。

* Windowsメニューを開きます。
* 「環境変数」と入力します。
* システムのプロパティを開き、「環境変数」をクリックします。
* システム環境変数の「Path」をダブルクリックします。
* 変数「Path」に登録されているディレクトリに :file:`C:/msys64/mingw64/bin` を追加します。

こうすることで、QtがMinGWの実行ファイルとライブラリを見つけられるようになります。


## ビルド


### ソース

まず、Polyphoneのソースを<a href="software" target="_blank">こちら</a>または<a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>からダウンロードし、たとえばWindowsのデスクトップに展開します。

### Qt Creatorでビルドする

続いて、Qt CreatorでPolyphoneをビルドするためのチュートリアル「[Qt CreatorでPolyphoneをビルドする](development/using-qt-creator-to-build-polyphone.md)」に進みます。ファイル :file:`polyphone.pro` を変更する必要はありません。

### コマンドラインでビルドする

ターミナル :program:`MSYS2 MINGW64` を開き、QtをPathに追加して、新しいディレクトリでプロジェクトをビルドします（パスは事前に環境に合わせて変更してください）。

```
export PATH="/c/Qt/6.6.2/mingw_64/bin:$PATH"
mkdir build
cd build
qmake ../sources/polyphone.pro CONFIG+=release
mingw32-make -j4
```

コンパイルがうまくいけば、ディレクトリ :file:`build/release` に実行ファイル :file:`polyphone.exe` ができているはずです。


### Polyphoneの起動

実行ファイルを単独で実行できるようにするには、そのフォルダーにQtのライブラリを追加する必要があります。
ターミナルで次のコマンドを実行します（パスは事前に環境に合わせて変更してください）。

```
C:\Qt\6.6.2\mingw_64\bin\windeployqt.exe "C:\Users\User\Desktop\build\release\polyphone.exe"
```

Qtのライブラリ（たとえば :file:`Qt6Core`）が追加され、ファイル :file:`polyphone.exe` をダブルクリックするとソフトウェアが起動します。
別のWindowsコンピューターでもPolyphoneを起動できるようにするには、:file:`C:\msys64\mingw64\bin` にある次のファイルもフォルダーに追加する必要があります。

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


## トラブルシューティング


必要な場合は、フォーラムのこの[カテゴリ](forum/support-bug-reports)が役に立つでしょう。
