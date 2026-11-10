Use estas instruções para saber como compilar o Polyphone com o Qt Creator.
Supõe-se que você já tenha no computador o Qt Creator instalado com o framework Qt, um compilador e as dependências necessárias para compilar o Polyphone (veja as páginas correspondentes).


## Pré-requisitos


Os seguintes elementos são necessários:

* um compilador C++ (GCC, Clang, MinGW…),
* o Qt Creator com seu framework (Qt 6, ou Qt 5),
* o :program:`CMake` (versão 3.16 ou superior), que pode ser instalado com o Qt Creator,
* as dependências do Polyphone.

As instruções para instalá-los dependem do sistema operacional que você usa; veja as páginas de documentação correspondentes.


## Compilar o Polyphone


### Copiar o código-fonte


Obtenha o código-fonte do Polyphone <a href="software" target="_blank">aqui</a> ou no <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


### Abrir e configurar o projeto


Inicie o Qt Creator e abra o arquivo :file:`CMakeLists.txt` localizado no diretório :file:`sources`.


![Abrir o Qt Creator](images/open-Qt-Creator.png "Abrir o Qt Creator")


Em seguida, configure o projeto escolhendo um kit, ou seja, uma versão do Qt e um compilador.

![Configurar o projeto](images/configure-project.png "Configurar o projeto")


O Qt Creator executa então o :program:`CMake`, que verifica se as dependências são encontradas.
As mensagens são exibidas no painel :guilabel:`General Messages`: se uma biblioteca estiver faltando, instale-a e execute o :program:`CMake` novamente com o item :guilabel:`Run CMake` do menu :guilabel:`Build`.

As opções do projeto ficam na seção :guilabel:`Projects` da coluna esquerda, em :guilabel:`Build` → :guilabel:`CMake`.
No Linux, as cópias do :program:`RtAudio` e do :program:`RtMidi` incluídas no código-fonte podem ser usadas no lugar das bibliotecas do sistema: ative as opções ``POLYPHONE_LOCAL_RTAUDIO`` e ``POLYPHONE_LOCAL_RTMIDI`` e clique em :guilabel:`Run CMake`.
Isso não é necessário no Windows e no macOS, pois as versões distribuídas são sempre usadas.
Na mesma seção, selecione a configuração de compilação (:guilabel:`Release` é a preferida).


### Compilar o projeto


Por fim, clique na ação :guilabel:`Build Project "Polyphone"` do menu :guilabel:`Build`. Durante a compilação do projeto, podem aparecer avisos, mas não deve haver erros.

Agora você deve ter o executável no subdiretório :file:`bin` do diretório de compilação criado pelo Qt Creator (:file:`build/Desktop_Qt_6_…-Release` no diretório :file:`sources`, por exemplo):

* no Linux, o arquivo :file:`polyphone`,
* no Windows, o arquivo :file:`polyphone.exe`,
* no macOS, o bundle :file:`Polyphone.app`.


## Solução de problemas


Consulte esta [categoria](forum/support-bug-reports) se precisar de mais informações ou de ajuda.


<p class="endpage">Um agradecimento especial a <a href="dashboard/820-tomotello">Tomotello</a> por este artigo.</p>