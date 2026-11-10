Use estas instruções para compilar o [plugin](manual/plugin.md) do Polyphone (VST3, CLAP, LV2 e AU no macOS) a partir do seu código-fonte.
O plugin é feito com <a href="https://juce.com" target="_blank">JUCE</a> e <a href="https://github.com/free-audio/clap-juce-extensions" target="_blank">clap-juce-extensions</a>, baixados automaticamente durante a configuração do projeto: ao contrário do editor, ele não precisa do Qt.


## Pré-requisitos


Os seguintes elementos são necessários:

* o :program:`CMake` 3.22 ou superior e um compilador C++17,
* a biblioteca :program:`libvorbisfile`, para ler os arquivos sf3,
* a biblioteca :program:`libsndfile`, para ler as amostras dos arquivos sfz e as ondas dos arquivos dls.

Você também precisa do código-fonte do Polyphone, disponível <a href="software" target="_blank">aqui</a> ou no <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


### Linux

Os pacotes exigidos pelo JUCE também são necessários: ALSA, freetype, fontconfig e as bibliotecas X11.
No Ubuntu ou no Debian:

```
sudo apt-get install build-essential cmake pkgconf libvorbis-dev libsndfile1-dev libasound2-dev libfreetype-dev libfontconfig1-dev libx11-dev libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev libxi-dev libxrandr-dev libxrender-dev libgl-dev
```

No Fedora:

```
sudo dnf install @development-tools cmake pkgconf libvorbis-devel libsndfile-devel alsa-lib-devel freetype-devel fontconfig-devel libX11-devel libXcomposite-devel libXcursor-devel libXext-devel libXinerama-devel libXi-devel libXrandr-devel libXrender-devel mesa-libGL-devel
```


### Windows e macOS

As bibliotecas podem ser baixadas e compiladas durante a configuração, adicionando as opções ``-DPOLYPHONE_FETCH_VORBIS=ON -DPOLYPHONE_FETCH_SNDFILE=ON`` ao primeiro comando abaixo.
Compilada dessa forma, a :program:`libsndfile` não tem nenhuma biblioteca externa: as amostras dos arquivos sfz podem ser lidas nos formatos wav e aiff, mas não flac, ogg ou opus.

As versões oficiais usam, em vez disso, o <a href="https://vcpkg.io" target="_blank">vcpkg</a>, que fornece bibliotecas estáticas completas (flac, ogg, opus e mp3 podem ser lidos).
As bibliotecas são instaladas com:

```
vcpkg install --triplet <triplet> --overlay-triplets=packaging/vcpkg-triplets libsndfile libvorbis
```

e as seguintes opções são então adicionadas ao primeiro comando abaixo:

```
-DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=<triplet> -DVCPKG_OVERLAY_TRIPLETS=packaging/vcpkg-triplets -DVCPKG_MANIFEST_MODE=OFF
```

sendo o triplet ``x64-windows-polyphone``, ``arm64-osx-polyphone`` ou ``x64-osx-polyphone``.
No Windows, o plugin é compilado com o :program:`clang-cl`, do :program:`Visual Studio`: a opção ``-T ClangCL`` também deve ser adicionada.


## Compilação


Na raiz do projeto (o diretório que contém :file:`sources`), abra um terminal e execute os seguintes comandos:

```
cmake -S sources/plugin -B build/plugin -DCMAKE_BUILD_TYPE=Release
cmake --build build/plugin --config Release --parallel 4
```

Os plugins ficam então no diretório :file:`build/plugin/PolyphonePlugin_artefacts/Release`: :file:`VST3/Polyphone.vst3`, :file:`CLAP/Polyphone.clap`, :file:`LV2/Polyphone.lv2` e :file:`AU/Polyphone.component` no macOS.
Eles são instalados copiando-os nas pastas dos hospedeiros, como descrito na página “[Plugin do Polyphone](manual/plugin.md#doc_install)”.

O diretório :file:`Standalone` também contém um aplicativo autônomo, o :program:`Polyphone Player`, que toca as soundfonts sem hospedeiro.

O plugin também pode ser compilado junto com o editor, adicionando a opção ``-DPOLYPHONE_BUILD_PLUGIN=ON`` à [compilação do Polyphone](development/building-polyphone-for-ubuntu.md).


## Traduções


Os textos do plugin estão nos arquivos de tradução do Polyphone (contextos :guilabel:`PluginEditor`, :guilabel:`PluginProcessor` e :guilabel:`PluginJuce`): eles são traduzidos ao mesmo tempo que o editor (veja [Traduzir o Polyphone](development/translate-polyphone.md)) e incorporados ao plugin durante a sua compilação.


## Solução de problemas


Esta [categoria](forum/support-bug-reports) do fórum pode ajudar você, se necessário.
