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
Quando uma soundfont é aberta, sua estrutura é lida imediatamente: os instrumentos são listados e podem ser tocados.
Suas amostras são então lidas em segundo plano, com um indicador de carregamento antes do seu nome (sua dica mostra o progresso): uma nota cujas amostras ainda não foram lidas fica em silêncio.
As amostras das notas tocadas e dos instrumentos escolhidos são lidas primeiro, e as amostras de um arquivo sf3 são descompactadas.
Depois que tudo é lido, nada é lido no disco durante a reprodução; uma exportação offline espera até que todas as amostras sejam lidas.
Quando várias instâncias do plugin tocam o mesmo arquivo (uma por faixa, por exemplo), o arquivo é lido e mantido na memória uma única vez.
Se o arquivo tiver mudado, por exemplo depois de ser salvo novamente no Polyphone, ele é lido de novo.


## Modos de reprodução {#doc_modes}


Três botões acima da lista de instrumentos (as predefinições da soundfont) escolhem como os instrumentos são tocados:

:guilabel:`Um instrumento`
: O instrumento selecionado na lista é tocado nos 16 canais MIDI.
  Os instrumentos são exibidos com os seus números de banco e de programa, por exemplo “000:024 Guitar”.

:guilabel:`16 canais`
: Cada canal MIDI toca o seu próprio instrumento, escolhido na sua linha.
  A opção :guilabel:`Mudança de programa` toca o instrumento escolhido pelas mensagens de seleção de banco e de mudança de programa recebidas pelo canal, como faz um sintetizador General MIDI: o banco 128 é então usado no canal 10, para a bateria.
  O instrumento tocado é exibido à direita de cada linha.
  Este modo é prático para tocar um arquivo MIDI arranjado para vários instrumentos.

:guilabel:`MPE`
: O instrumento selecionado é tocado por um controlador MPE (MIDI Polyphonic Expression): cada nota tem o seu próprio canal e, portanto, o seu próprio pitch bend, pressão e timbre (CC 74).
  Uma zona inferior de 15 canais é usada até que o controlador envie a sua configuração (RPN 6).
  O canal mestre de uma zona age sobre todas as suas notas: o seu pitch bend é somado ao de cada nota, e os seus controladores se aplicam a todas elas.
  A extensão do pitch bend é de 48 semitons para as notas e de 2 semitons para o canal mestre, e pode ser alterada pelo controlador (RPN 0).


![Plugin no modo “16 canais”](images/plugin_channels.png "Plugin no modo “16 canais”")


## Parâmetros {#doc_parameters}


