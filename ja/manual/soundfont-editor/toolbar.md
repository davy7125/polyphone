エディターの上部にあるツールバーには、次の操作のためのアイコンがあります。

* よく使う[編集機能](#doc_edit)
* エディターの[表示](#doc_view)の切り替え
* [レコーダー](#doc_recorder)の表示
* [仮想キーボード](#doc_keyboard)の表示


![ツールバー](images/toolbar.png "ツールバー")


## 編集機能 {#doc_edit}


ツールバーの左側にある要素は、よく使う編集機能のためのものです。

![新しいサンプル](images/toolbar_sample.png "新しいサンプル"){.inline} サンプルを追加
: SoundFontに新しい[サンプル](manual/soundfont-editor/editing-pages/sample-editor.md)を追加します。
  オーディオファイルを選択するダイアログが表示されます（:file:`.wav`、:file:`.flac`、:file:`.mp3`、:file:`.ogg`、:file:`.aif`、:file:`.aiff`、:file:`.aifc`、:file:`.snd` ファイルをインポートできます）。一度に複数のファイルを選択できます。

![新しいインストゥルメント](images/toolbar_instrument.png "新しいインストゥルメント"){.inline} インストゥルメントを追加
: 新しい[インストゥルメント](manual/soundfont-editor/editing-pages/instrument-editor.md)を作成します。
  ダイアログでインストゥルメントの名前を尋ねられ、[ツリー](manual/soundfont-editor/tree.md)でサンプルを選択している場合は、それらを新しいインストゥルメントにリンクするかどうかも尋ねられます。
  この場合、ステレオサンプルは左右に振り分けられ、すべてのサンプルにループがあればループが有効になります。また、サンプルのルートキーが異なる場合は、キー範囲が自動的に設定されます（[サンプル自動配置](manual/soundfont-editor/tools/instrument-tools.md#doc_position)ツールを参照）。

![新しいプリセット](images/toolbar_preset.png "新しいプリセット"){.inline} プリセットを追加
: 新しい[プリセット](manual/soundfont-editor/editing-pages/preset-editor.md)を作成します。
  ダイアログでプリセットの名前を尋ねられ、ツリーでインストゥルメントを選択している場合は、それらを新しいプリセットにリンクするかどうかも尋ねられます。
  空いている最初のバンク番号とプリセット番号が使われます。

![ツール](images/toolbar_toolbox.png "ツール"){.inline} ツールボックス
: このボタンをクリックすると、[ツリー](manual/soundfont-editor/tree.md)で選択している要素に使用できるすべての[ツール](manual/soundfont-editor/tools/index.md)を含むメニューが表示されます。

![元に戻す](images/toolbar_undo.png "元に戻す"){.inline} 元に戻す
: 最後の変更を元に戻します。:kbd:`Ctrl`+:kbd:`Z` でも実行できます。

![やり直し](images/toolbar_redo.png "やり直し"){.inline} やり直し
: 最後に元に戻した変更をやり直します。:kbd:`Ctrl`+:kbd:`Y` または :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`Z` でも実行できます。

![保存](images/toolbar_save.png "保存"){.inline} 保存
: SoundFontを保存します。:kbd:`Ctrl`+:kbd:`S` でも実行できます。


## 表示の切り替え {#doc_view}


ツールバーの右側には、表示されている[エディター](manual/soundfont-editor/editing-pages/index.md)に応じていくつかのアイコンが表示されます。

* インストゥルメントの編集中は、![テーブル](images/toolbar_table.png "テーブル"){.inline} [テーブル表示](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)、![範囲](images/toolbar_range.png "範囲"){.inline} [範囲エディター](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range)、![エンベロープ](images/toolbar_adsr.png "エンベロープ"){.inline} [エンベロープエディター](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope)を切り替えられます。
* プリセットの編集中は、![テーブル](images/toolbar_table.png "テーブル"){.inline} テーブル表示と ![範囲](images/toolbar_range.png "範囲"){.inline} 範囲エディターを切り替えられます。
* それ以外の場合は何も表示されません。

選択した表示は、別のインストゥルメントやプリセットを選択しても維持されます。SoundFontを開いたときに最初のインストゥルメントまたはプリセットで表示される画面は、[設定](manual/settings.md#doc_interface)の :guilabel:`起動時の表示` オプションで選べます。


## レコーダー {#doc_recorder}


アイコン ![レコーダー](images/toolbar_recorder.png "レコーダー"){.inline} をクリックするとレコーダーのウィンドウが開き、Polyphoneの出力を :file:`.wav` ファイルに録音できます。経過時間がウィンドウに表示されます。


![レコーダー](images/recorder.png "レコーダー")


左のボタンには2つの状態があります。

録音
: 録音していない場合に、新しい録音を開始します。
  ダイアログで、録音したオーディオ信号を保存するファイルの名前を尋ねられます。

停止
: 現在の録音を停止します。
  ウィンドウを閉じた場合も録音は停止します。

右のボタンにも2つの状態があります（録音中のみ使用できます）。

一時停止
: 現在の録音を一時停止します。

再開
: 現在の録音を再開します。


## 仮想キーボード {#doc_keyboard}


アイコン ![ピアノ](images/toolbar_piano.png "ピアノ"){.inline} をクリックすると、サンプル、インストゥルメント、プリセットを試すための仮想キーボードのウィンドウが開きます。
このウィンドウはMIDI信号を送信します。

* ピアノ鍵盤からは **note on** / **note off** / **aftertouch**
* **MIDIコントローラー部分**を展開している場合は、その他のMIDI信号

演奏中のノートのキー、ベロシティ、アフタータッチはウィンドウの下部に表示され、その横にはキーボードのサイズ（5オクターブ、6オクターブ、88鍵、128鍵）を選ぶドロップダウンリストがあります。


![仮想キーボード](images/virtual_keyboard.png "仮想キーボード")


### ピアノ鍵盤


ピアノ鍵盤は、キー番号とベロシティを含む信号を送信します。
次の3つの方法で操作できます。

マウスを使う
: 目的の鍵盤をマウスでクリックします。
  ベロシティはクリックした位置の縦方向の位置で決まります。
  クリックした後、同じ鍵盤内でマウスを動かすとアフタータッチの信号が送信されます。

パソコンのキーボードを使う
: 使用するキーは[設定](manual/settings.md#doc_keyboard)で定義します。

外部MIDIキーボードを使う
: MIDI入力は[設定](manual/settings.md#doc_general)で定義します。
  ピアノ鍵盤が表示されていれば、演奏している内容がそこに表示されます。


#### マーカーと色


* [インストゥルメント](manual/soundfont-editor/editing-pages/instrument-editor.md)または[プリセット](manual/soundfont-editor/editing-pages/preset-editor.md)を選択すると、ディビジョンでカバーされていない範囲が**グレー**で表示されます。
  これにより、インストゥルメントやプリセットの音域全体をひと目で確認できます。
* インストゥルメントまたはプリセットのディビジョンを選択すると、その範囲が同じように表示されます。
  さらに、そのルートキーが**オレンジ色のマーカー**で示されます。
* 鍵盤上でキーを選択すると、そのキーが**青**（またはソフトウェアの[設定](manual/settings.md#doc_interface)で選んだ選択色）で表示されます。
  さらに、対応するディビジョンの範囲が**水色**で表示されます。
  これはインストゥルメントレベルでのみ適用されます。
* 中央のC（キー60）は**黒いマーカー**で示されます。


#### 便利なショートカット


:kbd:`Ctrl`+:kbd:`K`
: いつでも仮想キーボードにフォーカスを移します（キーボードが非表示の場合は表示されます）。
  たとえばテーブルの編集中に、セルを編集する代わりにパソコンのキーボードで演奏できるようになります。
  仮想キーボードが光って見えると、フォーカスがキーボードにあることがわかります。

:kbd:`Ctrl`+:kbd:`1`、:kbd:`Ctrl`+:kbd:`2`、… :kbd:`Ctrl`+:kbd:`8`
: パソコンのキーボードで演奏するオクターブを切り替えます。
  :kbd:`Ctrl`+:kbd:`1` を使うと、最も低いオクターブを演奏できます。
  最も高いオクターブには :kbd:`Ctrl`+:kbd:`8` で届きます。

:kbd:`→` / :kbd:`←`
: 右矢印または左矢印を押すと、最後に弾いたキーのすぐ上またはすぐ下の音を、同じベロシティで鳴らします。
  :kbd:`Shift` キーと組み合わせると、1キーではなく6キー移動します。

:kbd:`↑` / :kbd:`↓`
: 上矢印または下矢印を押すと、最後に弾いたキーと同じ音を、より強いまたはより弱いベロシティで鳴らします。
  :kbd:`Shift` キーと組み合わせると、ベロシティが1ではなく10ずつ変わります。


### MIDIコントローラー


右下の矢印をクリックすると、MIDIコントローラーエリアの表示 / 非表示を切り替えられます。
モジュレーターを使う場合は、これらの機能を使ってSoundFontをテストすることをおすすめします。


![MIDIコントローラー付きの仮想キーボード](images/virtual_keyboard_2.png "MIDIコントローラー付きの仮想キーボード")


このエリアは次のボタンで構成されています。

ピッチホイール
: ピッチホイールは音の高さを上げ下げします。
  自動的に元の位置0に戻ります。

感度
: 感度は、ピッチホイールで変化させる範囲を半音単位で定義します。
  初期値は2（全音1つ分）です。

プレッシャー
: プレッシャーは鍵盤にかかる圧力の量です。
  アフタータッチに似ていますが、アフタータッチがキーごとの信号であるのに対し、プレッシャーはすべてのキーに共通のグローバルな信号です。

ノブ
: MIDIコントローラーの値を変えるためのノブが3つあります。
  各ノブについて、コントローラー番号を指定してからその値を設定します。

オン / オフLED
: MIDIコントローラーの値を0または127に設定するためのLEDです（たとえばサステインペダルに便利です）。
  ノブと同じようなものですが、取り得る値は2つだけです。

[設定](manual/settings.md#doc_general)でMIDI入力を正しく設定していれば、これらの要素はすべて、コンピューターに接続したMIDIキーボードと同期できます。ノブの操作には、相対値のMIDIコントローラーにも対応しています。

*[MIDI]: Musical Instrument Digital Interface