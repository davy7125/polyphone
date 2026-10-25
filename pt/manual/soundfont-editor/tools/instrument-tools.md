Quando um instrumento ou uma de suas divisões está selecionado na [árvore](manual/soundfont-editor/tree.md), o [menu de ferramentas](manual/soundfont-editor/tools/index.md) contém várias ferramentas organizadas em 4 categorias:

* [análise](#doc_cat_analyze),
* [edição rápida](#doc_cat_fast),
* [moduladores](#doc_cat_mod),
* [transformação](#doc_cat_transformation).

É possível usar as ferramentas simultaneamente em vários instrumentos (com uma seleção múltipla), exceto para criar acordes, mixturas e para exibir parâmetros.


## Análise {#doc_cat_analyze} 


### Exibir parâmetros {#doc_display}


Esta ferramenta exibe a evolução de um parâmetro em função da nota.
A legenda distingue o valor definido na divisão global, os valores definidos nas divisões e o valor padrão do parâmetro.
Uma escala logarítmica está disponível (cuidado com os valores menores ou iguais a 0!).


![Ferramenta de exibição de parâmetros](images/tool_display_parameters.png "Ferramenta de exibição de parâmetros")


## Edição rápida {#doc_cat_fast}


### Alterar atenuações {#doc_attenuation}


A ferramenta calcula as atenuações mínima e máxima em todos os instrumentos selecionados e permite adicionar um deslocamento a todos os valores de atenuação sem ultrapassar os limites.

Por exemplo, se a menor atenuação das divisões dos instrumentos selecionados for de 16 dB e se a atenuação mínima desejada for de 5 dB (para aumentar o volume geral), a atenuação de todas as divisões de todos os instrumentos pode ser diminuída em 11 dB.


![Ferramenta de alteração de atenuação](images/tool_change_attenuation.png "Ferramenta de alteração de atenuação")


### Desafinar {#doc_detune}


Este recurso desafina ligeiramente as notas do instrumento, de modo a criar um efeito ondulante em conjunto com um instrumento afinado.
A intensidade do efeito é expressa em número de batimentos por segundo e é ajustável:

* número de batimentos por segundo na nota 60, que é o “dó” no meio do teclado (o sinal desse número determina o sentido da desafinação);
* evolução do número de batimentos de acordo com a nota, dividindo ou multiplicando o número de batimentos por um fator constante de uma oitava para outra (o valor “1” significa que o número de batimentos será constante em todo o teclado).


![Ferramenta de desafinação](images/tool_detune.png "Ferramenta de desafinação")


Eis um exemplo de um instrumento <a href="files/celeste_tuning_off.mp3" download>afinado</a>:

![](files/celeste_tuning_off.mp3)

E este instrumento foi combinado consigo mesmo em uma versão <a href="files/celeste_tuning_on.mp3" download>desafinada</a>:

![](files/celeste_tuning_on.mp3)

O arquivo .sf2 correspondente pode ser baixado <a href="files/celeste_tuning.sf2" download>aqui</a>.


### Configuração baseada em notas {#doc_global}


Esta ferramenta permite editar um parâmetro em todas as divisões de um instrumento simultaneamente, de acordo com o alcance de notas de cada divisão.


![Ferramenta de configuração baseada em notas](images/tool_keybased_configuration.png "Ferramenta de configuração baseada em notas")


O gráfico representa a intensidade da modificação, sendo que o primeiro valor à esquerda corresponde à alteração aplicada à divisão que contém a nota 0 e o último valor à direita corresponde à alteração aplicada à divisão que contém a nota 127.
Os valores mínimo e máximo do gráfico são ajustáveis.
Mudando o padrão, é possível editar a curva:

Manual
: A curva é desenhada com o mouse.

Linear ascendente / Linear descendente
: A curva é uma reta.

Exponencial ascendente / Exponencial descendente
: A curva é uma exponencial, cuja curvatura é definida pelo parâmetro :guilabel:`Rigidez`.

Aleatório
: Os valores são gerados aleatoriamente, de acordo com o parâmetro :guilabel:`Distribuição`.

Caso uma lei linear ou exponencial seja usada, um clique esquerdo no gráfico define o início da área linear ou exponencial, e um clique direito define o fim.

O parâmetro a modificar é escolhido em :guilabel:`Parâmetro`, e o tipo de modificação é escolhido em :guilabel:`Modificação`:

Adição
: O modificador é somado ao valor do parâmetro selecionado.

Multiplicação
: O modificador é multiplicado pelo valor do parâmetro selecionado.

Substituição
: O modificador substitui o valor do parâmetro selecionado.

É possível aplicar as alterações somente às divisões cujo alcance de velocidade esteja contido em um intervalo específico, o que é útil, por exemplo, quando um piano tem várias camadas de velocidade.


### Release natural {#doc_release}


Esta ferramenta cria um release para cada divisão do instrumento.
Os tempos de release são calculados levando em conta os dois primeiros parâmetros (a duração do release para a nota 36 e o fator entre duas oitavas consecutivas, :guilabel:`Divisão para a próxima oitava`), e uma modificação de altura pode ser adicionada com o terceiro (:guilabel:`Desafinação induzida`).


![Ferramenta de release natural](images/tool_natural_release.png "Ferramenta de release natural")


### Posicionamento automático das amostras {#doc_position}


O software determina os alcances de notas de todas as divisões de um instrumento, de modo a otimizar seu uso em todo o teclado.
Isso também é feito automaticamente quando amostras com notas base diferentes são vinculadas a um instrumento vazio.


### Espacialização do som {#doc_spatialization}


Este recurso espacializa os sons de acordo com a altura, o que pode ser útil para um piano, por exemplo (graves e agudos da esquerda para a direita).


![Ferramenta de espacialização do som](images/tool_sound_spatialization.png "Ferramenta de espacialização do som")


Cada barra do gráfico representa uma corda ou um tubo, com um comprimento relacionado à altura.
A barra mais curta corresponde à nota mais aguda e a barra mais longa corresponde à nota mais grave.

Cada nota é posicionada no espaço, correspondendo a extrema esquerda a um som vindo da esquerda e a extrema direita a um som vindo da direita.
Vários padrões podem ser selecionados:

Ascendente
: Os agudos ficam à esquerda, os graves à direita.

Descendente
: Os graves ficam à esquerda, os agudos à direita.

Côncavo
: Os graves ficam nas duas extremidades, os agudos no centro.

Ponta
: Os agudos ficam nas duas extremidades, os graves no centro.

Aleatório
: Agudos e graves são posicionados aleatoriamente.

Outras opções estão disponíveis:

* número de repetições do padrão (:guilabel:`Número de divisões`),
* intensidade do efeito (:guilabel:`Extensão`),
* preenchimento de uma divisão (:guilabel:`Preenchimento`),
* deslocamento esquerda — direita se a extensão não for de 100% (:guilabel:`Deslocamento`),
* :guilabel:`Inversões`.

O gráfico é atualizado a cada alteração de ajuste, para uma melhor visualização.
Ao passar o mouse, é exibida a correspondência número da nota / balanço.


### Transpor {#doc_transpose}


A ferramenta de transposição permite transpor um instrumento inteiro.
Os sons do instrumento continuam os mesmos, mas em outra posição no teclado.

Como entrada, é necessário um deslocamento em semitons.
Para que a altura fique mais aguda, o deslocamento deve ser positivo.
Inversamente, para que a altura fique mais grave, o deslocamento deve ser negativo.
As operações realizadas pela ferramenta, para cada divisão do instrumento, são então as seguintes:

* a nota base é ajustada,
* modificação da afinação fina se a transposição for feita com um número de semitons com casas decimais,
* deslocamento do alcance de notas de acordo com o número de semitons, se :guilabel:`Adaptar alcance de notas` estiver marcado.


![Ferramenta de transposição](images/tool_transpose_inst.png "Ferramenta de transposição")


## Moduladores {#doc_cat_mod}


### Substituir um modulador padrão {#doc_default_mod}


Moduladores padrão são atribuídos automaticamente a todos os instrumentos.
Esta ferramenta exibe a lista desses moduladores e pode criar moduladores que vão substituir ou desativar os que foram selecionados (botões :guilabel:`Substituir` e :guilabel:`Desativar`).


![Ferramenta de substituição](images/tool_default_mod.png "Ferramenta de substituição")


### Remover moduladores {#doc_remove_mod}


Todos os moduladores de todos os instrumentos selecionados serão removidos.
Uma mensagem indica quantos moduladores foram excluídos.

**Observação:** os moduladores padrão continuarão sendo aplicados.
A única maneira de desativar um modulador padrão é substituí-lo com a ferramenta “[Substituir um modulador padrão](#doc_default_mod)”.


## Transformação {#doc_cat_transformation}


### Criar acordes {#doc_chords}


A partir de um instrumento, esta ferramenta permite criar todos os acordes possíveis.
Isso pode ser interessante, por exemplo, para criar um coro ou pads.

O acorde é definido escolhendo quais notas ele contém (nota base, terça, quinta, sétima e nona) e sua qualidade (maior, menor, justa, diminuta, aumentada), o número de inversões e a oitava.
As amostras que contêm os acordes são criadas para cada nota, a cada 3 notas ou a cada 6 notas, com um loop automático opcional, em mono ou em estéreo.


![Ferramenta para criar acordes](images/tool_create_chords.png "Ferramenta para criar acordes")


Eis um exemplo, <a href="files/chord_base.mp3" download>antes</a>:

![](files/chord_base.mp3)

E <a href="files/chord_done.mp3" download>depois</a>:

![](files/chord_done.mp3)

Um arquivo .sf2 de exemplo, baseado em uma flauta, pode ser baixado <a href="files/chords.sf2" download>aqui</a>.


### Duplicação de divisão {#doc_division}


Esta ação duplica todas as divisões de um instrumento.
Por exemplo, se uma amostra vinculada a um instrumento tem um alcance de notas de 36-38, o software duplica essa divisão para ter essa amostra vinculada nos alcances 36-36, 37-37 e 38-38.
Isso permite então editar detalhadamente cada parâmetro da amostra para cada nota diferente, em vez de editar divisões inteiras com os mesmos parâmetros.

Também é possível duplicar as divisões de acordo com os alcances de velocidade.
Um instrumento sem amostras não é compatível com esta ferramenta.


![Ferramenta de duplicação de divisões](images/tool_division_duplication.png "Ferramenta de duplicação de divisões")


### Criação de mixtura {#doc_mixture}


Específica para o órgão, esta ferramenta permite criar uma mixtura a partir de um instrumento existente.
Aparece uma interface para especificar as diferentes divisões da mixtura e, para cada uma, a lista das fileiras.
Os tipos de fileiras possíveis são muito variados, indo da oitava ao 27º harmônico.


![Ferramenta de criação de mixtura](images/tool_mixture.png "Ferramenta de criação de mixtura")


Para funcionar, esta ferramenta se baseia em todas as amostras associadas ao instrumento selecionado, no alcance definido para cada amostra e na atenuação correspondente.
As amostras precisam estar afinadas independentemente da definição do instrumento (pelo [editor de amostras](manual/soundfont-editor/editing-pages/sample-editor.md)), e a nota base especificada não deve levar em conta o pertencimento a um registro (nenhuma transposição se o som for um principal de 4', por exemplo).

Uma opção permite o loop automático das amostras criadas, outra define o intervalo de criação das amostras (para cada nota, a cada 3 notas ou a cada 6 notas).
Por fim, é possível escolher o tipo dos sons criados: mono ou estéreo.

Eis um exemplo, <a href="files/mixture_base.mp3" download>antes</a>:

![](files/mixture_base.mp3)

E <a href="files/mixture_done.mp3" download>depois</a>:

![](files/mixture_done.mp3)

Um arquivo .sf2 de exemplo, baseado em um registro de órgão, pode ser baixado <a href="files/mixture.sf2" download>aqui</a>.
