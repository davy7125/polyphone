ここでは、<a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>にある最新のソースを使って、Windows向けにPolyphoneをビルドする手順を説明します。最新の機能を含む、開発の最先端の状態のソフトウェアを使えますが、不安定な部分があるかもしれません。


## 前提条件


### Qtのインストール

PolyphoneはC++とQt開発環境（バージョン6）で書かれています。Qtをインストールするには、この<a href="https://www.qt.io/download-qt-installer-oss" target="_blank">ページ</a>に移動してインストーラーをダウンロードします。先にアカウントを作成する必要があります。

次にインストーラーを起動し、次のコンポーネントを選択します。

![選択するコンポーネント](images/select-qt-components.png "選択するコンポーネント")

選択肢がこれとまったく同じでない場合は、利用できる最新のバージョンを選んでください。
Qtのバージョンのコンポーネント「MinGW 64-bit」で、MinGW向けにビルドされたQtのライブラリが入ります。


### コンパイラーと依存関係のインストール

Polyphoneは、次の理由からMinGWでビルドします。

* 何よりも実用上の理由で、以下の依存ライブラリがMinGWに対応しているためです。
* また、sfArk形式のファイルを正しくインポート/エクスポートできるようにするためでもあります（コンパイルオプション ``-ffloat-store`` が必要です）。

コンパイラー、ビルドツールの :program:`CMake` と :program:`Ninja`、そしてPolyphoneが使うライブラリは、<a href="https://www.msys2.org/" target="_blank">MSYS2</a>でインストールします。
MSYS2をデフォルトのディレクトリ :file:`C:/msys64` にインストールし、ターミナル :program:`MSYS2 MINGW64` を開いて次のコマンドを実行します。

```
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-zlib mingw-w64-x86_64-libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-openssl mingw-w64-x86_64-flac mingw-w64-x86_64-libsndfile
```

次に、Windowsのシステム設定で、フォルダー :file:`C:/msys64/mingw64/bin` をPathに追加します。

* Windowsメニューを開きます。
* 「環境変数」と入力します。
* システムのプロパティを開き、「環境変数」をクリックします。
* システム環境変数の「Path」をダブルクリックします。
* 変数「Path」に登録されているディレクトリに :file:`C:/msys64/mingw64/bin` を追加します。

こうすることで、Qt CreatorとPolyphoneが、MSYS2のコンパイラー、ツール、ライブラリを見つけられるようになります。


## ビルド


### ソース

まず、Polyphoneのソースを<a href="software" target="_blank">こちら</a>または<a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>からダウンロードし、たとえばWindowsのデスクトップに展開します。

### Qt Creatorでビルドする

続いて、Qt CreatorでPolyphoneをビルドするためのチュートリアル「[Qt CreatorでPolyphoneをビルドする](development/using-qt-creator-to-build-polyphone.md)」に進みます。
使用するキットには、MSYS2でインストールしたライブラリを見つけられるMSYS2のコンパイラー（:file:`C:\msys64\mingw64\bin\g++.exe`）と、MinGW 64-bit向けのQtのバージョンを設定する必要があります。

### コマンドラインでビルドする

この方法は、Windows版のPolyphoneをビルドするときに実際に使っているものです。
ターミナル :program:`MSYS2 MINGW64` を開き、プロジェクトのルート（:file:`sources` を含むディレクトリ）に移動して、次のコマンドを実行します（Qtのパスは事前に環境に合わせて変更してください）。

```
export PATH="/mingw64/bin:/c/Qt/6.6.2/mingw_64/bin:$PATH"
cmake -S sources -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:/Qt/6.6.2/mingw_64
cmake --build build --parallel 4
```

コンパイルがうまくいけば、ディレクトリ :file:`build/bin` に実行ファイル :file:`polyphone.exe` ができているはずです。


### Polyphoneの起動

実行ファイルを単独で実行できるようにするには、そのフォルダーにQtのライブラリを追加する必要があります。
ターミナルで次のコマンドを実行します（パスは事前に環境に合わせて変更してください）。

```
C:\Qt\6.6.2\mingw_64\bin\windeployqt.exe --release "C:\Users\User\Desktop\polyphone\build\bin\polyphone.exe"
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
