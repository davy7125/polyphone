Use estas instruções se você não conseguir instalar o Polyphone no seu Mac com os instaladores disponíveis na seção “[Software](software)”.


## Pré-requisitos


Os seguintes aplicativos são necessários:

* <a href="https://www.qt.io/download-open-source" target="_blank">Qt Creator com seu framework</a> (versão 6),
* Xcode ou apenas as "Command Line Tools" (a versão a usar é indicada no site do Qt — <a href="https://doc.qt.io/qt-6/supported-platforms.html" target="_blank">aqui</a>, por exemplo).

Você também precisa do código-fonte do Polyphone, disponível <a href="software" target="_blank">aqui</a> ou no <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.

Para abrir corretamente um projeto com o Qt Creator, é preciso primeiro conceder a ele a permissão de ler todos os arquivos do computador (em “Privacidade e Segurança” => “Acesso Total ao Disco” => adicionar “Qt Creator.app”, que fica no seu diretório de instalação).

As bibliotecas usadas pelo Polyphone são instaladas facilmente com o <a href="https://brew.sh/" target="_blank">Homebrew</a>. Depois de instalar essa ferramenta, execute este comando:

```
brew install libsndfile libogg libvorbis flac lame mpg123 opus openssl
```

O arquivo de projeto :file:`polyphone.pro` procura os cabeçalhos e as bibliotecas em :file:`/opt/homebrew` (Apple Silicon), em :file:`/usr/local` (Intel) e no diretório :file:`macos`, localizado ao lado do diretório :file:`sources`.
O arquivo de bibliotecas estáticas disponível [aqui](files/macos.zip) também pode ser descompactado para obter o diretório :file:`macos`, mas seu conteúdo pode estar desatualizado.

Se algumas bibliotecas não estiverem atualizadas ou não forem compatíveis com a versão do seu Mac, como a SSL, por exemplo, será preciso procurar o código-fonte delas na internet e recompilá-las ativando a ligação estática:

```
configure --enable-static --disable-shared --prefix=...
make
make install
```

Depois, copie todos os .h (cabeçalhos) em :file:`macos/include` e todos os .a (bibliotecas estáticas) em :file:`macos/lib`.


## Compilação


Abra o arquivo :file:`polyphone.pro` com o :program:`Qt Creator` (veja [Usar o Qt Creator para compilar o Polyphone](development/using-qt-creator-to-build-polyphone.md)) e compile o projeto.
Um bundle :file:`Polyphone.app` deve aparecer no diretório :file:`macos`, localizado ao lado do diretório :file:`sources`.

A compilação também pode ser feita pela linha de comando:

```
cd sources
qmake polyphone.pro CONFIG+=release
make -j4
```

Se o compilador reclamar de que alguns recursos não estão disponíveis com a versão de macOS de destino especificada, você pode aumentar esta variável no arquivo :file:`polyphone.pro` (o valor padrão é 12.0):

```
QMAKE_MACOSX_DEPLOYMENT_TARGET = 12.0
```

Esse valor também deve ser escrito no arquivo :file:`packaging/mac/polyphone.plist`, abaixo de “LSMinimumSystemVersion”.


## Completar o bundle


Execute o seguinte comando para incluir as bibliotecas e os frameworks dentro do bundle (ajuste o caminho antes!):

```
/Path/To/Qt/6.7.3/macos/bin/macdeployqt /Path/To/macos/Polyphone.app
```

Em um Mac com processador Apple Silicon, o bundle também precisa ser assinado (mesmo com uma assinatura ad hoc) para poder ser executado:

```
xattr -cr /Path/To/macos/Polyphone.app
codesign --force --deep --sign - /Path/To/macos/Polyphone.app
```

Você pode então executar o programa ou compactá-lo em um arquivo .zip ou .dmg para compartilhá-lo.


## Solução de problemas


Consulte este [tópico](forum/support-bug-reports/8-success-build-polyphone-on-osx-10-11-6-qt-5-7) se precisar de mais informações ou de ajuda.
