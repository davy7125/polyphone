オンラインSoundFontリポジトリにはPolyphoneから直接アクセスでき、SoundFontをダウンロードして試したり、場合によっては再利用したりできます。


## リポジトリへのアクセス {#doc_access}


オンラインリポジトリを開くには、:guilabel:`オンラインリポジトリ` をクリックするか、[ホーム画面](manual/index.md)の検索フィールドにキーワードを入力して :kbd:`Enter` を押します。
オンラインリポジトリが新しいタブで開くと、ダウンロード可能なSoundFontの一覧が表示され、結果を検索、並べ替え、絞り込みできます。
結果は1ページに25件ずつ表示され、下部のボタンで前のページや次のページに移動できます。


![オンラインリポジトリ](images/online_repository.png "オンラインリポジトリ")


### SoundFontの検索


SoundFontを検索するには、上部のバーにテキストを入力します。入力に合わせて一覧が更新され、タイトルにそのテキストを含むSoundFontだけが表示されます。
:command:`Author:"someone"` と入力すると、特定の作成者のSoundFontを検索できます（二重引用符の間には作成者の正確な名前を入力する必要があります）。
これは、タイトルを検索するテキストと組み合わせることもできます。たとえば :command:`piano Author:"someone"` のように入力します。


### 結果の並べ替え


検索バーの隣にあるドロップダウンメニューで、SoundFontの結果を4通りの方法で並べ替えられます。

日付
: 新しいSoundFontが先頭に表示されます（初期設定）。

ダウンロード数
: ダウンロード数の多いSoundFontが先頭に表示されます。

評価スコア
: 評価の高いSoundFontが先頭に表示されます。

タイトル (A→Z)
: SoundFontの結果がアルファベット順に並びます。


### 結果の絞り込み


結果はさまざまな方法で絞り込むことができ、各フィルターでは複数の値を選択できます。:guilabel:`リセット` ボタンを押すと、すべてのフィルターが解除されます。

カテゴリ
: インストゥルメントの種類です。

ライセンス
: 商用利用や、SoundFontを編集した後の共有（またはその両方）が許可されているライセンスのSoundFontを一覧表示できます。

サンプルの出典
: サンプルの由来です。

音色
: 音色の説明です。

アーティキュレーション
: アーティキュレーションの説明です。

ジャンル
: SoundFontが対象とするジャンルです。

MIDI規格
: [MIDI規格](manual/annexes/midi-standards.md)に準拠したSoundFontだけを一覧表示できます。

タグ
: 特定のタグが付いたSoundFontを表示します。


## SoundFontの説明 {#doc_description}


SoundFontの説明は、[ホーム画面](manual/index.md#doc_right)の :guilabel:`今日のSoundFont` または[SoundFontの一覧](#doc_access)でセルをダブルクリックする（または :kbd:`Enter` を押す）と開きます。
:guilabel:`今日のSoundFont` に含まれていないSoundFontを見るには、[プレミアムアカウント](premium)が必要です。
ソフトウェアの[設定](manual/settings.md#doc_repository)で、Polyphoneを自分のアカウントに接続できます。


![SoundFontの説明ページ](images/online_repository_description.png "SoundFontの説明ページ")


SoundFontの説明は次の部分で構成されています。

* **ヘッダー**：タイトル、コメント数、ダウンロード数、評価が表示されます。
* **左側の部分**：作成者名、日付、ライセンスの種類（必ず確認してください）、カテゴリ、その他SoundFontの特徴を表す属性が表示されます。
* **中央の部分**：説明文、ダウンロードできるファイルの一覧、コメント（ある場合）が表示されます。


## ダウンロード {#doc_download}


ファイルをダウンロードするには、:guilabel:`ダウンロード` セクションで、ファイル名の隣にある :guilabel:`ダウンロード` アイコンをクリックします。
ダウンロードの進行状況は、ソフトウェアの右上隅、[メインメニュー](manual/menu.md)の隣にあるメニューで確認できます。
ファイルは[設定](manual/settings.md#doc_repository)で指定したダウンロードディレクトリに保存されます。
ダウンロードしたファイルがSoundFontであれば、:guilabel:`開く` アイコンをクリックして[SoundFontエディター](manual/soundfont-editor/index.md)で開けます。

メニューから、完了したダウンロードをクリアできます。
すべてクリアすると、ダウンロードメニューは表示されなくなります。


![SoundFontのダウンロード](images/online_repository_download.png "SoundFontのダウンロード")
