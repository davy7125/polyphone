O menu, localizado no canto superior direito do software, contém vários itens que dão acesso a diferentes recursos.


![Menu](images/menu.png "Menu")


## Abrir um editor de soundfonts {#doc_open}


Novo
: Cria uma nova soundfont vazia, em que tudo precisa ser feito do zero.
  Também acessível com o atalho :kbd:`Ctrl`+:kbd:`N`.

Abrir arquivo…
: Abre um arquivo [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [.sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) ou [.organ](manual/annexes/the-different-soundfont-formats.md#doc_organ).
  Também acessível com o atalho :kbd:`Ctrl`+:kbd:`O`.


## Salvar uma soundfont {#doc_save}


Salvar
: Salva o arquivo atual.
  Também acessível com o atalho :kbd:`Ctrl`+:kbd:`S`.

Salvar como…
: Salva o arquivo atual em outro arquivo.
  Também acessível com o atalho :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`S`.

Exportar soundfonts…
: Mescla e exporta uma ou várias predefinições das soundfonts abertas no formato [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3) ou [.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz). O formato adicional .csv exporta todos os parâmetros para uma análise detalhada do conteúdo da soundfont.
  Também acessível com o atalho :kbd:`Ctrl`+:kbd:`E`.


![Janela de exportação](images/export.png "Janela de exportação")


Na janela de exportação, as predefinições de todas as soundfonts abertas são listadas, e as que devem ser exportadas precisam ser marcadas (os botões :guilabel:`Selecionar tudo` e :guilabel:`Desmarcar tudo` ajudam a alterar rapidamente a seleção).
Os outros parâmetros são:

Destino
: O diretório em que os arquivos são gravados.
  Um arquivo nunca é sobrescrito: se já existir um arquivo com o mesmo nome, um número é acrescentado ao fim do nome do novo arquivo.

Formato
: Um entre :guilabel:`.sf2`, :guilabel:`.sf3`, :guilabel:`.sfz` ou :guilabel:`.csv`.
  Se as predefinições vierem de várias soundfonts, elas são mescladas em uma única soundfont, na qual cada soundfont de origem se torna um banco.
  É por isso que no máximo 127 soundfonts, cada uma com no máximo 127 predefinições selecionadas, podem ser exportadas ao mesmo tempo.

Qualidade
: Somente para o formato .sf3, qualidade da compressão das amostras.

Um arquivo por predefinição
: Somente para os formatos .sf2 e .sf3, criação de um arquivo por predefinição selecionada em vez de um único arquivo.

Uma pasta por banco, número de predefinição como prefixo, Classificação General MIDI
: Somente para o formato .sfz. A exportação cria um diretório contendo os arquivos sfz e um subdiretório :file:`samples`.
  Os arquivos sfz podem ser colocados em um diretório por banco, seu nome pode receber como prefixo o número da predefinição, e eles podem ser organizados em diretórios correspondentes às categorias General MIDI.

Valores brutos
: Somente para o formato .csv. Os valores são escritos tal como são armazenados no formato sf2, em vez de serem convertidos em unidades usuais (segundos, decibéis, cents etc.).
  A exportação cria um diretório contendo o arquivo :file:`samples.csv` e um arquivo por instrumento e por predefinição.


## Opções de visualização {#doc_view}


Tela cheia
: Ativa ou desativa o modo de tela cheia.
  Também acessível com o atalho :kbd:`F11`.


## Atalhos {#doc_shortcuts}


Configurações
: Abre as [configurações](manual/settings.md) do software.
  Também acessível com o atalho :kbd:`Ctrl`+:kbd:`,`.

Manual online
: Mostra este manual do usuário em um navegador web.
  Também acessível com o atalho :kbd:`F1`.

Sobre Polyphone…
: Mostra a versão do software e os direitos autorais. O botão :guilabel:`Créditos` mostra a lista de pessoas que contribuíram para o projeto.


## Fechar {#doc_close}


Fechar aba
: Fecha a aba atual (um editor de soundfonts, um explorador de soundfonts, as configurações etc.).
  Também acessível com o atalho :kbd:`Ctrl`+:kbd:`W`.

Sair
: Sai do software.
  Também acessível com o atalho :kbd:`Ctrl`+:kbd:`Q`.