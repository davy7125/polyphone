A barra de ferramentas, localizada na parte superior do editor, contém ícones para:

* [funções de edição](#doc_edit) comuns,
* alternar a [visualização](#doc_view) nos editores,
* exibir o [gravador](#doc_recorder),
* exibir o [teclado virtual](#doc_keyboard).


![Barra de ferramentas](images/toolbar.png "Barra de ferramentas")


## Funções de edição {#doc_edit}


Os elementos da parte esquerda da barra de ferramentas servem para as funções de edição comuns:

![nova amostra](images/toolbar_sample.png "nova amostra"){.inline} Adicionar uma amostra
: Adiciona novas [amostras](manual/soundfont-editor/editing-pages/sample-editor.md) à soundfont.
  Uma caixa de diálogo é exibida para selecionar arquivos de áudio (arquivos :file:`.wav`, :file:`.flac`, :file:`.mp3`, :file:`.ogg`, :file:`.aif`, :file:`.aiff`, :file:`.aifc` e :file:`.snd` podem ser importados), sendo possível selecionar vários arquivos de uma vez.

![novo instrumento](images/toolbar_instrument.png "novo instrumento"){.inline} Adicionar um instrumento
: Cria um novo [instrumento](manual/soundfont-editor/editing-pages/instrument-editor.md).
  Uma caixa de diálogo pede o nome do instrumento e, se houver amostras selecionadas na [árvore](manual/soundfont-editor/tree.md), oferece vinculá-las ao novo instrumento.
  Nesse caso, as amostras estéreo são posicionadas à esquerda e à direita, o loop é ativado se todas as amostras tiverem loop e, quando as amostras têm notas base diferentes, os alcances de notas são definidos automaticamente (veja a ferramenta [Posicionamento automático das amostras](manual/soundfont-editor/tools/instrument-tools.md#doc_position)).

![nova predefinição](images/toolbar_preset.png "nova predefinição"){.inline} Adicionar uma predefinição
: Cria uma nova [predefinição](manual/soundfont-editor/editing-pages/preset-editor.md).
  Uma caixa de diálogo pede o nome da predefinição e, se houver instrumentos selecionados na árvore, oferece vinculá-los à nova predefinição.
  São usados os primeiros números de banco e de predefinição disponíveis.

![ferramentas](images/toolbar_toolbox.png "ferramentas"){.inline} Caixa de ferramentas
: Clicar neste botão mostra um menu com todas as [ferramentas](manual/soundfont-editor/tools/index.md) disponíveis para os elementos selecionados na [árvore](manual/soundfont-editor/tree.md).

![desfazer](images/toolbar_undo.png "desfazer"){.inline} Desfazer
: Desfaz a última modificação, também acessível com :kbd:`Ctrl`+:kbd:`Z`.

![refazer](images/toolbar_redo.png "refazer"){.inline} Refazer
: Refaz a última modificação desfeita, também acessível com :kbd:`Ctrl`+:kbd:`Y` ou :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`Z`.

![salvar](images/toolbar_save.png "salvar"){.inline} Salvar
: Salva a soundfont, também acessível com :kbd:`Ctrl`+:kbd:`S`.


## Alternar a visualização {#doc_view}


Na parte direita da barra de ferramentas, alguns ícones podem aparecer, dependendo do [editor](manual/soundfont-editor/editing-pages/index.md) exibido:

* é possível alternar entre a ![tabela](images/toolbar_table.png "tabela"){.inline} [visualização em tabela](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table), o ![alcances](images/toolbar_range.png "alcances"){.inline} [editor de alcances](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range) e o ![envelope](images/toolbar_adsr.png "envelope"){.inline} [editor de envelopes](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope) ao editar um instrumento,
* é possível alternar entre a ![tabela](images/toolbar_table.png "tabela"){.inline} visualização em tabela e o ![alcances](images/toolbar_range.png "alcances"){.inline} editor de alcances ao editar uma predefinição,
* nada é exibido nos demais casos.

A visualização escolhida é mantida quando outro instrumento ou predefinição é selecionado. A visualização exibida para o primeiro instrumento ou predefinição ao abrir uma soundfont pode ser escolhida com a opção :guilabel:`Visualização inicial` das [configurações](manual/settings.md#doc_interface).


## Gravador {#doc_recorder}


Ao clicar no ícone ![gravador](images/toolbar_recorder.png "gravador"){.inline}, abre-se uma janela com o gravador, que pode gravar a saída do Polyphone em um arquivo :file:`.wav`. O tempo decorrido é exibido na janela.


![Gravador](images/recorder.png "Gravador")


O botão esquerdo pode ter dois estados:

Gravar
: Inicia uma nova gravação se a captura estiver desligada.
  Uma caixa de diálogo pede o nome do arquivo que conterá o sinal de áudio gravado.

Parar
: Interrompe a captura em andamento.
  A captura também é interrompida quando a janela é fechada.

O botão direito também pode ter dois estados (acessível somente se a captura estiver ligada):

Pausar
: Pausa a captura em andamento.

Retomar
: Retoma a captura em andamento.


## Teclado virtual {#doc_keyboard}


Ao clicar no ícone ![piano](images/toolbar_piano.png "piano"){.inline}, abre-se uma janela com o teclado virtual, criado para testar amostras, instrumentos e predefinições.
Essa janela emite sinais MIDI:

* **note on** / **note off** / **aftertouch** com o teclado de piano,
* outros sinais MIDI se a **parte dos controladores MIDI** estiver expandida.

A nota, a velocidade e o aftertouch da nota tocada são exibidos na parte inferior da janela, junto com uma lista suspensa para escolher o tamanho do teclado (5 oitavas, 6 oitavas, 88 teclas ou 128 teclas).
Dois botões no canto inferior direito exibem ou ocultam os controladores MIDI acima do teclado e os efeitos de áudio abaixo dele.

A janela mantém as proporções do teclado: alterar sua largura altera sua altura, e vice-versa.
Exibir ou ocultar uma área altera apenas a altura da janela.


![Teclado virtual](images/virtual_keyboard.png "Teclado virtual")


### Teclado de piano


O teclado de piano emite sinais que contêm números de nota e de velocidade.
Ele pode ser controlado de três maneiras:

com o mouse
: A tecla desejada pode ser clicada com o mouse.
  A velocidade é definida de acordo com a posição vertical do clique.
  Após um clique, os movimentos seguintes dentro da mesma tecla emitem sinais de aftertouch.

com o teclado do computador
: As teclas usadas são definidas nas [configurações](manual/settings.md#doc_keyboard).
  Elas tocam assim que a janela do teclado virtual está ativa, mesmo que outro elemento dessa janela tenha o foco (um botão giratório, a lista suspensa…).

com um teclado MIDI externo
: A entrada MIDI é definida nas [configurações](manual/settings.md#doc_general).
  Se o teclado de piano estiver visível, ele mostrará o que você está tocando.


#### Marcadores e cores


* Quando um [instrumento](manual/soundfont-editor/editing-pages/instrument-editor.md) ou uma [predefinição](manual/soundfont-editor/editing-pages/preset-editor.md) está selecionado, a área não coberta pelas divisões fica **acinzentada**.
  Assim, é possível visualizar a extensão completa de um instrumento ou de uma predefinição.
* Quando uma divisão de um instrumento ou de uma predefinição está selecionada, seu alcance é exibido da mesma forma que acima.
  Além disso, sua nota base é identificada por um **marcador laranja**.
* Quando uma tecla é selecionada no teclado, ela aparece em **azul** (ou na cor de seleção escolhida nas [configurações](manual/settings.md#doc_interface) do software).
  Além disso, o alcance da divisão correspondente aparece em **azul-claro**.
  Isso se aplica somente no nível do instrumento.
* O dó central (nota 60) é identificado por um **marcador preto**.


#### Atalhos úteis


:kbd:`Ctrl`+:kbd:`K`
: Este atalho coloca o foco no teclado virtual a qualquer momento (o teclado é exibido se estiver oculto).
  Isso permite, por exemplo durante a edição de uma tabela, usar o teclado do computador para tocar em vez de editar uma célula.
  Um efeito de brilho no teclado virtual confirma que o foco está nele.

:kbd:`Ctrl`+:kbd:`1`, :kbd:`Ctrl`+:kbd:`2`, … :kbd:`Ctrl`+:kbd:`8`
: Estes atalhos mudam as oitavas tocadas com o teclado do computador.
  Depois de usar :kbd:`Ctrl`+:kbd:`1`, as oitavas mais graves podem ser tocadas.
  As oitavas mais agudas são alcançadas com :kbd:`Ctrl`+:kbd:`8`.
  Eles também funcionam assim que a janela do teclado virtual está ativa.

:kbd:`→` / :kbd:`←`
: A seta para a direita ou para a esquerda toca a altura imediatamente acima ou abaixo da última tecla tocada, com a mesma velocidade.
  Combinada com a tecla :kbd:`Shift`, 6 teclas são puladas em vez de 1.

:kbd:`↑` / :kbd:`↓`
: A seta para cima ou para baixo toca a mesma altura da última tecla tocada, mas com uma velocidade maior ou menor.
  Combinada com a tecla :kbd:`Shift`, a velocidade muda em 10 em vez de 1.

As setas só tocam notas se o teclado de piano tiver o foco: caso contrário, elas agem sobre o elemento que tem o foco, por exemplo um botão giratório.


### Controladores MIDI


Clicando no botão MIDI do canto inferior direito, a área dos controladores MIDI é exibida acima do teclado ou ocultada.
A janela então se desloca para que o teclado fique no mesmo lugar da tela (isso não é possível com o Wayland no Linux).
Se você estiver trabalhando com moduladores, é recomendável usar esses recursos para testar sua soundfont.


![Teclado virtual com controladores MIDI](images/virtual_keyboard_2.png "Teclado virtual com controladores MIDI")


Essa área é composta de diferentes botões:

Roda de pitch
: A roda de pitch aumenta ou diminui a altura.
  Ela volta automaticamente à posição original 0.

Sensibilidade
: A sensibilidade define a amplitude da modificação da roda de pitch, em semitons.
  Por padrão, é 2 (um tom inteiro).

Pressão
: A pressão é a quantidade de pressão exercida sobre o teclado.
  Pode ser comparada ao aftertouch, mas, enquanto o aftertouch é específico de cada tecla, a pressão é um sinal global para todas as teclas.

Botões giratórios
: 3 botões giratórios permitem alterar valores de controladores MIDI.
  Para cada botão, é possível especificar um número de controlador e depois seu valor.

LED liga / desliga
: Este LED serve para definir o valor de um controlador MIDI como 0 ou 127 (útil, por exemplo, para o pedal de sustain).
  É como um botão giratório, mas com apenas dois valores possíveis.

Todos esses elementos podem ser sincronizados com um teclado MIDI conectado ao computador, se a entrada MIDI estiver configurada corretamente nas [configurações](manual/settings.md#doc_general). Valores relativos de controladores MIDI são suportados para controlar os botões giratórios.


### Efeitos de áudio {#doc_keyboard_effects}


Clicando no botão do alto-falante no canto inferior direito, uma área é exibida abaixo do teclado para ajustar as principais configurações do sintetizador enquanto se toca:

Reverberação
: Um LED liga ou desliga a reverberação e, em seguida, 4 botões giratórios alteram seu nível, o tamanho da sala, a largura e o amortecimento.
  O nome de um botão giratório é exibido ao passar o mouse sobre ele.

Chorus
: Um LED liga ou desliga o chorus e, em seguida, 3 botões giratórios alteram seu nível, sua profundidade e sua frequência.

Volume
: O controle deslizante altera o volume geral.

Essas configurações são as mesmas das [configurações de som](manual/settings.md#doc_sound): uma alteração feita em um lugar é exibida no outro.
Uma alteração é progressiva e não interrompe as notas que estão sendo tocadas.

*[MIDI]: Musical Instrument Digital Interface