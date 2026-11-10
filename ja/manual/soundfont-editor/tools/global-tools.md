[ツリー](manual/soundfont-editor/tree.md)で見出し :guilabel:`一般` を選択すると、[ツールメニュー](manual/soundfont-editor/tools/index.md)には次の3つのカテゴリに分類されたツールが表示されます。

* [クリーンアップ](#doc_cat_clean)
* [モジュレーター](#doc_cat_mod)
* [ユーティリティ](#doc_cat_util)


## クリーンアップ {#doc_cat_clean}


### 未使用要素を削除 {#doc_unused}


どのインストゥルメントでも使用されていないすべてのサンプルと、どのプリセットでも使用されていないすべてのインストゥルメントが自動的に削除されます。
削除された要素の数がメッセージで表示されます。


### 重複をクリーンアップ {#doc_duplicates}


同一のサンプルが統合されます。データ、サンプルレート、ループ、ルートキー、補正がすべて同じである必要があり、ステレオサンプルはリンクされたサンプルとあわせて比較されます。
同一のサンプルのグループごとに、（ファイル内の順序で）最初のサンプルが残され、インストゥルメント内でほかのサンプルを置き換えます。ほかのサンプルは削除されます。
削除されたサンプルの数がメッセージで表示されます。

重複は、事前に[サンプルの構成サマリー](manual/soundfont-editor/configuration-summaries.md#doc_sample)で確認できます。


![サンプルの構成サマリーに表示された重複](images/tool_clean_duplicates.png "サンプルの構成サマリーに表示された重複")



## モジュレーター {#doc_cat_mod}


### すべてのモジュレーターを削除 {#doc_removemods}


すべての[インストゥルメント](manual/soundfont-editor/editing-pages/instrument-editor.md)と[プリセット](manual/soundfont-editor/editing-pages/preset-editor.md)にあるすべての[モジュレーター](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator)が削除されます。


## ユーティリティ {#doc_cat_util}


### プリセットリストをエクスポート {#doc_export}


このツールを使うと、すべての[プリセット](manual/soundfont-editor/editing-pages/preset-editor.md)の一覧を、バンク番号とプリセット番号付きのテキストとして書き出せます。
このテキストは、表計算ソフトに取り込めるcsv形式や、Webサイトに組み込めるhtmlテーブルの形式にできます。
ボタンを押すと、テキストがクリップボードにコピーされます。


![プリセットリスト](images/en_preset_list.png "プリセットリスト")