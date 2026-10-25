Um instrumento construído em uma soundfont é composto de amostras, vindas por exemplo de gravações de um instrumento real.
Este tutorial descreve como adicionar e preparar uma amostra em uma soundfont.


## Importação {#doc_import}


Primeiro, é preciso ter amostras no seu computador.
Muitas amostras gratuitas podem ser encontradas na internet, seja gravadas a partir de instrumentos reais (flauta, violão, violoncelo, …), seja criadas com diferentes tipos de síntese sonora.

Para este tutorial, é fornecida esta amostra de flauta: <a href="files/flute.wav" download>flute.wav</a>.

Depois de obtê-la, crie uma nova soundfont com o Polyphone (:menuselection:`Menu --> Novo`) e importe a amostra (:guilabel:`Adicionar uma amostra` na [barra de ferramentas](manual/soundfont-editor/toolbar.md#doc_edit)).


![Importar uma amostra](images/import-sample.png "Importar uma amostra")


## Loop {#doc_loop}


### Introdução


Um loop é uma área dentro de uma amostra que pode ser repetida para prolongar a duração da amostra.
Digamos que uma amostra dure apenas 0,5 segundo: com um bom loop, o som pode ser prolongado até 10 segundos ou mais.
Isso só é útil para amostras de instrumentos não percussivos, como flautas, trompetes, órgãos e violinos.
Ao contrário, uma bateria não precisa de loops.
Esse também pode ser o caso de um piano ou de um violão, dependendo de como esses sons são sintetizados.

Assim, esta seção é necessária se, e somente se, você sentir necessidade de prolongar a duração original de uma amostra.
Caso contrário, vá diretamente para a próxima seção, “Afinação”.


### Configurar um loop


Na árvore, clique na amostra que deve receber o loop e concentre-se nos parâmetros do loop.


![Loop da amostra: parâmetros do loop](images/loop-sample-1.png "Loop da amostra: parâmetros do loop")


Se você tiver sorte, esses valores já vieram na amostra .wav e esses campos já estão preenchidos.
Caso contrário, ou se o loop estiver mal configurado (isso acontece…), clique arbitrariamente à esquerda e à direita no gráfico (área de exibição do WAV) para definir o início e o fim do loop.
Isso geralmente é feito na fase estável do som, e não durante o ataque ou o release.
Em seguida, dê zoom em torno do início do loop e tente sobrepor as curvas verde (ou vermelha) e azul.


![Loop da amostra: alinhamento das ondas](images/loop-sample-2.png "Loop da amostra: alinhamento das ondas")


### Ouvir o resultado


Quando o loop estiver configurado, pressione :guilabel:`Reproduzir` no reprodutor, com a opção :guilabel:`loop` ativada, para verificar o resultado.


![Loop da amostra: verificar o resultado](images/loop-sample-3.png "Loop da amostra: verificar o resultado")


Se você não ouvir a transição quando o fim do loop é lido, o loop está bom.
Caso contrário, mantenha o reprodutor ligado e mova ligeiramente o fim e / ou o início do loop até encontrar pontos com uma transição a mais suave possível.
Ouça as gravações a seguir para perceber a diferença entre um loop ruim e um loop bom:

* <a href="files/bad-loop.mp3" download>loop ruim</a>

![](files/bad-loop.mp3)

* <a href="files/good-loop.mp3" download>loop bom</a>

![](files/good-loop.mp3)


**Observação:** existe uma ferramenta para “trapacear” se você não conseguir encontrar um loop facilmente: a ferramenta de [loop automático](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop).
Ela cria automaticamente um loop cortando a amostra e sobrepondo duas seções, para que o fim da seção corresponda exatamente ao seu início.
Consulte a documentação correspondente para mais informações.


## Afinação {#doc_tuning}


Todas as amostras precisam ser afinadas se você não quiser um instrumento desafinado.
Ao indicar na soundfont a altura exata de todas as amostras importadas, a soundfont poderá adaptá-las às notas.
Algumas amostras podem já conter essa informação; nesse caso, você não deveria precisar alterar os valores de :guilabel:`Nota base` e :guilabel:`Correção`.
Mas, por precaução, é sempre bom verificar.


![Afinar a amostra: informações de altura](images/tune-sample-1.png "Afinar a amostra: informações de altura")


Ative o loop, se a amostra tiver um, e pressione :guilabel:`Reproduzir` no reprodutor.
Em seguida, ative a calibração :guilabel:`seno` e ajuste o volume para ouvir os dois sons o mais claramente possível.


![Afinar a amostra: tocar a amostra com o sinal de calibração](images/tune-sample-2.png "Afinar a amostra: tocar a amostra com o sinal de calibração")


Comece a afinação alterando a nota base até que os dois sons fiquem o mais próximos possível.
Continue ajustando a correção.
Nesta etapa, preste atenção aos batimentos que surgem e faça com que sejam os mais lentos possível.
Eis exemplos com o som de uma flauta e um seno de calibração:

* <a href="files/very-bad-tune.mp3" download>afinação muito ruim</a> (a nota base não corresponde)

![](files/very-bad-tune.mp3)

* <a href="files/bad-tune.mp3" download>afinação ruim</a> (a correção não está certa e ouvem-se batimentos)

![](files/bad-tune.mp3)

* <a href="files/good-tune.mp3" download>afinação boa</a> (os dois valores estão corretos)

![](files/good-tune.mp3)


**Observação:** a [análise de frequências](manual/soundfont-editor/editing-pages/sample-editor.md#doc_freq) pode ser um bom indicador para começar a afinação. Mas lembre-se de que nenhum algoritmo é perfeito para essa tarefa, e só os seus ouvidos encontrarão a afinação certa.


![Afinar a amostra: análise de frequências](images/tune-sample-3.png "Afinar a amostra: análise de frequências")


## Para ir mais longe {#doc_further}


Várias ferramentas estão disponíveis no menu :guilabel:`Caixa de ferramentas` da [barra de ferramentas](manual/soundfont-editor/toolbar.md#doc_edit).
Você pode editar as amostras:

* [removendo o silêncio](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank) inicial,
* [equalizando](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq) o som,
* [transpondo](manual/soundfont-editor/tools/sample-tools.md#doc_transpose),
* [normalizando](manual/soundfont-editor/tools/sample-tools.md#doc_volume) o volume,
* [ajustando o balanço](manual/soundfont-editor/tools/sample-tools.md#doc_balance) das amostras estéreo.


## Observação sobre as amostras estéreo {#doc_stereo}


As amostras estéreo são compostas de duas amostras mono.
O vínculo é armazenado nas informações de uma amostra.


![Amostra estéreo](images/stereo-sample.png "Amostra estéreo")


Gerenciar duas amostras mono pode dificultar a criação do loop, porque um loop pode soar bem em um lado e não no outro.
Nesse caso, não hesite em trapacear configurando dois loops diferentes nos lados direito e esquerdo.
A qualidade do som não será prejudicada.
