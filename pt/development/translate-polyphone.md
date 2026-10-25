O Polyphone foi projetado para suportar traduções, a fim de ampliar sua acessibilidade.
Todos podem ajudar nesse processo: uma tradução pode ser criada do zero, ou uma tradução existente pode ser completada ou melhorada.
Desde a versão 2.6, todas as traduções que estavam incompletas foram completadas com a ajuda de uma IA e merecem ser revisadas por falantes nativos.


## Pré-requisitos


### Arquivo de tradução


Baixe o pacote de tradução correspondente ao idioma que você quer traduzir nesta [página](software/translations).
Se o idioma ainda não estiver na lista, use o formulário de [contato](contact) para solicitá-lo (basta indicar o novo idioma desejado).
Você receberá rapidamente um novo pacote de tradução.

Os arquivos de tradução também estão disponíveis no repositório do <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>, no diretório :file:`sources` (um arquivo :file:`polyphone_XX.ts` por idioma).


### Qt Linguist


O único software necessário para traduzir é o :program:`Qt Linguist`: uma ferramenta para gerar arquivos de tradução.
Esse software é instalado com o Qt (veja as [páginas de desenvolvimento](development/index.md)) e também pode ser encontrado <a href="https://github.com/thurask/Qt-Linguist/releases" target="_blank">aqui</a> para usuários do Windows, ou nos repositórios das distribuições Linux (pacote :program:`linguist-qt6` no Debian e no Ubuntu, :program:`qt6-linguist` no Fedora).


## Abrir o arquivo de tradução


Descompacte o pacote de tradução que você acabou de baixar.
Você encontrará um arquivo .ts :file:`polyphone_XX.ts`, em que :file:`XX` corresponde ao seu idioma.
Com o :program:`Qt Linguist`, abra :file:`polyphone_XX.ts` e você poderá começar a traduzir.


## Traduzir


Cada item de cada contexto precisa ser traduzido.
Basta revisar cada elemento, um por um, e preencher a sua tradução.
Três casos podem ocorrer, dependendo do seu grau de confiança.

* Se você não sabe como traduzir, não hesite em pular o elemento.
  Isso não é um problema: outras pessoas poderão completá-lo.
* Você acha que tem a tradução certa, mas não tem certeza: escreva sua proposta e depois use :kbd:`Ctrl`+:kbd:`J`.
  O item não será validado, mas a tradução será usada mesmo assim na interface traduzida.
  Isso apenas indica às outras pessoas que melhoram a tradução que você não tinha certeza.
* Você tem a tradução certa: escreva-a e use :kbd:`Ctrl`+:kbd:`Enter`.
  Este item será validado.

Uma tradução exige atenção (em um mundo onde tudo é feito para <a href="https://medium.com/thrive-global/how-technology-hijacks-peoples-minds-from-a-magician-and-google-s-design-ethicist-56d62ef5edf3" target="_blank">solicitá-la</a>).
Preste atenção a:

* a pontuação,
* os espaços iniciais e finais,
* as maiúsculas e minúsculas,
* as formas de singular e de plural.

Um último conselho: **não tenha pressa!**
E não se esqueça de salvar seu trabalho com :kbd:`Ctrl`+:kbd:`S`.


## Testar o seu trabalho


Você pode verificar seu trabalho a qualquer momento.
No :program:`Qt Linguist`, vá em :menuselection:`File --> Release As…` e crie um arquivo .qm correspondente à tradução que você está fornecendo.
Siga as instruções do pacote de tradução para instalá-lo (arquivo :file:`readme.txt`) e depois abra o Polyphone.
Vá às [configurações](manual/settings.md#doc_interface), selecione a sua tradução na lista de idiomas e reinicie o Polyphone.
Você poderá então revisar a sua tradução.

**Observação:** a operação :guilabel:`Release As…` pode traduzir apenas os textos sem acentos. Se esse bug ocorrer, abra um terminal e use o seguinte comando: :command:`lrelease polyphone_XX.ts` (adapte-o ao seu arquivo).
Um arquivo de tradução :file:`polyphone_XX.qm` também pode ser carregado a partir de um diretório chamado :file:`translations` (no diretório atual ou no diretório de configuração do Polyphone), em vez de ser incorporado ao software.


## Enviar a tradução


Quando terminar (ou se cansar!), envie o arquivo .ts (e não o .qm!) que você estava editando para <contact@polyphone.io>.
O pacote de tradução desta [página](software/translations) será atualizado.

Se você está acostumado com o GitHub, também pode propor seu arquivo .ts com um pull request no <a href="https://github.com/davy7125/polyphone" target="_blank">repositório</a> (veja o guia de contribuição).