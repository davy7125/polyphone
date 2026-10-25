Ao clicar em uma ou mais predefinições na [árvore](manual/soundfont-editor/tree.md), aparece a página de edição das predefinições.
Uma predefinição é uma combinação de um ou mais instrumentos e é o elemento visível de fora de uma [soundfont](manual/annexes/the-different-soundfont-formats.md#doc_sf2).

Assim como no [editor de instrumentos](manual/soundfont-editor/editing-pages/instrument-editor.md), várias partes estão presentes:

* a [tabela](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table),
* o [editor de alcances](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range), quando o botão :guilabel:`Alcances` da [barra de ferramentas](manual/soundfont-editor/toolbar.md#doc_view) é clicado,
* a [seção dos moduladores](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator).


![Página da predefinição](images/edit_preset.png "Página da predefinição")


Se apenas uma predefinição estiver selecionada, a página permite editar os parâmetros globais da predefinição e os parâmetros de todas as suas divisões.
Caso várias predefinições estejam selecionadas, os parâmetros globais de cada uma dessas predefinições podem ser editados.


## Diferenças em relação aos instrumentos {#doc_diff}


Os parâmetros editados aqui são ditos relativos, ao contrário dos parâmetros de um instrumento, que, salvo em casos especiais, são ditos absolutos.
Os valores dos parâmetros de uma predefinição são somados aos parâmetros dos instrumentos que a compõem ou multiplicados por eles.
Por esse motivo, uma atenuação (ou qualquer outro parâmetro) pode ser negativa no nível da predefinição, o que não é o caso no nível do instrumento.
Alguns parâmetros não são editáveis no nível da predefinição:

* reprodução em loop,
* nota base,
* classe exclusiva,
* nota fixa,
* velocidade fixa,
* os quatro offsets.

Uma predefinição é identificada por um número de predefinição e um número de banco, sendo essa combinação única e ajustável abaixo da tabela.
Cada banco comporta 128 predefinições (numeradas de 0 a 127).

O último banco (128) é geralmente usado para kits de percussão.