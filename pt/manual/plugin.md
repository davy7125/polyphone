O Polyphone também existe como plugin para estações de trabalho de áudio digital (DAW) e para os outros programas hospedeiros de plugins de áudio.
O plugin toca soundfonts nos formatos sf2, sf3, sfz e dls com o sintetizador do Polyphone: uma soundfont soa da mesma forma em um hospedeiro e no editor.
Ele toca as soundfonts, mas não as edita: as alterações são feitas no Polyphone, e a soundfont é então aberta novamente no plugin.

O plugin está disponível nos formatos VST3, CLAP e LV2 no Linux, no Windows e no macOS, e também Audio Unit (AU) no macOS.


![Plugin do Polyphone](images/plugin.png "Plugin do Polyphone")


## Instalação {#doc_install}


O plugin é distribuído em um arquivo compactado para cada sistema operacional, disponível na seção “[Software](software)”.
Copie os formatos usados pelo seu hospedeiro nas pastas onde ele procura os plugins e, em seguida, faça uma nova varredura dos plugins no hospedeiro.

Linux
: :file:`Polyphone.vst3` em :file:`~/.vst3/`,\
  :file:`Polyphone.clap` em :file:`~/.clap/`,\
  :file:`Polyphone.lv2` em :file:`~/.lv2/`.

macOS
: :file:`Polyphone.vst3` em :file:`~/Library/Audio/Plug-Ins/VST3/`,\
  :file:`Polyphone.clap` em :file:`~/Library/Audio/Plug-Ins/CLAP/`,\
  :file:`Polyphone.component` em :file:`~/Library/Audio/Plug-Ins/Components/` (formato Audio Unit, usado pelo Logic Pro e pelo GarageBand),\
  :file:`Polyphone.lv2` em :file:`~/Library/Audio/Plug-Ins/LV2/`.

Windows
: :file:`Polyphone.vst3` em :file:`C:\Program Files\Common Files\VST3`,\
  :file:`Polyphone.clap` em :file:`C:\Program Files\Common Files\CLAP`,\
  :file:`Polyphone.lv2` em :file:`%APPDATA%\LV2`.

No macOS, os plugins não são notarizados: se o hospedeiro se recusar a carregá-los, remova o atributo de quarentena dos arquivos baixados, por exemplo:

```
xattr -dr com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/Polyphone.vst3
```


## Soundfont {#doc_soundfont}


O topo da janela mostra o nome da soundfont e o seu caminho.
Uma soundfont é aberta:

* com o botão :guilabel:`Abrir...`,
* com a seta à sua direita, que lista as 10 últimas soundfonts abertas,
* soltando um arquivo na janela.

Os arquivos sfz e dls são convertidos da mesma forma que o Polyphone os importa, sendo as amostras de um arquivo sfz os arquivos de áudio que ele referencia.
A soundfont inteira é lida quando é aberta, e as amostras de um arquivo sf3 são descompactadas: nada é lido no disco durante a reprodução.
Quando várias instâncias do plugin tocam o mesmo arquivo (uma por faixa, por exemplo), o arquivo é lido e mantido na memória uma única vez.
Se o arquivo tiver mudado, por exemplo depois de ser salvo novamente no Polyphone, ele é lido de novo.


## Modos de reprodução {#doc_modes}


Três botões acima da lista de predefinições escolhem como as predefinições são tocadas:

:guilabel:`Uma predefinição`
: A predefinição selecionada na lista é tocada nos 16 canais MIDI.
  As predefinições são exibidas com os seus números de banco e de predefinição, por exemplo “000:024 Guitar”.

:guilabel:`16 canais`
: Cada canal MIDI toca a sua própria predefinição, escolhida na sua linha.
  A opção :guilabel:`Mudança de programa` toca a predefinição escolhida pelas mensagens de seleção de banco e de mudança de programa recebidas pelo canal, como faz um sintetizador General MIDI: o banco 128 é então usado no canal 10, para a bateria.
  A predefinição tocada é exibida à direita de cada linha.
  Este modo é prático para tocar um arquivo MIDI arranjado para vários instrumentos.

