Polyphoneの**ユーザーマニュアル**へようこそ！SoundFontとは何か、Polyphoneでどのように編集するのかをすばやく知りたい場合は、[チュートリアル](tutorials/index.md)から始めるか、<a href="https://www.youtube.com/results?search_query=polyphone+sf2" target="_blank">YouTube</a>の動画をご覧ください（役立つ動画を作ってくださったPolyphoneユーザーの皆さんに感謝します）。ソフトウェアの機能をすべて知りたい場合は、このまま読み進めてください。

さらにサポートが必要な場合や、これらのページを読むのが大変だと感じた場合は、[フォーラム](forum)で助けを得られます。


## ホーム画面 {#doc_home}


Polyphoneの最初の画面は、基本的に3つの部分で構成されています。

* 開いたファイルやディレクトリの履歴とショートカットを含む[左側部分](#doc_left)
* SoundFontリポジトリ専用の[右側部分](#doc_right)
* タブとメニューを含む[上部エリア](#doc_top)


![Polyphoneのホーム画面](images/page_home.png "Polyphoneのホーム画面")


## 左側部分 {#doc_left}


### SoundFontを開く


SoundFontを開く方法はいくつかあります。

* :guilabel:`SoundFontを開く` をクリックするとファイルブラウザーが開き、拡張子が :file:`.sf2`、:file:`.sf3`、:file:`.sfArk`、:file:`.sfz`、:file:`.dls`、:file:`.organ` のファイルを選択できます（各形式については[こちら](manual/annexes/the-different-soundfont-formats.md)で説明しています）。一度に複数のファイルを選択できます。
* :guilabel:`新規 SoundFont` をクリックすると空のSoundFontが開き、すべてを一から作成することになります。
* :guilabel:`ディレクトリを開く` をクリックすると[SoundFontエクスプローラー](manual/soundfont-explorer.md)が開き、ディレクトリ全体をスキャンして目的のファイルを開けます。
* 履歴内のSoundFontをダブルクリックすると、そのSoundFontが再び開きます（選択した項目は :kbd:`Enter` でも開け、:kbd:`Del` で履歴から削除できます）。
* ファイルやディレクトリを履歴エリアにドラッグ＆ドロップすると、それが開きます。

SoundFontを開くと、[SoundFontエディター](manual/soundfont-editor/index.md)を含む新しいタブが表示されます。


### ショートカット


左側部分の下部には3つのボタンがあります。

* :guilabel:`ドキュメント`：Webブラウザーを開いてPolyphoneの[ドキュメント](documentation)を表示します。
* :guilabel:`フォーラム`：Webブラウザーを開いてPolyphoneの[フォーラム](forum)を表示します。
* :guilabel:`設定`：ソフトウェアの[設定](manual/settings.md)を新しいタブで開きます（:kbd:`Ctrl`+:kbd:`,` でも開けます）。


## 右側部分 {#doc_right}


### オンラインリポジトリ


:guilabel:`オンラインリポジトリ` ボタンをクリックすると、[オンラインSoundFontリポジトリ](manual/online-repository.md)の内容を表示するタブが開きます。


### 今日のSoundFont


新しいSoundFontとの出会いを促すため、ここには毎日ランダムに選ばれた5つのSoundFontが表示されます。
そのうちの1つをダブルクリックすると、対応する[SoundFontの説明](manual/online-repository.md#doc_description)を表示する新しいタブが開きます。
作成者名やSoundFontの属性をクリックすると、対応するフィルターを適用した状態で[オンラインリポジトリ](manual/online-repository.md)が開きます。


### 検索


ここにキーワードを入力すると、オンラインリポジトリで特定のSoundFontを検索でき、結果は[専用のタブ](manual/online-repository.md)に表示されます。


## 上部エリア {#doc_top}


### タブ


タブを使うと、次の画面にすばやく切り替えられます。

* [ホーム画面](#doc_home)（最初のタブで、常に開いています）
* 開いているSoundFontの[エディター](manual/soundfont-editor/index.md)
* [SoundFontエクスプローラー](manual/soundfont-explorer.md)（開いたディレクトリごとに1つ）
* ソフトウェアの[設定](manual/settings.md)
* [オンラインリポジトリ](manual/online-repository.md)
* オンラインリポジトリにある[SoundFontの説明](manual/online-repository.md#doc_description)

タブ間の移動にはキーボードショートカットを使えます。

* :kbd:`Ctrl`+:kbd:`Tab` または :kbd:`Ctrl`+:kbd:`PageDown`：次のタブに移動します（ホーム画面のタブを除く）。
* :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`Tab` または :kbd:`Ctrl`+:kbd:`PageUp`：前のタブに移動します（ホーム画面のタブを除く）。
* :kbd:`Ctrl`+:kbd:`Home` と :kbd:`Ctrl`+:kbd:`End`：最初のタブと最後のタブに移動します（ホーム画面のタブを除く）。
* :kbd:`Ctrl`+:kbd:`W`：現在のタブを閉じます。
* :kbd:`Ctrl`+:kbd:`H`：ホーム画面に移動します。

ショートカット :kbd:`Ctrl`+:kbd:`F` を押すと、現在のタブの検索フィールドにフォーカスが移ります。対象は、ホーム画面の検索フィールド、[オンラインリポジトリ](manual/online-repository.md)の検索フィールド、SoundFontエディターの[ツリーの検索バー](manual/soundfont-editor/tree.md#doc_search)、または[SoundFontエクスプローラー](manual/soundfont-explorer.md)の検索フィールドです。


### メニュー


右上隅にあるメニューには一般的な機能がまとめられています。詳しくは[こちら](manual/menu.md)をご覧ください。
