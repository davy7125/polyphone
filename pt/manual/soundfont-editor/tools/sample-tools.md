Quando uma ou mais amostras estão selecionadas na [árvore](manual/soundfont-editor/tree.md), o [menu de ferramentas](manual/soundfont-editor/tools/index.md) contém várias ferramentas organizadas em 5 categorias:

* [processamento de áudio](#doc_cat_processing),
* [edição rápida](#doc_cat_fast_editing),
* [arquivos](#doc_cat_files),
* [loop / reprodução](#doc_cat_loop_playback),
* [amostras estéreo](#doc_cat_stereo).

É possível usar as ferramentas simultaneamente em várias amostras (com uma seleção múltipla).


## Processamento de áudio {#doc_cat_processing}


### Mudar volume {#doc_volume}

Você pode mudar o volume da amostra de três maneiras diferentes:

* somando uma certa quantidade de dB (positiva ou negativa),
* multiplicando o sinal por um número,
* normalizando o sinal para que seu máximo represente uma certa porcentagem da intensidade máxima.

![Ferramenta de mudança de volume](images/tool_change_volume.png "Ferramenta de mudança de volume")


### Comando externo {#doc_externalcommand}

Esta ferramenta permite usar um comando ou programa externo para editar uma amostra.
Não é mais preciso exportar e reimportar uma amostra no Polyphone, tudo é feito automaticamente:

1. baixe um software de edição de arquivos .wav, como :program:`Audacity` ou :program:`Sweep` (Linux),
2. use a ferramenta :guilabel:`Comando externo` em uma ou mais amostras,
3. preencha a linha de comando no campo :guilabel:`Comando` para chamar o software externo (pode ser necessário ler a documentação desse software), sendo o argumento :command:`{wav}` obrigatório, pois ele é substituído pelo caminho do arquivo exportado (por exemplo :command:`sweep {wav}`),
4. edite as amostras com a ferramenta externa,
5. salve a versão modificada dos arquivos wav com a ferramenta externa e feche-a,
6. as amostras serão carregadas automaticamente na soundfont.

O botão à direita do campo ajuda a selecionar um arquivo executável, e a lista :guilabel:`Histórico` lembra os últimos comandos usados.
Duas opções estão disponíveis:

Permitir edição estéreo
: Quando uma amostra estéreo é processada, ela é exportada com sua amostra vinculada em um único arquivo estéreo, e as duas amostras são atualizadas quando o arquivo é reimportado.

Substituir informação da amostra se possível
: O loop, a nota base e a correção armazenados no arquivo devolvido pelo software externo substituem os da amostra.
  Caso contrário, as informações existentes são mantidas.

![Ferramenta de comando externo](images/tool_external_command.png "Ferramenta de comando externo")

**Observação:** durante o processo, o Polyphone exporta um arquivo .wav e reimporta esse mesmo arquivo depois que o comando externo é executado. Se o comando externo criar outros arquivos ou salvar o resultado em outro arquivo, esses arquivos serão ignorados e o Polyphone reimportará o arquivo exportado inicialmente, que provavelmente não terá sido modificado.


### Filtrar frequências {#doc_filter}

Ao usar esta ferramenta, é exibida uma janela que mostra as frequências contidas nas amostras selecionadas.
Você pode então usar o mouse para desenhar uma área vermelha.
Tudo o que estiver nessa área será removido: isso significa que você pode diminuir ou remover manualmente algumas frequências.

![Ferramenta de filtragem de frequências](images/tool_filter_frequencies.png "Ferramenta de filtragem de frequências")

**Observação:** esse filtro não é comum, pois corta as frequências de forma abrupta.
Esta ferramenta pode ser útil para criar um filtro “brick-wall”, mas na maioria dos casos você provavelmente vai preferir usar o [equalizador](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq).


### Transpor {#doc_transpose}

A ferramenta pede um deslocamento em semitons e então reamostra o som para alterar a altura.

![Ferramenta de transposição](images/tool_transpose_smpl.png "Ferramenta de transposição")


## Edição rápida {#doc_cat_fast_editing}


### Carregar um parâmetro dos instrumentos {#doc_load_from_instruments}

Parâmetros no nível do instrumento podem substituir ou deslocar os parâmetros definidos no nível da amostra.
Com esta ferramenta, é possível corrigir ou completar os parâmetros das amostras com o que está definido nos instrumentos que usam essas amostras:

* modificando a nota base da amostra com a substituição definida no nível do instrumento,
* deslocando a nota base da amostra com o parâmetro “Afinação (semitons)”,
* deslocando a correção de altura da amostra com o parâmetro “Afinação (cents)”,
* definindo o início e o fim do loop da amostra com os parâmetros de offset do loop.

![Ferramenta para carregar um parâmetro dos instrumentos](images/tool_load_from_instruments.png "Ferramenta para carregar um parâmetro dos instrumentos")


### Deslocar um parâmetro {#doc_shift_parameter}

Esta ferramenta permite editar simultaneamente, somando um valor ou multiplicando por um valor, um destes parâmetros em todas as amostras selecionadas:

* nota base,
* correção,
* início do loop,
* fim do loop.

![Ferramenta de edição rápida](images/tool_shift_parameter.png "Ferramenta de edição rápida")


## Arquivos {#doc_cat_files}


### Exportação WAV {#doc_wavexport}

Depois de selecionar um diretório de destino, todas as amostras selecionadas serão exportadas como arquivos .wav.


## Loop / reprodução {#doc_cat_loop_playback}


### Loop automático {#doc_autoloop}

O software procura duas posições na amostra, distantes entre si e adequadas para criar um loop.
Se necessário, um pequeno trecho de som é copiado com um crossfade para que a transição no ponto de junção seja a mais suave possível.
Com uma amostra estéreo, o mesmo loop é criado para os dois lados.

Em caso de falha, aparece uma mensagem de aviso indicando que nenhum loop pôde ser encontrado (a amostra provavelmente é curta demais ou turbulenta demais).

**Atenção:** esta ferramenta modifica a amostra original.
As modificações podem ser desfeitas com a função “[desfazer](manual/soundfont-editor/toolbar.md#doc_edit)”, mas, se o arquivo for salvo e o Polyphone for fechado, não há como voltar atrás.

**Observação:** por padrão, o loop fica desativado em um instrumento.
Para ouvi-lo, é preciso selecionar ![loop ligado](images/loop_on.png "loop ligado"){.inline} na linha do parâmetro :guilabel:`Reprodução em loop`, na coluna :guilabel:`Global` ou nas colunas das notas individuais da [tabela](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).
![loop ligado](images/loop_on.png "loop ligado"){.inline} o liga, ![loop desligado](images/loop_off.png "loop desligado"){.inline} o desliga.
![loop ligado e fim](images/loop_on_end.png "loop ligado e fim"){.inline} liga o loop, e a amostra é tocada até o fim depois que a tecla é solta.


### Loop com crossfade {#doc_crossfadeloop}

A ferramenta de crossfade pode ser vista como a versão manual da [ferramenta de loop automático](#doc_autoloop). Primeiro, é preciso posicionar o início e o fim do loop no [gráfico](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph) em torno de uma área estável. Em seguida, esta ferramenta cria um loop a partir dessas posições aplicando um crossfade cuja duração (em milissegundos) é configurável. Tudo é, portanto, configurável, mas é preciso cuidado ao procurar o início e o fim do loop:

* nas duas posições, os harmônicos do som devem coincidir, para não ouvir um efeito de disco de vinil arranhado durante a transição,
* as fases dos diferentes harmônicos também devem coincidir, para não ouvir um cancelamento de frequências durante o crossfade, o que altera fortemente o timbre de um instrumento,
* tanto quanto possível, o fim do loop deve ficar longe do início, para evitar um som sintético demais.

Uma dica é se concentrar nas posições em que a onda cruza o eixo x (valor 0).

![Ferramenta de loop com crossfade](images/tool_crossfade_loop.png "Ferramenta de loop com crossfade")


### Remover silêncio inicial {#doc_removeblank}

Qualquer área vazia antes do início da amostra é eliminada, reduzindo o tempo até o som ser ouvido.
Com uma amostra estéreo, o mesmo comprimento é removido dos dois lados.

![Remover silêncio inicial](images/remove_blank.png "Remover silêncio inicial")

**Observação:** esta ferramenta pode ser aplicada automaticamente sempre que uma amostra é importada; a opção fica nas [configurações](manual/settings.md#doc_general) do software.


### Cortar no fim do loop {#doc_trimloop}

Se um loop estiver definido para a amostra selecionada, o software corta o som após o fim do loop (deixando uma pequena margem no fim).

![Cortar no fim do loop](images/trim_to_loop.png "Cortar no fim do loop")

**Observação:** esta ferramenta pode ser aplicada automaticamente sempre que uma amostra é importada, de acordo com as [configurações](manual/settings.md#doc_general) do software.


## Amostras estéreo {#doc_cat_stereo}


### Ajuste de balanço {#doc_balance}

Se a amostra estiver vinculada a outra (estéreo), o software pode igualar a intensidade média das duas amostras para ajustar o balanço.
Isso facilita então o posicionamento preciso de uma amostra no panorama dentro de um [instrumento](manual/soundfont-editor/editing-pages/instrument-editor.md).
Esta ferramenta não pode ser usada com amostras mono.


### Encontrar um vínculo {#doc_link}

Entre as amostras selecionadas, as amostras cujos nomes diferem apenas pela última letra (L&nbsp;/&nbsp;R) são vinculadas automaticamente (espaços e os caracteres ``-``, ``_``, ``(``, ``)``, ``[``, ``]``, ``{``, ``}`` e ``|`` são ignorados nos nomes).
Por exemplo, se em uma soundfont duas amostras se chamam ``bassoon45L`` e ``bassoon45R``, ``bassoon45L`` será reconhecida como a parte esquerda de ``bassoon45R`` e ``bassoon45R`` será reconhecida como a parte direita de ``bassoon45L``.

Com essa informação, o balanço correto será usado quando essas amostras forem adicionadas a um instrumento.
Você também pode aproveitar a edição simultânea das partes esquerda e direita na [tabela](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) do instrumento, se a edição estéreo estiver ativada nas [configurações](manual/settings.md#doc_general).

**Atenção:** esta ferramenta pode desfazer vínculos existentes se os nomes não seguirem a regra acima.
Uma mensagem lista as amostras para as quais nenhuma correspondência foi encontrada.


### Desvincular {#doc_unlink}

Todas as amostras selecionadas que estão vinculadas (estéreo) são desvinculadas.

Assim, todas as amostras serão processadas como sons mono e terão todas o mesmo balanço se forem adicionadas a um instrumento.
A edição estéreo, se ativada nas [configurações](manual/settings.md#doc_general), deixará de ser aplicável.
