A página de edição das informações gerais é exibida quando o cabeçalho :guilabel:`Geral` é selecionado na [árvore](manual/soundfont-editor/tree.md).
Esta página é composta de três tipos de elementos:

* [campos editáveis](#doc_edit),
* [informações gerais](#doc_info) não editáveis,
* [estatísticas](#doc_stats) sobre a soundfont.

O botão :guilabel:`Moduladores padrão` da [barra de ferramentas](manual/soundfont-editor/toolbar.md#doc_view) exibe os [moduladores padrão](#doc_default_mod) da soundfont, e o botão :guilabel:`Geral` exibe esta página novamente.


![Página de informações gerais](images/edit_general.png "Página de informações gerais")


## Campos editáveis {#doc_edit}


A página de edição das informações gerais oferece campos para alterar os seguintes atributos:

Título
: É o nome interno da soundfont, que não tem necessariamente relação com o nome do arquivo.

Autor
: Refere-se ao nome do autor da soundfont.

Data
: Data de criação do arquivo (um calendário ajuda a escolhê-la).

Produto
: Produto para o qual a soundfont foi projetada.

Direitos autorais
: Menção a preencher se o arquivo estiver sujeito a direitos autorais.

Comentários
: Informações não incluídas em nenhuma das categorias anteriores (o texto pode conter qualquer caractere UTF-8).

Profundidade de bits
: Indica se as amostras são armazenadas com resolução de 16 bits ou de 24 bits (neste caso, a compressão não é permitida).
  Observe que amostras de 24 bits podem não ser suportadas por alguns sintetizadores.

Compressão
: Ativa a compressão das amostras (neste caso, a profundidade de bits é definida como 16 bits). Se esta opção estiver ativada, um botão de compressão é exibido no [editor de amostras](manual/soundfont-editor/editing-pages/sample-editor.md).
  A compressão é um recurso oferecido pelo [formato sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3) e pode não ser suportada por todos os sintetizadores.
  Ativar ou desativar esta opção determina o formato do arquivo ao salvar, e o Polyphone altera a extensão do arquivo .sf2 / .sf3 se necessário.


## Informações gerais {#doc_info}


A página de edição das informações gerais fornece as seguintes informações não editáveis:

Formato SoundFont
: Versão do [formato sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2) usada pelo arquivo.

Nome e versão da ROM
: Nome e versão de uma ROM de amostras, se utilizada.

Motor de som
: Nome de um motor de som.

Software de edição
: Software de edição usado para criar a soundfont.

Essas informações são exibidas em uma dica quando o mouse passa sobre o ícone de informação, no fim da linha que mostra o autor e a data.

Nome e tamanho do arquivo
: Endereço e nome do arquivo, e seu tamanho em disco, exibidos na parte inferior da janela (o tamanho é o do arquivo tal como foi salvo pela última vez).


## Estatísticas {#doc_stats}


É feita uma contagem:

* do número de [amostras](manual/soundfont-editor/editing-pages/sample-editor.md), de amostras não utilizadas e de amostras duplicadas,
* do número de [instrumentos](manual/soundfont-editor/editing-pages/instrument-editor.md) e de instrumentos não utilizados,
* do número de [predefinições](manual/soundfont-editor/editing-pages/preset-editor.md),
* do número de parâmetros e de moduladores editados nos instrumentos,
* do número de parâmetros e de moduladores editados nas predefinições.

O botão :guilabel:`Detalhes` ao lado das amostras, dos instrumentos e das predefinições exibe o [resumo de configuração](manual/soundfont-editor/configuration-summaries.md) correspondente.

Amostras e instrumentos não utilizados podem ser removidos facilmente com a ferramenta “[Remover elementos não utilizados](manual/soundfont-editor/tools/global-tools.md#doc_unused)”.
Amostras duplicadas podem ser removidas com a ferramenta “[Limpar duplicatas](manual/soundfont-editor/tools/global-tools.md#doc_duplicates)”.

**Observação:** o número de 65536 parâmetros nos instrumentos e nas predefinições não deve ser ultrapassado.
Alguns sintetizadores podem não conseguir ler todos eles.



## Moduladores padrão {#doc_default_mod}


Os moduladores padrão são aplicados a cada instrumento da soundfont, antes dos [moduladores](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) do instrumento: um modulador de um instrumento com as mesmas fontes, o mesmo destino e a mesma transformação substitui um modulador padrão (indicação :guilabel:`substitui mod. padrão`).

Sem lista própria, uma soundfont usa os moduladores padrão do formato SoundFont, exibidos sem poder ser editados: velocidade para atenuação e para frequência de corte, pressão do canal e roda de modulação para vibrato, volume, pan, expressão, reverberação, chorus e roda de pitch.
O modulador velocidade para frequência de corte depende das [configurações de som](manual/settings.md#doc_sound).

O botão :guilabel:`Personalizar` dá à soundfont sua própria lista, que começa com esses moduladores e é depois editada como os moduladores de um instrumento (adicionar, remover, copiar, colar, alterar).
Essa lista substitui os moduladores padrão do formato SoundFont: um modulador removido da lista deixa de se aplicar a qualquer instrumento, e um modulador adicionado se aplica a todos.
O botão :guilabel:`Redefinir para os padrões do SoundFont` remove a lista e restaura os moduladores padrão do formato.

A lista é salva no bloco DMOD dos arquivos sf2 e sf3, uma [extensão do formato](manual/annexes/the-different-soundfont-formats.md#doc_sf2) lida pelo FluidSynth 2.5 e posteriores, pelo SpessaSynth e pelo Polyphone 2.7 e posteriores (incluindo o [plugin](manual/plugin.md)).
Os outros sintetizadores a ignoram e aplicam os moduladores padrão do formato SoundFont.

Quando um instrumento é copiado para outra soundfont cujos moduladores padrão são diferentes, moduladores globais são adicionados ao instrumento para que ele soe igual.
A [exportação DLS](manual/annexes/the-different-soundfont-formats.md#doc_dls) converte os moduladores padrão da soundfont; a exportação sfz mantém apenas o efeito da velocidade sobre o volume e sobre a frequência de corte.