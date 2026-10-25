Polyphoneは、より多くの人が使えるように、翻訳に対応するよう設計されています。
誰でもこの作業に協力できます。翻訳を一から作成することも、既存の翻訳を補ったり改善したりすることもできます。
バージョン2.6以降、未完成だった翻訳はすべてAIの助けを借りて完成させているため、ネイティブスピーカーによる見直しが望まれます。


## 前提条件


### 翻訳ファイル


翻訳したい言語に対応する翻訳パッケージを、この[ページ](software/translations)からダウンロードします。
その言語がまだ一覧にない場合は、[お問い合わせ](contact)フォームで依頼してください（追加したい新しい言語を伝えるだけで大丈夫です）。
すぐに新しい翻訳パッケージが用意されます。

翻訳ファイルは<a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>リポジトリのディレクトリ :file:`sources` でも入手できます（言語ごとに1つのファイル :file:`polyphone_XX.ts`）。


### Qt Linguist


翻訳に必要なソフトウェアは、翻訳ファイルを作成するためのツール :program:`Qt Linguist` だけです。
このソフトウェアはQtと一緒にインストールされます（[開発に関するページ](development/index.md)を参照）。また、Windowsユーザーは<a href="https://github.com/thurask/Qt-Linguist/releases" target="_blank">こちら</a>から、Linuxユーザーはディストリビューションのリポジトリ（DebianとUbuntuではパッケージ :program:`linguist-qt6`、Fedoraでは :program:`qt6-linguist`）から入手することもできます。


## 翻訳ファイルを開く


ダウンロードした翻訳パッケージを展開します。
.tsファイル :file:`polyphone_XX.ts` が入っています（:file:`XX` は言語を表します）。
:program:`Qt Linguist` で :file:`polyphone_XX.ts` を開けば、翻訳を始められます。


## 翻訳する


各コンテキストの各項目を翻訳する必要があります。
要素を1つずつ確認して、翻訳を入力していくだけです。
自信の度合いに応じて、次の3つのケースがあります。

* 翻訳の仕方がわからない場合は、遠慮なくその要素を飛ばしてください。
  問題ありません。ほかの人が補ってくれます。
* 正しい翻訳だと思うけれど自信がない場合は、案を入力してから :kbd:`Ctrl`+:kbd:`J` を押します。
  その項目は確定されませんが、翻訳されたインタフェースではその訳が使われます。
  翻訳を改善するほかの人に、自信がなかったことを伝えるだけです。
* 正しい翻訳がわかっている場合は、入力して :kbd:`Ctrl`+:kbd:`Enter` を押します。
  その項目は確定されます。

翻訳には注意力が必要です（あらゆるものが<a href="https://medium.com/thrive-global/how-technology-hijacks-peoples-minds-from-a-magician-and-google-s-design-ethicist-56d62ef5edf3" target="_blank">注意を奪おうとする</a>世の中ではなおさらです）。
次の点に気をつけてください。

* 句読点
* 先頭と末尾のスペース
* 大文字と小文字の区別
* 単数形と複数形

最後にひとこと：**時間をかけてください！**
そして、:kbd:`Ctrl`+:kbd:`S` で作業内容を保存するのを忘れないでください。


## 作業内容を確認する


作業内容はいつでも確認できます。
:program:`Qt Linguist` で :menuselection:`File --> Release As…` を選び、作成中の翻訳に対応する.qmファイルを作成します。
翻訳パッケージに含まれる説明（ファイル :file:`readme.txt`）に従ってインストールし、Polyphoneを開きます。
[設定](manual/settings.md#doc_interface)を開き、言語の一覧から自分の翻訳を選んでPolyphoneを再起動します。
これで翻訳を見直せます。

**注意：**:guilabel:`Release As…` の操作では、アクセント記号を含まないテキストしか翻訳されない場合があります。この不具合が起きた場合は、ターミナルを開いて次のコマンドを使ってください：:command:`lrelease polyphone_XX.ts`（ファイル名は実際のファイルに合わせてください）。
翻訳ファイル :file:`polyphone_XX.qm` は、ソフトウェアに組み込む代わりに、:file:`translations` という名前のディレクトリ（カレントディレクトリまたはPolyphoneの設定ディレクトリ内）から読み込むこともできます。


## 翻訳を提出する


作業が終わったら（または疲れてしまったら！）、編集していた.tsファイル（.qmではありません！）を<contact@polyphone.io>に送ってください。
この[ページ](software/translations)の翻訳パッケージが更新されます。

GitHubに慣れている場合は、<a href="https://github.com/davy7125/polyphone" target="_blank">リポジトリ</a>へのプルリクエストで.tsファイルを提案することもできます（コントリビューションガイドを参照してください）。
