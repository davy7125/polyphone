A árvore é o primeiro elemento do editor em que você deve se concentrar.
Ela representa a arquitetura da soundfont aberta e controla a parte direita do software quando você seleciona um ou mais elementos.

A árvore é sempre exibida com a mesma [estrutura](#doc_structure), oferece um [menu de contexto](#doc_menu) e tem recursos de [arrastar e soltar](#doc_dragdrop), [copiar / colar](#doc_copypaste) e [pesquisa](#doc_search).


## Estrutura {#doc_structure}


A estrutura da árvore é composta de 4 cabeçalhos e do conteúdo dos cabeçalhos :guilabel:`Amostras`, :guilabel:`Instrumentos` e :guilabel:`Predefinições`.


![Árvore: estrutura](images/tree_1.png "Árvore: estrutura")


### Cabeçalho “Geral”


Um clique no cabeçalho :guilabel:`Geral` abre o editor das [informações gerais](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md).


### Seção “Amostras”


Esta seção lista todas as amostras contidas na soundfont, que são a matéria-prima sobre a qual todos os instrumentos são construídos.
Selecionar uma ou mais amostras leva ao [editor de amostras](manual/soundfont-editor/editing-pages/sample-editor.md).
Um clique no cabeçalho :guilabel:`Amostras` exibe o [resumo de configuração](manual/soundfont-editor/configuration-summaries.md#doc_sample) de todas as amostras.


### Seção “Instrumentos”


Esta seção lista todos os instrumentos contidos na soundfont, sendo o instrumento um elemento intermediário composto de amostras e utilizado pelas predefinições.
Os instrumentos contêm vínculos com amostras chamados “divisões”, exibidos em lista abaixo de cada instrumento.
Selecionar um instrumento ou uma de suas divisões leva ao [editor de instrumentos](manual/soundfont-editor/editing-pages/instrument-editor.md).
Um clique no cabeçalho :guilabel:`Instrumentos` exibe o [resumo de configuração](manual/soundfont-editor/configuration-summaries.md#doc_instrument) de todos os instrumentos.


### Seção “Predefinições”


Esta seção lista todas as predefinições contidas na soundfont, sendo a predefinição um elemento final, composto de instrumentos, acessível de fora da soundfont.
As predefinições contêm vínculos com instrumentos chamados “divisões”, exibidos em lista abaixo de cada predefinição.
Selecionar uma predefinição ou uma de suas divisões mostra o [editor de predefinições](manual/soundfont-editor/editing-pages/preset-editor.md).
Um clique no cabeçalho :guilabel:`Predefinições` exibe o [resumo de configuração](manual/soundfont-editor/configuration-summaries.md#doc_preset) de todas as predefinições.


## Menu de contexto {#doc_menu}


Um clique com o botão direito em um elemento mostra um menu com as seguintes ações:

Vincular a…
: Vincula as amostras selecionadas a um instrumento, ou os instrumentos selecionados a uma predefinição.
  O destino é escolhido em uma lista que pode ser filtrada.
  Quando amostras com notas base diferentes são vinculadas a um instrumento vazio, seus alcances de notas são definidos automaticamente (como com a ferramenta [Posicionamento automático das amostras](manual/soundfont-editor/tools/instrument-tools.md#doc_position)).

Substituir por…
: Substitui uma amostra por outra amostra em uma divisão de instrumento, ou um instrumento por outro instrumento em uma divisão de predefinição.
  A configuração da divisão é mantida.

Copiar
: Copia os elementos selecionados, também acessível com :kbd:`Ctrl`+:kbd:`C`.

Colar
: Cola os elementos copiados anteriormente, também acessível com :kbd:`Ctrl`+:kbd:`V`.

Duplicar
: Duplica os elementos selecionados, que podem ser amostras, instrumentos, predefinições ou divisões.
  Também acessível com :kbd:`Ctrl`+:kbd:`D`.

Excluir
: Exclui os elementos selecionados, também acessível com :kbd:`Del`.
  Uma amostra usada por um instrumento e um instrumento usado por uma predefinição não podem ser excluídos.

Silenciar
: Para fins de teste, uma divisão dentro de um instrumento ou de uma predefinição pode ser silenciada. Essa opção também pode ser encontrada no menu de contexto do [cabeçalho da tabela](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_solo).

Tocar sempre
: Ao contrário da função “Silenciar”, um ou mais instrumentos ou predefinições podem ser adicionados durante a execução. Assim, é possível encontrar um bom equilíbrio entre diferentes instrumentos ou predefinições tocando juntos.

Restaurar reprodução
: Cancela os efeitos de “Silenciar” e “Tocar sempre” em toda a soundfont.

Renomear ou renomear em massa
: Renomeia os elementos selecionados, também acessível com :kbd:`F2`.
  Renomear uma divisão renomeia a amostra ou o instrumento ao qual ela está vinculada.
  Quando vários elementos estão selecionados, uma caixa de diálogo propõe diferentes opções:
  
  * sobrescrever nome existente com nome da nota como sufixo (somente para amostras e divisões de instrumentos),
  * sobrescrever nome existente com nome da nota e velocidade como sufixo (somente para divisões de instrumentos),
  * sobrescrever nome existente com sufixo numérico ascendente,
  * substituir caracteres,
  * inserir texto em uma posição específica,
  * excluir intervalo de caracteres.


![Árvore: menu de contexto](images/tree_2.png "Árvore: menu de contexto")


## Arrastar e soltar {#doc_dragdrop}


A operação “arrastar e soltar” na árvore permite associações ou cópias rápidas.
Antes de arrastar e soltar, é preciso selecionar elementos.
Pode ser um único elemento ou uma seleção múltipla, possível com as teclas :kbd:`Ctrl` e :kbd:`Shift`.

O resultado de arrastar e soltar depende então do tipo dos elementos arrastados e do tipo do elemento de destino.

Amostra → instrumento
: Associação da amostra ao instrumento, criação de uma divisão de instrumento.

Amostra → cabeçalho :guilabel:`Instrumentos`
: Criação de um instrumento que contém todas as amostras arrastadas ou criação de um instrumento por amostra arrastada.

Instrumento → predefinição
: Associação do instrumento à predefinição, criação de uma divisão de predefinição.

Instrumento → instrumento
: Cópia de todas as divisões do instrumento de origem para o instrumento de destino (pode ser o mesmo instrumento).

Divisão de instrumento → instrumento ou divisão de instrumento
: Cópia das divisões selecionadas do instrumento de origem para o instrumento de destino (pode ser o mesmo instrumento).

Instrumento → cabeçalho :guilabel:`Predefinições`
: Criação de uma predefinição que contém todos os instrumentos arrastados ou criação de uma predefinição por instrumento arrastado.

Predefinição → predefinição
: Cópia de todas as divisões da predefinição de origem para a predefinição de destino (pode ser a mesma predefinição).

Divisão de predefinição → predefinição ou divisão de predefinição
: Cópia das divisões selecionadas da predefinição de origem para a predefinição de destino (pode ser a mesma predefinição).

Arquivo de áudio (do explorador de arquivos do computador) → árvore
: Importação do arquivo como uma nova amostra, se o formato for suportado (veja o [editor de amostras](manual/soundfont-editor/editing-pages/sample-editor.md)).


## Copiar / colar {#doc_copypaste}


Tudo o que é possível com arrastar e soltar, como visto anteriormente, também é possível com copiar / colar, acessível pelo menu de contexto ou pelos atalhos :kbd:`Ctrl`+:kbd:`C` e :kbd:`Ctrl`+:kbd:`V`.

Copiar / colar também é permitido entre soundfonts diferentes.
É possível copiar um elemento em uma primeira soundfont e colá-lo em uma segunda soundfont.
Uma seleção múltipla é permitida.

Amostras selecionadas
: Cópia das amostras.

Instrumentos selecionados
: Cópia dos instrumentos, incluindo todas as amostras vinculadas.

Predefinições selecionadas
: Cópia das predefinições, incluindo todos os instrumentos e amostras vinculados.

Durante uma cópia de elementos entre soundfonts diferentes, se já existir um elemento com o mesmo nome, várias opções são oferecidas:

* ignorar a operação, caso em que a cópia é cancelada para o elemento de mesmo nome;
* substituir o elemento existente, caso em que o elemento existente é sobrescrito pelo elemento copiado;
* duplicar o elemento, caso em que o elemento é copiado ao lado do elemento existente com um nome diferente.


## Pesquisa {#doc_search}


Para facilitar a busca de itens na árvore, há uma barra de pesquisa para filtrá-los (o atalho :kbd:`Ctrl`+:kbd:`F` coloca o foco nela).


![Árvore: barra de pesquisa](images/tree_3.png "Árvore: barra de pesquisa")


Quando um texto é digitado na barra de pesquisa, os elementos que o contêm são mostrados.
Além disso:

* se o texto estiver incluído no nome de uma amostra, os instrumentos que usam a amostra e as predefinições que usam esses instrumentos são exibidos;
* se o texto estiver incluído no nome de um instrumento, as amostras usadas pelo instrumento e as predefinições que usam o instrumento são exibidas;
* se o texto estiver incluído no nome de uma predefinição, os instrumentos usados pela predefinição e as amostras usadas por esses instrumentos são exibidos.

Um clique na cruz à direita do campo editável cancela o filtro.