Os parâmetros são os do sintetizador do Polyphone, que podem ser encontrados nas suas configurações de [som](manual/settings.md#doc_sound) e de [teclado](manual/settings.md#doc_keyboard).
:guilabel:`Ganho` (de -24 a +24 dB) fica no cabeçalho; os outros parâmetros ficam agrupados à direita:

:guilabel:`Nota`
: O modo de execução, na lista da primeira linha (veja os [modos de execução](manual/plugin.md#doc_play_modes)): :guilabel:`Poli` (padrão), :guilabel:`Mono` ou :guilabel:`Legato`; o portamento do legato é ajustado à sua direita.
  :guilabel:`Tom de referência`: uma nota de lá#3 a lá4 e sua frequência (de 400 a 480 Hz para o lá4, 440 Hz por padrão); essa nota soa nessa frequência qualquer que seja o temperamento.
  :guilabel:`Transposição`: de -24 a +24 semitons, sem transpor a bateria.

:guilabel:`Velocidade`
: A resposta à velocidade das notas; cada ajuste é representado por um gráfico (velocidade de 0 a 127 da esquerda para a direita):
  
  * :guilabel:`Corte do filtro`: modulador padrão que abaixa a frequência de corte do filtro conforme a velocidade, até 2 oitavas, como nas configurações de [som](manual/settings.md#doc_sound) do Polyphone. A versão 2.04 do formato SoundFont (padrão) a abaixa tanto mais quanto menor for a velocidade. A versão 2.01 tinha um salto na velocidade 64, um erro corrigido pela versão 2.04: é um modo de compatibilidade para as soundfonts antigas feitas compensando esse salto. Com :guilabel:`Desativado`, a frequência de corte não depende da velocidade. Não tem efeito em uma soundfont que tenha seus próprios [moduladores padrão](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md#doc_default_mod).
  * :guilabel:`Curva`: curva aplicada à velocidade das notas recebidas, linear no meio (padrão, restaurada com um clique duplo), côncava para a esquerda (velocidade mais suave: as velocidades altas exigem um toque mais forte) e convexa para a direita (a velocidade sobe rapidamente).

:guilabel:`Temperamentos e escalas`
: Os temperamentos e escalas do Polyphone em suas categorias, e depois o temperamento personalizado, transpostos para a :guilabel:`Tonalidade` escolhida.
  O teclado à direita mostra o ajuste fino de cada nota em cents e é editado como nas [configurações do teclado](manual/settings.md#doc_keyboard) do Polyphone: roda do mouse ou arraste vertical, clique duplo para voltar a 0, menu de contexto para alterar o uso de uma nota ou colocar nela a tonalidade. Um temperamento editado passa a ser o temperamento personalizado.
  As notas não utilizadas por uma escala não são tocadas, a menos que :guilabel:`Tocar tudo` esteja marcado.

:guilabel:`Envelope`
: Attack, decay, sustain e release dos instrumentos, modificados (:guilabel:`Relativo`) ou substituídos (:guilabel:`Fixo`); veja o [envelope](manual/plugin.md#doc_envelope).

:guilabel:`Reverberação`
: :guilabel:`Nível`, :guilabel:`Tamanho`, :guilabel:`Largura` e :guilabel:`Amortecimento`.

:guilabel:`Chorus`
: :guilabel:`Nível`, :guilabel:`Profundidade` e :guilabel:`Frequência`.

Os grupos :guilabel:`Temperamentos e escalas`, :guilabel:`Envelope`, :guilabel:`Reverberação` e :guilabel:`Chorus` têm um interruptor à direita do título: desligado, o grupo fica recolhido e seu temperamento, envelope ou efeito não é aplicado (temperamento igual com todas as notas, envelopes da soundfont, nenhum efeito), e seus ajustes são mantidos.
A altura da janela acompanha os grupos expandidos.

O hospedeiro pode automatizar todos esses parâmetros, exceto a nota do tom de referência (a frequência do lá4 é que é automatizada) e os ajustes finos do temperamento personalizado, que são salvos com o projeto.


## Modos de execução {#doc_play_modes}


A lista da primeira linha do grupo :guilabel:`Nota` escolhe como as notas de cada canal MIDI são tocadas:

:guilabel:`Poli`
: As notas são tocadas juntas (padrão).

:guilabel:`Mono`
: Uma nota por vez: uma nota começa com seu ataque e a anterior para rapidamente.

:guilabel:`Legato`
: Uma nota por vez: uma nota tocada antes de soltar a anterior a continua, sem um novo ataque.
  Se uma divisão que toca a nota anterior também contém a nova tecla, sua voz continua com a mesma amostra, mudando apenas sua altura.
  As outras amostras da nova tecla começam no início do seu loop (no seu início se não tiverem loop), com um crossfade.
  A barra à direita da lista ajusta o portamento: :guilabel:`Nenhum` (padrão, a altura muda imediatamente) ou a duração do deslize da altura a partir da nota anterior, até 5 s.
  Uma nota tocada depois de soltar a anterior começa com seu ataque, como no modo :guilabel:`Mono`.

Entre parênteses fica a tecla ouvida quando várias teclas são mantidas: a última, a mais aguda ou a mais grave.
Quando a tecla ouvida é solta enquanto outras ainda estão mantidas, a nota da que tem a prioridade volta a ser ouvida: manter ré e repetir mi toca um trinado, deslizado em legato.
A bateria (banco 128) e o modo :guilabel:`MPE` continuam polifônicos.


## Envelope {#doc_envelope}


O grupo :guilabel:`Envelope` modifica o envelope de volume de todos os instrumentos tocados, exceto a bateria (banco 128), sem editar a soundfont.
Seu modo é escolhido à esquerda do interruptor, e cada modo mantém seus próprios valores:

:guilabel:`Relativo`
: O envelope de cada som da soundfont é modificado (padrão): :guilabel:`Attack`, :guilabel:`Decay` e :guilabel:`Release` multiplicam suas durações, de ×0,10 a ×10, e :guilabel:`Sustain` eleva ou reduz seu nível de sustain, de -20 a +20 dB (até o nível máximo).
  Cada som mantém a forma do seu envelope: um release que varia conforme as teclas ou as camadas de velocity continua variando.
  As durações do envelope de modulação (filtro e altura) mudam nas mesmas proporções, de modo que o timbre de um som cujo filtro se abre com o attack acompanha o novo attack.
  Um fator de ×1,00 e uma variação de 0 dB mantêm a soundfont.

:guilabel:`Fixo`
: Os valores substituem os da soundfont: :guilabel:`Attack` de 1 ms a 10 s, :guilabel:`Decay` e :guilabel:`Release` de 1 ms a 20 s, :guilabel:`Sustain` de -96 a 0 dB.
  Um botão girado totalmente para a esquerda (:guilabel:`Origem`) mantém o valor da soundfont, para substituir apenas o release, por exemplo.
  O envelope de modulação não é modificado.

O gráfico à direita desenha o envelope, com as durações comprimidas para que as etapas curtas e longas fiquem visíveis.
No modo :guilabel:`Relativo`, um exemplo de envelope de uma soundfont é desenhado tracejado sob o envelope tocado; no modo :guilabel:`Fixo`, as etapas que mantêm a soundfont são desenhadas tracejadas.

Uma amostra sem loop (um piano, por exemplo) para no seu fim, qualquer que seja o seu envelope: um decay mais longo ou um sustain mais alto não podem fazê-la durar mais.
Os moduladores da soundfont continuam sendo aplicados (uma velocity que encurta o attack, por exemplo).
As alterações se aplicam imediatamente às notas que estão tocando, de modo que o envelope pode ser automatizado: uma nota mantida assume o novo nível de sustain e é solta com o release ajustado naquele momento.
Um clique duplo em um botão restaura seu valor padrão (a soundfont).


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

:guilabel:`Latência fixa (notas tocadas em sua posição exata)`
: Quando esta opção está desativada (padrão), as mensagens MIDI de um bloco de áudio são adiantadas juntas para que a primeira nota do bloco comece no seu início: as notas tocadas ao vivo soam mais cedo, em média meia duração do buffer, mas a sua latência varia conforme a sua posição no buffer do hospedeiro.
  Quando ela está ativada, cada mensagem é processada na sua posição exata: a latência das notas tocadas ao vivo é constante.
  Durante uma exportação offline, as mensagens são sempre processadas na sua posição exata.

:guilabel:`Clipping em [-1; 1]`
: As saídas são limitadas ao intervalo [-1; 1] (padrão), como no Polyphone.
  Sem clipping, os valores maiores são passados ao hospedeiro, que os mixa em ponto flutuante: o ganho pode então ser aumentado sem saturação, ajustando o nível antes da saída do hospedeiro.

:guilabel:`Salvar a soundfont no projeto`
: A soundfont é salva no projeto do hospedeiro, que pode então ser aberto em outro computador sem o arquivo.
  Isso não é possível com um arquivo sfz, cujas amostras são outros arquivos.

:guilabel:`Canais e efeitos em suas próprias saídas`
: Além da saída principal, as saídas “Canal 1” a “Canal 16” recebem as notas do seu canal sem efeito, e a saída “Efeitos” recebe a parte enviada à reverberação e ao chorus.
  Um canal cuja saída não está ativada pelo hospedeiro permanece na saída principal.
  Quando esta opção está desativada (padrão), tudo é mixado na saída principal.


## Uso em um projeto {#doc_project}


O projeto do hospedeiro salva os parâmetros, os instrumentos tocados, a polifonia e o caminho da soundfont (ou o seu conteúdo, se ela estiver armazenada no projeto).
Quando um projeto é aberto, apenas a estrutura da soundfont é lida antes que o restante seja restaurado: o hospedeiro não espera pelas amostras, e uma exportação offline pode começar imediatamente (ela espera pelas amostras ainda não lidas).

Uma nota começa exatamente na posição da sua mensagem MIDI, qualquer que seja o tamanho do buffer do hospedeiro, durante uma exportação offline e em tempo real com uma [latência fixa](manual/plugin.md#doc_settings).
Durante uma exportação offline, as vozes nunca são limitadas pela carga do processador.
