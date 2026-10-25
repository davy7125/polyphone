Quando uma predefinição ou uma de suas divisões está selecionada na [árvore](manual/soundfont-editor/tree.md), o [menu de ferramentas](manual/soundfont-editor/tools/index.md) contém várias ferramentas organizadas em 4 categorias:

* [análise](#doc_cat_analyze),
* [edição rápida](#doc_cat_fast),
* [moduladores](#doc_cat_mod),
* [transformação](#doc_cat_transformation).

É possível usar as ferramentas simultaneamente em várias predefinições (com uma seleção múltipla), exceto para exibir parâmetros.
Todas as ferramentas também estão disponíveis para os [instrumentos](manual/soundfont-editor/tools/instrument-tools.md).


## Análise {#doc_cat_analyze}


### Exibir parâmetros {#doc_display}

Mesma ferramenta que “[Exibir parâmetros](manual/soundfont-editor/tools/instrument-tools.md#doc_display)” para os instrumentos.


## Edição rápida {#doc_cat_fast}


### Alterar atenuações {#doc_attenuation}

Mesma ferramenta que “[Alterar atenuações](manual/soundfont-editor/tools/instrument-tools.md#doc_attenuation)” para os instrumentos.


### Configuração baseada em notas {#doc_global}

Mesma ferramenta que “[Configuração baseada em notas](manual/soundfont-editor/tools/instrument-tools.md#doc_global)” para os instrumentos.


### Espacialização do som {#doc_spatialization}

Mesma ferramenta que “[Espacialização do som](manual/soundfont-editor/tools/instrument-tools.md#doc_spatialization)” para os instrumentos.


## Moduladores {#doc_cat_mod}


### Remover moduladores {#doc_remove_mod}

Mesma ferramenta que “[Remover moduladores](manual/soundfont-editor/tools/instrument-tools.md#doc_remove_mod)” para os instrumentos.


## Transformação {#doc_cat_transformation}


### Duplicação de divisão {#doc_division}

Mesma ferramenta que “[Duplicação de divisão](manual/soundfont-editor/tools/instrument-tools.md#doc_division)” para os instrumentos.


### Mesclar amostras para cada nota {#doc_mergesamples}

Quando uma predefinição chama simultaneamente vários instrumentos e amostras ao pressionar uma tecla, esta ferramenta mescla, para cada nota, todas as amostras tocadas. O resultado é uma nova predefinição que toca apenas uma amostra por nota, reduzindo assim a carga de CPU necessária para executar a predefinição.
A janela pede o nome da nova predefinição, a duração do sustain e a duração do release dos sons gravados, o intervalo entre as amostras criadas (cada nota, a cada 3 notas ou a cada 6 notas), o loop automático e o tipo de som (mono ou estéreo).

As opções definidas na imagem abaixo fazem a ferramenta simular, a cada 3 notas, uma tecla pressionada durante 5 segundos. A gravação de áudio será em estéreo e o release não será gravado. As amostras assim criadas receberão então um loop com a [ferramenta de loop automático](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop), e o instrumento que contém todas essas amostras, assim como a nova predefinição que o utiliza, serão chamados “Cymbale III”.

**Observação:** usar a opção de loop automático remove a parte do release e diminui ligeiramente a duração sustentada de cada amostra.

![Mesclar amostras para cada nota](images/tool_merge_samples.png "Mesclar amostras para cada nota")
