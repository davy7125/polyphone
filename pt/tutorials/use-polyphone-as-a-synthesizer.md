O Polyphone não é apenas um editor de soundfonts, é também um sintetizador completo que permite tocar com um teclado MIDI.
Este tutorial mostra como iniciar o Polyphone em modo sintetizador.


## Pré-requisitos

Primeiro, você precisa de um **teclado controlador** ou de outro controlador MIDI, conectado ao computador por meio de um **adaptador MIDI-USB**.

Baixe a versão mais recente do Polyphone [aqui](software), depois instale e abra o programa. Nas [configurações](manual/settings.md#doc_general), você deve encontrar seu dispositivo MIDI entre as entradas MIDI. Se não for o caso, é provável que o driver do adaptador MIDI não esteja instalado no computador (resolva isso antes de continuar).

Ainda nas configurações, verifique se a saída de áudio está correta.

## Abrir o Polyphone em modo sintetizador


### Windows

Crie um atalho para :file:`polyphone.exe` com o argumento :command:`-s`, como explicado na seção [Opções de inicialização do Polyphone](#doc_launch) abaixo.

### Linux

Abra um terminal e digite a seguinte linha de comando:

```
polyphone -s
```

### Mac

Abra um terminal e digite a seguinte linha de comando:

```
/Applications/Polyphone.app/Contents/MacOS/Polyphone -s
```

**Observação:** se você moveu o aplicativo Polyphone para uma pasta diferente de :file:`Applications`, será preciso adaptar o caminho acima.


## Tocar um instrumento


Para tocar um instrumento, basta abrir uma soundfont, que você pode baixar do [repositório](soundfonts), por exemplo, e selecionar um instrumento contido nessa soundfont. Os instrumentos são organizados por banco: selecione um banco para exibir a lista de instrumentos abaixo.

![Polyphone aberto com a opção do modo sintetizador](images/synth_mode.png "Polyphone aberto com a opção do modo sintetizador")

Depois de selecionar um instrumento, seu alcance de notas é exibido no teclado virtual, e você pode tocá-lo com as teclas correspondentes do seu controlador MIDI.
O botão :guilabel:`Pânico MIDI` interrompe todos os sons que estão tocando (útil se uma nota ficar presa), e o botão :guilabel:`Gravador` abre o [gravador](manual/soundfont-editor/toolbar.md#doc_recorder).


## Opções


### Tocar vários instrumentos ao mesmo tempo

Para tocar vários instrumentos quando uma tecla é pressionada no teclado, basta definir a opção :guilabel:`Seleção múltipla` como :guilabel:`ligado`. Vários instrumentos podem então ser selecionados na lista de instrumentos.

### Tocar vários instrumentos separadamente

Se você tiver vários controladores MIDI conectados ao computador, pode abrir várias soundfonts e escolher um canal específico para cada soundfont aberta. Assim, por exemplo, um primeiro teclado será usado para tocar um instrumento, e um segundo teclado será usado para tocar outro. A mesma soundfont pode ser aberta várias vezes para tocar vários de seus instrumentos simultaneamente.

A atribuição dos canais é feita alterando o valor de :guilabel:`Canal`. Por exemplo, a primeira aba aberta escutará o canal 1, enquanto a segunda escutará o canal 2.
O valor :guilabel:`todos` (padrão) escuta todos os canais, e o valor :guilabel:`detectar...` seleciona o canal da próxima tecla pressionada no seu teclado.

![Configuração do sintetizador com 2 canais](images/synth_2_channels.png "Configuração do sintetizador com 2 canais")

**Observação:** alguns teclados podem ser divididos em duas partes, de modo que um único teclado possa enviar notas para 2 canais diferentes.

### Trocar de instrumento com uma tecla

A troca de instrumento geralmente é feita por uma mensagem MIDI do tipo “Program change”, e o Polyphone é capaz de lê-las.

No entanto, outro mecanismo pode ser usado: teclas programadas para enviar mensagens “Note ON” / “Note OFF”, com o número da nota correspondente ao número do instrumento. Esse mecanismo pode ser útil quando a soundfont tem apenas alguns instrumentos e quando as teclas podem ser programadas para enviar notas específicas (por exemplo, os pads de um teclado controlador).

Se a opção :guilabel:`Seleção por notas` estiver definida como :guilabel:`ligado`:

* um instrumento é ativado quando uma mensagem MIDI “Note ON” é recebida,
* um instrumento é desativado quando uma mensagem MIDI “Note OFF” é recebida.

Essa opção pode ser usada quando o pad de um teclado controlador está configurado como alternância (um toque ativa a nota, outro toque a desativa).

Se :guilabel:`alternar` estiver selecionado:

* um instrumento é ativado quando uma mensagem MIDI “Note ON” é recebida,
* um instrumento é desativado quando outra mensagem MIDI “Note ON” é recebida.

Desta vez, a alternância ocorre no nível do Polyphone.


## Opções de inicialização do Polyphone {#doc_launch}


Todas as opções vistas acima podem ser chamadas ao abrir o Polyphone, adicionando parâmetros à linha de comando. Também é possível especificar a soundfont a abrir.

No **Linux** ou no **macOS**, basta completar a linha de comando. No **Windows**, crie um atalho clicando com o botão direito no arquivo executável :file:`polyphone.exe` e selecione :guilabel:`Enviar para` → :guilabel:`Área de trabalho (criar atalho)` (em :guilabel:`Mostrar mais opções` do menu de contexto no Windows 11). Depois, exibindo as propriedades desse atalho, é possível adicionar parâmetros ao destino, que poderia ser:

```
"C:\Program Files\Polyphone\polyphone.exe" -s -c "all|on|toggle"
```

As diferentes opções são descritas nesta [página](manual/annexes/command-line.md#doc_synth).


## Usos do sintetizador


Agora que você sabe como transformar o Polyphone em sintetizador, por que não [usar um Raspberry Pi para criar um instrumento musical](tutorials/create-a-synthesizer-with-raspberry-pi.md)?

O modo sintetizador do Polyphone também pode ser usado para [ler arquivos MIDI](tutorials/read-midi-file-with-soundfont.md).
