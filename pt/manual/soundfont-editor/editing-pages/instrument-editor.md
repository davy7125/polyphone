Ao clicar em um ou mais instrumentos na [árvore](manual/soundfont-editor/tree.md), aparece a página de edição dos instrumentos.

Esta página contém várias partes:

* a [tabela](#doc_table),
* o [editor de alcances](#doc_range), quando o botão :guilabel:`Alcances` da [barra de ferramentas](manual/soundfont-editor/toolbar.md#doc_view) é clicado,
* o [editor de envelopes](#doc_envelope), quando o botão :guilabel:`Envelopes` da [barra de ferramentas](manual/soundfont-editor/toolbar.md#doc_view) é clicado,
* a [visualização do timbre](#doc_timbre), quando o botão :guilabel:`Timbre` da [barra de ferramentas](manual/soundfont-editor/toolbar.md#doc_view) é clicado,
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


### Mover os pontos


Cada ponto da curva pode ser movido com o botão esquerdo:

* os finais de **delay**, **attack**, **hold** e **release** se movem horizontalmente;
* o final do **decay** se move nas duas direções: horizontalmente para o **decay**, verticalmente para o **sustain**;
* o ponto em que a tecla é solta se move verticalmente, para o **sustain**.

Os parâmetros do ponto sob o cursor do mouse e seus valores são exibidos em um canto superior do gráfico.
Quando vários pontos estão no mesmo lugar, o círculo é dividido em partes, da primeira etapa à esquerda até a última à direita.

Os parâmetros **Nota → Hold** e **Nota → Decay** são editados com uma pequena régua que atravessa o final do **hold** ou do **decay**: sua extremidade superior indica a duração para a nota mais aguda da divisão, sua extremidade inferior a duração para a nota mais grave.
Mover uma extremidade altera apenas a duração da sua nota, enquanto a outra extremidade permanece no lugar; mantendo a tecla :kbd:`Ctrl` pressionada, as duas extremidades se movem em torno do meio da régua.
A régua aparece quando o mouse se aproxima do ponto e continua visível quando o parâmetro é diferente de 0.

A tecla :kbd:`Shift` permite um ajuste mais fino.
Empurrar um ponto contra a borda direita do gráfico desloca a visualização ou diminui o zoom, para alcançar durações mais longas.
A tecla :kbd:`Esc` cancela uma alteração em andamento.
Um clique duplo em um ponto redefine seus parâmetros (apenas **Nota → Hold** ou **Nota → Decay** para uma extremidade da régua): volta a valer o valor da divisão :guilabel:`Global` ou o valor padrão.

Se várias divisões ou instrumentos estiverem selecionados, todos os seus envelopes são alterados juntos: as durações na mesma proporção, os outros valores na mesma quantidade.


### Deslocamento e zoom


Manter o clique direito pressionado permite alterar o zoom em torno do ponto selecionado.

Manter o clique esquerdo pressionado permite deslocar o gráfico se o clique não começar sobre um ponto da curva.


## Visualização do timbre {#doc_timbre}


A visualização do timbre mostra, ao longo de todo o teclado, o timbre de um instrumento **tal como é ouvido**: o nível de cada harmônico, depois que cada amostra é transposta para as notas que toca e depois de aplicados a atenuação, o filtro e os moduladores de sua divisão.
As irregularidades se tornam visíveis, em particular as rupturas nas junções entre amostras.
A partir desta visualização, é possível criar notas calculadas, remover amostras redundantes e corrigir o timbre ou o nível de uma amostra.
A visualização mede e propõe; nada é aplicado automaticamente.


![Visualização do timbre](images/edit_timbre.png "Visualização do timbre")


### Divisões consideradas


A visualização mostra o que é tocado na velocidade escolhida no canto superior esquerdo (100 por padrão, depois a última usada).
A barra ao lado mostra as camadas de velocidade do instrumento: um clique em uma camada seleciona a velocidade no seu meio.

As divisões consideradas são aquelas que tocam nessa velocidade, que não estão [silenciadas](#doc_solo) e que não são tocadas apenas quando a tecla é solta (modo de reprodução “release”).
Silenciar uma divisão é, portanto, a maneira de excluí-la da visualização, por exemplo um ruído ou um som de release.
Quando o [solo na seleção](manual/soundfont-editor/tree.md#doc_menu) está ativado, os silenciamentos seguem a seleção e não são levados em conta.
As divisões que tocam a mesma nota (par estéreo, camadas) são combinadas: suas potências se somam.

Uma **nota base** reúne todas as divisões que têm essa nota base, eventualmente com vários alcances de notas.
As amostras são analisadas em segundo plano (o número de amostras restantes é exibido no canto superior direito): os harmônicos são medidos no loop ou, se não houver loop, na parte estável do som.
Essas análises são mantidas até que os dados, o loop, a nota base ou a correção de afinação de uma amostra mudem.


### Mapa


Cada coluna é uma nota, alinhada com o teclado exibido abaixo, e cada linha um harmônico (H1: fundamental, H2: segundo harmônico...), o harmônico 1 ficando embaixo, como em um espectrograma.
Os primeiros harmônicos são exibidos um a um, e os seguintes são agrupados em bandas (veja [Parciais](#doc_timbre_partials)).

Acima do mapa, linhas de resumo mostram para cada nota:

* :guilabel:`Nível`: nível total,
* :guilabel:`Timbre`: desvio de timbre da nota base em relação à sua referência (hachurado quando a referência é menos confiável, nas extremidades do teclado),
* :guilabel:`Transposição`: azul quando a amostra é transposta para baixo, vermelho quando é transposta para cima,
* e, à escolha no menu :guilabel:`Opções` > :guilabel:`Resumos`: :guilabel:`Brilho` (centro de gravidade dos harmônicos), :guilabel:`Ímpares / pares` (harmônicos ímpares comparados aos harmônicos pares), :guilabel:`Ruído` (relação entre harmônicos e ruído), :guilabel:`Afinação` (cents em relação à nota mais próxima do temperamento igual) e :guilabel:`Loop` (qualidade do loop).

No mapa, linhas verticais marcam as junções entre divisões (mais grossas quando a amostra muda), um ponto marca cada nota base, hachuras mostram os harmônicos ausentes (fora da banda da amostra ou da saída, acima do limite superior) ou medidos de forma pouco confiável, e as notas que não tocam nada ficam acinzentadas.
Uma dica dá os valores da célula sob o cursor do mouse: nota, harmônico, nível, desvio, amostras, transposição e fator de duração (o ataque, o trêmolo e os batimentos são mais rápidos quando uma amostra é transposta para cima).

A lista :guilabel:`Visualização` escolhe o que as cores representam:

Níveis absolutos
: Nível de cada harmônico em dB.

Níveis relativos
: Nível de cada harmônico em relação ao mais forte da nota: o timbre independentemente do nível.

Desvio em relação à referência
: Diferença entre a nota base e sua referência para cada harmônico, em todas as notas tocadas pela nota base: forte demais (vermelho) ou fraco demais (azul).

Saltos
: Diferença em relação à nota anterior: as rupturas nas junções.

Transposição
: Semitons entre a nota e a nota base de sua amostra.

Confiança
: Parte da potência medida de forma confiável.

O menu :guilabel:`Opções` também permite exibir o teclado inteiro ou apenas uma coluna por nota base, colorir em patamares conforme os limiares (os desvios dentro da tolerância ficando neutros) e tocar as notas clicadas.
O teclado abaixo do mapa toca o instrumento na velocidade da visualização.


### Referência e decisões


A **referência** de uma nota base é o que suas vizinhas preveem: as notas base vizinhas são tocadas na sua nota (com os parâmetros de suas divisões) e seus níveis são interpolados harmônico por harmônico, em dB.
Nas duas extremidades do teclado, a referência é extrapolada a partir das duas vizinhas do mesmo lado, o que é menos confiável.
Com :guilabel:`Regressão local`, é usada em vez disso uma regressão robusta sobre várias vizinhas de cada lado, para que uma vizinha defeituosa pese menos.

O desvio de uma nota base em relação à sua referência é resumido por dois números:

* o desvio de **nível**, diferença dos níveis totais, que pode ser corrigido pela atenuação,
* o desvio de **timbre**, média dos desvios das linhas depois de removido o desvio de nível, cada linha pesando de acordo com sua intensidade sonora (um harmônico fraco conta, sem dominar).

A faixa :guilabel:`Decisões` dá um rótulo a cada nota base e a cada junção, com uma forma e uma cor:

Coerente ✓ (verde)
: O desvio de timbre está dentro da tolerância.

Redundante − (azul)
: A nota base é coerente, e removê-la mudaria suas notas menos do que o limiar :guilabel:`Redundância`, sem ruptura na nova junção: ela pode ser removida.

A corrigir ~ (laranja)
: O desvio de timbre está entre a tolerância e o máximo: o timbre pode ser corrigido.

Diferente demais × (vermelho)
: O desvio de timbre ultrapassa o máximo: a nota base pode ser removida ou substituída por uma nota calculada.

Ruptura + (roxo)
: Em uma mudança de amostra, o salto de timbre ultrapassa o limiar :guilabel:`Ruptura` e existe uma nota sem amostra entre as duas notas base: uma nota calculada pode ser criada ali.

Nível ▲ ▼ (amarelo)
: O desvio de nível ultrapassa o limiar :guilabel:`Nível` (▲ alto demais, ▼ baixo demais): a atenuação pode ser corrigida.

Os limiares são ajustados no menu :guilabel:`Limiares`.
Seus valores padrão (:guilabel:`Tolerância` 4,5 dB, :guilabel:`Máximo` 8 dB, :guilabel:`Redundância` 2,5 dB, :guilabel:`Ruptura` 6 dB, :guilabel:`Nível` 2,5 dB) vêm de conjuntos de amostras de órgão, em que 70% das notas base se desviam menos de 4,5 dB de suas vizinhas: eles podem ser adaptados ao instrumento.
Uma junção ou um desvio pode ser intencional (um instrumento que se torna outro, a repetição de uma mixtura): os rótulos são apenas propostas.


### Parciais {#doc_timbre_partials}


O menu :guilabel:`Parciais` define as linhas do mapa, compartilhadas pela exibição, pelos desvios e pelas correções:

Harmônicos exibidos um a um
: Número de harmônicos exibidos e comparados um a um (10 por padrão). O ouvido distingue individualmente os primeiros harmônicos, enquanto os seguintes são percebidos em conjunto como brilho.

Harmônicos seguintes
: Os harmônicos seguintes são agrupados em bandas de um terço de oitava (11-12, 13-16, 17-20...) ou de uma oitava, para que muitos harmônicos agudos e fracos não pesem mais do que os primeiros.

Limite superior
: Os parciais acima dessa frequência são ignorados (16 kHz por padrão, ou a metade da taxa de amostragem).

Faixa dinâmica
: Os harmônicos que ficam mais do que este valor abaixo do mais forte da nota não são comparados (60 dB por padrão).


### Detalhes e ações


Um clique em uma nota mostra abaixo do mapa o que ela toca: as amostras com sua transposição, atenuação e filtro, o fator de duração, a altura, os desvios da nota base e o que sua remoção mudaria.
O gráfico de barras compara os níveis ouvidos com a referência, ou com a nota calculada que uma nota sem amostra poderia receber (uma coluna tracejada no mapa).

Um clique em uma nota base seleciona suas divisões na [árvore](manual/soundfont-editor/tree.md), e selecionar divisões na árvore seleciona suas notas base.
Várias notas base podem ser selecionadas com :kbd:`Ctrl` ou :kbd:`Shift`, ou arrastando o mouse sobre as notas.

Os botões à direita e o menu de contexto do mapa se aplicam às notas base selecionadas:

Criar uma nota aqui...
: Em uma nota sem amostra: a nota base mais próxima é transposta e os níveis de seus harmônicos são interpolados (ou extrapolados) a partir de uma segunda nota base. As notas base usadas podem ser escolhidas, e um aviso é exibido quando a nota está longe delas. Os alcances de notas ao redor são ajustados; as divisões inteiramente contidas no alcance da nova nota são removidas.

Remover...
: As divisões da nota base são removidas e suas vizinhas assumem suas notas. As amostras que não são mais usadas também podem ser removidas.

Substituir por uma nota calculada...
: A nota base é removida e depois recriada a partir de suas vizinhas.

Corrigir o timbre...
: Abre a janela de [correção](#doc_timbre_correction).

Corrigir o nível
: A atenuação das divisões é alterada para que o desvio de nível se torne zero (uma atenuação não pode ser negativa).

Verificar por renderização
: A nota é tocada pelo motor de som e os harmônicos do resultado são medidos: eles são exibidos no gráfico de barras para verificar o cálculo da visualização.

Silenciar estas divisões
: No menu de contexto: exclui as divisões da visualização.

O menu :guilabel:`Propor` seleciona todas as notas base (ou junções) que têm um rótulo e aplica a elas a ação correspondente, após uma confirmação: corrigir as notas base a corrigir, substituir as diferentes demais, remover as redundantes, criar notas calculadas nas rupturas, corrigir os níveis que se desviam.
Elas são processadas da pior para a menos ruim, com as referências recalculadas após cada ação: uma nota base defeituosa também distorce a referência de suas vizinhas, que parecem meio defeituosas enquanto ela não for corrigida.

Cada ação, ou série de ações, pode ser desfeita em uma única etapa (:kbd:`Ctrl`+:kbd:`Z`).


### Correção do timbre {#doc_timbre_correction}


![Correção do timbre](images/timbre_correction.png "Correção do timbre")


A janela mostra, para cada linha, os níveis ouvidos, o alvo e o resultado esperado, e oferece os seguintes parâmetros:

Referência
: Alvo da correção: :guilabel:`Interpolação entre as vizinhas`, :guilabel:`Regressão local` sobre várias vizinhas de cada lado, ou :guilabel:`Alinhamento com uma nota base` (outra nota base tocada na nota).

Corrigir o timbre (dados da amostra)
: O ganho de cada linha é aplicado aos dados das amostras com uma curva de fase mínima que acompanha a altura: a correção continua válida em todas as notas da divisão, qualquer que seja a transposição, e o loop é mantido. A mesma correção é aplicada às duas amostras de um par estéreo.

Corrigir o nível (atenuação)
: O desvio de nível é corrigido pela atenuação das divisões, o que não altera os dados. Se a atenuação tiver que ficar negativa, o restante vai para os dados, dentro do limite do fundo de escala.

Tolerância
: Um desvio menor de uma linha não é corrigido; apenas a parte que o excede é corrigida.

Intensidade
: Parte do desvio que é corrigida.

Correção máxima
: Maior ganho de uma linha, nos dois sentidos.

Harmônicos corrigidos
: Todas as linhas, ou apenas os harmônicos de 1 até um harmônico dado.

Brilho, Harmônicos ímpares
: Inclinação adicionada ao alvo (positiva para um som mais brilhante) e nível adicionado aos harmônicos ímpares 3, 5, 7... do alvo.

A seção :guilabel:`Audição` compara o som original (A) e o som corrigido (B) com o botão :guilabel:`Reproduzir`.
Se a correção ultrapassar o fundo de escala, a amostra é abaixada e a atenuação compensa.
Quando uma amostra também é usada por outras divisões ou instrumentos, o Polyphone pergunta se ela deve ser duplicada para as notas base corrigidas ou modificada em toda parte (a resposta pode ser memorizada até o Polyphone ser fechado).


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
