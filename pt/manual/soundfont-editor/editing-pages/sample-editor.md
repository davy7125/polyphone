Ao clicar em uma ou mais amostras na [árvore](manual/soundfont-editor/tree.md), aparece a página de edição das amostras.

Esta página é dividida em várias partes:

* um [gráfico](#doc_graph) que representa a amostra,
* uma área de [informações](#doc_info),
* uma análise das [frequências](#doc_freq),
* um [equalizador](#doc_eq),
* um [reprodutor](#doc_player) de amostras.


![Página da amostra](images/edit_sample.png "Página da amostra")


## O gráfico {#doc_graph}


### Zoom e deslocamento


O gráfico permite visualizar a forma de onda.
É possível dar zoom vertical ou horizontal mantendo pressionado o botão direito do mouse e arrastando:

* arrastar para a direita expande o eixo horizontal,
* arrastar para a esquerda contrai o eixo horizontal,
* arrastar para cima expande o eixo vertical,
* arrastar para baixo contrai o eixo vertical.

Manter pressionado o botão esquerdo do mouse permite deslocar o gráfico horizontalmente, se o eixo horizontal tiver sido expandido.
A roda do mouse também rola o gráfico horizontalmente.


### Posições do loop e da reprodução


Uma barra vertical verde indica a posição de início do loop (alterável com um clique esquerdo no gráfico), e uma barra vermelha indica a posição de fim do loop (alterável com um clique direito no gráfico).
Uma linha pontilhada que representa uma parte da onda está associada a cada barra vertical, útil para visualizar a sobreposição do início e do fim do loop.

Durante a reprodução, uma barra vertical branca percorre o gráfico e indica a posição de reprodução em tempo real.


### Cortar a amostra


Mantendo a tecla :kbd:`Alt` pressionada, é possível definir uma área a ser cortada:

* pressionar o botão esquerdo do mouse define o início da área,
* soltar esse botão define o fim da área.

Depois que a área é definida, uma caixa de diálogo pede confirmação antes de cortar a amostra.
A resposta :guilabel:`Sim e criar outra amostra` corta a amostra e também cria uma nova amostra a partir da parte removida.


![Cortar a amostra](images/cutting_sample.png "Cortar a amostra")


### Cores do gráfico


As cores do gráfico podem ser editadas nas [configurações](manual/settings.md#doc_interface) do software.


## Seção “Informação” {#doc_info}


A seção :guilabel:`Informação` permite ver — e, na maioria dos casos, editar — as seguintes informações:

Tamanho
: São exibidos o número de valores e a duração correspondente em segundos.

Loop
: Os pontos de início e de fim do loop podem ser modificados aqui.
  Um botão ajusta as posições de início e de fim para que o loop abranja a amostra inteira.

Afinação
: Permite ajustar a nota base de uma amostra e sua correção em cents para que o som fique afinado no temperamento igual.
  Um botão ajusta a nota base e a correção para que os valores correspondam à [frequência](#doc_freq) estimada.

Taxa de amostragem
: Qualquer modificação provoca uma reamostragem, para que a altura do som não seja alterada.

Canal
: Seleciona o tipo entre :guilabel:`mono`, :guilabel:`esquerda`, :guilabel:`direita` ou :guilabel:`vínculo`.
  Se o tipo for :guilabel:`esquerda`, :guilabel:`direita` ou :guilabel:`vínculo`, é possível cancelar o estéreo selecionando :guilabel:`mono`.
  Quando :guilabel:`mono` está selecionado, a volta a um estado estéreo é feita selecionando uma amostra vinculada.

Vínculo
: Seleção da amostra a vincular para tornar o som estéreo.
  Se uma amostra for selecionada, seu tipo passará a :guilabel:`vínculo` (esse tipo pode então ser alterado para :guilabel:`esquerda` ou :guilabel:`direita`).

Se a [compressão](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md#doc_edit) estiver ativada para a soundfont ([formato sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3)), um botão de compressão é exibido para as amostras que ainda não estão comprimidas.
Ao comprimir, a escolha da qualidade de compressão (:guilabel:`Alta`, :guilabel:`Média` ou :guilabel:`Baixa`) é solicitada em uma caixa de diálogo.
O loop de uma amostra pode ser alterado pela compressão.\
**Observação:** a descompressão é feita automaticamente ao editar a amostra (aplicação de um filtro, corte, amplificação etc.).

![Seleção da qualidade de compressão](images/sample_compress_dialog.png "Seleção da qualidade de compressão")

Qualquer alteração em uma amostra estéreo pode ser aplicada à amostra vinculada se a opção :guilabel:`Edição estéreo` estiver marcada nas [configurações](manual/settings.md#doc_general) (ou com o botão localizado no canto inferior direito da janela).
Por meio de uma seleção múltipla na [árvore](manual/soundfont-editor/tree.md), várias amostras podem ser editadas simultaneamente.


## Análise de frequências {#doc_freq}


A análise de frequências inclui uma transformada de Fourier (gráfico que mostra a intensidade das frequências contidas no sinal).

![Gráfico de Fourier](images/fourier_graph.png "Gráfico de Fourier")

À direita é mostrada a lista dos picos dominantes, com as seguintes informações:

* relação entre a intensidade do pico e a intensidade do pico máximo,
* frequência,
* nota correspondente mais próxima,
* correção (distância até a nota correspondente mais próxima).

A partir desses dados é feita uma estimativa da altura do som.
Essa estimativa pode ser usada para a afinação.

Um clique direito no gráfico abre um menu de contexto que permite:

* exportar o gráfico em uma imagem .png,
* mostrar a lista de todos os picos e um botão para exportar essa lista em um arquivo .csv.

![Mostrar frequências de pico](images/tool_show_peak_frequencies.png "Mostrar frequências de pico")


## Equalizador {#doc_eq}


O equalizador permite aumentar ou diminuir algumas frequências, de -15 a 15 dB.
É possível ouvir o efeito com a opção :guilabel:`pré-visualizar` ativada e depois aplicar o efeito definitivamente pressionando o botão :guilabel:`Aplicar`.
Pressionar :guilabel:`Redefinir` coloca todos os controles deslizantes em 0 dB (mas não remove um efeito já aplicado ao som: para isso, use o botão :guilabel:`Desfazer` da [barra de ferramentas](manual/soundfont-editor/toolbar.md#doc_edit)).

Qualquer alteração em uma amostra estéreo pode ser aplicada automaticamente à amostra vinculada se a opção estiver marcada nas [configurações](manual/settings.md#doc_general).


## Reprodutor {#doc_player}


O reprodutor toca o som.
Durante a reprodução, um cursor branco percorre o gráfico para indicar a posição de reprodução.
As opções de reprodução são:

Loop
: Toca o som em loop se o início e o fim do loop estiverem definidos.

Estéreo
: Toca a amostra em estéreo, com sua amostra vinculada.
  Se o loop também for solicitado, são usados os dois loops respectivos (que podem ser diferentes).

Seno
: Um sinal de calibração senoidal é adicionado ao som, permitindo afinar uma amostra com precisão de um centésimo de semitom.
  Primeiro, a nota base precisa ser preenchida.
  Nesse nível, batimentos podem ser ouvidos se a frequência do seno for ligeiramente diferente da frequência fundamental do som.
  Resta ajustar finamente a correção para tornar os batimentos o mais lentos possível, para que a amostra fique o mais afinada possível.
  Veja o tutorial “[Como preparar uma amostra](tutorials/how-to-prepare-a-sample.md)” para mais informações.

Um controle deslizante define o volume da reprodução (esse volume não é armazenado na soundfont).

Se uma reprodução estiver em andamento ao passar de uma amostra para outra pela [árvore](manual/soundfont-editor/tree.md), ela recomeça automaticamente.
Além disso, pressionar a tecla :kbd:`Space` inicia ou interrompe a reprodução se o foco estiver na árvore.