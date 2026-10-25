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

- インターネット接続
- Webブラウザー
- MIDIファイル（たとえば<a href="files/ff7choco.mid" download>このファイル</a>）
- 必要に応じて、試してみたい<a href="https://ja.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a>規格準拠のSoundFont（[リポジトリ](soundfonts/midi-standard/gm)には専用のフィルターがあります）
- 必要に応じて、この[ページ](software)で入手できるPolyphoneの最新バージョン


## MIDIファイルの再生


簡単な方法は、MIDIファイルプレーヤーとSoundFontシンセサイザーを兼ねた<a href="https://spessasus.github.io/SpessaSynth/" target="_blank">SpessaSynth</a>を使うことです。オンラインで利用でき、特別なインストールが不要という利点があります。

:guilabel:`Load MIDI file` ボタンをクリックしてMIDIファイルを選択すれば、準備は完了です。

![SpessaSynthのMIDIプレーヤーとシンセサイザー](images/SpessaSynth.jpg "SpessaSynthのMIDIプレーヤーとシンセサイザー")


## SoundFontの選択


好きなSoundFontを使うには、:guilabel:`Upload soundfont` ボタンをクリックしてファイルを選択します。


## MIDI出力をPolyphoneに接続する


上で述べたように、**SpessaSynth**はMIDIファイルプレーヤーであると同時にシンセサイザーでもあります。MIDIファイルプレーヤーの機能だけを使い、信号をPolyphoneに送って、音の合成をPolyphoneに任せることもできます。これは、SoundFontが大きすぎてインターネット経由で送れない場合に便利です。

SpessaSynthをPolyphoneに接続するには、次のようにします（Linuxで動作確認済みの方法です）。

- Polyphoneを[シンセモード](tutorials/use-polyphone-as-a-synthesizer.md)で起動し、[設定](manual/settings.md#doc_general)を開きます。
- MIDI入力として「MIDI through」を選択します（名前を覚えておいてください）。
- WebブラウザーでSpessaSynthを開き、その設定を開きます。
- MIDI出力として「MIDI through」（PolyphoneのMIDI入力と同じ名前）を選択します。

MIDIファイルは16のチャンネルにメッセージを送信できるため、SoundFontを16回開き、タブごとに1から16までの異なるチャンネルを指定する必要があります。便利な方法として、Polyphoneを開くときに、開くSoundFontとオプション「each」を指定すれば、16個のタブを一度に開けます。たとえば、コマンドは次のようになります。

```
polyphone -s -i "/home/davy/Downloads/gm.sf2" -c "each|off|off"
```

コマンドラインについて詳しくは、この[ページ](manual/annexes/command-line.md#doc_synth)を参照してください。
