Quando o cabeçalho :guilabel:`Geral` está selecionado na [árvore](manual/soundfont-editor/tree.md), o [menu de ferramentas](manual/soundfont-editor/tools/index.md) oferece ferramentas organizadas em 3 categorias:

* [limpar](#doc_cat_clean),
* [moduladores](#doc_cat_mod),
* [utilitário](#doc_cat_util).


## Limpar {#doc_cat_clean}


### Remover elementos não utilizados {#doc_unused}


Todas as amostras que não são usadas por nenhum instrumento e todos os instrumentos que não são usados por nenhuma predefinição são excluídos automaticamente.
Uma mensagem indica quantos elementos foram excluídos.


### Limpar duplicatas {#doc_duplicates}


Amostras idênticas são mescladas: os dados, a taxa de amostragem, o loop, a nota base e a correção devem ser iguais, e as amostras estéreo são comparadas junto com a sua amostra vinculada.
Para cada grupo de amostras idênticas, a primeira (na ordem do arquivo) é mantida e substitui as outras nos instrumentos, e as outras são excluídas.
Uma mensagem indica quantas amostras foram excluídas.

As duplicatas podem ser vistas antes no [resumo de configuração das amostras](manual/soundfont-editor/configuration-summaries.md#doc_sample).


![Duplicatas no resumo de configuração das amostras](images/tool_clean_duplicates.png "Duplicatas no resumo de configuração das amostras")



## Moduladores {#doc_cat_mod}


### Remover todos os moduladores {#doc_removemods}


Todos os [moduladores](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) presentes em todos os [instrumentos](manual/soundfont-editor/editing-pages/instrument-editor.md) e em todas as [predefinições](manual/soundfont-editor/editing-pages/preset-editor.md) serão removidos.


## Utilitário {#doc_cat_util}


### Exportar a lista de predefinições {#doc_export}


Com esta ferramenta, a lista de todas as [predefinições](manual/soundfont-editor/editing-pages/preset-editor.md) é escrita em forma de texto, com seus números de banco e de predefinição.
Esse texto pode ser formatado em csv, para ser integrado a uma planilha, ou como uma tabela html, para ser integrado a um site.
Um botão copia o texto para a área de transferência.


![Lista de predefinições](images/en_preset_list.png "Lista de predefinições")