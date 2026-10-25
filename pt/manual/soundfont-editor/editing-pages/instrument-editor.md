Ao clicar em um ou mais instrumentos na [árvore](manual/soundfont-editor/tree.md), aparece a página de edição dos instrumentos.

Esta página contém várias partes:

* a [tabela](#doc_table),
* o [editor de alcances](#doc_range), quando o botão :guilabel:`Alcances` da [barra de ferramentas](manual/soundfont-editor/toolbar.md#doc_view) é clicado,
* o [editor de envelopes](#doc_envelope), quando o botão :guilabel:`Envelopes` da [barra de ferramentas](manual/soundfont-editor/toolbar.md#doc_view) é clicado,
* a [seção dos moduladores](#doc_modulator).


![Página do instrumento](images/edit_instrument.png "Página do instrumento")


Se apenas um instrumento estiver selecionado, a página permite editar os parâmetros globais do instrumento e os parâmetros de todas as suas divisões.

Caso vários instrumentos estejam selecionados, os parâmetros globais de cada um desses instrumentos podem ser editados.


![Página do instrumento com vários instrumentos selecionados](images/edit_multiple_instruments.png "Página do instrumento com vários instrumentos selecionados")


## Tabela {#doc_table}


### Apresentação


A tabela oferece uma visão geral e permite editar a configuração de um instrumento.
Uma alteração de um parâmetro na divisão :guilabel:`Global` se aplica a todas as divisões, exceto às divisões em que o mesmo parâmetro já está especificado (substituição local).

As linhas escritas em itálico (:guilabel:`Nota base da amostra`, :guilabel:`Comprimento da amostra` e :guilabel:`Loop`) não podem ser editadas: elas apenas lembram, a título informativo, as propriedades da amostra vinculada a cada divisão.

Os parâmetros editáveis são:

Alcance de notas
: Alcance de notas em que a amostra será ouvida.

Alcance de velocidade
: Alcance de velocidade em que a amostra será ouvida.
  É possível definir sons diferentes para a mesma nota, com alcances de velocidade diferentes, para distinguir diferentes nuances de uma tecla de piano, por exemplo.

Atenuação (dB)
: Atenuação em decibéis aplicada à amostra.\
  Observação: o formato sf2 não permite que esse valor seja negativo no nível do instrumento; portanto, nenhuma amplificação é possível.

Balanço
: Balanço aplicado ao som, para ouvi-lo à esquerda ou à direita em estéreo.

Reprodução em loop
: ![Loop desligado](images/loop_off.png "loop desligado"){.inline} indica um som reproduzido sem loop.
  ![Loop ligado](images/loop_on.png "loop ligado"){.inline} indica um som que se repete continuamente.
  ![Release](images/sample_mode_release.png "release"){.inline} serve para disparar a reprodução de uma amostra quando uma tecla é solta. O envelope de volume continua sendo usado para definir o volume, e um ataque curto (0,2 ms) é adicionado para suavizar o início da amostra no release.
  ![Loop ligado + release](images/loop_on_end.png "loop ligado e release"){.inline} indica um som que se repete enquanto a tecla está pressionada e depois continua a tocar o restante da amostra.\
  Observação: o modo de reprodução “release” NÃO é documentado pelo formato sf2; portanto, outros reprodutores, como o FluidSynth, podem decidir não implementá-lo.
  A outra maneira de obter esse comportamento é duplicar cada amostra com uma amostra contendo um loop silencioso curto seguido do som a emitir no release.
  A amostra adicionada será lida com o parâmetro de reprodução “loop ligado + release”.
  Veja o tutorial “[Usar releases personalizados em um instrumento](tutorials/using-custom-releases-in-an-instrument.md)”.

Nota base
: Permite alterar a nota base normal de uma amostra para uma nota diferente da habitualmente usada.
  Por exemplo, se uma amostra tem a nota base 50 e é tocada no alcance de notas [49 — 51], ela será inicialmente transposta em -1, 0 e +1 semitom nas notas 49, 50 e 51, respectivamente.
  Se a nota base for então substituída por 51 no nível do instrumento, a mesma amostra será inicialmente transposta em -2, -1 e 0 semitom nas notas 49, 50 e 51, respectivamente.

Afinação (semitons e cents)
: Ajusta a afinação em semitons e em centésimos de semitom.
  Essa transposição se soma à transposição inicial relacionada à nota disparada (por exemplo, a transposição inicial de uma amostra com nota base 60 será de +2 semitons se a tecla pressionada no teclado for a 62).

Afinação (escala)
: Este parâmetro modifica o intervalo entre duas notas consecutivas e, portanto, afeta a transposição inicial ligada à nota disparada.
  Um valor de 100 representa a escala do temperamento igual: 100 centésimos de semitom são usados para transpor uma amostra de uma nota para a seguinte.
  Um valor de 0 desativa essa transposição inicial, o que significa que a amostra será tocada de forma idêntica em todas as teclas do teclado.

Filtro
: Aplica um filtro passa-baixas, especificando a frequência de corte e a ressonância.

Envelope de volume
: Altera o envelope de volume e, eventualmente, como ele varia com a nota.

Modulação
: Define uma modulação (envelope, atraso, frequência, variações com a nota) e seu efeito sobre o tom, o filtro e o volume.

Vibrato
: Define um vibrato (atraso, frequência) e seu efeito sobre o tom.

Classe exclusiva
: Este parâmetro pode definir uma ou mais classes exclusivas, atribuindo a um conjunto de divisões de uma classe o mesmo valor de parâmetro diferente de 0.
  Quando uma classe exclusiva é definida, qualquer nota disparada a partir de uma das divisões da classe exclusiva encerra todos os outros sons da mesma classe.
  O escopo de uma classe exclusiva é a predefinição em que a classe é definida (um som disparado em um instrumento encerra o som de outro instrumento se os dois instrumentos estiverem na mesma predefinição).
  Esse efeito pode ser usado em instrumentos de percussão, como um prato.
  Basicamente, tocar uma nota interrompe automaticamente outra do mesmo grupo.

Chorus e reverberação
: Indica a intensidade dos efeitos de chorus e de reverberação.
  Os efeitos são configuráveis nas [configurações](manual/settings.md#doc_sound) do software.

Nota e velocidade fixas
: Força o valor da nota e da velocidade em toda a extensão da divisão.

Offsets
: Altera as posições de início e de fim de um som durante a reprodução, assim como o início e o fim do loop.
  A unidade de um offset é a amostra (número de valores).


### Facilidades de edição na tabela


Para facilitar e agilizar a edição na tabela, os seguintes recursos foram implementados:

* a roda do mouse pode ser usada para alterar um valor (a célula precisa primeiro estar em modo de edição),
* um teclado MIDI externo (ou o [teclado virtual](manual/soundfont-editor/toolbar.md#doc_keyboard)) pode ser usado para definir a nota base, o alcance de notas e o alcance de velocidade (a célula precisa primeiro estar em modo de edição),
* uma seleção múltipla seguida da edição da última célula edita todas as células simultaneamente,
* as teclas :kbd:`Del` e :kbd:`Backspace` apagam o conteúdo de todas as células selecionadas,
* as divisões estéreo podem ser editadas simultaneamente se a opção estiver ativada nas [configurações](manual/settings.md#doc_general) do software,
* os nomes das notas podem ser numéricos ou textuais (com C3, C4 ou C5 como dó central), dependendo da opção escolhida nas [configurações](manual/settings.md#doc_interface),
* copiar / colar / recortar um conjunto de células (não necessariamente adjacentes) pode ser feito com :kbd:`Ctrl`+:kbd:`C` / :kbd:`Ctrl`+:kbd:`V` / :kbd:`Ctrl`+:kbd:`X`.

Não se esqueça das [ferramentas](manual/soundfont-editor/tools/instrument-tools.md) disponíveis!
Em particular, a ferramenta que permite uma [configuração baseada em notas](manual/soundfont-editor/tools/instrument-tools.md#doc_global).


### Possibilidade de silenciar divisões {#doc_solo}


Para fins de teste, é possível silenciar uma divisão ou silenciar todas as divisões exceto uma nas tabelas.
As opções são acessíveis com um clique direito no cabeçalho de uma divisão e também no menu de contexto da [árvore](manual/soundfont-editor/tree.md#doc_menu).


![Silenciar divisões em uma tabela](images/table_mute_divisions.png "Silenciar divisões em uma tabela")


## Editor de alcances {#doc_range}


O editor de alcances permite organizar rápida e graficamente as amostras de acordo com seus alcances de notas e de velocidade.
Esse gráfico também evidencia problemas de configuração, como zonas sem amostras ou sobreposições indesejadas.


![Editor de alcances](images/edit_range.png "Editor de alcances")


Cada amostra é representada por um retângulo semitransparente, de modo que a sobreposição de vários elementos seja identificável.


### Seleção de retângulos


Um simples clique esquerdo em um retângulo o seleciona e remove todas as outras seleções.
Se vários retângulos estiverem sobrepostos, cliques consecutivos alternam a seleção sob o cursor do mouse.
Um clique duplo em um retângulo abre a [amostra](manual/soundfont-editor/editing-pages/sample-editor.md) correspondente (ou o instrumento correspondente no [editor de predefinições](manual/soundfont-editor/editing-pages/preset-editor.md)).

Uma seleção múltipla é possível:

* mantendo a tecla :kbd:`Ctrl` pressionada e clicando nos diferentes retângulos para selecioná-los um a um,
* usando a tecla :kbd:`Shift`: todos os retângulos entre o primeiro retângulo clicado e o seguinte são selecionados.


### Modificar os retângulos


Depois de selecionar um ou mais retângulos, é possível, com o botão esquerdo:

* alterar o tamanho, agarrando uma das bordas de um retângulo;
* alterar a posição, apontando para o centro de um retângulo.

Se vários retângulos estiverem selecionados, todas as alterações são feitas de forma sincronizada.
A tecla :kbd:`Esc` cancela uma alteração em andamento.


### Deslocamento e zoom


Manter o clique direito pressionado permite alterar o zoom em torno do ponto selecionado.

Manter o clique esquerdo pressionado permite deslocar o gráfico se o ponto inicial não estiver sobre um retângulo.

Um clique com o botão do meio toca uma nota: a nota é dada pela posição horizontal do mouse e a velocidade pela sua posição vertical.


## Editor de envelopes {#doc_envelope}


O editor de envelopes oferece uma visão gráfica dos 8 parâmetros que definem o envelope de volume e dos 8 parâmetros que definem o envelope de modulação.
É, portanto, uma ferramenta para entender melhor o impacto de cada um desses parâmetros.


![Editor de envelopes](images/edit_envelope.png "Editor de envelopes")


Os botões acima do gráfico selecionam alternadamente o envelope de volume (exibido em verde) ou o envelope de modulação (exibido em vermelho) para edição.
Durante a edição do envelope de modulação, o envelope de volume é exibido com uma linha fina.

A parte esquerda do gráfico representa a evolução do envelope quando uma tecla é **pressionada**.
7 dos 8 parâmetros participam dessa representação:

Delay
: Duração durante a qual o envelope permanece em 0 logo após uma tecla ser pressionada. Na maioria dos casos, queremos ouvir o áudio imediatamente após pressionar as teclas, e então esse parâmetro fica em zero. No entanto, o **delay** é útil, por exemplo, para criar sons de pad mais ricos, construídos com várias camadas. Suponha que queremos introduzir uma segunda camada de som 0,5 segundo após a tecla ser pressionada: basta digitar “0.5” neste campo. Um delay não adia a reprodução do som — ele silencia o áudio. Se o delay estiver em 0,5 segundo, começaremos a ouvir a amostra tal como ela está após 0,5 segundo de reprodução, já que os primeiros 0,5 segundo foram tocados em silêncio.

Attack
: Duração, após o **delay**, durante a qual o envelope cresce de 0 até o valor máximo. Por exemplo, um som de corda friccionada, como o de um violino, tem um ataque bastante longo, enquanto o de uma corda pinçada, como o de um violão, é muito curto. Mesmo quando queremos um ataque o mais curto possível, é recomendável digitar pelo menos um valor de 0.001; caso contrário, um “clique” pode ser ouvido depois de pressionar uma tecla.

Hold
: Duração, após o **attack**, durante a qual o envelope mantém seu valor máximo.

Decay
: Duração, após o **hold**, durante a qual o envelope passa progressivamente do valor máximo ao valor especificado pelo **sustain**. Quando nenhuma redução é especificada no **sustain** (0 dB, o valor padrão), o volume durante o **decay** permanece no máximo, qualquer que seja o valor especificado.

Sustain
: Valor, após o **decay**, mantido enquanto a tecla pressionada não for solta. Assim como a **atenuação**, o valor de sustain é uma redução expressa em decibéis. Caso se queira reduzir o volume pela metade após o tempo de decay, é preciso definir um valor de 6 dB. O número “144” pode ser especificado se quisermos silenciar o som completamente.

Nota → Hold
: Variação da duração do **hold** em função da nota (veja abaixo).

Nota → Decay
: Variação da duração do **decay** em função da nota (veja abaixo).

Um valor positivo em **Nota → XXX** diminui gradualmente a duração de XXX em direção aos agudos, enquanto um valor negativo faz o contrário.
Por exemplo:

* o valor 100 divide por dois a duração de XXX a cada oitava acima do dó central (60) e a duplica a cada oitava abaixo do dó central,
* o valor -100 duplica a duração de XXX a cada oitava acima do dó central (60) e a divide por dois a cada oitava abaixo do dó central.

Esses parâmetros podem ser úteis ao criar um violão ou um piano com o pedal de sustain acionado: quanto mais grave a nota, mais tempo ela dura até não ouvirmos mais nenhum som.

A parte direita representa a evolução do envelope quando uma tecla é **solta**.
Apenas a duração do **release** participa dessa representação.
O envelope volta gradualmente a 0 durante essa duração.
Sons de violinos ou de pads de sintetizador têm um tempo de release relativamente longo, enquanto o de outros sons, como os de órgão, é muito curto. Na prática, mesmo para tempos de release curtos, é recomendável definir uma duração de pelo menos 0,2 segundo, porque um tempo de release curto demais não soa agradável nem natural (mas isso também depende do que temos em mente para o instrumento que queremos criar).

Quando apenas uma divisão de um instrumento está selecionada na árvore, a [amostra](manual/soundfont-editor/editing-pages/sample-editor.md) sonora correspondente é exibida ao fundo.
Assim, é possível visualizar melhor a evolução do envelope em relação à amostra tocada.


## Moduladores {#doc_modulator}


A seção inferior da página de edição dos instrumentos é destinada à criação de moduladores, globalmente para um instrumento ou para uma divisão específica.
Quando você toca um instrumento, um modulador escuta um ou dois sinais MIDI e modifica um dos parâmetros da tabela de acordo com a regra matemática configurada.
A saída de um modulador também pode ser usada como entrada de outro modulador (esse recurso surgiu na versão 2.04 do [formato sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2) e pode não ser suportado por alguns sintetizadores).


![Editor de moduladores](images/modulator_editor_2.png "Editor de moduladores")


Os botões à esquerda servem para:

* expandir / recolher a área dos moduladores,
* adicionar / remover um modulador,
* copiar / colar / duplicar um ou mais moduladores.

Quando um modulador é adicionado, a borda da célula correspondente na tabela fica mais grossa (veja a afinação na coluna :guilabel:`Global` acima).

Os moduladores de uma divisão (ou da divisão global) só são exibidos se uma única divisão estiver selecionada.
Quando a seção está recolhida, um resumo lista o número de moduladores e os parâmetros que eles modificam.
Selecionar células na tabela seleciona os moduladores que atuam sobre os parâmetros correspondentes, e vice-versa.

Cada modulador é composto dos seguintes elementos, da esquerda para a direita:

Fontes
: A primeira fonte é um sinal MIDI: a velocidade ou a nota tocada, a pressão polifônica, a pressão de canal, a roda de pitch, a sensibilidade da roda de pitch ou um controlador MIDI (:guilabel:`CC`).
  Também pode ser a saída de um ou mais outros moduladores (:guilabel:`Link`), aqueles que têm este modulador como destino.
  A segunda fonte é opcional e modifica o efeito da primeira.

Curvas
: Ao lado de cada fonte, um botão dá acesso a uma grade de curvas para escolher como o sinal é convertido: linear, côncava, convexa ou em degrau, ascendente ou descendente, unipolar (de 0 a 1) ou bipolar (de -1 a 1).

Quantidade
: Amplitude máxima da modulação.

Transformação
: :guilabel:`nenhuma operação` ou :guilabel:`valor absoluto` (o valor absoluto do resultado da modulação é usado).

Destino
: O parâmetro modificado pelo modulador, ou outro modulador (:guilabel:`Modulador`).
  O texto abaixo do destino indica a amplitude da modulação: os valores somados ao parâmetro (:guilabel:`Somar de` … :guilabel:`Até`) ou os fatores aplicados a ele (:guilabel:`Multiplicar de` … :guilabel:`Até`).

Quando um modulador é idêntico a outro localizado acima na lista, é exibida a indicação :guilabel:`sobrescrito por`.
No nível do instrumento, a indicação :guilabel:`substitui mod. padrão` indica um modulador que substitui um dos [moduladores padrão](manual/soundfont-editor/tools/instrument-tools.md#doc_default_mod) do formato sf2, e :guilabel:`desativa mod. padrão` indica que sua quantidade é 0 (o modulador padrão fica então desativado).
