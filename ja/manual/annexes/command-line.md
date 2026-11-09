Polyphoneをコマンドラインで実行して、SoundFontを[sf2](#doc_sf2)、[sf3](#doc_sf3)、[sfz](#doc_sfz)、[csv](#doc_csv)、[dls](#doc_dls)のいずれかの形式に変換できます。変換に対応しているファイル形式は、[sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2)、[sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3)、[sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz)、[dls](manual/annexes/the-different-soundfont-formats.md#doc_dls)、[sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark)、[organ](manual/annexes/the-different-soundfont-formats.md#doc_organ)です。

[MIDIファイルをSoundFontで再生し](#doc_render)、wavファイルに書き出すこともできます。
Polyphoneを[シンセモード](#doc_synth)で開いたり、[MCPサーバー](#doc_mcp)を起動した状態で開いたりするためのオプションもあります。

ファイルやディレクトリを引数として渡して、グラフィカルインタフェースで開くこともできます。たとえば :command:`polyphone file1.sf2 file2.sf3 /path/to/directory` のように指定します（ディレクトリは[SoundFontエクスプローラー](manual/soundfont-explorer.md)で開きます）。
Polyphoneがすでに開いている場合、ファイルは実行中のインスタンスで開かれます。
また、設定をリセットしたり、ヘルプやバージョンを表示したりするための[その他のオプション](#doc_other)もいくつかあります。

すべてのオプションの一覧は :command:`polyphone --help` で表示されます。
引数の順序は問いません。
[以前のバージョン](#doc_legacy)のオプションも引き続き使用できます。

**注意：**Windowsでは、:command:`polyphone` だけでなく、Polyphoneのフルパスを書く必要があります。たとえば :command:`C:/Program files/Polyphone/polyphone.exe` です。


## 変換 {#doc_conversion}


### コマンドライン


```
polyphone --convert <形式> <変換する/ファイル> -d <出力/ディレクトリ> -o <出力/ファイル> -f
```


### 引数


--convert
: 変換の形式です：「sf2」、「sf3」、「sfz」、「csv」、「dls」のいずれか。各形式に固有のオプションは以下で説明します。
  出力ファイルが拡張子付きで指定されている場合、この引数は省略できます。たとえば :command:`polyphone file.sf2 -o file.sf3` のように指定します。

-d, --output-dir
: 出力ディレクトリです。このディレクトリは存在している必要があります。
  この引数は省略可能で、省略した場合は各入力ファイルのディレクトリになります。

-o, --output
: 出力ファイルです。拡張子は付けても付けなくてもかまいません（形式の拡張子は自動的に付けられます）。
  この引数は省略可能で、省略した場合、出力ファイルは入力ファイルと同じ名前になります。
  名前だけの場合は出力ディレクトリに書き込まれ、相対パスは現在のディレクトリ（-d を使用した場合は出力ディレクトリ）が起点になります。
  複数のファイルを変換する場合、この引数は使用できません。

-f, --force
: 既存の出力ファイルを置き換えます。
  この引数がない場合、出力ファイルがすでに存在すると変換は失敗します。
  入力ファイルをその変換結果で置き換えることもできます。たとえば :command:`polyphone --convert sf2 file.sf2 -f` は :file:`file.sf2` を書き直します。

1つのコマンドで複数のファイルを変換でき、各出力ファイルは対応する入力ファイルと同じ名前になります。
失敗した場合でも、ほかのファイルは変換されます。


### 例


```
polyphone --convert sf2 /path/to/file.sfArk
polyphone /path/to/file.sf2 -o /path/to/converted.sf3
polyphone --convert sf3 *.sf2 -d /path/to/directory
```


## sf2への変換 {#doc_sf2}


この変換に固有のオプションはありません。

```
polyphone --convert sf2 /path/to/file.dls
```


## sf3への変換 {#doc_sf3}


--quality
: 圧縮品質です：「low」（低）、「medium」（中）、「high」（高）のいずれか。
  この引数は省略可能で、省略した場合は「medium」になります。

```
polyphone --convert sf3 /path/to/file.sf2 --quality high
```


## sfzへの変換 {#doc_sfz}


--preset-prefix
: 各プリセットの前にそのプリセット番号を付けます。

--bank-dir
: バンクごとにディレクトリを作成します。

--gm-sort
: General MIDI分類を使ってプリセットを並べ替えます。

```
polyphone --convert sfz /path/to/file.sf3 --bank-dir --gm-sort
```

変換では、出力ファイルの名前を持つディレクトリが作成され、その中にsfzファイルとサブディレクトリ :file:`samples` が置かれます。
このディレクトリがすでに存在する場合は、名前に番号が付け加えられます。-f を使っても何も置き換えられません。


## .csvへの変換 {#doc_csv}


--raw
: 物理的に測定可能な単位を持つ値ではなく、生の値（SoundFontファイルに保存されているとおりの値）を書き出します。
  たとえば、書き出されるリリース時間は0.491（秒）、生の値では-1229になります。
  既定では、物理単位を持つ値が書き出されます。

```
polyphone --convert csv /path/to/file.sf2 --raw
```

変換では、出力ファイルの名前を持つディレクトリが作成され、その中にファイル :file:`samples.csv` と、インストゥルメントごと、プリセットごとのファイルが置かれます。
このディレクトリがすでに存在する場合は、名前に番号が付け加えられます。-f を使っても何も置き換えられません。


## dlsへの変換 {#doc_dls}


この変換に固有のオプションはありません。

```
polyphone --convert dls /path/to/file.sf2
```

ファイルはDLSレベル2形式で書き込まれます。変換の制限事項は[こちら](manual/annexes/the-different-soundfont-formats.md#doc_dls)で説明しています。


## MIDIファイルのレンダリング {#doc_render}


### コマンドライン


```
polyphone --render <MIDI/ファイル> <SoundFont> -d <出力/ディレクトリ> -o <出力/ファイル> -f
```


### 引数


--render
: 再生するMIDIファイルのパスです（拡張子は.mid、.midi、.kar、.rmi）。MIDIファイルがSoundFontで再生され、結果がwavファイルに書き込まれます。

-d, --output-dir
: wavファイルを書き込む出力ディレクトリです。
  この引数は省略可能で、省略した場合はMIDIファイルと同じディレクトリになります。

-o, --output
: 出力wavファイルです。拡張子は付けても付けなくてもかまいません。
  この引数は省略可能で、省略した場合、wavファイルはMIDIファイルと同じ名前になります。

-f, --force
: 既存のwavファイルを置き換えます。


### 例


```
polyphone --render /path/to/song.mid /path/to/file.sf2
```

MIDIファイルのすべてのチャンネルがSoundFontを演奏し、プリセットはファイルのプログラムチェンジによって選ばれます（パーカッション用に、チャンネル10ではバンク128）。
シンセサイザーの設定が使われます：[サウンド](manual/settings.md#doc_sound)（リバーブ、コーラス、全体の音量）、[同時発音数](manual/settings.md#doc_general)、[チューニング](manual/settings.md#doc_keyboard)。
結果は48,000 Hzのステレオ32ビット浮動小数点のwavファイルで、最後のノートの音が消えた時点で終わります。
SoundFontにないプリセットのノートは演奏されず、そのプリセットについて警告が表示されます。
グラフィカルインターフェースでは、[MIDIプレーヤー](manual/soundfont-editor/toolbar.md#doc_midi_player_settings)からもMIDIファイルを書き出せます。この場合、チャンネルごとに異なるSoundFontやプリセットを演奏させることができます。


## シンセモード {#doc_synth}


### コマンドライン


```
polyphone --synth <読み込む/ファイル> --channel <チャンネル> --multiple-selection --key-selection <モード>
```

![シンセモードのオプションで開いたPolyphone](images/synth_mode.png "シンセモードのオプションで開いたPolyphone")


### 引数


-s, --synth
: Polyphoneをシンセモードで開きます。

--channel
: 演奏用に受信するMIDIチャンネルの番号（1～16）です。
  値「all」の場合は、すべてのチャンネルが使われ、1つのチャンネルとして処理されます（既定値）。
  値「each」の場合は、各チャンネルを個別に受信します。これは、たとえばシーケンサーのMIDI出力を演奏するための前提条件です（MIDIファイルは[MIDIプレーヤー](manual/soundfont-editor/toolbar.md#doc_midi_player)で直接再生することもできます）。

--multiple-selection
: 演奏用に複数のプリセットを選択できます。

--key-selection
: 値「on」の場合、プリセット番号に対応するキーでプリセットが選択されます（たとえば、キー0でプリセット0が有効になります）。
  値「toggle」の場合、キーでそのプリセットの選択を解除することもできます。

これらの引数は省略可能で、設定は後からグラフィカルインタフェースで変更できます。


### 例


```
polyphone --synth /path/to/file.sf2 --channel 1 --multiple-selection --key-selection toggle
```


## MCPサーバー {#doc_mcp}


### コマンドライン


```
polyphone --mcp <開く/ファイル> --mcp-port <ポート>
```


### 引数


--mcp
: AIアシスタントが開いているSoundFontを参照、編集、分析できるようにするMCPサーバーを、[設定](manual/settings.md)で無効になっていても、このセッションで起動します。
  サーバーのアドレスはコンソールに表示されます。
  Polyphoneがすでに開いている場合でも、Polyphoneの新しいインスタンスが開きます。

--mcp-port
: MCPサーバーのポートです（この引数は --mcp を含みます）。
  この引数は省略可能で、省略した場合は設定のポート（8642）になります。

グラフィカルインタフェースでMCPサーバーの設定を変更すると、これらのオプションは置き換えられます。


### 例


```
polyphone --mcp-port 8700 /path/to/file.sf2
```


## その他のオプション {#doc_other}


-h, --help
: すべてのオプションの一覧を表示します。

-v, --version
: Polyphoneのバージョンを表示します。

-r, --reset-config
: Polyphoneの構成（設定、ウィンドウの配置、履歴など）をリセットします。
  問題が発生した場合に役立つことがあります。

```
polyphone --reset-config
```


## メッセージと戻り値 {#doc_exit}


読み込みと書き込みの状況はコンソールに出力され、エラーはエラー出力に書き込まれます。
メッセージとヘルプは、[設定](manual/settings.md)で選択した言語で表示されます。

戻り値は次のとおりです：

0
: 成功。

1
: 入力ファイルまたは出力ディレクトリが存在しない、または出力ファイルがすでに存在する（-f なしの場合）。

2
: 引数が無効：不明なオプション、誤った値、選択したモードで使用されないオプション、サポートされていない入力ファイル形式など。

3
: 入力ファイル（SoundFont、MIDIファイル）を読み込めない。

4
: 出力を書き込めない。

複数のファイルを変換した場合、戻り値は最初に失敗したときの値になります。


## 以前のバージョンのオプション {#doc_legacy}


Polyphoneの以前のバージョンのオプションも引き続き使用できます：

-1, -2, -3, -4, -6
: sf2、sf3、sfz、csv、dlsへの変換で、--convert と同じです。

-5
: MIDIファイルのレンダリングで、MIDIファイルのパスは -c で指定します。

-i
: 入力ファイルです。このオプションなしで書くこともできます。

-c
: 文字「|」で区切られた値からなる設定です：
  
  * sf3への変換：圧縮品質で、「0」は低、「1」は中、「2」は高。
  * sfzへの変換：--preset-prefix、--bank-dir、--gm-sort に対応する「1」または「0」の3つの値。たとえば「0|1|1」。
  * csvへの変換：--raw に対応する「raw」。
  * MIDIファイルのレンダリング（-5）：MIDIファイルのパス。
  * シンセモード：チャンネル（「all」、「each」、または1～16。0は「all」、-1は「each」と同じ）、複数選択（「on」または「off」）、キーによる選択（「on」、「off」、「toggle」）。たとえば「all|on|toggle」。

```
polyphone -2 -i /path/to/file.sf2 -c 2
polyphone -s -i /path/to/file.sf2 -c 'all|on|toggle'
```
