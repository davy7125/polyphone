Os resumos de configuração oferecem uma visão geral de todos os elementos contidos em uma soundfont.
Eles são acessíveis pelos cabeçalhos :guilabel:`Amostras`, :guilabel:`Instrumentos` e :guilabel:`Predefinições` na [árvore](manual/soundfont-editor/tree.md) e permitem visualizar os elementos correspondentes.

As informações reunidas nessas páginas podem servir de apoio para inspeções em soundfonts grandes.

Cada elemento é exibido em uma linha de uma tabela.
Um clique no cabeçalho de uma coluna ordena a tabela de acordo com essa coluna, e um clique duplo em uma linha abre o elemento correspondente em sua [página de edição](manual/soundfont-editor/editing-pages/index.md).
Os valores que merecem atenção são escritos em negrito, em amarelo (aviso) ou em vermelho (erro).


## Amostras {#doc_sample}


Ao clicar no cabeçalho :guilabel:`Amostras` de uma soundfont na [árvore](manual/soundfont-editor/tree.md), aparece o resumo de configuração das amostras.


![Resumo de configuração das amostras](images/conf_summary_sample.png "Resumo de configuração das amostras")


Para cada amostra, as colunas são: :guilabel:`Usada`, :guilabel:`Duração total`, :guilabel:`Duração do loop`, :guilabel:`Nota base`, :guilabel:`Correção`, :guilabel:`Tipo de amostra`, :guilabel:`Amostra vinculada`, :guilabel:`Taxa de amostragem` e :guilabel:`Compressão`.
Esta página permite, por exemplo, fazer as seguintes verificações:

* A amostra é usada?
* Sua duração total (e eventualmente a duração do loop) é longa o suficiente?
* O loop está correto? (um loop que pode produzir um clique é destacado)
* A afinação foi feita?
* Ela está corretamente vinculada em estéreo? (as duas amostras de um par estéreo devem ter a mesma nota base)
* A taxa de amostragem é alta o suficiente para uma boa qualidade de áudio?
* Ela foi comprimida?


## Instrumentos {#doc_instrument}


Ao clicar no cabeçalho :guilabel:`Instrumentos` de uma soundfont na [árvore](manual/soundfont-editor/tree.md), aparece o resumo de configuração dos instrumentos.


![Resumo de configuração dos instrumentos](images/conf_summary_instrument.png "Resumo de configuração dos instrumentos")


Para cada instrumento, as colunas são: :guilabel:`Usado`, :guilabel:`Número de amostras`, :guilabel:`Número de parâmetros`, :guilabel:`Número de moduladores`, :guilabel:`Alcance máximo de notas`, :guilabel:`Alcance máximo de velocidade`, :guilabel:`Atenuação`, :guilabel:`Reprodução em loop`, :guilabel:`Chorus` e :guilabel:`Reverberação`.
Esta página permite, por exemplo, fazer as seguintes verificações:

* O instrumento é usado?
* Os números de amostras vinculadas, de parâmetros e de moduladores são coerentes?
* Os alcances de notas e de velocidade estão corretos?
* As amostras foram atenuadas corretamente?
* A reprodução em loop está ativada?
* Os ajustes de chorus e de reverberação foram esquecidos?


## Predefinições {#doc_preset}


Ao clicar no cabeçalho :guilabel:`Predefinições` de uma soundfont na [árvore](manual/soundfont-editor/tree.md), aparece o resumo de configuração das predefinições.


![Resumo de configuração das predefinições](images/conf_summary_preset.png "Resumo de configuração das predefinições")


Para cada predefinição, as colunas são: :guilabel:`Banco - Predefinição`, :guilabel:`Número de instrumentos`, :guilabel:`Número de parâmetros`, :guilabel:`Número de moduladores`, :guilabel:`Alcance máximo de notas`, :guilabel:`Alcance máximo de velocidade`, :guilabel:`Atenuação`, :guilabel:`Chorus` e :guilabel:`Reverberação`.
Esta página permite, por exemplo, fazer as seguintes verificações:

* Os números de banco e de predefinição foram preenchidos corretamente?
* Os números de instrumentos vinculados, de parâmetros e de moduladores são coerentes?
* Os alcances de notas e de velocidade estão corretos?
* Os instrumentos foram atenuados com precisão?
* Os ajustes de chorus e de reverberação (como deslocamentos) foram esquecidos?