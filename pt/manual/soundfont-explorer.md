Com o Polyphone, é possível abrir um diretório a partir da [tela inicial](manual/index.md#doc_left) clicando no botão :guilabel:`Abrir diretório`. O explorador de soundfonts abre então em uma nova aba para exibir os arquivos desse diretório (seus subdiretórios não são analisados), apresentando, para cada soundfont, uma prévia do seu conteúdo.


![Explorador de soundfonts](images/soundfont_explorer.png "Explorador de soundfonts")


## Prévia das soundfonts {#doc_preview}


A lista principal, localizada no lado esquerdo da interface, contém a lista de arquivos, e o Polyphone os distingue da seguinte forma:

* os arquivos compatíveis que podem ser analisados rapidamente (formatos [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz) e [.dls](manual/annexes/the-different-soundfont-formats.md#doc_dls), por exemplo) são exibidos com detalhes sobre seu conteúdo,
* os arquivos compatíveis que não podem ser analisados rapidamente (formato [.sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark), em particular) são exibidos apenas com informações básicas,
* os arquivos não compatíveis ficam acinzentados e não podem ser abertos.

Clicar em um arquivo analisável exibe, nas listas do lado direito da interface, as amostras, os instrumentos e as predefinições que ele contém. Cada item dessas listas também traz informações que ajudam a avaliar o nível de detalhe da soundfont e a quantidade de trabalho investida nela.

### Detalhes no nível do arquivo

* o tamanho e a data da última modificação são exibidos para cada arquivo,
* para as soundfonts que podem ser analisadas, são acrescentadas as seguintes informações:
  * número de amostras, instrumentos e predefinições,
  * nomes das primeiras predefinições.

### Informações de cada amostra

* tipo (mono / estéreo),
* duração total,
* duração do loop, se houver,
* taxa de amostragem.

### Informações de cada instrumento ou predefinição

* número total de divisões, parâmetros e geradores,
* número de alcances de notas distintos,
* número de alcances de velocidade distintos.


## Funções de ordenação e de pesquisa {#doc_filter}


A seção superior traz um campo de texto (o atalho :kbd:`Ctrl`+:kbd:`F` coloca o foco nele). Quando ele é preenchido, o explorador exibe apenas os arquivos que atendem a pelo menos um dos seguintes critérios:

* o nome contém o texto pesquisado,
* o nome de uma das amostras, instrumentos ou predefinições contém o texto pesquisado.

As listas de amostras, instrumentos e predefinições também são filtradas para destacar os elementos correspondentes.

À direita do campo de texto há uma lista suspensa que permite ordenar os arquivos por nome (A→Z), pela data da última modificação (mais recentes primeiro) ou pelo tamanho do arquivo (maiores ou menores primeiro).


## Operações com arquivos {#doc_file_operations}


Ao clicar em um arquivo da lista principal, 2 ícones clicáveis aparecem antes do nome do arquivo:

* um ícone de lápis para renomear o arquivo (também acessível com :kbd:`F2`),
* um ícone de lixeira para mover o arquivo para a lixeira, após confirmação (também acessível com :kbd:`Del`).

Essas operações permitem gerenciar e organizar rapidamente uma coleção local de soundfonts.


## Abrir uma soundfont {#doc_open_soundfont}


Um clique duplo em um item da lista principal abre o arquivo — desde que seu formato seja compatível com o Polyphone — no local configurado nas [configurações](manual/settings.md#doc_interface) com a opção :guilabel:`Visualização inicial` (as [informações gerais](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md) são exibidas por padrão).

Um clique duplo em uma [amostra](manual/soundfont-editor/editing-pages/sample-editor.md), em um [instrumento](manual/soundfont-editor/editing-pages/instrument-editor.md) ou em uma [predefinição](manual/soundfont-editor/editing-pages/preset-editor.md) abre o arquivo no elemento selecionado.