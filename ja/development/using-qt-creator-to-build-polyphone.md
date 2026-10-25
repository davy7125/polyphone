ここでは、Qt CreatorでPolyphoneをビルドする方法を説明します。
コンピューターには、Qtフレームワーク付きのQt Creator、コンパイラー、Polyphoneのビルドに必要な依存関係がすでにインストールされているものとします（これについては関連するページを参照してください）。


## 前提条件


次のものが必要です。

* C++コンパイラー（VC++、Clang、GCCなど）
* Qt Creatorとそのフレームワーク（Qt 6）
* Polyphoneの依存関係

インストール方法は使っているOSによって異なります。関連するドキュメントのページを参照してください。


## Polyphoneのビルド


### ソースのコピー


Polyphoneのソースを<a href="software" target="_blank">こちら</a>または<a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>から入手します。


### プロジェクトを開いて設定する


まず、ファイル :file:`polyphone.pro` を開き、お使いのOSが :program:`rtmidi` と :program:`stk` を提供していない場合は、同梱版を使うように編集します（:program:`rtaudio` はデフォルトで同梱版が使われます）。
次の行の先頭にある「#」を削除して、コメントを解除します。

```
DEFINES += USE_LOCAL_RTMIDI
DEFINES += USE_LOCAL_STK
```

コンピューターにすでにインストールされているライブラリを使う場合は、この変更は省略できます（ライブラリがあることを確認してください！）。
WindowsとmacOSでは常に同梱版が使われるため、この手順は不要です。

Qt Creatorを起動し、ファイル :file:`polyphone.pro` を開きます。


![Qt Creatorを開く](images/open-Qt-Creator.png "Qt Creatorを開く")


次に、プロジェクトを設定します。

![プロジェクトの設定](images/configure-project.png "プロジェクトの設定")


### qmakeを実行してプロジェクトをビルドする


:guilabel:`Build` メニューから :guilabel:`Run qmake` を選びます。


![qmakeの実行](images/run-qmake.png "qmakeの実行")


右下隅の進行状況バーが緑色で100%に達すると、:command:`qmake` の実行は完了です。


![qmakeの完了](images/qmake-finished.png "qmakeの完了")


左の列の :guilabel:`Project` セクションで、さらに :guilabel:`Build&Run` セクションに進み、ビルド構成を選択します（:guilabel:`Release` がおすすめです）。

最後に、:guilabel:`Build` メニューの :guilabel:`Build project Polyphone` をクリックします。プロジェクトのビルド中に警告が表示されることがありますが、エラーは出ないはずです。

これで、Qt Creatorのビルド構成での選択に応じて、Qt Creatorが作成したビルドディレクトリ（たとえば、ソースの隣にある :file:`build-XXX`）に実行ファイルができているはずです。

* Linuxでは、ファイル :file:`polyphone` はサブディレクトリ :file:`bin` にあります。
* Windowsでは、ファイル :file:`polyphone.exe` はサブディレクトリ :file:`release` または :file:`debug` にあります。
* macOSでは、バンドル :file:`Polyphone.app` はディレクトリ :file:`sources` の隣にあるディレクトリ :file:`macos` にあります。


## トラブルシューティング


詳しい情報が必要な場合や、サポートを受けたい場合は、この[カテゴリ](forum/support-bug-reports)を参照してください。


<p class="endpage">この記事を執筆してくれた<a href="dashboard/820-tomotello">Tomotello</a>さんに感謝します。</p>
