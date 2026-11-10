ここでは、Qt CreatorでPolyphoneをビルドする方法を説明します。
コンピューターには、Qtフレームワーク付きのQt Creator、コンパイラー、Polyphoneのビルドに必要な依存関係がすでにインストールされているものとします（これについては関連するページを参照してください）。


## 前提条件


次のものが必要です。

* C++コンパイラー（GCC、Clang、MinGWなど）
* Qt Creatorとそのフレームワーク（Qt 6またはQt 5）
* :program:`CMake`（バージョン3.16以上。Qt Creatorと一緒にインストールできます）
* Polyphoneの依存関係

インストール方法は使っているOSによって異なります。関連するドキュメントのページを参照してください。


## Polyphoneのビルド


### ソースのコピー


Polyphoneのソースを<a href="software" target="_blank">こちら</a>または<a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>から入手します。


### プロジェクトを開いて設定する


Qt Creatorを起動し、ディレクトリ :file:`sources` にあるファイル :file:`CMakeLists.txt` を開きます。


![Qt Creatorを開く](images/open-Qt-Creator.png "Qt Creatorを開く")


次に、キット、つまりQtのバージョンとコンパイラーを選んで、プロジェクトを設定します。

![プロジェクトの設定](images/configure-project.png "プロジェクトの設定")


続いてQt Creatorが :program:`CMake` を実行し、依存関係が見つかるかどうかを確認します。
メッセージはパネル :guilabel:`General Messages` に表示されます。ライブラリが見つからない場合は、それをインストールしてから、:guilabel:`Build` メニューの :guilabel:`Run CMake` で :program:`CMake` をもう一度実行します。

プロジェクトのオプションは、左の列の :guilabel:`Projects` セクションの :guilabel:`Build` → :guilabel:`CMake` にあります。
Linuxでは、システムのライブラリの代わりに、ソースに含まれている :program:`RtAudio` と :program:`RtMidi` のコピーを使えます。その場合は、オプション ``POLYPHONE_LOCAL_RTAUDIO`` と ``POLYPHONE_LOCAL_RTMIDI`` を有効にしてから、:guilabel:`Run CMake` をクリックします。
WindowsとmacOSでは常に同梱版が使われるため、この手順は不要です。
同じセクションで、ビルド構成を選択します（:guilabel:`Release` がおすすめです）。


### プロジェクトをビルドする


最後に、:guilabel:`Build` メニューの :guilabel:`Build Project "Polyphone"` をクリックします。プロジェクトのビルド中に警告が表示されることがありますが、エラーは出ないはずです。

これで、Qt Creatorが作成したビルドディレクトリ（たとえば、ディレクトリ :file:`sources` の中の :file:`build/Desktop_Qt_6_…-Release`）のサブディレクトリ :file:`bin` に、実行ファイルができているはずです。

* Linuxでは、ファイル :file:`polyphone`
* Windowsでは、ファイル :file:`polyphone.exe`
* macOSでは、バンドル :file:`Polyphone.app`


## トラブルシューティング


詳しい情報が必要な場合や、サポートを受けたい場合は、この[カテゴリ](forum/support-bug-reports)を参照してください。


<p class="endpage">この記事を執筆してくれた<a href="dashboard/820-tomotello">Tomotello</a>さんに感謝します。</p>
