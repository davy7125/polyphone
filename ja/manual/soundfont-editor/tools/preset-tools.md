[ツリー](manual/soundfont-editor/tree.md)でプリセットまたはそのディビジョンを選択すると、[ツールメニュー](manual/soundfont-editor/tools/index.md)には次の4つのカテゴリに分類されたツールが表示されます。

* [分析](#doc_cat_analyze)
* [高速編集](#doc_cat_fast)
* [モジュレーター](#doc_cat_mod)
* [変換](#doc_cat_transformation)

パラメーターの表示を除き、複数選択すれば複数のプリセットに対して同時にツールを使えます。
これらのツールはすべて[インストゥルメント](manual/soundfont-editor/tools/instrument-tools.md)にも使えます。


## 分析 {#doc_cat_analyze}


### パラメータ表示 {#doc_display}

インストゥルメント用の「[パラメータ表示](manual/soundfont-editor/tools/instrument-tools.md#doc_display)」と同じツールです。


## 高速編集 {#doc_cat_fast}


### アッテネーションを変更 {#doc_attenuation}

インストゥルメント用の「[アッテネーションを変更](manual/soundfont-editor/tools/instrument-tools.md#doc_attenuation)」と同じツールです。


### キーベース設定 {#doc_global}

インストゥルメント用の「[キーベース設定](manual/soundfont-editor/tools/instrument-tools.md#doc_global)」と同じツールです。


### サウンド空間化 {#doc_spatialization}

インストゥルメント用の「[サウンド空間化](manual/soundfont-editor/tools/instrument-tools.md#doc_spatialization)」と同じツールです。


## モジュレーター {#doc_cat_mod}


### モジュレーターを削除 {#doc_remove_mod}

インストゥルメント用の「[モジュレーターを削除](manual/soundfont-editor/tools/instrument-tools.md#doc_remove_mod)」と同じツールです。


## 変換 {#doc_cat_transformation}


### ディビジョンの複製 {#doc_division}

インストゥルメント用の「[ディビジョンの複製](manual/soundfont-editor/tools/instrument-tools.md#doc_division)」と同じツールです。


### 各ノートのサンプルをマージ {#doc_mergesamples}

キーを押したときにプリセットが複数のインストゥルメントやサンプルを同時に鳴らす場合、このツールはノートごとに、鳴っているすべてのサンプルを1つにまとめます。その結果、キーごとに1つのサンプルだけを鳴らす新しいプリセットができ、プリセットの再生に必要なCPU負荷を減らせます。
ウィンドウでは、新しいプリセットの名前、録音する音のサステイン時間とリリース時間、作成するサンプルの間隔（全てのキー、3キーごと、6キーごと）、自動ループ、音の種類（モノラルまたはステレオ）を指定します。

下の画像のように設定すると、ツールは3ノートごとに5秒間キーを押した状態をシミュレートします。音声はステレオで録音され、リリースは録音されません。こうして作成されたサンプルには[自動ループツール](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop)でループが付けられ、これらのサンプルをすべて含むインストゥルメントと、それを使う新しいプリセットには「Cymbale III」という名前が付けられます。

**注意：**自動ループのオプションを使うと、リリース部分が削除され、各サンプルのサステイン部分の長さがわずかに短くなります。

![各ノートのサンプルをマージ](images/tool_merge_samples.png "各ノートのサンプルをマージ")
