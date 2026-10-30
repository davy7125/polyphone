O Polyphone suporta vários formatos de soundfont:

* [sf2](#doc_sf2),
* [sf3](#doc_sf3),
* [sfz](#doc_sfz),
* [dls](#doc_dls),
* [sfArk](#doc_sfark),
* [organ](#doc_organ).


## Soundfonts sf2 {#doc_sf2}


As soundfonts sf2 são os principais arquivos editados pelo Polyphone.
Cada arquivo sf2 contém um ou mais instrumentos musicais virtuais, compostos de amostras de áudio e de muitos parâmetros.
Os parâmetros definem como as amostras devem ser tocadas ao longo do teclado, eventualmente moduladas por sinais predefinidos ([moduladores](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator)).
Os arquivos sf2 são construídos segundo uma estrutura em 3 níveis:

* [amostras](manual/soundfont-editor/editing-pages/sample-editor.md), vindas de um trompete ou de um piano, por exemplo,
* [instrumentos](manual/soundfont-editor/editing-pages/instrument-editor.md), compostos de amostras,
* [predefinições](manual/soundfont-editor/editing-pages/preset-editor.md), compostas de instrumentos.

Além da definição e da configuração desses elementos, um arquivo sf2 também contém [informações gerais](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md) (o autor, os direitos autorais ou a data de edição, por exemplo).

As soundfonts são usadas por sintetizadores de software, como o <a href="http://www.fluidsynth.org/" target="_blank">FluidSynth</a>, controlados por sinais MIDI.
Também é possível usar soundfonts sf2 para ouvir arquivos MIDI com o <a href="https://sourceforge.net/projects/timidity" target="_blank">TiMidity</a>, o <a href="https://www.mindwerks.net/projects/wildmidi" target="_blank">WildMIDI</a> ou o QuickTime, e para ouvir partituras com o <a href="https://musescore.org/" target="_blank">MuseScore</a>.
Equipamentos como samplers, placas de som ou até sintetizadores também podem suportar o formato .sf2.

Mais informações sobre as soundfonts podem ser encontradas, por exemplo, na <a href="https://en.wikipedia.org/wiki/SoundFont" target="_blank">Wikipédia</a>.
As especificações sf2 estão disponíveis [aqui](files/sf_specifications_v2.01.pdf) para a versão 2.01 e [ali](files/sf_specifications_v2.04.pdf) para a versão 2.04.


## Soundfonts sf3 {#doc_sf3}


O formato sf3, desenvolvido pelo <a href="https://musescore.org/" target="_blank">MuseScore</a>, é em tudo semelhante ao formato sf2, exceto pelo fato de que as [amostras](manual/soundfont-editor/editing-pages/sample-editor.md) podem ser armazenadas no formato OGG (como o formato MP3, mas de código aberto) em vez de serem armazenadas como dados brutos.
A consequência é que o formato sf3 é cerca de 10 vezes mais leve que o formato sf2, para uma qualidade comparável.
No Polyphone, a compressão é ativada nas [informações gerais](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md#doc_edit) da soundfont, e cada amostra pode então ser comprimida no [editor de amostras](manual/soundfont-editor/editing-pages/sample-editor.md#doc_info) (a qualidade da compressão pode ser escolhida).

As amostras comprimidas de uma soundfont exportada neste formato não se destinam a ser editadas posteriormente, porque compressões sucessivas resultariam em uma qualidade sonora inferior.
Este formato deve ser visto como um produto final e é muito interessante para todos os usuários finais de soundfonts, pois:

* os downloads ficam mais fáceis pela internet,
* economiza-se espaço nos computadores,
* a combinação MIDI + sf3 oferece uma solução leve e poderosa para tocar trilhas sonoras (videogames, aplicativos móveis, …).

Ao contrário do formato <a href="http://www.fmjsoft.com/fmt/sf2pack.htm" target="_blank">sf2pack</a>, que responde às mesmas questões, o formato sf3 é inteiramente de código aberto.
O código-fonte está disponível, o que garante sua perenidade (evitemos os erros cometidos com o sfArk ou o sfPack!).


## Soundfonts sfz {#doc_sfz}


O formato sfz tem o mesmo objetivo que o formato sf2: criar um instrumento musical organizando e configurando amostras ao longo do teclado.
A principal diferença é que, enquanto o formato sf2 é um único arquivo que contém tudo, o formato sfz é um arquivo de texto acompanhado de um conjunto de amostras .wav.
Como ele foi pensado para ser editável por um humano, a principal vantagem era poder editar o arquivo sem precisar de um editor complexo.
Mas um editor continua sendo muito recomendado para instrumentos grandes, pois a quantidade de parâmetros pode rapidamente se tornar desanimadora.
O formato sfz também não é definido de forma tão rigorosa quanto o formato sf2: podem aparecer diferenças na maneira de editar e na maneira de tocar um instrumento sfz. Mas, ao mesmo tempo, as capacidades dos instrumentos sfz foram ampliadas em relação ao padrão sf2.


### Limitações da importação sfz


* Somente os opcodes que têm um equivalente nas [tabelas](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) são importados.
* Como o envelope aplicado à altura e ao filtro é o mesmo nos arquivos sf2 (Mod env), não é possível importar envelopes diferentes para a altura e para o filtro (opcodes ``fileg_*`` e ``pitcheg_*``).
* Como o LFO aplicado ao volume e ao filtro é o mesmo nos arquivos sf2 (Mod LFO), não é possível importar LFOs diferentes para o volume e para o filtro (opcodes ``amplfo_*`` e ``fillfo_*``).
* O único filtro que pode ser importado é o filtro passa-baixas de segunda ordem (``fil_type=lpf_2p``).
* Para serem importados corretamente, os opcodes ``group`` e ``off_by`` devem conter o mesmo valor dentro de um mesmo grupo.

**Atenção:** os opcodes ``width`` e ``position`` podem não ser interpretados corretamente.


### Limitações da exportação sfz


* Atualmente, os [moduladores](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) não são exportados.
* Como o formato sfz (versão 1) permite apenas uma modulação de altura, os dois parâmetros :guilabel:`Vib LFO → altura (c)` e :guilabel:`Mod LFO → altura (c)` não podem ser exportados simultaneamente.
* Um loop 0-1 pode ser exportado, mas é ignorado pela maioria dos reprodutores sfz.
* Amostras de 24 bits podem não ser suportadas por alguns sintetizadores; consulte o manual de especificações deles.


### Em geral


A correspondência dos níveis sonoros (em dB) durante o processo de conversão sfz / sf2 foi testada com o software Sforzando.
Podem, no entanto, permanecer algumas diferenças.

Além disso, a maneira como os parâmetros variam de acordo com a nota (por meio de :guilabel:`Nota → Vol env hold / decay` e :guilabel:`Nota → Mod env hold / decay`) não tem correspondência exata.
O formato sf2 usa uma lei exponencial para definir as durações de decay e de hold em função da nota, enquanto o formato sfz usa uma lei linear.


## Arquivos DLS {#doc_dls}


O formato DLS (DownLoadable Sounds), publicado pela MIDI Manufacturers Association, tem o mesmo objetivo que o formato sf2: um único arquivo contém amostras e os parâmetros necessários para tocá-las como instrumentos musicais.
Ele existe em dois níveis: DLS nível 1 (1997) e DLS nível 2 (cuja versão mais recente é a 2.2).
Os arquivos DLS foram usados pelo sintetizador de software do Windows (o arquivo :file:`gm.dls`), pelo DirectMusic, por videogames e por telefones celulares.

A estrutura é diferente da do formato sf2: um arquivo DLS contém ondas (as amostras) e instrumentos, cada instrumento tendo um banco e um número de programa, como uma predefinição.
Um instrumento é composto de regiões, cada uma tocando uma onda em uma faixa de notas e de velocidades.
Os parâmetros (envelopes, LFOs, filtro, modulações…) são escritos como conexões entre uma fonte (por exemplo, um controlador MIDI ou um LFO), um controle opcional e um destino.
Eles podem ser definidos para o instrumento inteiro ou para uma única região; os parâmetros de uma região substituem então os do instrumento.

O Polyphone importa arquivos DLS dos dois níveis (somente arquivos .dls; bancos DLS incorporados em arquivos .rmi ou .xmf não são suportados) e [exporta](manual/menu.md#doc_save) soundfonts no formato DLS nível 2.2.
Um arquivo DLS importado não é modificado quando a soundfont é salva: a soundfont é salva no formato sf2.


### Limitações da importação DLS


* Alguns parâmetros não têm equivalente no formato sf2 e são ignorados: o tempo de desligamento do envelope de volume, as saídas além do balanço estéreo (canais central, traseiros e LFE), o LFO de vibrato aplicado ao volume ou ao filtro e o envelope de modulação aplicado ao volume.
* Uma nota tocada novamente interrompe a anterior em um sintetizador DLS, enquanto em um sintetizador sf2 as duas notas são ouvidas.
* Somente o MSB do banco (CC0) é mantido, ou o LSB do banco (CC32) se o MSB for 0. Se vários instrumentos tiverem o mesmo banco e o mesmo número de programa, o número de programa de um deles é alterado.


### Limitações da exportação DLS


* Um sintetizador DLS nível 1 toca as amostras, mas ignora os envelopes, os LFOs, o filtro e as modulações.
* As amostras de 24 bits são arredondadas para 16 bits.
* Não são exportados:
  os parâmetros :guilabel:`Nota fixa` e :guilabel:`Velocidade fixa`,
  as divisões que usam a [reprodução em loop](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) “release”,
  os moduladores com a transformação “valor absoluto” ou ligados a outros moduladores,
  os moduladores de um parâmetro que não existe no formato DLS (offsets das amostras, nota → hold ou decay dos envelopes…),
  os moduladores que alteram a profundidade de um LFO ou do envelope de modulação com 2 fontes.
* Dois moduladores com as mesmas fontes e o mesmo destino, mas com curvas diferentes, são mesclados.
* Somente 15 classes exclusivas por predefinição podem ser exportadas.
* :guilabel:`Afinação (escala)` e as modulações de controladores MIDI diferentes de CC1, CC7, CC10, CC11, CC91 e CC93 podem ser ignoradas por alguns sintetizadores DLS.


### Em geral


As modulações padrão dos formatos sf2 e DLS são diferentes (velocidade para a frequência de corte do filtro, vibrato com a roda de modulação e a pressão de canal, níveis de reverberação e de chorus): moduladores são adicionados durante as conversões para que o som permaneça o mesmo.

As conversões foram testadas comparando as renderizações dos arquivos originais e dos arquivos convertidos, e também com o <a href="https://github.com/spessasus/spessasynth_core" target="_blank">SpessaSynth</a>, que lê e grava arquivos DLS.
Como poucos sintetizadores suportam o formato DLS nível 2, a forma como alguns parâmetros são tocados pode variar de um sintetizador para outro.


## Arquivos sfArk {#doc_sfark}


Os arquivos sfArk, assim como o formato sf3, são um formato comprimido.
Isso significa que a qualidade dos sons é um pouco reduzida (mas muitas vezes de forma imperceptível).
Esse formato tende a ficar obsoleto, por isso o Polyphone permite apenas importar arquivos sfArk (e não exportá-los).


## Disposições do GrandOrgue {#doc_organ}


O GrandOrgue é um programa cujo principal objetivo é emular instrumentos como órgãos de tubos. Um órgão é definido por uma série de arquivos wav, cada um contendo o som de um tubo, e por um arquivo com a extensão “.organ”, que contém todos os parâmetros necessários para criar um órgão a partir de todos os arquivos wav. Basicamente, todas as amostras são associadas a notas específicas e agrupadas por registros, que podem ser ativados ou desativados ao tocar o órgão. O Polyphone é capaz de criar soundfonts a partir desse mapeamento, mas não suporta a conversão de soundfonts para o formato do GrandOrgue.

### Limitação da importação

Atualmente, as transposições não são suportadas.


*[MIDI]: Musical Instrument Digital Interface
