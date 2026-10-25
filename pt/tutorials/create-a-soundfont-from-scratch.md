Para criar uma [soundfont](manual/annexes/the-different-soundfont-formats.md#doc_sf2), clique em :guilabel:`Nova soundfont` na [tela inicial](manual/index.md#doc_left).
As soundfonts têm uma estrutura em 3 níveis; por isso, a criação de uma nova soundfont envolve três etapas fundamentais:

* a [preparação das amostras](#doc_sample),
* a [criação dos instrumentos](#doc_instrument), compostos de amostras,
* a [criação das predefinições](#doc_preset), compostas de instrumentos.


## Preparação das amostras {#doc_sample}


### Carregamento das amostras


A preparação das amostras começa pelo **carregamento** de arquivos de áudio (.wav, por exemplo).
Para isso, clique em :guilabel:`Adicionar uma amostra` na [barra de ferramentas](manual/soundfont-editor/toolbar.md#doc_edit) e selecione um ou mais arquivos de áudio (os arquivos também podem ser arrastados e soltos na [árvore](manual/soundfont-editor/tree.md#doc_dragdrop)).
As amostras podem ser obtidas:

* pela internet,
* gravando um instrumento musical real,
* por síntese, usando um software especializado.


### Loop das amostras


Em seguida, pode ser necessário um **loop** na amostra, se você quiser que ela soe por mais tempo do que sua duração normal.
Um exemplo seria ter apenas uma amostra de flauta de 1 segundo e querer que ela soe indefinidamente.
Isso pode ser feito atribuindo, manual ou automaticamente, pontos de loop (início + fim) dentro dos limites da amostra, para que a área do loop possa ser repetida e a amostra toque por mais tempo.
Embora você possa atribuir os pontos de loop manualmente, usar a função “[Loop automático](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop)” para atribuí-los automaticamente geralmente dá melhores resultados e é muito mais rápido.

Para atribuir os pontos de loop manualmente:

1. clique na [árvore](manual/soundfont-editor/tree.md) na amostra que deve receber o loop,
2. no [editor de amostras](manual/soundfont-editor/editing-pages/sample-editor.md), posicione arbitrariamente o início e o fim do loop no [gráfico](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph) (área de exibição do WAV), a menos que um loop já esteja definido.
  Use o clique esquerdo para o início e o clique direito para o fim.
  O ponto direito deve ser definido primeiro (já que o ponto direito fica por padrão na posição 0 e não é possível colocar um ponto esquerdo depois do ponto direito).
3. pressione o botão :guilabel:`Reproduzir` depois de ativar a opção :guilabel:`loop`,
4. ajuste o início e / ou o fim do loop, enquanto a amostra toca, até que a transição entre as duas posições seja a mais suave possível.


**Observação:** para ouvir o loop em um instrumento, é preciso selecionar ![loop ligado](images/loop_on.png "loop ligado"){.inline} na linha do parâmetro :guilabel:`Reprodução em loop`, na coluna :guilabel:`Global` ou nas colunas das notas individuais da [tabela de parâmetros](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table). ![loop ligado](images/loop_on.png "loop ligado"){.inline} o liga, ![loop desligado](images/loop_off.png "loop desligado"){.inline} ou um campo vazio o desliga.


![Ilustração do loop](images/loop_illustration.png "Ilustração do loop")


### Afinação das amostras


Por fim, é preciso fazer a **afinação**.
Para isso, uma ferramenta de calibração (seno) está disponível na página de edição das [amostras](manual/soundfont-editor/editing-pages/sample-editor.md#doc_player).
Para cada amostra, o método é o seguinte:

1. inicie a reprodução (pressione o botão :guilabel:`Reproduzir`), se possível com a opção :guilabel:`loop` ativada,
2. ative a opção :guilabel:`seno`,
3. ajuste o controle de volume para ouvir os dois sons o mais claramente possível,
4. altere a :guilabel:`Nota base` até que os dois sons fiquem o mais próximos possível,
5. ajuste a :guilabel:`Correção` (em cents) para afinar a amostra na nota base.
   Para isso, preste atenção aos batimentos que surgem e faça com que sejam os mais lentos possível.

A análise de [frequências](manual/soundfont-editor/editing-pages/sample-editor.md#doc_freq) pode ser um bom indicador para começar a afinação.


### Para ir mais longe


Várias ferramentas estão disponíveis para editar as amostras, como:

* [remover o silêncio](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank) inicial,
* [equalizar](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq) o som,
* [transpor](manual/soundfont-editor/tools/sample-tools.md#doc_transpose),
* [normalizar](manual/soundfont-editor/tools/sample-tools.md#doc_volume) o volume,
* [ajustar o balanço](manual/soundfont-editor/tools/sample-tools.md#doc_balance) das amostras estéreo.


## Criação dos instrumentos {#doc_instrument}


Nesta etapa, supõe-se que uma ou mais amostras estejam disponíveis para a criação de um instrumento.


### Criar um instrumento e adicionar amostras


Primeiro, clique em :guilabel:`Adicionar um instrumento` na [barra de ferramentas](manual/soundfont-editor/toolbar.md#doc_edit).
É preciso então informar um nome (se houver amostras selecionadas na árvore, elas podem ser vinculadas diretamente ao novo instrumento).

Caso contrário, adicione amostras ao instrumento com um “[arrastar e soltar](manual/soundfont-editor/tree.md#doc_dragdrop)”.
Na árvore, você notará que divisões aparecem no instrumento.

**Observação:** quando as divisões são criadas, as amostras não são copiadas, mas vinculadas. Não há restrição quanto ao número de divisões.


### Distribuição das divisões


Quando amostras são adicionadas aos instrumentos, elas aparecem como divisões (colunas) na [tabela do editor de instrumentos](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).
Cada divisão deve então ser posicionada no teclado alterando o :guilabel:`Alcance de notas` na tabela.
Em geral, é uma boa prática que o alcance de uma divisão inclua a nota base da amostra que ela representa.
Toda a superfície do teclado deve ser coberta (por exemplo, da nota 36 à nota 96 para um teclado de sintetizador clássico).

A ferramenta “[Posicionamento automático das amostras](manual/soundfont-editor/tools/instrument-tools.md#doc_position)” distribui automaticamente as amostras pelo teclado.


### Configuração das divisões


Para ativar o modo de reprodução em loop em um instrumento que contém amostras com loop, selecione ![loop ligado](images/loop_on.png "loop ligado"){.inline} na linha do parâmetro :guilabel:`Reprodução em loop`, na coluna :guilabel:`Global` ou nas colunas das notas individuais da tabela de parâmetros.

* ![loop ligado](images/loop_on.png "loop ligado"){.inline} o liga,
* ![loop desligado](images/loop_off.png "loop desligado"){.inline} ou um campo vazio o desliga.
* ![loop ligado e fim](images/loop_on_end.png "loop ligado e fim"){.inline} liga o loop, e o restante da amostra é lido depois que a tecla é solta.

A divisão global, como o nome sugere, permite informar um parâmetro para o instrumento inteiro.
Um parâmetro global se aplica somente às divisões em que o mesmo parâmetro não está especificado.
Isso significa que os parâmetros das divisões comuns têm prioridade sobre os ajustes da divisão global.

No contexto deste tutorial simples, nenhum outro parâmetro precisa ser editado para que o instrumento funcione.
Já deve ser possível tocá-lo com o [teclado virtual](manual/soundfont-editor/toolbar.md#doc_keyboard).

No entanto, para melhorar o instrumento, é recomendável informar um valor na linha :guilabel:`Vol env release (s)`, o que evita que o som pare bruscamente assim que uma tecla é solta.
Os outros parâmetros são explicados na descrição das [tabelas](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).


## Criação das predefinições {#doc_preset}


Nesta etapa, supõe-se que um ou mais instrumentos estejam disponíveis para a criação de uma predefinição.


### O que é uma predefinição?


Uma predefinição é o equivalente de um nome de patch de som encontrado nos sintetizadores de hardware.
Ela é visível de fora de uma soundfont e é identificada por um número de banco e um número de predefinição.
Uma soundfont pode ter uma ou várias predefinições.
Cada predefinição contém um ou mais instrumentos, assim como os instrumentos contêm uma ou mais amostras.


### Criar uma predefinição e adicionar instrumentos


Primeiro, clique em :guilabel:`Adicionar uma predefinição` na [barra de ferramentas](manual/soundfont-editor/toolbar.md#doc_edit).
É preciso então informar um nome (se houver instrumentos selecionados na árvore, eles podem ser vinculados diretamente à nova predefinição).

Caso contrário, adicione instrumentos à predefinição com um “[arrastar e soltar](manual/soundfont-editor/tree.md#doc_dragdrop)”.
Na árvore, você notará que uma ou mais divisões aparecem na predefinição.
Muitas vezes pode haver apenas uma, como no caso de uma predefinição com um único instrumento.

É possível criar tantas predefinições quanto instrumentos (ou até mais), e cada predefinição pode conter um ou mais instrumentos.


### Configurar uma predefinição


Alterar os parâmetros da predefinição pode não ser necessário, pois:

* ao criar uma predefinição, o número de banco e o número de predefinição são atribuídos automaticamente,
* ao adicionar um instrumento a uma predefinição, o alcance de notas é calculado automaticamente de acordo com o instrumento.

No escopo deste tutorial, a criação da soundfont está concluída!
