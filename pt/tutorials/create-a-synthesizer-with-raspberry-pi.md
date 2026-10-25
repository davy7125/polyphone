Um Raspberry Pi é um computador do tamanho de um cartão de crédito no qual o Polyphone pode ser instalado. Integrando-o a uma caixa com alto-falantes e associando-o a um teclado controlador, é possível criar um verdadeiro sintetizador!

![Sintetizador caseiro — frente](images/synth_front.jpg "Sintetizador caseiro — frente")

![Sintetizador caseiro — traseira](images/synth_back.jpg "Sintetizador caseiro — traseira")

## Material necessário

Esta lista pode ser adaptada ao seu projeto:

- um Raspberry Pi,
- um cartão micro SD com capacidade de 32 GB ou mais,
- uma fonte de alimentação para o Raspberry Pi,
- um conversor digital/analógico (DAC) para transformar uma saída USB em saída de áudio P2 (jack),
- alto-falantes de computador, que têm a vantagem de reunir os alto-falantes e a parte de amplificação do sinal,
- um interruptor geral liga/desliga,
- um cabo USB duplo macho-fêmea para embutir,
- uma caixa para colocar tudo dentro,
- e, claro, um teclado controlador para tocar.

Para configurar o Raspberry Pi com facilidade, também é recomendável ter:

- uma tela com um cabo micro-HDMI para a conexão com o Raspberry Pi,
- um teclado,
- um mouse.

No exemplo desta página, foram usados alto-falantes Edifier R1280T de segunda mão. Por um preço modesto, eles reproduzem os graves relativamente bem e, como você verá abaixo, será possível corrigir as deficiências da resposta em frequência.

O teclado controlador usado é um Akai MPK Mini Plus, conectável por USB. O adaptador USB / P2 é um UGREEN de entrada, e o Raspberry Pi é a versão 5.

(o objetivo desses detalhes não é fazer propaganda de nenhuma marca, mas apresentar uma combinação que funciona)

## Preparar a caixa

Este tutorial não vai se concentrar em uma maneira específica de fazer uma caixa. O uso de madeira valorizará suas habilidades de marcenaria, como neste exemplo, mas o papelão pode funcionar igualmente bem. Cabe a você expressar seus talentos criativos! No geral, você vai precisar de uma tampa para acessar os componentes e de aberturas para:

- os alto-falantes,
- o cabo de alimentação,
- o interruptor liga/desliga,
- o volume e — se necessário — os botões de equalização,
- os ressonadores de Helmholtz para o bass-reflex etc.

Uma ordem de instalação sugerida na caixa é a seguinte:

- comece embutindo o botão liga/desliga, os conectores USB no painel traseiro e quaisquer outros botões ligados aos alto-falantes,
- posicione o cabo de alimentação e conecte-o à entrada do botão liga/desliga,
- posicione o transformador associado aos alto-falantes e conecte-o à saída do botão liga/desliga,
- posicione a placa eletrônica dos alto-falantes e conecte-a ao transformador,
- fixe os alto-falantes e conecte-os à placa eletrônica,
- fixe o Raspberry Pi,
- conecte a fonte do Raspberry Pi à saída do botão liga/desliga (a saída do botão alimenta tanto os alto-falantes quanto o Raspberry Pi),
- conecte o cabo USB duplo ao Raspberry Pi,
- conecte o conversor USB/P2 a outra porta USB,
- conecte a placa eletrônica dos alto-falantes por meio de um cabo P2.

Na imagem a seguir, a fonte de alimentação e a placa eletrônica dos alto-falantes estão circuladas em laranja. O Raspberry Pi está circulado em azul.

![Interior do sintetizador](images/synth_inside.jpg "Interior do sintetizador")

Observação: os alto-falantes devem ser posicionados de modo que não haja nenhum retorno possível do som entre a saída do alto-falante e o interior da caixa; caso contrário, aparecerá uma ressonância. **Ou seja, não como neste exemplo!** Às vezes é preciso experimentar para descobrir as armadilhas…

![O Raspberry Pi](images/synth_raspberry_pi.jpg "O Raspberry Pi")

3 portas USB são usadas:

- uma para obter uma saída P2,
- as outras 2 são levadas para a traseira da caixa e servirão para conectar o teclado controlador ou, durante a configuração do Raspberry Pi, um teclado e um mouse.

## Preparar o Raspberry Pi

### Instalar o Debian

Um dos primeiros passos ao receber um Raspberry Pi é instalar o **Debian** em um cartão micro SD. É fácil encontrar instruções na internet, por exemplo <a href="https://www.raspberrypi.com/documentation/computers/getting-started.html#installing-the-operating-system" target="_blank">aqui</a>.

Depois de preparar o cartão micro SD, insira-o no slot do Raspberry Pi. Em seguida, conecte a fonte, o monitor, o mouse e o teclado, e seu computador estará pronto para uso!

### Instalar e configurar o Polyphone

Primeiro, atualize o sistema com:

```
sudo apt-get update
sudo apt-get upgrade
```

Em seguida, instale o **Polyphone** usando o pacote .deb disponível nesta [página](software). Conecte o teclado MIDI por USB ao Raspberry Pi (desconecte o teclado do computador se não houver porta USB livre), depois abra o Polyphone e selecione o teclado MIDI na lista suspensa das entradas MIDI nas [configurações](manual/settings.md#doc_general). Baixando uma soundfont, por exemplo do [repositório](soundfonts), já é possível tocar um instrumento com o teclado MIDI associado ao Raspberry Pi!

### Iniciar o Polyphone automaticamente

Esta etapa é necessária para dispensar a tela. O Polyphone pode ser iniciado automaticamente quando o Raspberry Pi liga, criando um arquivo :file:`synthesizer.desktop` com este texto:

```
[Desktop Entry]
Name=Synthesizer
Exec=polyphone -s /home/pi/Desktop/my-soundfont.sf2
Terminal=false
Type=Application
StartupNotify=true
Icon=/home/pi/Desktop/my-icon.png
Categories=AudioVideo;
```

Esse arquivo deve então ser colocado na pasta :file:`/home/pi/.config/autostart`.

A linha de comando pode ser completada com opções para escutar um canal MIDI específico, tocar vários instrumentos simultaneamente ou ativar / desativar instrumentos por meio de teclas. Esta [página](tutorials/use-polyphone-as-a-synthesizer.md) detalha as opções de uso do Polyphone em modo sintetizador.

## Para ir mais longe

### Equalização do som

Cada alto-falante, por causa de sua morfologia, responde de forma mais ou menos diferente conforme as frequências que deve emitir. Para obter uma resposta em frequência plana e assim reproduzir o som com fidelidade, é possível criar uma equalização que compense as deficiências do alto-falante. A ferramenta <a href="https://github.com/erdewit/HiFiScan" target="_blank">HiFiScan</a> calcula automaticamente os parâmetros de equalização, e o protocolo é descrito na página do projeto.

Depois de calculada a curva de equalização, o **HiFiScan** pode exportar coeficientes para integração em equalizadores. Aqui nos interessa a equalização de 12 bandas, que pode ser usada com o **Calf**.

### Conexão remota

Até aqui, a configuração do Raspberry Pi exigiu uma tela para exibir a área de trabalho. No entanto, é possível configurar o sistema para permitir o acesso remoto pelo **protocolo VNC**, e há muitos recursos na internet para configurá-lo.
