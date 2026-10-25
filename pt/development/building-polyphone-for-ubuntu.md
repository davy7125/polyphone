Use estas instruções se você não conseguir instalar o Polyphone na sua distribuição Linux com os instaladores disponíveis na seção “[Software](software)”.
Este é o método usado para compilar as versões Linux do Polyphone, no Ubuntu 24.04.


## Pré-requisitos


As seguintes ferramentas e bibliotecas são necessárias:

* um compilador (``build-essential``)
* qt 6 (``qt6-base-dev``, ``qt6-svg-dev``, ``qt6-tools-dev``, ``qt6-tools-dev-tools`` e ``qt6-l10n-tools``)
* alsa (``libasound2-dev``)
* jack (``libjack-jackd2-dev``)
* rtaudio (``librtaudio-dev``)
* rtmidi (``librtmidi-dev``)
* stk (``libstk-dev``)
* vorbis (``libvorbis-dev``)
* ogg (``libogg-dev``)
* flac (``libflac-dev``)
* zlib (``zlib1g-dev``)
* ssl (``libssl-dev``)
* sndfile (``libsndfile1-dev``)

Elas podem ser instaladas com o seguinte comando:

```
sudo apt-get install build-essential qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools libasound2-dev libjack-jackd2-dev librtaudio-dev librtmidi-dev libstk-dev libvorbis-dev libogg-dev libflac-dev zlib1g-dev libssl-dev libsndfile1-dev
```

Você também precisa do código-fonte do Polyphone, disponível <a href="software" target="_blank">aqui</a> ou no <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


## Compilação


No diretório :file:`sources` do projeto (onde fica o arquivo :file:`polyphone.pro`), abra um terminal e compile o Polyphone executando o seguinte comando:

```
qmake6 polyphone.pro PREFIX=/usr && make
```

Um arquivo executável :file:`polyphone` deve então aparecer no diretório :file:`bin`, se tudo tiver corrido bem.
Ele pode ser instalado no sistema com o comando :command:`sudo make install`.

Por padrão, é usada uma cópia local da biblioteca :program:`RtAudio`.
Caso bibliotecas como :program:`RtMidi` ou :program:`Stk` estejam ausentes ou não estejam em uma versão compatível na sua distribuição, você pode editar o arquivo :file:`.pro` para usar cópias locais delas.
Para isso, descomente as linhas correspondentes. Por exemplo,

```
#DEFINES += USE_LOCAL_RTMIDI
```

se tornará

```
DEFINES += USE_LOCAL_RTMIDI
```

(sem o “#” inicial).

**Observação:** se você usa o :program:`Qt Creator`, o projeto pode ser aberto pelo arquivo :file:`.pro` presente no diretório :file:`sources`.
