Polyphoneでは、[ホーム画面](manual/index.md#doc_left)の :guilabel:`ディレクトリを開く` ボタンをクリックしてディレクトリを開けます。すると、SoundFontエクスプローラーが新しいタブで開き、そのディレクトリ内のファイルを表示して（サブディレクトリはスキャンされません）、各SoundFontの内容をプレビューします。


![SoundFontエクスプローラー](images/soundfont_explorer.png "SoundFontエクスプローラー")


## SoundFontのプレビュー {#doc_preview}


画面左側にあるメインリストにはファイルの一覧が表示され、Polyphoneはそれらを次のように区別します。

* すばやくスキャンできる対応ファイル（たとえば[.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2)、[.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3)、[.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz)、[.dls](manual/annexes/the-different-soundfont-formats.md#doc_dls)形式）は、内容の詳細とともに表示されます。
* すばやくスキャンできない対応ファイル（特に[.sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark)形式）は、基本的な情報だけが表示されます。
* 対応していないファイルはグレー表示になり、開くことはできません。

スキャン可能なファイルをクリックすると、画面右側のリストに、そのファイルに含まれるサンプル、インストゥルメント、プリセットが表示されます。これらのリストの各項目には、SoundFontの作り込みの度合いや、制作にかけられた手間を判断するのに役立つ情報も含まれています。

### ファイル単位の詳細

* 各ファイルのサイズと最終更新日が表示されます。
* スキャン可能なSoundFontについては、次の情報が加わります。
  * サンプル、インストゥルメント、プリセットの数
  * 最初のいくつかのプリセットの名前

これらの詳細は、見出しの下に列として揃えて表示されます。テキストフィールドの右にある :guilabel:`表示` ボタンをクリックするとメニューが開き、表示する列（名前以外はすべて非表示にできます）と、:guilabel:`プリセットの概要`（2行目に表示される最初のいくつかのプリセットの名前）を表示するかどうかを選べます。概要を表示しない場合、各ファイルは表のように1行に収まります。

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

列の見出しをクリックすると、名前、サイズ、日付、またはサンプル、インストゥルメント、プリセットの数でファイルが並べ替えられ、もう一度クリックすると順序が逆になります。最初のクリックでは、名前はAからZの順に、その他の列は大きい値または新しいファイルから順に並べ替えられます。要素の数で並べ替える場合、内容が不明なファイルは常にリストの最後に置かれます。

表示オプション、並べ替え、および右側のリストの幅と展開・折りたたみの状態は、次回も保持されます。


## ファイル操作 {#doc_file_operations}


メインリストでファイルをクリックすると、ファイル名の前に2つのクリック可能なアイコンが表示されます。

* 鉛筆アイコン：ファイル名を変更します（:kbd:`F2` でも実行できます）。
* ゴミ箱アイコン：確認の後、ファイルをごみ箱に移動します（:kbd:`Del` でも実行できます）。

これらの操作を使うと、手元のSoundFontコレクションをすばやく管理・整理できます。


## 要素を演奏する {#doc_play}


:guilabel:`表示` ボタンの右にある ![piano](images/toolbar_piano.png "piano"){.inline} ボタンで[仮想キーボード](manual/soundfont-editor/toolbar.md#doc_keyboard)の表示 / 非表示を、その隣の ![レコーダー](images/toolbar_recorder.png "レコーダー"){.inline} ボタンで[レコーダー](manual/soundfont-editor/toolbar.md#doc_recorder)の表示 / 非表示を、エディターのツールバーのボタンと同じように切り替えられます。

右側のリストで選択したサンプル、インストゥルメント、プリセットは、仮想キーボードで演奏できるほか、仮想キーボードが非表示のときでも外部MIDIキーボードで演奏できます。エディターと同じく、仮想キーボードには要素の音域、またはサンプルのルートキーが表示されます。サンプルは、ループがあればループ再生され、サンプルの[プレーヤー](manual/soundfont-editor/editing-pages/sample-editor.md#doc_player)の :guilabel:`ステレオ` オプションがオンならステレオで再生されます。

ファイルの要素を初めて選択すると、タブを開かずにバックグラウンドでSoundFontが読み込まれます。その後、同じファイルの要素を選択している間はSoundFontが保持され、メインリストで別のファイルを選択すると解放されます。

すでにタブで開いているSoundFontは、保存していない変更も含め、エディター上の状態のまま演奏されます。


## SoundFontを開く {#doc_open_soundfont}


メインリストの項目をダブルクリックすると、ファイルが開きます（Polyphoneが対応している形式に限ります）。最初に表示される場所は、[設定](manual/settings.md#doc_interface)の :guilabel:`起動時の表示` オプションで決まります（初期設定では[一般情報](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md)が表示されます）。

[サンプル](manual/soundfont-editor/editing-pages/sample-editor.md)、[インストゥルメント](manual/soundfont-editor/editing-pages/instrument-editor.md)、[プリセット](manual/soundfont-editor/editing-pages/preset-editor.md)をダブルクリックすると、選択した要素の位置でファイルが開きます。

要素を演奏するためにSoundFontがすでに読み込まれている場合、タブはすぐに開きます。
