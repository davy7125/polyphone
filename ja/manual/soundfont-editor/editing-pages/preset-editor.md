[ツリー](manual/soundfont-editor/tree.md)で1つ以上のプリセットをクリックすると、プリセットの編集ページが表示されます。
プリセットは1つ以上のインストゥルメントの組み合わせで、[SoundFont](manual/annexes/the-different-soundfont-formats.md#doc_sf2)の外部から見える要素です。

[インストゥルメントエディター](manual/soundfont-editor/editing-pages/instrument-editor.md)と同様に、次の部分があります。

* [テーブル](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)
* [範囲エディター](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range)（[ツールバー](manual/soundfont-editor/toolbar.md#doc_view)の :guilabel:`範囲` ボタンをクリックしたとき）
* [モジュレーターセクション](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator)


![プリセットページ](images/edit_preset.png "プリセットページ")


プリセットを1つだけ選択している場合は、プリセットのグローバルパラメーターと、そのすべてのディビジョンのパラメーターを編集できます。
複数のプリセットを選択している場合は、それぞれのプリセットのグローバルパラメーターを編集できます。


## インストゥルメントとの違い {#doc_diff}


ここで編集するパラメーターは相対値と呼ばれます。一方、インストゥルメントのパラメーターは、特別な場合を除いて絶対値と呼ばれます。
プリセットのパラメーターの値は、そのプリセットを構成するインストゥルメントのパラメーターに加算または乗算されます。
そのため、アッテネーション（または他のパラメーター）は、インストゥルメントレベルでは負にできませんが、プリセットレベルでは負にできます。
次のパラメーターは、プリセットレベルでは編集できません。

* ループ再生
* ルートキー
* エクスクルーシブクラス
* 固定キー
* 固定ベロシティ
* 4つのオフセット

プリセットはプリセット番号とバンク番号で識別され、この組み合わせは一意で、テーブルの下で変更できます。
各バンクには128個のプリセット（0～127の番号）があります。

最後のバンク（128）は、通常ドラムキットに使われます。