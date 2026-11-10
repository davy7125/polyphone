Use estas instruções para compilar o Polyphone para Windows com a versão mais recente do código-fonte no <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>. Você terá o software no estágio mais avançado de desenvolvimento, com os recursos mais recentes, mas talvez também com algumas instabilidades.


## Pré-requisitos


### Instalação do Qt

O Polyphone é escrito em C++ com o ambiente de desenvolvimento Qt (versão 6). Para instalar o Qt, acesse esta <a href="https://www.qt.io/download-qt-installer-oss" target="_blank">página</a> e baixe o instalador. Será preciso criar uma conta primeiro.

Em seguida, execute o instalador e selecione os seguintes componentes:

![Componentes a selecionar](images/select-qt-components.png "Componentes a selecionar")

Escolha a versão mais recente disponível se essas opções não forem exatamente as mesmas.
O componente “MinGW 64-bit” da versão do Qt fornece as bibliotecas do Qt compiladas para o MinGW.


### Instalação do compilador e das dependências

O Polyphone é compilado com o MinGW pelos seguintes motivos:

* há sobretudo um motivo prático, já que as dependências abaixo são compatíveis com o MinGW,
* mas também para permitir a importação / exportação correta de arquivos no formato sfArk (a opção de compilação ``-ffloat-store`` é necessária).

O compilador, as ferramentas de compilação :program:`CMake` e :program:`Ninja` e as bibliotecas usadas pelo Polyphone são instalados com o <a href="https://www.msys2.org/" target="_blank">MSYS2</a>.
Instale-o no diretório padrão :file:`C:/msys64`, abra o terminal :program:`MSYS2 MINGW64` e execute este comando:

```
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-zlib mingw-w64-x86_64-libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-openssl mingw-w64-x86_64-flac mingw-w64-x86_64-libsndfile
```

Em seguida, acesse as configurações do sistema Windows para adicionar a pasta :file:`C:/msys64/mingw64/bin` ao Path:

* abra o menu do Windows,
* digite “Variáveis de ambiente”,
* abra as propriedades do sistema e clique em “Variáveis de Ambiente”,
* clique duas vezes em “Path” nas variáveis do sistema,
* e adicione :file:`C:/msys64/mingw64/bin` aos diretórios listados na variável “Path”.

Dessa forma, o compilador, as ferramentas e as bibliotecas do MSYS2 podem ser encontrados pelo Qt Creator e pelo Polyphone.


## Compilação


### Código-fonte

Comece baixando o código-fonte do Polyphone <a href="software" target="_blank">aqui</a> ou no <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> e descompacte os arquivos na sua área de trabalho do Windows, por exemplo.

### Compilação com o Qt Creator

Continue agora com este tutorial para compilar o Polyphone com o Qt Creator: [Usar o Qt Creator para compilar o Polyphone](development/using-qt-creator-to-build-polyphone.md).
O kit usado deve ter o compilador do MSYS2 (:file:`C:\msys64\mingw64\bin\g++.exe`), que encontra as bibliotecas instaladas com o MSYS2, e a versão do Qt para MinGW 64-bit.

### Compilação pela linha de comando

Este é o método usado para compilar as versões Windows do Polyphone.
Abra o terminal :program:`MSYS2 MINGW64`, vá para a raiz do projeto (o diretório que contém :file:`sources`) e execute os seguintes comandos (adapte o caminho do Qt antes):

```
export PATH="/mingw64/bin:/c/Qt/6.6.2/mingw_64/bin:$PATH"
cmake -S sources -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:/Qt/6.6.2/mingw_64
cmake --build build --parallel 4
```

Se a compilação correr bem, você verá o arquivo executável :file:`polyphone.exe` no diretório :file:`build/bin`.


### Iniciar o Polyphone

Para que o executável possa ser executado de forma independente, sua pasta precisa ser completada com as bibliotecas do Qt.
Execute o seguinte comando em um terminal (adapte os caminhos antes):

```
C:\Qt\6.6.2\mingw_64\bin\windeployqt.exe --release "C:\Users\User\Desktop\polyphone\build\bin\polyphone.exe"
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
