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

- acesso à internet,
- um navegador web,
- um arquivo MIDI, como <a href="files/ff7choco.mid" download>este</a>,
- opcionalmente, uma soundfont que você queira testar e que seja compatível com o padrão <a href="https://pt.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> (existe um filtro no [repositório](soundfonts/midi-standard/gm)),
- opcionalmente, a versão mais recente do Polyphone, disponível nesta [página](software).


## Ler um arquivo MIDI


Um método simples é usar o sintetizador <a href="https://spessasus.github.io/SpessaSynth/" target="_blank">SpessaSynth</a>, que é ao mesmo tempo um leitor de arquivos MIDI e um sintetizador de soundfonts. Ele tem a vantagem de estar disponível online e de não exigir nenhuma instalação especial.

Clique no botão :guilabel:`Load MIDI file`, selecione seu arquivo MIDI e pronto.

![O leitor MIDI e sintetizador SpessaSynth](images/SpessaSynth.jpg "O leitor MIDI e sintetizador SpessaSynth")


## Escolher uma soundfont


Para usar a soundfont de sua escolha, clique no botão :guilabel:`Upload soundfont` e selecione seu arquivo.


## Conectar a saída MIDI ao Polyphone


Como mencionado acima, o **SpessaSynth** é ao mesmo tempo um leitor de arquivos MIDI e um sintetizador. É possível usar apenas o recurso de leitor de arquivos MIDI e enviar os sinais ao Polyphone, que assume a síntese sonora. Isso pode ser útil se a soundfont for grande demais para ser enviada pela internet.

Para conectar o SpessaSynth ao Polyphone (método testado no Linux):

- inicie o Polyphone em [modo sintetizador](tutorials/use-polyphone-as-a-synthesizer.md) e abra as [configurações](manual/settings.md#doc_general),
- selecione “MIDI through” como entrada MIDI (memorize o nome),
- abra o SpessaSynth no navegador web e abra as configurações dele,
- selecione a saída MIDI “MIDI through” (mesmo nome da entrada MIDI do Polyphone).

Como um arquivo MIDI pode enviar mensagens para 16 canais, será preciso abrir a soundfont 16 vezes e especificar um canal diferente, de 1 a 16, em cada aba. Por praticidade, ao abrir o Polyphone, você pode especificar a soundfont a abrir e a opção “each” para abrir as 16 abas de uma só vez. O comando seria, por exemplo:

```
polyphone -s -i "/home/davy/Downloads/gm.sf2" -c "each|off|off"
```

Consulte esta [página](manual/annexes/command-line.md#doc_synth) para mais informações sobre as linhas de comando.
