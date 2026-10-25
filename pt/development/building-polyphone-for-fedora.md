Use estas instruções se você não conseguir instalar o Polyphone na sua distribuição Fedora com os instaladores disponíveis na seção “[Software](software)”.


## Pré-requisitos


### Instalação do Qt

Após uma instalação básica do Fedora, instale os seguintes pacotes (incluindo um compilador), pré-requisitos para o desenvolvimento com Qt:

```
sudo dnf update
sudo dnf install @development-tools
```

Em seguida, instale o framework Qt 6 com o seguinte comando:

```
sudo dnf install qt-creator qt6-qtbase-devel qt6-qtsvg-devel qt6-qttools-devel
```


### Dependências do Polyphone

Instale as seguintes dependências (se você tiver um gerenciador de pacotes gráfico instalado, também pode usá-lo para a instalação):

```
sudo dnf install alsa-lib-devel jack-audio-connection-kit-devel rtaudio-devel zlib-devel libogg-devel flac-devel libvorbis-devel glib2-devel openssl-devel rtmidi-devel stk-devel libsndfile-devel
```

**Observação:** nas versões recentes do Fedora, o pacote :program:`jack-audio-connection-kit-devel` pode ser substituído por :program:`pipewire-jack-audio-connection-kit-devel`.


## Compilação


### Código-fonte


Primeiro, obtenha o código-fonte do Polyphone <a href="software" target="_blank">aqui</a> ou no <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.

Se quiser continuar a compilação com o Qt Creator, siga agora este tutorial: [compilar com o Qt Creator](development/using-qt-creator-to-build-polyphone.md). Caso contrário, continue a leitura deste documento.


### Obter o executável


Vá para o diretório do código-fonte (onde fica o arquivo :file:`polyphone.pro`), abra um terminal e compile o Polyphone com este comando:

```
qmake6 && make
```

Se tudo correr bem, você terá o arquivo compilado :file:`polyphone` no subdiretório :file:`bin`.
Quando a compilação é feita com o Qt Creator, esse arquivo fica, em vez disso, no diretório :file:`build-XXX` criado ao lado de :file:`sources`.


## Solução de problemas


Esta [categoria](forum/support-bug-reports) do fórum pode ajudar você, se necessário.
