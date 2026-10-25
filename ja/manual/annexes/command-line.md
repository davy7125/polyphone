Polyphoneをコマンドラインで実行して、SoundFontを[sf2](#doc_sf2)、[sf3](#doc_sf3)、[sfz](#doc_sfz)、[csv](#doc_csv)のいずれかの形式に変換できます。変換に対応しているファイル形式は、[sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2)、[sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3)、[sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz)、[sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark)、[organ](manual/annexes/the-different-soundfont-formats.md#doc_organ)です。

Polyphoneを[シンセモード](#doc_synth)で開くためのオプションもあります。

ファイルやディレクトリを引数として渡して、グラフィカルインタフェースで開くこともできます。たとえば :command:`polyphone file1.sf2 file2.sf3 /path/to/directory` のように指定します（ディレクトリは[SoundFontエクスプローラー](manual/soundfont-explorer.md)で開きます）。
また、設定をリセットしたりヘルプを表示したりするための[その他のオプション](#doc_other)もいくつかあります。

変換時には、読み込みと書き込みの状況がコンソールに出力されます。
変換に成功した場合の戻り値は0で、失敗した場合（入力ファイルが存在しない、ファイルを読み書きできない、sf2形式とsf3形式で出力ファイルがすでに存在する、など）はそれ以外の値になります。


## sf2への変換 {#doc_sf2}


### コマンドライン


```
polyphone -1 -i <file/to/convert> -d <output/directory> -o <output_file_name>
```

**注意：**Windowsでは、:command:`polyphone` だけでなく、Polyphoneのフルパスを書く必要があります。たとえば :command:`C:/Program files/Polyphone/polyphone.exe` です。


### 引数


-1
: モード「1」は「sf2に変換」です。

-d
: 入力ファイルの変換先となる出力ディレクトリです。
  この引数は省略可能で、省略した場合は入力ファイルと同じディレクトリになります。

-o
: 変換後のファイルの出力名（拡張子なし）です。拡張子「.sf2」は自動的に付けられます。
  この引数は省略可能で、省略した場合は入力ファイルと同じ名前になります。


### 例


```
polyphone -1 -i /path/to/file.sfArk
```


## sf3への変換 {#doc_sf3}


### コマンドライン


```
polyphone -2 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**注意：**Windowsでは、:command:`polyphone` だけでなく、Polyphoneのフルパスを書く必要があります。たとえば :command:`C:/Program files/Polyphone/polyphone.exe` です。


### 引数


-2
: モード「2」は「sf3に変換」です。

-d
: 入力ファイルの変換先となる出力ディレクトリです。
  この引数は省略可能で、省略した場合は入力ファイルと同じディレクトリになります。

-o
: 変換後のファイルの出力名（拡張子なし）です。拡張子「.sf3」は自動的に付けられます。
  この引数は省略可能で、省略した場合は入力ファイルと同じ名前になります。

-c
: 変換の設定です。
  圧縮品質を指定できます。「0」は低、「1」は中、「2」は高です。
  この引数は省略可能で、省略した場合は「1」（中品質）になります。


### 例


```
polyphone -2 -i /path/to/file.sf2 -c 2
```


## sfzへの変換 {#doc_sfz}


### コマンドライン


```
polyphone -3 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**注意：**Windowsでは、:command:`polyphone` だけでなく、Polyphoneのフルパスを書く必要があります。たとえば :command:`C:/Program files/Polyphone/polyphone.exe` です。


### 引数


-3
: モード「3」は「sfzに変換」です。

-d
: 入力ファイルの変換先となる出力ディレクトリです。
  この引数は省略可能で、省略した場合は入力ファイルと同じディレクトリになります。

-o
: 変換後のファイルの出力名（拡張子なし）です。拡張子「.sfz」は自動的に付けられます。
  この引数は省略可能で、省略した場合は入力ファイルと同じ名前になります。

-c
: 文字「|」で区切られた3つの値からなる変換の設定です。この引数は省略可能で、省略した場合は「0|0|0」になります。
  
  * 1つ目の値は、各プリセットの名前の先頭にプリセット番号を付ける場合は「1」、付けない場合は「0」です。
  * 2つ目の値は、バンクごとにディレクトリを作成する場合は「1」、作成しない場合は「0」です。
  * 3つ目の値は、General MIDI分類を使ってプリセットを分類する場合は「1」、使わない場合は「0」です。


### 例


```
polyphone -3 -i /path/to/file.sf3 -c '0|1|1'
```

変換すると、sfzファイルとサブディレクトリ :file:`samples` を含むディレクトリ（出力名と同じ名前）が作成されます。このディレクトリがすでに存在する場合は、名前に番号が付けられます。


## .csvへの変換 {#doc_csv}


### コマンドライン


```
polyphone -4 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**注意：**Windowsでは、:command:`polyphone` だけでなく、Polyphoneのフルパスを書く必要があります。たとえば :command:`C:/Program files/Polyphone/polyphone.exe` です。


### 引数


-4
: モード「4」は「.csvに変換」です。

-d
: 入力ファイルの変換先となる出力ディレクトリです。
  この引数は省略可能で、省略した場合は入力ファイルと同じディレクトリになります。

-o
: 変換後のファイルの出力名（拡張子なし）です。拡張子「.csv」は自動的に付けられます。
  この引数は省略可能で、省略した場合は入力ファイルと同じ名前になります。

-c
: 変換の設定です。
  オプション「raw」を使うと、測定可能な物理単位の値ではなく、（SoundFontファイルに格納されているとおりの）生の値をエクスポートできます。たとえば、エクスポートされるリリース時間は0.491（秒）ですが、生の値では-1229になります。初期設定では、物理単位の値がエクスポートされます。


### 例


```
polyphone -4 -i /path/to/file.sf2 -c 'raw'
```

変換すると、ファイル :file:`samples.csv` と、インストゥルメントごと・プリセットごとに1つずつのファイルを含むディレクトリ（出力名と同じ名前）が作成されます。このディレクトリがすでに存在する場合は、名前に番号が付けられます。


## シンセモード {#doc_synth}


### コマンドライン


```
polyphone -s -i <file/to/read> -c <configuration>
```

**注意：**Windowsでは、:command:`polyphone` だけでなく、Polyphoneのフルパスを書く必要があります。たとえば :command:`C:/Program files/Polyphone/polyphone.exe` です。

![シンセモードのオプションで開いたPolyphone](images/synth_mode.png "シンセモードのオプションで開いたPolyphone")


### 引数


-s
: モード「s」は「Polyphoneをシンセモードで開く」です。

-c
: 文字「|」で区切られた3つの値からなるシンセの設定です。この引数は省略可能で、省略した場合は「0|off|off」になります。これらのパラメーターは、後からグラフィカルインタフェースで変更できます。
  
  * 1つ目の値は、演奏用に受信するMIDIチャンネルの番号（1～16）です。値が0または「all」の場合は、すべてのチャンネルが使われ、1つのチャンネルとして処理されます。値が-1または「each」の場合は、各チャンネルを個別に受信します。これは、たとえば[MIDIファイルを再生する](tutorials/read-midi-file-with-soundfont.md)ための前提条件です。
  * 2つ目の値は「off」または「on」で、演奏用に複数のプリセットを選択する機能を無効または有効にします。
  * 3つ目の値は「off」、「on」、「toggle」のいずれかで、プリセット番号に対応するキーでプリセットを選択する機能を無効または有効にします（たとえば、キー0でプリセット0が有効になります）。


### 例


```
polyphone -s -i /path/to/file.sf2 -c '1|on|toggle'
```


## その他のオプション {#doc_other}


-h
: 短いヘルプメッセージを表示します。

-r
: Polyphoneの構成（設定、ウィンドウの位置とサイズ、履歴など）をリセットします。
  問題が起きたときに役立つことがあります。

```
polyphone -r
```
