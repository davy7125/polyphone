Muitas soundfonts tocam uma única amostra por nota: qualquer que seja a força com que uma tecla é tocada, ouve-se a mesma gravação, apenas mais forte ou mais fraca.
Os instrumentos acústicos não se comportam assim.
Uma tecla de piano tocada suavemente produz um som redondo e aveludado, enquanto a mesma tecla tocada com força produz um som mais brilhante, mais rico em harmônicos, com um ataque mais percussivo.

As camadas de velocidade trazem essa riqueza para uma soundfont: cada nota é gravada em várias dinâmicas, e a gravação tocada é escolhida de acordo com a velocidade da nota.
Este tutorial usa o piano como exemplo, mas o método se aplica a todo instrumento cujo timbre muda com a força da execução: violões, baterias, metais, cordas…
Ele termina com outros detalhes que tornam um instrumento ainda mais realista.


## Por que uma única amostra não basta {#doc_why}


Ouça a tecla Dó4 tocada quatro vezes, do suave ao forte (velocidades 30, 60, 90 e 120):

* com uma <a href="files/velocity-layers-one-layer.mp3" download>única amostra</a>, gravada em uma dinâmica média,

![](files/velocity-layers-one-layer.mp3)

* com <a href="files/velocity-layers-four-layers.mp3" download>quatro camadas</a>, a gravação sendo escolhida de acordo com a velocidade.

![](files/velocity-layers-four-layers.mp3)

Com uma única amostra, as notas suaves soam como notas fortes ouvidas de longe, e falta brilho à nota mais forte.
O sintetizador abaixa levemente a frequência de corte de um filtro para as notas suaves (é um dos moduladores padrão), mas isso está longe da mudança de timbre de um piano de verdade.
Com quatro camadas, o timbre acompanha a força da execução.

A diferença também é evidente em um contexto musical, aqui com arpejos cada vez mais fortes:

* <a href="files/velocity-layers-phrase-one-layer.mp3" download>única amostra</a>,

![](files/velocity-layers-phrase-one-layer.mp3)

* <a href="files/velocity-layers-phrase-four-layers.mp3" download>quatro camadas</a>.

![](files/velocity-layers-phrase-four-layers.mp3)


## Preparar as amostras {#doc_samples}


### Gravar as camadas


Cada nota deve ser gravada em várias dinâmicas, por exemplo *pp* (pianissimo), *mp* (mezzo-piano), *mf* (mezzo-forte) e *ff* (fortissimo).
Com 3 ou 4 camadas a melhoria já é evidente, enquanto as melhores bibliotecas de piano usam de 8 a 16 camadas para transições mais suaves.
Todas as gravações devem ser feitas nas mesmas condições (microfones, distância, ganho): só a força da execução deve mudar.

O exemplo deste tutorial usa 4 camadas, com uma amostra a cada 3 teclas em toda a extensão do piano.


### Importação


Dê aos arquivos um nome com a nota e a dinâmica, por exemplo :file:`C4 pp.wav`, :file:`C4 mp.wav`, :file:`C4 mf.wav` e :file:`C4 ff.wav`.
Quando a nota base não está gravada em um arquivo de áudio, o Polyphone a encontra no nome do arquivo se ele contiver o nome da nota em notação inglesa (C4 para o Dó4, tecla 60) ou o número da tecla.
A dinâmica escrita no nome será útil na próxima etapa.

