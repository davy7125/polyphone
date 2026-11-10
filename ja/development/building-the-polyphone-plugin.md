ここでは、Polyphoneの[プラグイン](manual/plugin.md)（VST3、CLAP、LV2、およびmacOSではAU）をソースからビルドする手順を説明します。
プラグインは<a href="https://juce.com" target="_blank">JUCE</a>と<a href="https://github.com/free-audio/clap-juce-extensions" target="_blank">clap-juce-extensions</a>で作られており、これらはプロジェクトの設定時に自動的にダウンロードされます。エディターとは異なり、Qtは必要ありません。


## 前提条件


次のものが必要です。

* :program:`CMake` 3.22以上とC++17コンパイラー
* sf3ファイルを読み込むためのライブラリ :program:`libvorbisfile`
* sfzファイルのサンプルとdlsファイルの波形を読み込むためのライブラリ :program:`libsndfile`

また、<a href="software" target="_blank">こちら</a>または<a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>で入手できるPolyphoneのソースも必要です。


### Linux

JUCEが必要とするパッケージ（ALSA、freetype、fontconfig、X11のライブラリ）も必要です。
UbuntuまたはDebianでは、次のようにします。

```
sudo apt-get install build-essential cmake pkgconf libvorbis-dev libsndfile1-dev libasound2-dev libfreetype-dev libfontconfig1-dev libx11-dev libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev libxi-dev libxrandr-dev libxrender-dev libgl-dev
```

Fedoraでは、次のようにします。

```
sudo dnf install @development-tools cmake pkgconf libvorbis-devel libsndfile-devel alsa-lib-devel freetype-devel fontconfig-devel libX11-devel libXcomposite-devel libXcursor-devel libXext-devel libXinerama-devel libXi-devel libXrandr-devel libXrender-devel mesa-libGL-devel
```


### WindowsとmacOS

以下の最初のコマンドにオプション ``-DPOLYPHONE_FETCH_VORBIS=ON -DPOLYPHONE_FETCH_SNDFILE=ON`` を追加すると、ライブラリをプロジェクトの設定時にダウンロードしてビルドできます。
この方法でビルドした :program:`libsndfile` は外部ライブラリを使わないため、sfzファイルのサンプルはwav形式とaiff形式なら読み込めますが、flac、ogg、opusは読み込めません。

公式版では、代わりに<a href="https://vcpkg.io" target="_blank">vcpkg</a>を使っています。vcpkgは完全な静的ライブラリを提供します（flac、ogg、opus、mp3を読み込めます）。
ライブラリは次のコマンドでインストールします。

```
vcpkg install --triplet <triplet> --overlay-triplets=packaging/vcpkg-triplets libsndfile libvorbis
```

続いて、以下の最初のコマンドに次のオプションを追加します。

```
-DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=<triplet> -DVCPKG_OVERLAY_TRIPLETS=packaging/vcpkg-triplets -DVCPKG_MANIFEST_MODE=OFF
```

トリプレット（triplet）は ``x64-windows-polyphone``、``arm64-osx-polyphone``、``x64-osx-polyphone`` のいずれかです。
Windowsでは、プラグインは :program:`Visual Studio` の :program:`clang-cl` でビルドします。そのため、オプション ``-T ClangCL`` も追加する必要があります。


## ビルド


プロジェクトのルート（:file:`sources` を含むディレクトリ）でターミナルを開き、次のコマンドを実行します。

```
cmake -S sources/plugin -B build/plugin -DCMAKE_BUILD_TYPE=Release
cmake --build build/plugin --config Release --parallel 4
```

プラグインはディレクトリ :file:`build/plugin/PolyphonePlugin_artefacts/Release` にできます（:file:`VST3/Polyphone.vst3`、:file:`CLAP/Polyphone.clap`、:file:`LV2/Polyphone.lv2`、およびmacOSでは :file:`AU/Polyphone.component`）。
ページ「[Polyphoneプラグイン](manual/plugin.md#doc_install)」で説明しているように、これらをホストのフォルダーにコピーしてインストールします。

ディレクトリ :file:`Standalone` には、ホストなしでSoundFontを演奏できるスタンドアロンアプリケーション :program:`Polyphone Player` も含まれています。

[Polyphoneのビルド](development/building-polyphone-for-ubuntu.md)にオプション ``-DPOLYPHONE_BUILD_PLUGIN=ON`` を追加すると、エディターと一緒にプラグインをビルドすることもできます。


## 翻訳


プラグインのテキストはPolyphoneの翻訳ファイル（コンテキスト :guilabel:`PluginEditor`、:guilabel:`PluginProcessor`、:guilabel:`PluginJuce`）に含まれています。これらはエディターと同時に翻訳され（「[Polyphoneを翻訳する](development/translate-polyphone.md)」を参照）、ビルド時にプラグインに埋め込まれます。


## トラブルシューティング


必要な場合は、フォーラムのこの[カテゴリ](forum/support-bug-reports)が役に立つでしょう。
