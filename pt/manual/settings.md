As configurações do software são acessíveis pela [tela inicial](manual/index.md) ou pelo [menu](manual/menu.md#doc_shortcuts) (atalho :kbd:`Ctrl`+:kbd:`,`).
Elas são divididas em cinco seções:

* [Geral](#doc_general)
* [Interface](#doc_interface)
* [Som](#doc_sound)
* [Teclado](#doc_keyboard)
* [Repositório online](#doc_repository)

Todas as modificações são salvas imediatamente.


## Geral {#doc_general}


A seção :guilabel:`Geral` permite modificar os seguintes parâmetros:

Saída de áudio
: Especifica o sistema de áudio e o dispositivo usados para tocar os sons.
  Os sistemas disponíveis dependem do sistema operacional (por exemplo, WASAPI e ASIO no Windows, ALSA e JACK no Linux, Core Audio no macOS).
  Escolher “-” desliga a saída de áudio.

Tamanho do buffer
: Especifica o tamanho do buffer de áudio.
  Um buffer pequeno resulta em uma latência pequena, mas o som pode ficar com estalos.
  Com o valor :guilabel:`Padrão`, o tamanho depende do sistema de áudio.

Polifonia
: Número máximo de vozes tocadas ao mesmo tempo, de 64 a 4096 (1024 por padrão).
  Uma voz é uma amostra sendo tocada: uma nota pode usar várias vozes, duas com uma amostra estéreo, por exemplo.
  Acima desse número, algumas notas desaparecem gradualmente: primeiro as que foram soltas, depois as mais suaves.
  As notas que acabaram de começar, assim como a tecla pressionada mais aguda e a mais grave, são conservadas pelo maior tempo possível.

Carga do processador
: Parcela máxima do tempo do processador usada para calcular as vozes, de 20 a 100% (60% por padrão).
  Acima dessa parcela, algumas notas desaparecem gradualmente da mesma forma.
  Um valor alto permite tocar mais notas ao mesmo tempo, mas o som pode ficar com estalos.

Entrada MIDI
: Especifica a entrada MIDI que controla o sintetizador e o teclado virtual.

Além disso, é possível marcar as seguintes opções:

Edição estéreo: alterar amostra vinculada
: Ao editar um lado de uma amostra estéreo, o outro lado é automaticamente editado da mesma forma.
  O mesmo vale ao editar uma divisão estéreo de um instrumento: a divisão correspondente ao outro canal (com os mesmos alcances de notas e de velocidade) é editada da mesma forma.

Importação de amostra: cortar no fim do loop
: Ao importar uma amostra, os dados localizados após o fim do loop são removidos automaticamente (mesmo efeito da ferramenta “[Cortar no fim do loop](manual/soundfont-editor/tools/sample-tools.md#doc_trimloop)”).

Importação de amostra: remover silêncio inicial
: Ao importar uma amostra, qualquer área vazia localizada antes do início do ataque é removida automaticamente (mesmo efeito da ferramenta “[Remover silêncio inicial](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)”).


![Configurações, geral](images/settings_general.png "Configurações, geral")


## Interface {#doc_interface}


Esta seção mostra opções relacionadas à interface do software:

Idioma
: O idioma pode ser alterado aqui.

Nomes de notas
: As notas podem ser nomeadas pelo seu número (de acordo com a especificação MIDI) ou de acordo com o nome do dó central (C3, C4 ou C5).
  Os nomes das notas podem conter sustenidos ou bemóis.

Instância única
: Abre todos os arquivos em uma única janela.

Visualização inicial
: Seleciona a visualização a exibir quando uma soundfont é aberta, entre as seguintes: :guilabel:`informações gerais` (padrão), :guilabel:`lista de amostras`, :guilabel:`primeira amostra`, :guilabel:`lista de instrumentos`, :guilabel:`primeiro instrumento`, :guilabel:`lista de predefinições`, :guilabel:`primeira predefinição`.

Ordenar divisões
: A ordem das divisões em um instrumento ou em uma predefinição pode ser alterada aqui: :guilabel:`por alcance de notas`, :guilabel:`por alcance de velocidade`, :guilabel:`em ordem alfabética` ou :guilabel:`nenhum` (a ordem de criação).

Estilo
: O estilo dos elementos da interface.
  O estilo :guilabel:`Fusion` é o padrão, e outros podem ser oferecidos dependendo do sistema operacional (Adwaita, Windows, macOS…).

Cores
: Diferentes conjuntos de cores predefinidos estão disponíveis para personalizar a aparência do Polyphone.
  Também é possível escolher manualmente as cores do texto, da janela, dos botões, da seleção e das listas.
  As cores só podem ser alteradas com os estilos :guilabel:`Fusion` e :guilabel:`Windows`.

Bordas da janela
: Exibe ou oculta as bordas da janela, para economizar espaço na tela (não disponível no macOS).

Decorações
: As decorações de fundo de algumas listas podem ser desativadas para melhorar a legibilidade.

Uma modificação do idioma, dos nomes de notas, do estilo, das cores, das bordas da janela ou das decorações é aplicada na próxima inicialização do software.


![Configurações, interface](images/settings_interface.png "Configurações, interface")


## Som {#doc_sound}


A seção :guilabel:`Som` permite modificar a reverberação (nível, tamanho da sala, amortecimento e largura) e o chorus (nível, profundidade e frequência) do sintetizador, assim como seu volume geral.
O volume geral pode ser alterado com o controlador MIDI de volume se um teclado externo estiver conectado.

Como o modulador padrão que modifica a frequência de corte do filtro mudou entre as versões 2.01 e 2.04 das especificações sf2, é possível definir aqui qual versão usar:

* :guilabel:`SoundFont 2.01`: usa a regra “baixar a frequência do filtro levando em conta a velocidade SE a velocidade for menor que 64” (uma lacuna clara e problemática pode ser ouvida entre as velocidades 63 e 64);
* :guilabel:`SoundFont 2.04` (padrão): usa a regra “baixar a frequência do filtro levando em conta a velocidade” (sem lacuna);
* :guilabel:`desativado`: esse modulador padrão não é definido.


![Configurações, som](images/settings_sound.png "Configurações, som")


## Teclado {#doc_keyboard}


As teclas do teclado do computador usadas para controlar o [teclado virtual](manual/soundfont-editor/toolbar.md#doc_keyboard) são definidas na seção :guilabel:`Teclado`.
A altura pode ser aumentada ou diminuída oitava por oitava com o ajuste :guilabel:`Primeiro Dó`.

Para os músicos mais experientes, a afinação pode ser alterada aqui:

Tom de referência
: A frequência do lá localizado acima do dó central (nota 69), entre 390 e 470 Hz (440 Hz por padrão).

Temperamento
: Um temperamento diferente do temperamento igual pode ser aplicado ao tocar.
  Quando um temperamento é selecionado, a nota que serve de referência para esse temperamento (sua tônica) também pode ser escolhida.

O tom de referência e o temperamento também podem ser alterados com mensagens MIDI RPN (master fine tuning, tuning program select e tuning bank select).


![Configurações, teclado](images/settings_keyboard.png "Configurações, teclado")


## Repositório online {#doc_repository}


O endereço de e-mail e a senha de uma conta podem ser informados aqui para vincular o Polyphone a essa conta, sendo que uma [conta Premium](premium) libera todo o [repositório online](manual/online-repository.md).
Após uma conexão bem-sucedida, o Polyphone se conecta automaticamente à conta a cada inicialização, até que o botão :guilabel:`Sair` seja usado.

O diretório usado ao baixar soundfonts do [repositório online](manual/online-repository.md) também pode ser alterado aqui.


![Configurações, repositório](images/settings_repository.png "Configurações, repositório")



*[JACK]: JACK Audio Connection Kit
*[ASIO]: Audio Stream Input/Output
*[MIDI]: Musical Instrument Digital Interface