Importe todos os arquivos com :guilabel:`Adicionar uma amostra` na [barra de ferramentas](manual/soundfont-editor/toolbar.md#doc_edit), ou [arrastando e soltando](manual/soundfont-editor/tree.md#doc_dragdrop) os arquivos na árvore.
Um arquivo estéreo gera duas amostras, cujos nomes terminam com “L” e “R”.
Depois verifique a afinação de cada amostra como explicado no tutorial “[Como preparar uma amostra](tutorials/how-to-prepare-a-sample.md#doc_tuning)”.
As amostras de piano não precisam de loop: o som se extingue naturalmente.


## Um instrumento por camada {#doc_instruments}


Cada camada se torna um instrumento que contém as amostras de uma única dinâmica.
Para cada dinâmica:

1. digite o nome dela (“pp”, por exemplo) na [barra de pesquisa](manual/soundfont-editor/tree.md#doc_search) da árvore, para que apenas as amostras dessa camada sejam exibidas,
2. selecione todas essas amostras: clique na primeira e depois na última mantendo a tecla :kbd:`Shift` pressionada,
3. clique em :guilabel:`Adicionar um instrumento` na [barra de ferramentas](manual/soundfont-editor/toolbar.md#doc_edit), dê um nome ao instrumento (“Piano pp”, por exemplo) e deixe marcada a opção :guilabel:`Vincular amostras selecionadas`.

As amostras estéreo são automaticamente posicionadas à esquerda e à direita, e os alcances de notas são distribuídos pelo teclado de acordo com as notas base (veja a ferramenta “[Posicionamento automático das amostras](manual/soundfont-editor/tools/instrument-tools.md#doc_position)”).
Cada instrumento já pode ser tocado sozinho com o [teclado virtual](manual/soundfont-editor/toolbar.md#doc_keyboard).

Os parâmetros comuns a todas as camadas podem ser editados de uma só vez: quando os quatro instrumentos estão selecionados na árvore, a [tabela](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) mostra as divisões globais deles lado a lado, e um valor digitado depois de selecionar as quatro células de uma linha é escrito em todas elas.
No exemplo, o parâmetro :guilabel:`Vol env release (s)` vale 1 s para que o som não pare bruscamente quando uma tecla é solta.


## Empilhar as camadas em uma predefinição {#doc_preset}


Selecione os quatro instrumentos na árvore e clique em :guilabel:`Adicionar uma predefinição`, com a opção :guilabel:`Vincular instrumentos selecionados` marcada.
A predefinição passa a conter quatro divisões, todas cobrindo o teclado inteiro.
As camadas são empilhadas dando a cada divisão da predefinição um :guilabel:`Alcance de velocidade` diferente no [editor de predefinições](manual/soundfont-editor/editing-pages/preset-editor.md):

* 0-39 para a camada *pp*,
* 40-69 para a camada *mp*,
* 70-99 para a camada *mf*,
* 100-127 para a camada *ff*.

Os alcances devem cobrir todas as velocidades de 0 a 127, sem lacunas (algumas velocidades ficariam mudas) nem sobreposições (duas camadas seriam ouvidas ao mesmo tempo).
Os limites não precisam ser regulares: eles podem ser deslocados de acordo com as gravações.
Os valores da linha :guilabel:`Atenuação (dB)` são explicados na próxima seção.


![Camadas de velocidade na tabela da predefinição](images/velocity_layers_table.png "Camadas de velocidade na tabela da predefinição")


No [editor de alcances](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range), cada camada é uma faixa horizontal: a divisão selecionada (aqui *mp*) aparece hachurada.
Essa visualização mostra rapidamente uma lacuna ou uma sobreposição.


![Camadas de velocidade no editor de alcances](images/velocity_layers_ranges.png "Camadas de velocidade no editor de alcances")


**Observação:** os alcances de velocidade também podem ser definidos em um único instrumento contendo todas as amostras, cada divisão tendo seu próprio alcance de velocidade.
Nesse caso, a escolha :guilabel:`por alcance de velocidade` da opção :guilabel:`Ordenar divisões` nas [configurações](manual/settings.md#doc_interface) mantém juntas as divisões de uma mesma camada.


## Equilibrar as camadas {#doc_balance}


Dentro de uma camada, o volume já muda com a velocidade: isso é feito por outro modulador padrão, definido pelo formato sf2.
Mas, em cada limite, o volume aumenta de repente, porque as gravações da camada superior são mais fortes: cerca de 5 a 7 dB no exemplo.
O objetivo é que apenas o timbre mude ao passar de uma camada para a seguinte, com o volume aumentando de forma gradual.

Use o [teclado virtual](manual/soundfont-editor/toolbar.md#doc_keyboard) para comparar os dois lados de um limite: a velocidade de uma nota depende da posição vertical do clique, e as setas :kbd:`↑` e :kbd:`↓` tocam de novo a última tecla com uma velocidade aumentada ou diminuída em 1.
Em seguida, para cada limite, de baixo para cima:

1. toque uma tecla com a última velocidade de uma camada (39, por exemplo) e depois com a primeira velocidade da camada seguinte (40),
2. aumente a atenuação da camada superior na tabela da predefinição até que as duas notas tenham mais ou menos o mesmo volume,
3. passe para o limite seguinte: as atenuações se somam, cada camada ficando pelo menos tão atenuada quanto a camada de baixo.

No exemplo, as atenuações são de 0, 6, 11 e 16 dB, como mostra a tabela acima.
Verifique várias teclas ao longo do teclado: se um registro continuar desequilibrado, a atenuação das divisões em questão pode ser refinada no instrumento da camada, por exemplo com a ferramenta “[Configuração baseada em notas](manual/soundfont-editor/tools/instrument-tools.md#doc_global)”.

A faixa dinâmica global do instrumento, da nota mais suave à mais forte, é então dada pelo modulador padrão.
Se as notas suaves estiverem fracas demais ou não o suficiente, esse modulador pode ser alterado com a ferramenta “[Substituir um modulador padrão](manual/soundfont-editor/tools/instrument-tools.md#doc_default_mod)”.

Por fim, nada melhor do que uma música de verdade para testar o resultado: toque a soundfont com um teclado MIDI ou com o [reprodutor MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player) (veja o tutorial “[Ler um arquivo MIDI com uma soundfont](tutorials/read-midi-file-with-soundfont.md)”).


## Mais detalhes para mais realismo {#doc_details}


As camadas de velocidade são um grande passo, mas um instrumento real tem outras características que uma soundfont pode reproduzir.
Aqui estão duas delas para o piano.


### Ruídos ao soltar as teclas


Quando uma tecla é solta, o abafador cai de volta sobre as cordas e o mecanismo faz um pequeno ruído.
Algumas bibliotecas de amostras fornecem esses ruídos: a usada no exemplo tem um por tecla.

Eles são colocados em um quinto instrumento, “Piano release”, adicionado à predefinição em todo o alcance de velocidade:

* selecione o modo de reprodução ![release](images/sample_mode_release.png "release"){.inline} (release) na linha :guilabel:`Reprodução em loop`, para que uma amostra seja tocada quando a tecla é solta,
* use um valor longo de :guilabel:`Vol env release (s)` (5 s no exemplo) para que o ruído seja tocado por inteiro,
* atenue bastante o instrumento (40 dB no exemplo): o ruído deve permanecer discreto.

Graças ao modulador padrão que liga a velocidade ao volume, o ruído é mais forte quando a nota foi tocada com mais força.

**Observação:** o modo de reprodução “release” não é documentado pelo formato sf2 e pode não ser suportado por outros reprodutores.
O tutorial “[Usar releases personalizados em um instrumento](tutorials/using-custom-releases-in-an-instrument.md)” descreve um método que funciona com qualquer reprodutor.


### Cordas sem abafadores


Em um piano, as cordas mais agudas não têm abafadores (aproximadamente a última oitava e meia): elas continuam soando depois que a tecla é solta.
Em cada instrumento de camada, as divisões dessas teclas (a partir do Fá6, tecla 89, no exemplo) recebem, portanto, um valor mais longo de :guilabel:`Vol env release (s)`: 3 s em vez de 1 s.

Ouça notas curtas no meio do teclado, depois na oitava mais aguda e, por fim, um acorde nos graves:

* <a href="files/velocity-layers-without-details.mp3" download>sem esses detalhes</a>,

![](files/velocity-layers-without-details.mp3)

* <a href="files/velocity-layers-with-details.mp3" download>com os ruídos ao soltar as teclas e as cordas sem abafadores</a>.

![](files/velocity-layers-with-details.mp3)


### Outras ideias


Vale a pena experimentar outras melhorias:

* gravar todas as teclas em vez de uma a cada três: em um piano, cada tecla tem suas próprias cordas, e uma amostra transposta nunca é exatamente a nota vizinha,
* usar gravações estéreo, que dão amplitude ao som,
* calcular tempos de release que mudam ao longo do teclado com a ferramenta “[Release natural](manual/soundfont-editor/tools/instrument-tools.md#doc_release)”,
* ajustar o volume ou o filtro por registro com a ferramenta “[Configuração baseada em notas](manual/soundfont-editor/tools/instrument-tools.md#doc_global)”.

E a mesma abordagem funciona para outros instrumentos: uma corda de violão tocada suavemente ou com força, uma caixa entre ghost notes e acentos, um trompete que passa de um som redondo para um som metálico…
Cada detalhe tirado do instrumento real deixa a soundfont mais viva.
