Use estas instruções para compilar o Polyphone para Windows com a versão mais recente do código-fonte no <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>. Você terá o software no estágio mais avançado de desenvolvimento, com os recursos mais recentes, mas talvez também com algumas instabilidades.


## Pré-requisitos


### Instalação do Qt

O Polyphone é escrito em C++ com o ambiente de desenvolvimento Qt (versão 6). Para instalar o Qt, acesse esta <a href="https://www.qt.io/download-qt-installer-oss" target="_blank">página</a> e baixe o instalador. Será preciso criar uma conta primeiro.

Em seguida, execute o instalador e selecione os seguintes componentes:

![Componentes a selecionar](images/select-qt-components.png "Componentes a selecionar")

Escolha a versão mais recente disponível se essas opções não forem exatamente as mesmas.


### Instalação do compilador e das dependências

O Qt precisa de um compilador para compilar o código-fonte do Polyphone. É necessário usar o MinGW pelos seguintes motivos:

* há sobretudo um motivo prático, já que as dependências abaixo são compatíveis com o MinGW,
* mas também para permitir a importação / exportação correta de arquivos no formato sfArk (a opção de compilação ``-ffloat-store`` é necessária).

O compilador e as bibliotecas usadas pelo Polyphone são instalados com o <a href="https://www.msys2.org/" target="_blank">MSYS2</a>.
Instale-o no diretório padrão :file:`C:/msys64` (o arquivo de projeto :file:`polyphone.pro` procura as bibliotecas em :file:`C:/msys64/mingw64`), abra o terminal :program:`MSYS2 MINGW64` e execute este comando:

```
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-zlib mingw-w64-x86_64-libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-openssl mingw-w64-x86_64-flac mingw-w64-x86_64-libsndfile
```

Em seguida, acesse as configurações do sistema Windows para adicionar a pasta :file:`C:/msys64/mingw64/bin` ao Path:

* abra o menu do Windows,
* digite “Variáveis de ambiente”,
* abra as propriedades do sistema e clique em “Variáveis de Ambiente”,
* clique duas vezes em “Path” nas variáveis do sistema,
* e adicione :file:`C:/msys64/mingw64/bin` aos diretórios listados na variável “Path”.

Dessa forma, os executáveis e as bibliotecas do MinGW podem ser encontrados pelo Qt.


## Compilação


### Código-fonte

Comece baixando o código-fonte do Polyphone <a href="software" target="_blank">aqui</a> ou no <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> e descompacte os arquivos na sua área de trabalho do Windows, por exemplo.

### Compilação com o Qt Creator

Continue agora com este tutorial para compilar o Polyphone com o Qt Creator: [Usar o Qt Creator para compilar o Polyphone](development/using-qt-creator-to-build-polyphone.md). Nenhuma modificação do arquivo :file:`polyphone.pro` será necessária.

### Compilação pela linha de comando

Abra o terminal :program:`MSYS2 MINGW64`, adicione o Qt ao Path e compile o projeto em um novo diretório (adapte os caminhos antes):

```
export PATH="/c/Qt/6.6.2/mingw_64/bin:$PATH"
mkdir build
cd build
qmake ../sources/polyphone.pro CONFIG+=release
mingw32-make -j4
```

Se a compilação correr bem, você verá o arquivo executável :file:`polyphone.exe` no diretório :file:`build/release`.


### Iniciar o Polyphone

Para que o executável possa ser executado de forma independente, sua pasta precisa ser completada com as bibliotecas do Qt.
Execute o seguinte comando em um terminal (adapte os caminhos antes):

```
C:\Qt\6.6.2\mingw_64\bin\windeployqt.exe "C:\Users\User\Desktop\build\release\polyphone.exe"
```

As bibliotecas do Qt aparecerão (:file:`Qt6Core`, por exemplo), e um clique duplo no arquivo :file:`polyphone.exe` iniciará o software.
Para que o Polyphone possa ser iniciado em outro computador Windows, estes arquivos de :file:`C:\msys64\mingw64\bin` também são necessários para completar a pasta:

* libgcc_s_seh-1.dll
* libstdc++-6.dll
* libwinpthread-1.dll
* libssp-0.dll
* libsndfile-1.dll
* libFLAC.dll
* libogg-0.dll
* libvorbis-0.dll
* libvorbisenc-2.dll
* libvorbisfile-3.dll
* libopus-0.dll
* libmp3lame-0.dll
* libmpg123-0.dll
* libcrypto-3-x64.dll
* libssl-3-x64.dll
* zlib1.dll


## Solução de problemas


Esta [categoria](forum/support-bug-reports) do fórum pode ajudar você, se necessário.
