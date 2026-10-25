ソフトウェアの右上隅にあるメニューには、さまざまな機能を利用するための項目があります。


![メニュー](images/menu.png "メニュー")


## SoundFontエディターを開く {#doc_open}


新規作成
: 空のSoundFontを新しく作成します。すべてを一から作成することになります。
  ショートカット :kbd:`Ctrl`+:kbd:`N` でも実行できます。

開く…
: [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2)、[.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3)、[.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz)、[.sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark)、[.organ](manual/annexes/the-different-soundfont-formats.md#doc_organ)のいずれかのファイルを開きます。
  ショートカット :kbd:`Ctrl`+:kbd:`O` でも実行できます。


## SoundFontを保存する {#doc_save}


保存
: 現在のファイルを保存します。
  ショートカット :kbd:`Ctrl`+:kbd:`S` でも実行できます。

名前を付けて保存…
: 現在のファイルを別のファイルとして保存します。
  ショートカット :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`S` でも実行できます。

SoundFontをエクスポート…
: 開いているSoundFontから1つ以上のプリセットをまとめて、[.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2)、[.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3)、[.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz)のいずれかの形式でエクスポートします。追加の.csv形式では、SoundFontの内容を詳しく分析するために、すべてのパラメーターがエクスポートされます。
  ショートカット :kbd:`Ctrl`+:kbd:`E` でも実行できます。


![エクスポートウィンドウ](images/export.png "エクスポートウィンドウ")


エクスポートウィンドウには、開いているすべてのSoundFontのプリセットが一覧表示されるので、エクスポートするものにチェックを入れます（:guilabel:`全て選択` ボタンと :guilabel:`全て選択解除` ボタンで、選択をすばやく変更できます）。
その他のパラメーターは次のとおりです。

保存先
: ファイルを書き込むディレクトリです。
  ファイルが上書きされることはありません。同じ名前のファイルがすでにある場合は、新しいファイルの名前の末尾に番号が付けられます。

形式
: :guilabel:`.sf2`、:guilabel:`.sf3`、:guilabel:`.sfz`、:guilabel:`.csv` のいずれかです。
  プリセットが複数のSoundFontから来ている場合、それらは1つのSoundFontにまとめられ、元の各SoundFontが1つのバンクになります。
  そのため、一度にエクスポートできるのは最大127個のSoundFontで、それぞれから選択できるプリセットは最大127個です。

品質
: .sf3形式の場合のみ、サンプルの圧縮品質を指定します。

プリセットごとに1つのファイル
: .sf2形式と.sf3形式の場合のみ、1つのファイルにまとめるのではなく、選択したプリセットごとに1つのファイルを作成します。

バンクごとに1つのディレクトリ、プリセット番号をプレフィックスとして、General MIDI分類
: .sfz形式の場合のみ使えます。エクスポートすると、sfzファイルとサブディレクトリ :file:`samples` を含むディレクトリが作成されます。
  sfzファイルは、バンクごとに別のディレクトリに入れたり、名前の先頭にプリセット番号を付けたり、General MIDIのカテゴリに対応するディレクトリに分類したりできます。

生の値
: .csv形式の場合のみ使えます。値は、一般的な単位（秒、デシベル、セントなど）に変換されず、sf2形式で格納されているとおりに書き出されます。
  エクスポートすると、ファイル :file:`samples.csv` と、インストゥルメントごと・プリセットごとに1つずつのファイルを含むディレクトリが作成されます。


## 表示オプション {#doc_view}


フルスクリーン
: フルスクリーンモードを切り替えます。
  ショートカット :kbd:`F11` でも実行できます。


## ショートカット {#doc_shortcuts}


設定
: ソフトウェアの[設定](manual/settings.md)を開きます。
  ショートカット :kbd:`Ctrl`+:kbd:`,` でも実行できます。

オンラインマニュアル
: このユーザーマニュアルをWebブラウザーで表示します。
  ショートカット :kbd:`F1` でも実行できます。

Polyphoneについて…
: ソフトウェアのバージョンと著作権を表示します。:guilabel:`クレジット` ボタンを押すと、プロジェクトに貢献した人の一覧が表示されます。


## 閉じる {#doc_close}


タブを閉じる
: 現在のタブ（SoundFontエディター、SoundFontエクスプローラー、設定など）を閉じます。
  ショートカット :kbd:`Ctrl`+:kbd:`W` でも実行できます。

終了
: ソフトウェアを終了します。
  ショートカット :kbd:`Ctrl`+:kbd:`Q` でも実行できます。
