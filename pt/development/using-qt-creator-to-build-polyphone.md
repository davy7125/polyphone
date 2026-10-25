Use estas instruções para saber como compilar o Polyphone com o Qt Creator.
Supõe-se que você já tenha no computador o Qt Creator instalado com o framework Qt, um compilador e as dependências necessárias para compilar o Polyphone (veja as páginas correspondentes).


## Pré-requisitos


Os seguintes elementos são necessários:

* um compilador C++ (VC++, Clang, GCC, …),
* o Qt Creator com seu framework (Qt 6),
* as dependências do Polyphone.

As instruções para instalá-los dependem do sistema operacional que você usa; veja as páginas de documentação correspondentes.


## Compilar o Polyphone


### Copiar o código-fonte


Obtenha o código-fonte do Polyphone <a href="software" target="_blank">aqui</a> ou no <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


### Abrir e configurar o projeto


Primeiro, abra o arquivo :file:`polyphone.pro` e edite-o para, eventualmente, usar as versões distribuídas de :program:`rtmidi` e de :program:`stk`, caso seu sistema operacional não as forneça (a versão distribuída de :program:`rtaudio` é usada por padrão).
Descomente as linhas a seguir removendo o ‘#’ inicial:

```
DEFINES += USE_LOCAL_RTMIDI
DEFINES += USE_LOCAL_STK
```

Você pode pular essas modificações se quiser usar as bibliotecas já instaladas no computador (verifique se elas estão presentes!).
Esta etapa não é necessária no Windows e no macOS, pois as versões distribuídas são sempre usadas.

Inicie o Qt Creator e abra o arquivo :file:`polyphone.pro`.


![Abrir o Qt Creator](images/open-Qt-Creator.png "Abrir o Qt Creator")


Em seguida, configure o projeto.

![Configurar o projeto](images/configure-project.png "Configurar o projeto")


### Executar o qmake e compilar o projeto


Vá ao menu :guilabel:`Build` e escolha o item :guilabel:`Run qmake`.


![Executar o qmake](images/run-qmake.png "Executar o qmake")


A execução do :command:`qmake` termina quando a barra de progresso, no canto inferior direito, chega a 100% em verde.


![qmake concluído](images/qmake-finished.png "qmake concluído")


Na seção :guilabel:`Project` da coluna esquerda e depois na seção :guilabel:`Build&Run`, selecione a configuração de compilação (:guilabel:`Release` é a preferida).

Por fim, clique na ação :guilabel:`Build project Polyphone` do menu :guilabel:`Build`. Durante a compilação do projeto, podem aparecer avisos, mas não deve haver erros.

Agora você deve ter o executável no diretório de compilação criado pelo Qt Creator (:file:`build-XXX`, ao lado do código-fonte, por exemplo), de acordo com a sua escolha na configuração de compilação do Qt Creator:

* no Linux, o arquivo :file:`polyphone` fica no subdiretório :file:`bin`,
* no Windows, o arquivo :file:`polyphone.exe` fica no subdiretório :file:`release` ou :file:`debug`,
* no macOS, o bundle :file:`Polyphone.app` fica no diretório :file:`macos`, localizado ao lado do diretório :file:`sources`.


## Solução de problemas


Consulte esta [categoria](forum/support-bug-reports) se precisar de mais informações ou de ajuda.


<p class="endpage">Um agradecimento especial a <a href="dashboard/820-tomotello">Tomotello</a> por este artigo.</p>