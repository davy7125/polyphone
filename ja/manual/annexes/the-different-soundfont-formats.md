Polyphoneは、次のSoundFont形式に対応しています。

* [sf2](#doc_sf2)
* [sf3](#doc_sf3)
* [sfz](#doc_sfz)
* [dls](#doc_dls)
* [sfArk](#doc_sfark)
* [organ](#doc_organ)


## Sf2形式のSoundFont {#doc_sf2}


Sf2形式のSoundFontは、Polyphoneで編集する主なファイルです。
各sf2ファイルには1つ以上の音楽用バーチャルインストゥルメントが含まれており、それらはオーディオサンプルと多数のパラメーターで構成されています。
パラメーターは、サンプルを鍵盤全体でどのように再生するかを定義し、あらかじめ定義された信号（[モジュレーター](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator)）によって変調されることもあります。
Sf2ファイルは次の3階層の構造で作られています。

* [サンプル](manual/soundfont-editor/editing-pages/sample-editor.md)：たとえばトランペットやピアノから録音したもの
* [インストゥルメント](manual/soundfont-editor/editing-pages/instrument-editor.md)：サンプルで構成されるもの
* [プリセット](manual/soundfont-editor/editing-pages/preset-editor.md)：インストゥルメントで構成されるもの

これらの要素の定義と設定に加えて、sf2ファイルには[一般情報](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md)（たとえば作成者、著作権、編集日時）も含まれています。

SoundFontは、MIDI信号で制御される<a href="http://www.fluidsynth.org/" target="_blank">FluidSynth</a>などのソフトウェアシンセサイザーで使われます。
また、sf2形式のSoundFontを使って、<a href="https://sourceforge.net/projects/timidity" target="_blank">TiMidity</a>、<a href="https://www.mindwerks.net/projects/wildmidi" target="_blank">WildMIDI</a>、QuickTimeでMIDIファイルを聴いたり、<a href="https://musescore.org/" target="_blank">MuseScore</a>で楽譜を聴いたりすることもできます。
サンプラー、サウンドカード、さらにはシンセサイザーなどのハードウェアも、.sf2形式に対応している場合があります。

SoundFontについての詳しい情報は、たとえば<a href="https://ja.wikipedia.org/wiki/SoundFont" target="_blank">Wikipedia</a>で調べられます。
sf2の仕様書は、バージョン2.01が[こちら](files/sf_specifications_v2.01.pdf)、バージョン2.04が[こちら](files/sf_specifications_v2.04.pdf)から入手できます。


## Sf3形式のSoundFont {#doc_sf3}


<a href="https://musescore.org/" target="_blank">MuseScore</a>が開発したsf3形式は、あらゆる点でsf2形式と同じですが、[サンプル](manual/soundfont-editor/editing-pages/sample-editor.md)を生データのままではなく、OGG形式（MP3形式に似ていますが、オープンソースです）で格納できる点が異なります。
その結果、sf3形式は同程度の品質でsf2形式のおよそ10分の1の大きさになります。
Polyphoneでは、SoundFontの[一般情報](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md#doc_edit)で圧縮を有効にすると、[サンプルエディター](manual/soundfont-editor/editing-pages/sample-editor.md#doc_info)で各サンプルを圧縮できるようになります（圧縮品質も選べます）。

この形式でエクスポートしたSoundFontの圧縮済みサンプルは、後から編集することを想定していません。圧縮を繰り返すと音質が低下するためです。
この形式は最終成果物と考えるべきもので、SoundFontを利用するすべてのエンドユーザーにとって、次の点で非常に魅力的です。

* インターネットからのダウンロードが楽になる
* コンピューターの容量を節約できる
* MIDIとsf3の組み合わせは、サウンドトラック（ビデオゲーム、モバイルアプリなど）を再生するための軽量で強力なソリューションになる

同じ課題に応える<a href="http://www.fmjsoft.com/fmt/sf2pack.htm" target="_blank">sf2pack</a>形式とは異なり、sf3形式は完全にオープンソースです。
ソースコードが公開されているため、将来にわたって使い続けられることが保証されます（sfArkやsfPackと同じ過ちは避けましょう！）。


## Sfz形式のSoundFont {#doc_sfz}


sfz形式の目的はsf2形式と同じで、サンプルを鍵盤上に配置・設定して楽器を作ることです。
主な違いは、sf2形式がすべてを含む1つのファイルであるのに対し、sfz形式は一連の.wavサンプルと一緒に提供されるテキストファイルであることです。
人が編集できるように設計されているため、複雑なエディターがなくてもファイルを編集できることが主な利点でした。
とはいえ、大きなインストゥルメントでは、やはりエディターを使うことを強くおすすめします。パラメーターの量にすぐ嫌気がさしてしまうからです。
また、sfz形式はsf2形式ほど厳密に定義されていません。sfzインストゥルメントの編集方法や再生方法に違いが生じることがあります。その一方で、sfzインストゥルメントの機能はsf2規格に比べて拡張されています。


### sfzインポートの制限


* [テーブル](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)に対応する項目があるオペコードだけがインポートされます。
* sf2ファイルではピッチとフィルターに適用されるエンベロープが共通（モジュレーションエンベロープ）なので、ピッチとフィルターで異なるエンベロープをインポートすることはできません（オペコード ``fileg_*`` と ``pitcheg_*``）。
* sf2ファイルではボリュームとフィルターに適用されるLFOが共通（モジュレーションLFO）なので、ボリュームとフィルターで異なるLFOをインポートすることはできません（オペコード ``amplfo_*`` と ``fillfo_*``）。
* インポートできるフィルターは、2次のローパスフィルター（``fil_type=lpf_2p``）だけです。
* 正しくインポートするには、オペコード ``group`` と ``off_by`` が、同じグループ内で同じ値になっている必要があります。

**警告：**オペコード ``width`` と ``position`` は正しく解釈されない場合があります。


### sfzエクスポートの制限


* [モジュレーター](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator)は現在エクスポートされません。
* sfz形式（バージョン1）ではピッチの変調が1つしか認められていないため、:guilabel:`ビブラートLFO → ピッチ (セント)` と :guilabel:`モジュレーションLFO → ピッチ (セント)` の2つのパラメーターを同時にエクスポートすることはできません。
* 0-1のループはエクスポートできますが、ほとんどのsfzプレーヤーでは無視されます。
* 24ビットのサンプルには対応していないシンセサイザーもあります。各シンセサイザーの仕様書を参照してください。


### 全般


sfz / sf2の変換処理における音量レベル（dB）の対応は、Sforzandoで検証しています。
ただし、多少の違いが残る場合があります。

また、キーに応じたパラメーターの変化（:guilabel:`キー → ボリュームエンベロープホールド / ディケイ` と :guilabel:`キー → モジュレーションエンベロープホールド / ディケイ`）には、完全に対応するものがありません。
sf2形式はキーに応じたディケイとホールドの時間を指数関数で定義しますが、sfz形式は線形関数を使います。


## DLSファイル {#doc_dls}


DLS形式（DownLoadable Sounds）はMIDI Manufacturers Associationが公開している形式で、sf2形式と同じ目的を持っています。1つのファイルに、サンプルと、それを楽器として演奏するために必要なパラメーターが含まれます。
DLSレベル1（1997年）とDLSレベル2（最新バージョンは2.2）の2つのレベルがあります。
DLSファイルは、Windowsのソフトウェアシンセサイザー（ファイル :file:`gm.dls`）、DirectMusic、ビデオゲーム、携帯電話などで使われてきました。

構造はsf2形式とは異なります。DLSファイルにはウェーブ（サンプル）とインストゥルメントが含まれ、各インストゥルメントはプリセットと同じようにバンクとプログラム番号を持ちます。
インストゥルメントはリージョンで構成され、各リージョンがキーとベロシティの範囲でウェーブを再生します。
パラメーター（エンベロープ、LFO、フィルター、モジュレーションなど）は、ソース（たとえばMIDIコントローラーやLFO）、省略可能なコントロール、デスティネーションの間の接続として記述されます。
パラメーターはインストゥルメント全体にも1つのリージョンにも定義でき、その場合はリージョンのパラメーターがインストゥルメントのパラメーターに置き換わります。

Polyphoneは両方のレベルのDLSファイルをインポートし（.dlsファイルのみ。.rmiファイルや.xmfファイルに埋め込まれたDLSバンクには対応していません）、SoundFontをDLSレベル2.2形式で[エクスポート](manual/menu.md#doc_save)します。
インポートしたDLSファイルは、SoundFontを保存しても変更されません。SoundFontはsf2形式で保存されます。


### DLSインポートの制限


* 一部のパラメーターはsf2形式に相当するものがないため無視されます。ボリュームエンベロープのシャットダウン時間、ステレオのパン以外の出力（センター、リア、LFEチャンネル）、音量やフィルターに送られるビブラートLFO、音量に送られるモジュレーションエンベロープです。
* DLSシンセサイザーでは同じノートをもう一度弾くと前のノートが止まりますが、sf2シンセサイザーでは両方のノートが聞こえます。
* バンクのMSB（CC0）のみが保持され、MSBが0の場合はバンクのLSB（CC32）が使われます。複数のインストゥルメントが同じバンクとプログラム番号を持つ場合は、そのうち1つのプログラム番号が変更されます。


### DLSエクスポートの制限


* DLSレベル1のシンセサイザーはサンプルを再生しますが、エンベロープ、LFO、フィルター、モジュレーションは無視します。
* 24ビットのサンプルは16ビットに丸められます。
* 次のものはエクスポートされません。
  パラメーター :guilabel:`固定キー` と :guilabel:`固定ベロシティ`、
  [ループ再生](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)「リリース」を使うディビジョン、
  変換が「絶対値」のモジュレーター、または他のモジュレーターにリンクされたモジュレーター、
  DLS形式に存在しないパラメーターのモジュレーター（サンプルのオフセット、キーからエンベロープのホールドやディケイへ、など）、
  2つのソースでLFOやモジュレーションエンベロープの深さを変えるモジュレーター。
* ソースとデスティネーションが同じでカーブが異なる2つのモジュレーターは、1つにまとめられます。
* エクスポートできるエクスクルーシブクラスは、プリセットごとに15個までです。
* :guilabel:`スケールチューニング` と、CC1、CC7、CC10、CC11、CC91、CC93以外のMIDIコントローラーによるモジュレーションは、一部のDLSシンセサイザーで無視される場合があります。


### 全般


sf2形式とDLS形式では、デフォルトのモジュレーション（ベロシティからフィルターのカットオフ周波数、モジュレーションホイールとチャンネルプレッシャーによるビブラート、リバーブとコーラスのレベル）が異なります。そのため、変換時には音が変わらないようにモジュレーターが追加されます。

変換は、元のファイルと変換後のファイルのレンダリングを比較するとともに、DLSファイルの読み書きに対応した<a href="https://github.com/spessasus/spessasynth_core" target="_blank">SpessaSynth</a>でも検証しています。
DLSレベル2形式に対応したシンセサイザーは少ないため、一部のパラメーターの再生方法はシンセサイザーによって異なる場合があります。


## SfArkアーカイブ {#doc_sfark}


SfArkアーカイブは、sf3形式と同じく圧縮形式です。
そのため、音質はわずかに低下します（ただし、多くの場合は聞き取れない程度です）。
この形式は廃れつつあるため、PolyphoneではsfArkアーカイブのインポートだけができます（エクスポートはできません）。


## GrandOrgueの構成ファイル {#doc_organ}


GrandOrgueは、パイプオルガンなどの楽器をエミュレートすることを主な目的としたプログラムです。オルガンは、それぞれ1本のパイプの音を収めた一連のwavファイルと、すべてのwavファイルをもとにオルガンを作るために必要なパラメーターをすべて含む、拡張子「.organ」のファイルで定義されます。基本的に、すべてのサンプルは特定のキーに割り当てられ、ストップごとにまとめられており、オルガンの演奏中にストップを有効または無効にできます。Polyphoneはこの割り当てをもとにSoundFontを作成できますが、SoundFontをGrandOrgue形式に変換することはできません。

### インポートの制限

トランスポジションには現在対応していません。


*[MIDI]: Musical Instrument Digital Interface
