「[ソフトウェア](software)」セクションで提供されているインストーラーでMacにPolyphoneをインストールできない場合は、この手順に従ってください。


## 前提条件


次のアプリケーションが必要です。

* <a href="https://www.qt.io/download-open-source" target="_blank">Qt Creatorとそのフレームワーク</a>（バージョン6）
* Xcode、または「Command Line Tools」のみ（使用するバージョンはQtのWebサイトに記載されています。たとえば<a href="https://doc.qt.io/qt-6/supported-platforms.html" target="_blank">こちら</a>）

また、<a href="software" target="_blank">こちら</a>または<a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>で入手できるPolyphoneのソースも必要です。

Qt Creatorでプロジェクトを正しく開くには、まずQt Creatorにコンピューター上のすべてのファイルを読み取る権限を与える必要があります（「プライバシーとセキュリティ」=>「フルディスクアクセス」=> インストールディレクトリにある「Qt Creator.app」を追加）。

Polyphoneが使うライブラリは、<a href="https://brew.sh/" target="_blank">Homebrew</a>で簡単にインストールできます。このツールをインストールした後、次のコマンドを実行します。

```
brew install libsndfile libogg libvorbis flac lame mpg123 opus openssl
```

プロジェクトファイル :file:`polyphone.pro` は、ヘッダーとライブラリを :file:`/opt/homebrew`（Apple Silicon）、:file:`/usr/local`（Intel）、そしてディレクトリ :file:`sources` の隣にあるディレクトリ :file:`macos` の中で探します。
[こちら](files/macos.zip)で入手できる静的ライブラリのアーカイブを展開して、ディレクトリ :file:`macos` を用意することもできますが、内容が古くなっている可能性があります。

SSLなど、一部のライブラリが最新でなかったり、お使いのMacのバージョンと互換性がなかったりする場合は、インターネットでそのソースを探し、静的リンクを有効にして再コンパイルする必要があります。

```
configure --enable-static --disable-shared --prefix=...
make
make install
```

その後、すべての.h（ヘッダー）を :file:`macos/include` に、すべての.a（静的ライブラリ）を :file:`macos/lib` にコピーします。


## ビルド


:program:`Qt Creator` でファイル :file:`polyphone.pro` を開き（「[Qt CreatorでPolyphoneをビルドする](development/using-qt-creator-to-build-polyphone.md)」を参照）、プロジェクトをビルドします。
ディレクトリ :file:`sources` の隣にあるディレクトリ :file:`macos` に、バンドル :file:`Polyphone.app` ができるはずです。

コマンドラインでビルドすることもできます。

```
cd sources
qmake polyphone.pro CONFIG+=release
make -j4
```

指定したmacOSのターゲットでは一部の機能が使えないとコンパイラーが警告する場合は、ファイル :file:`polyphone.pro` の次の変数の値を上げることができます（デフォルト値は12.0です）。

```
QMAKE_MACOSX_DEPLOYMENT_TARGET = 12.0
```

この値は、ファイル :file:`packaging/mac/polyphone.plist` の「LSMinimumSystemVersion」の下にも書き込む必要があります。


## バンドルの仕上げ


次のコマンドを実行して、ライブラリとフレームワークをバンドルに含めます（先にパスを調整してください！）。

```
/Path/To/Qt/6.7.3/macos/bin/macdeployqt /Path/To/macos/Polyphone.app
```

Apple Siliconプロセッサーを搭載したMacでは、実行できるようにするために、バンドルに署名する必要もあります（アドホック署名でもかまいません）。

```
xattr -cr /Path/To/macos/Polyphone.app
codesign --force --deep --sign - /Path/To/macos/Polyphone.app
```

これで、プログラムを実行したり、.zipファイルや.dmgファイルに圧縮して共有したりできます。


## トラブルシューティング


詳しい情報が必要な場合や、サポートを受けたい場合は、この[トピック](forum/support-bug-reports/8-success-build-polyphone-on-osx-10-11-6-qt-5-7)を参照してください。
