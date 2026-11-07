Os arquivos MIDI contêm mensagens MIDI que, juntas, formam uma melodia. Eles podem ser combinados com um sintetizador que usa uma soundfont, e este tutorial mostra como usar um arquivo MIDI para testar o resultado sonoro de uma soundfont.


## Um pouco de contexto


<div class="too_complicated">
<p>Quando se toca em um teclado de sintetizador, são emitidos sinais <a href="https://pt.wikipedia.org/wiki/MIDI" target="_blank">MIDI</a>, que contêm informações como as notas tocadas e sua velocidade (termo que indica a força com que a tecla é pressionada). Um arquivo MIDI, em termos simples, contém um registro cronológico dos sinais emitidos por um teclado assim. Até aqui, nenhum som foi criado.</p>

<p>Para que um som seja emitido pelo alto-falante, é necessária uma etapa de conversão, e é aqui que entra o sintetizador: ele dispara sons a partir dos sinais MIDI recebidos. Existem vários tipos de sintetizador, e aqui vamos falar dos sintetizadores baseados em amostras, mais precisamente dos sintetizadores que usam bancos de sons no formato SoundFont.</p>

<p>Um arquivo MIDI pode controlar até 16 canais diferentes, o que permite usar 16 instrumentos diferentes em paralelo. Além de indicar quais notas tocar, o arquivo MIDI contém instruções que dizem ao sintetizador para usar o instrumento X do banco Y para as próximas notas do canal Z. Para garantir uma seleção correta dos instrumentos, o banco de sons usado pelo sintetizador deve ser compatível com o padrão <a href="https://pt.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a>, que atribui um instrumento específico a cada número X e Y. Por exemplo, o instrumento <strong>001</strong> deve corresponder a um piano na soundfont.</p>

<p>Recapitulando, vamos falar aqui da leitura de um arquivo MIDI que controla um sintetizador, o qual dispara sons contidos em uma soundfont compatível com o padrão General MIDI. O que vem a seguir é mais simples!</p>
</div>


## Pré-requisitos


Para tocar um arquivo MIDI com o método descrito abaixo, você vai precisar de:

- a versão mais recente do Polyphone, disponível nesta [página](software),
- um arquivo MIDI, como <a href="files/ff7choco.mid" download>este</a>,
- uma soundfont que você queira testar e que seja compatível com o padrão <a href="https://pt.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> (existe um filtro no [repositório](soundfonts/midi-standard/gm)).

Uma soundfont que não seja compatível com esse padrão também pode ser testada, [escolhendo você mesmo as suas predefinições](#doc_presets).


## Ler um arquivo MIDI


Abra a sua soundfont no Polyphone e clique no ícone ![MIDI](images/toolbar_midi.png "MIDI"){.inline} à direita da [barra de ferramentas](manual/soundfont-editor/toolbar.md) para exibir o [reprodutor MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player).

Clique no botão da pasta, no canto superior esquerdo da janela, selecione o seu arquivo MIDI e clique no botão de reprodução (ou pressione :kbd:`Space`), e pronto.

![O reprodutor MIDI do Polyphone](images/midi_player.png "O reprodutor MIDI do Polyphone")

Cada canal do arquivo que contém notas tem a sua própria linha na tabela. A coluna :guilabel:`Predefinição` mostra a predefinição pedida pelo arquivo, que pode mudar ao longo da música, e o LED da coluna :guilabel:`Volume` acende cada vez que uma nota começa. O controle deslizante abaixo do nome do arquivo permite ir a outro trecho da música.

O reprodutor MIDI também está disponível quando o Polyphone é aberto no [modo sintetizador](tutorials/use-polyphone-as-a-synthesizer.md).


## Testar a soundfont


A soundfont é tocada como está no editor, incluindo as alterações não salvas: você pode editar um instrumento enquanto a música toca e ouvir o resultado imediatamente, por exemplo ao ajustar o seu volume, o seu envelope ou o seu filtro.

O reprodutor também ajuda a ouvir os detalhes:

- os botões :guilabel:`S` (solo) e :guilabel:`M` (silenciar) de um canal isolam um instrumento ou o retiram da mixagem, e o controle deslizante da coluna :guilabel:`Volume` o equilibra com os outros;
- um ícone de aviso na coluna :guilabel:`Predefinição` significa que a soundfont não tem a predefinição pedida pelo arquivo: as notas desse canal não são tocadas, o que revela uma soundfont que não é totalmente compatível com o padrão General MIDI;
- o botão MIDI de um canal lista os controladores usados pelo arquivo naquele momento (roda de modulação, expressão, reverberação…), o que é útil para verificar os [moduladores](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) da soundfont;
- as [configurações de reprodução](manual/soundfont-editor/toolbar.md#doc_midi_player_settings), à direita da faixa superior, permitem tocar a música em loop, desacelerá-la para ouvir um trecho rápido ou transpô-la para testar outras notas.


## Escolher as soundfonts e as predefinições {#doc_presets}


Cada canal pode tocar uma soundfont diferente: abra várias soundfonts no Polyphone e dê um clique duplo em uma célula da coluna :guilabel:`Soundfont` para escolher a que o canal toca. Você pode, por exemplo, comparar os pianos de duas soundfonts na mesma parte, ou manter uma soundfont General MIDI para o acompanhamento enquanto a sua própria soundfont toca a melodia.

Por padrão, os canais estão definidos como :guilabel:`Seleção`: eles tocam a soundfont da aba atual, ou o arquivo selecionado em um [explorador de soundfonts](manual/soundfont-explorer.md#doc_play). Para comparar todas as soundfonts de um diretório, abra-o no Polyphone e selecione seus arquivos um após o outro enquanto a música toca: cada soundfont é carregada em segundo plano e ouvida assim que estiver pronta, sem abrir uma aba.

![Escolher a soundfont de um canal](images/midi_player_soundfonts.png "Escolher a soundfont de um canal")

Da mesma forma, um clique duplo em uma célula da coluna :guilabel:`Predefinição` impõe uma predefinição da soundfont no lugar da pedida pelo arquivo. É assim que se testa uma soundfont que não é compatível com o padrão General MIDI, por exemplo uma soundfont com um único instrumento: escolha a sua predefinição para o canal da melodia, e as mudanças de programa do arquivo passam a ser ignoradas nesse canal.


## Exportar o resultado


Quando o resultado lhe agradar, :guilabel:`Exportar áudio...` nas configurações de reprodução grava a música em um arquivo wav tal como é ouvida no reprodutor, com as mesmas soundfonts, predefinições, volumes, tempo e tom.
Um arquivo MIDI também pode ser convertido em arquivo wav pela [linha de comando](manual/annexes/command-line.md#doc_render), por exemplo em um script.
