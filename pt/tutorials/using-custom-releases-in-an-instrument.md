O objetivo deste tutorial é criar instrumentos expressivos usando amostras com loop cujos releases foram gravados.
Você vai aprender a aproveitar plenamente as possibilidades de uma amostra desse tipo, usando o ataque e o sustain da amostra, mas também o seu release.


## Release tradicional e release personalizado


Quando você dispara uma nota, ouve primeiro o início da amostra (o ataque) e depois, enquanto não soltar a tecla, o loop dentro da amostra é tocado repetidamente (a fase sustentada).
Quando você solta a tecla, o loop continua sendo usado e o som é atenuado progressivamente até não ser mais ouvido.
Esta é a forma tradicional de fazer um release com as soundfonts: fácil de configurar, mas que ignora os harmônicos transitórios que poderiam ocorrer durante o release real de um instrumento acústico.
Esse tipo de release pode ser visto como um simples prolongamento da fase sustentada.

Um release mais realista consistiria em usar o release real fornecido pela amostra gravada.
O ataque e a fase sustentada seriam feitos da mesma forma, mas, quando a tecla fosse solta, a fase sustentada cairia de repente, dando lugar ao release gravado, que começaria a soar.
Um release personalizado é usado em vez de prolongar o som da fase sustentada, acrescentando detalhes ao som.

Para criar um instrumento assim, o problema é dividido em dois:

* primeiro, definir o ataque e a fase sustentada,
* depois, disparar o release.

O exemplo usado aqui é o som de um trompete de órgão, por seu rico conteúdo harmônico e sua progressão audível.

**Observação:** desde a versão 2.4, o Polyphone também pode disparar uma amostra quando uma tecla é solta, graças ao modo de reprodução em loop ![release](images/sample_mode_release.png "release"){.inline} (veja o [editor de instrumentos](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)), o que torna desnecessário o loop silencioso curto descrito abaixo.
No entanto, esse modo não é documentado pelo formato sf2 e pode não ser suportado por outros reprodutores, enquanto o método descrito neste tutorial funciona com qualquer reprodutor.


## Preparação das amostras


Dois casos podem ocorrer: você tem um arquivo com a amostra completa, do ataque ao release, ou tem um arquivo adicional que contém apenas o release, já extraído.


### Amostra completa com release


Carregue a amostra duas vezes no Polyphone e, na segunda instância da amostra, remova tudo exceto o release.
A remoção pode ser feita mantendo a tecla :kbd:`Alt` pressionada e definindo uma área no [gráfico da amostra](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph).
Para encontrar o ponto em que o release começa, toque a amostra várias vezes e tente identificar o ponto em que, na sua opinião, mais nenhuma energia alimentava o som.
Essa última parte será o release.
Depois, siga as instruções abaixo.


### Amostra e release separados


Amostras para o exemplo: <a href="files/trumpet.wav" download>amostra 1</a>, <a href="files/trumpet-release.wav" download>amostra 2</a>.

Se você tem 2 amostras, a primeira contendo o início e a fase sustentada do som e a segunda contendo o release, basta garantir que o release comece com um silêncio muito curto (alguns pontos) próximo de 0.
Um loop curto e inaudível será feito com essa parte.

Afine e faça o loop da primeira amostra como explicado no tutorial “[como preparar uma amostra](tutorials/how-to-prepare-a-sample.md)”.

Faça o loop da segunda amostra em uma parte muito curta do início, por exemplo do ponto 1 ao ponto 2 ou 3.
Especifique a mesma nota base e a mesma correção da primeira amostra.


## Primeiro instrumento: ataque e fase sustentada


Crie um primeiro instrumento com as amostras que contêm o ataque e a fase sustentada do som.
Configure a divisão global da seguinte forma:

* selecione ![loop ligado](images/loop_on.png "loop ligado"){.inline} no parâmetro :guilabel:`Reprodução em loop`,
* especifique 0.01 s no parâmetro :guilabel:`Vol env release (s)` (release muito curto).

Você tem agora um instrumento sem release.


## Segundo instrumento: release


Crie um segundo instrumento para o release, com as amostras de release que têm um loop silencioso muito curto no início.
Configure-o da seguinte forma:

* selecione ![loop ligado e release](images/loop_on_end.png "loop ligado e release"){.inline} (loop ligado + release) no parâmetro :guilabel:`Reprodução em loop`,
* informe um número alto, como 100 segundos, no parâmetro :guilabel:`Vol env release (s)`, para que o release não seja atenuado,
* determine aproximadamente a duração do ataque na primeira amostra e use esse valor no parâmetro :guilabel:`Vol env attack (s)`.

Você tem agora um instrumento que permanece em silêncio enquanto você não solta uma tecla, e então se ouve a fase de release.
Você também pode notar que um toque muito rápido em uma tecla dispara o release de forma atenuada.
Isso é pouco perceptível aqui com o exemplo, pois o ataque de um trompete é muito rápido.
A duração do ataque foi ligeiramente aumentada no exemplo.


## Combinar os dois instrumentos


Crie uma predefinição com esses dois instrumentos.
O primeiro instrumento é ouvido quando uma tecla é pressionada, e o segundo instrumento é disparado quando a tecla é solta.
Você pode então ouvir o resultado e tentar suavizar a transição entre a fase sustentada e o release:

* atenuando o primeiro ou o segundo instrumento,
* aumentando a duração do release do primeiro instrumento.

O resultado com os arquivos de exemplo está <a href="files/custom%20release.sf2" download>aqui</a>.

Você pode ouvir aqui as diferenças entre:

* um <a href="files/classic-release.mp3" download>release clássico</a>

![](files/classic-release.mp3)

* e um <a href="files/custom-release.mp3" download>release personalizado</a>

![](files/custom-release.mp3)