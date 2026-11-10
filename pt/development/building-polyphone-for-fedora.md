Use estas instruções se você não conseguir instalar o Polyphone na sua distribuição Fedora com os instaladores disponíveis na seção “[Software](software)”.


## Pré-requisitos


### Instalação do Qt

Após uma instalação básica do Fedora, instale os seguintes pacotes (incluindo um compilador e o :program:`CMake`), pré-requisitos para o desenvolvimento com Qt:

```
sudo dnf update
sudo dnf install @development-tools cmake pkgconf
```

Em seguida, instale o framework Qt 6 com o seguinte comando:

```
sudo dnf install qt-creator qt6-qtbase-devel qt6-qtsvg-devel qt6-qttools-devel
```


### Dependências do Polyphone

Instale as seguintes dependências (se você tiver um gerenciador de pacotes gráfico instalado, também pode usá-lo para a instalação):

```
sudo dnf install alsa-lib-devel jack-audio-connection-kit-devel rtaudio-devel zlib-devel libogg-devel flac-devel libvorbis-devel glib2-devel openssl-devel rtmidi-devel libsndfile-devel
```

**Observação:** nas versões recentes do Fedora, o pacote :program:`jack-audio-connection-kit-devel` pode ser substituído por :program:`pipewire-jack-audio-connection-kit-devel`.


## Compilação


### Código-fonte


Primeiro, obtenha o código-fonte do Polyphone <a href="software" target="_blank">aqui</a> ou no <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.

Se quiser continuar a compilação com o Qt Creator, siga agora este tutorial: [compilar com o Qt Creator](development/using-qt-creator-to-build-polyphone.md). Caso contrário, continue a leitura deste documento.


### Obter o executável


O Polyphone é compilado com o :program:`CMake`, sendo o projeto descrito pelo arquivo :file:`CMakeLists.txt` do diretório :file:`sources`.
Na raiz do projeto (o diretório que contém :file:`sources`), abra um terminal e compile o Polyphone com estes comandos:

```
cmake -S sources -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 4
```

Se tudo correr bem, você terá o arquivo compilado :file:`polyphone` no diretório :file:`build/bin`.
Quando a compilação é feita com o Qt Creator, esse arquivo fica, em vez disso, no subdiretório :file:`bin` do diretório de compilação escolhido no Qt Creator.

Se o :program:`RtAudio` ou o :program:`RtMidi` não estiverem disponíveis em uma versão compatível, as cópias incluídas no código-fonte do Polyphone podem ser usadas adicionando as opções ``-DPOLYPHONE_LOCAL_RTAUDIO=ON -DPOLYPHONE_LOCAL_RTMIDI=ON`` ao primeiro comando.
As outras opções são descritas na página [Compilar o Polyphone para Ubuntu](development/building-polyphone-for-ubuntu.md).


## Solução de problemas


Esta [categoria](forum/support-bug-reports) do fórum pode ajudar você, se necessário.
