Polyphoneでは、[ホーム画面](manual/index.md#doc_left)の :guilabel:`ディレクトリを開く` ボタンをクリックしてディレクトリを開けます。すると、SoundFontエクスプローラーが新しいタブで開き、そのディレクトリ内のファイルを表示して（サブディレクトリはスキャンされません）、各SoundFontの内容をプレビューします。


![SoundFontエクスプローラー](images/soundfont_explorer.png "SoundFontエクスプローラー")


## SoundFontのプレビュー {#doc_preview}


画面左側にあるメインリストにはファイルの一覧が表示され、Polyphoneはそれらを次のように区別します。

* すばやくスキャンできる対応ファイル（たとえば[.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2)、[.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3)、[.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz)形式）は、内容の詳細とともに表示されます。
* すばやくスキャンできない対応ファイル（特に[.sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark)形式）は、基本的な情報だけが表示されます。
* 対応していないファイルはグレー表示になり、開くことはできません。

スキャン可能なファイルをクリックすると、画面右側のリストに、そのファイルに含まれるサンプル、インストゥルメント、プリセットが表示されます。これらのリストの各項目には、SoundFontの作り込みの度合いや、制作にかけられた手間を判断するのに役立つ情報も含まれています。

### ファイル単位の詳細

* 各ファイルのサイズと最終更新日が表示されます。
* スキャン可能なSoundFontについては、次の情報が加わります。
  * サンプル、インストゥルメント、プリセットの数
  * 最初のいくつかのプリセットの名前

### 各サンプルの情報

* 種類（モノラル / ステレオ）
* 全体の長さ
* ループの長さ（ループがある場合）
* サンプルレート

### 各インストゥルメントまたはプリセットの情報

* ディビジョン、パラメーター、ジェネレーターの総数
* 異なるキー範囲の数
* 異なるベロシティ範囲の数


## 並べ替えと検索の機能 {#doc_filter}


上部にはテキストフィールドがあります（ショートカット :kbd:`Ctrl`+:kbd:`F` でここにフォーカスが移ります）。入力すると、エクスプローラーは次の条件の少なくとも1つに一致するファイルだけを表示します。

* ファイル名に検索テキストが含まれている
* サンプル、インストゥルメント、プリセットのいずれかの名前に検索テキストが含まれている

サンプル、インストゥルメント、プリセットのリストも絞り込まれ、一致する要素が強調表示されます。

テキストフィールドの右にはドロップダウンリストがあり、ファイルを名前順（A→Z）、最終更新日順（新しい順）、ファイルサイズ順（大きい順または小さい順）に並べ替えられます。


## ファイル操作 {#doc_file_operations}


メインリストでファイルをクリックすると、ファイル名の前に2つのクリック可能なアイコンが表示されます。

* 鉛筆アイコン：ファイル名を変更します（:kbd:`F2` でも実行できます）。
* ゴミ箱アイコン：確認の後、ファイルをごみ箱に移動します（:kbd:`Del` でも実行できます）。

これらの操作を使うと、手元のSoundFontコレクションをすばやく管理・整理できます。


## SoundFontを開く {#doc_open_soundfont}


メインリストの項目をダブルクリックすると、ファイルが開きます（Polyphoneが対応している形式に限ります）。最初に表示される場所は、[設定](manual/settings.md#doc_interface)の :guilabel:`起動時の表示` オプションで決まります（初期設定では[一般情報](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md)が表示されます）。

[サンプル](manual/soundfont-editor/editing-pages/sample-editor.md)、[インストゥルメント](manual/soundfont-editor/editing-pages/instrument-editor.md)、[プリセット](manual/soundfont-editor/editing-pages/preset-editor.md)をダブルクリックすると、選択した要素の位置でファイルが開きます。