:guilabel:`MPE`
: A predefinição selecionada é tocada por um controlador MPE (MIDI Polyphonic Expression): cada nota tem o seu próprio canal e, portanto, o seu próprio pitch bend, pressão e timbre (CC 74).
  Uma zona inferior de 15 canais é usada até que o controlador envie a sua configuração (RPN 6).
  O canal mestre de uma zona age sobre todas as suas notas: o seu pitch bend é somado ao de cada nota, e os seus controladores se aplicam a todas elas.
  A extensão do pitch bend é de 48 semitons para as notas e de 2 semitons para o canal mestre, e pode ser alterada pelo controlador (RPN 0).


![Plugin no modo “16 canais”](images/plugin_channels.png "Plugin no modo “16 canais”")


## Parâmetros {#doc_parameters}


Os parâmetros à direita são os do sintetizador do Polyphone, que podem ser encontrados nas suas configurações de [som](manual/settings.md#doc_sound) e de [teclado](manual/settings.md#doc_keyboard):

:guilabel:`Sintetizador`
: :guilabel:`Ganho` (de -50 a +50 dB), :guilabel:`Transposição` (de -24 a +24 semitons, sem transpor a bateria), :guilabel:`Afinação` (altura de referência do lá acima do dó central, de 400 a 480 Hz) e :guilabel:`Temperamento`, cujos ajustes finos são relativos à tonalidade escolhida abaixo da lista.

:guilabel:`Reverberação`
: :guilabel:`Nível`, :guilabel:`Tamanho`, :guilabel:`Largura` e :guilabel:`Amortecimento`.

:guilabel:`Chorus`
: :guilabel:`Nível`, :guilabel:`Profundidade` e :guilabel:`Frequência`.

Todos esses parâmetros podem ser automatizados pelo hospedeiro.


## Teclado e estado {#doc_keyboard}


O teclado na parte inferior da janela mostra as notas recebidas e pode ser tocado com o mouse.
Abaixo dele, são exibidos o número de vozes tocando e a parcela do processador usada pelo plugin.

O botão :guilabel:`Pânico` interrompe todos os sons e redefine os controladores, o que é útil se uma nota ficar presa.


## Configurações {#doc_settings}


O botão :guilabel:`Configurações` abre um menu com as seguintes opções:

:guilabel:`Polifonia`
: Número máximo de vozes, de 64 a 4096 (1024 por padrão).
  Acima desse número, as notas mais suaves desaparecem gradualmente.

:guilabel:`Teclado`
: Canal MIDI das notas tocadas com o teclado da janela.

:guilabel:`Idioma`
: Idioma do plugin, por padrão o do sistema.
  Essa escolha é compartilhada por todas as instâncias do plugin.
  Os nomes dos parâmetros e das saídas fornecidos ao hospedeiro são os do idioma usado quando o plugin é carregado (sempre inglês com o LV2).

:guilabel:`Salvar a soundfont no projeto`
: A soundfont é salva no projeto do hospedeiro, que pode então ser aberto em outro computador sem o arquivo.
  Isso não é possível com um arquivo sfz, cujas amostras são outros arquivos.

:guilabel:`Canais e efeitos em suas próprias saídas`
: Além da saída principal, as saídas “Canal 1” a “Canal 16” recebem as notas do seu canal sem efeito, e a saída “Efeitos” recebe a parte enviada à reverberação e ao chorus.
  Um canal cuja saída não está ativada pelo hospedeiro permanece na saída principal.
  Quando esta opção está desativada (padrão), tudo é mixado na saída principal.


## Uso em um projeto {#doc_project}


O projeto do hospedeiro salva os parâmetros, as predefinições tocadas, a polifonia e o caminho da soundfont (ou o seu conteúdo, se ela estiver armazenada no projeto).
Quando um projeto é aberto, a soundfont é lida antes que o restante seja restaurado, para que uma exportação offline possa começar imediatamente.

Uma nota começa exatamente na posição da sua mensagem MIDI, qualquer que seja o tamanho do buffer do hospedeiro.
Durante uma exportação offline, as vozes nunca são limitadas pela carga do processador.
