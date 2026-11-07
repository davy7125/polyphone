MIDIファイルにはMIDIメッセージが含まれており、それらを組み合わせるとメロディーになります。MIDIファイルはSoundFontを使うシンセサイザーと組み合わせることができます。このチュートリアルでは、MIDIファイルを使ってSoundFontの鳴り方を試す方法を説明します。


## 少しだけ背景知識


<div class="too_complicated">
<p>シンセサイザーのキーボードで演奏すると、<a href="https://ja.wikipedia.org/wiki/MIDI" target="_blank">MIDI</a>信号が送出されます。この信号には、演奏されたノートやそのベロシティ（キーを押す強さを表す用語）などの情報が含まれています。簡単に言えば、MIDIファイルには、このようなキーボードが送出した信号が時系列で記録されています。この時点では、まだ音は何も作られていません。</p>

<p>スピーカーから音を出すには変換の段階が必要で、ここでシンセサイザーの出番となります。シンセサイザーは、受信したMIDI信号に応じて音を鳴らします。シンセサイザーにはいくつかの種類がありますが、ここで取り上げるのはサンプルベースのシンセサイザー、より具体的にはSoundFont形式の音源を使うシンセサイザーです。</p>

<p>MIDIファイルは最大16の異なるチャンネルを制御でき、16種類のインストゥルメントを並行して使えます。MIDIファイルには、どのノートを演奏するかだけでなく、チャンネルZのこれから演奏するノートにはバンクYのインストゥルメントXを使うよう、シンセサイザーに指示する命令も含まれています。インストゥルメントを正しく選択できるようにするには、シンセサイザーが使う音源が<a href="https://ja.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a>規格に準拠している必要があります。この規格では、XとYの番号ごとに特定のインストゥルメントが割り当てられています。たとえば、SoundFont内のインストゥルメント<strong>001</strong>はピアノでなければなりません。</p>

<p>まとめると、ここで取り上げるのは、General MIDI規格に準拠したSoundFontに含まれる音を鳴らすシンセサイザーを、MIDIファイルの再生によって制御することです。ここから先はもっと簡単です！</p>
</div>


## 前提条件


以下で説明する方法でMIDIファイルを再生するには、次のものが必要です。

- この[ページ](software)で入手できるPolyphoneの最新バージョン
- MIDIファイル（たとえば<a href="files/ff7choco.mid" download>このファイル</a>）
- 試してみたい<a href="https://ja.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a>規格準拠のSoundFont（[リポジトリ](soundfonts/midi-standard/gm)には専用のフィルターがあります）

この規格に準拠していないSoundFontも、[プリセットを自分で選ぶ](#doc_presets)ことで試すことができます。


## MIDIファイルの再生


PolyphoneでSoundFontを開き、[ツールバー](manual/soundfont-editor/toolbar.md)の右側にあるアイコン ![MIDI](images/toolbar_midi.png "MIDI"){.inline} をクリックして[MIDIプレーヤー](manual/soundfont-editor/toolbar.md#doc_midi_player)を表示します。

ウィンドウ左上のフォルダーボタンをクリックしてMIDIファイルを選択し、再生ボタンをクリック（または :kbd:`Space` を押す）すれば、準備は完了です。

![PolyphoneのMIDIプレーヤー](images/midi_player.png "PolyphoneのMIDIプレーヤー")

ファイルのうちノートを含む各チャンネルが、表の1行に表示されます。:guilabel:`プリセット` 列には、ファイルが指定しているプリセット（曲の途中で変わることがあります）が表示され、:guilabel:`ボリューム` 列のLEDはノートが始まるたびに点灯します。ファイル名の下にあるスライダーで、曲の別の箇所へ移動できます。

MIDIプレーヤーは、Polyphoneを[シンセモード](tutorials/use-polyphone-as-a-synthesizer.md)で開いた場合にも使用できます。


## SoundFontを試す


SoundFontはエディターでの状態のまま、保存されていない変更も含めて演奏されます。曲を再生しながらインストゥルメントを編集し、その結果をすぐに聞くことができます。たとえば、ボリューム、エンベロープ、フィルターを調整するときに便利です。

プレーヤーには、細部を聞き取るための機能もあります。

- チャンネルの :guilabel:`S`（ソロ）ボタンと :guilabel:`M`（ミュート）ボタンで、インストゥルメントを単独で鳴らしたりミックスから外したりでき、:guilabel:`ボリューム` 列のスライダーでほかのチャンネルとのバランスを取れます。
- :guilabel:`プリセット` 列の警告アイコンは、ファイルが指定したプリセットがSoundFontにないことを示します。そのチャンネルのノートは演奏されないため、General MIDI規格に完全には準拠していないSoundFontであることがわかります。
- チャンネルのMIDIボタンには、ファイルがその時点で使っているコントローラー（モジュレーションホイール、エクスプレッション、リバーブ…）が一覧表示されるので、SoundFontの[モジュレーター](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator)を確認するのに役立ちます。
- バナーの右側にある[再生設定](manual/soundfont-editor/toolbar.md#doc_midi_player_settings)では、曲をループ再生したり、速いパッセージを聞くために遅くしたり、ほかの音を試すために移調したりできます。


## SoundFontとプリセットの選択 {#doc_presets}


チャンネルごとに異なるSoundFontを演奏させることができます。Polyphoneで複数のSoundFontを開き、:guilabel:`SoundFont` 列のセルをダブルクリックして、そのチャンネルが演奏するSoundFontを選びます。たとえば、同じパートで2つのSoundFontのピアノを比べたり、自分のSoundFontでメロディーを演奏しながら、伴奏にはGeneral MIDIのSoundFontを使ったりできます。

初期設定では、チャンネルは :guilabel:`選択` に設定されており、現在のタブのSoundFont、または[SoundFontエクスプローラー](manual/soundfont-explorer.md#doc_play)で選択したファイルを演奏します。ディレクトリ内のすべてのSoundFontを比べるには、そのディレクトリをPolyphoneで開き、曲の再生中にファイルを1つずつ選択していきます。各SoundFontはタブを開かずにバックグラウンドで読み込まれ、準備ができ次第聴こえます。

![チャンネルのSoundFontの選択](images/midi_player_soundfonts.png "チャンネルのSoundFontの選択")

同様に、:guilabel:`プリセット` 列のセルをダブルクリックすると、ファイルが指定したプリセットの代わりに、SoundFontのプリセットを強制的に使用できます。これにより、General MIDI規格に準拠していないSoundFont（たとえばインストゥルメントが1つだけのSoundFont）を試すことができます。メロディーのチャンネルにそのプリセットを選ぶと、そのチャンネルではファイルのプログラムチェンジが無視されます。


## 結果の書き出し


結果に満足したら、再生設定の :guilabel:`オーディオをエクスポート...` で、プレーヤーで聞こえるとおりに曲をwavファイルに書き出せます。SoundFont、プリセット、ボリューム、テンポ、ピッチはプレーヤーと同じです。
MIDIファイルは[コマンドライン](manual/annexes/command-line.md#doc_render)からwavファイルに変換することもできます（たとえばスクリプトで使う場合）。
