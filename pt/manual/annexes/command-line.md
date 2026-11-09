O Polyphone pode ser executado pela linha de comando para converter soundfonts no formato [sf2](#doc_sf2), [sf3](#doc_sf3), [sfz](#doc_sfz), [csv](#doc_csv) ou [dls](#doc_dls). Os formatos de arquivo suportados para a conversão são [sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [dls](manual/annexes/the-different-soundfont-formats.md#doc_dls), [sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) e [organ](manual/annexes/the-different-soundfont-formats.md#doc_organ).

Um [arquivo MIDI pode ser tocado](#doc_render) com uma soundfont e gravado em um arquivo wav.
Também é possível usar opções para abrir o Polyphone em [modo sintetizador](#doc_synth) ou com o [servidor MCP](#doc_mcp) iniciado.

Arquivos e diretórios também podem ser passados como argumentos para abri-los na interface gráfica, por exemplo :command:`polyphone file1.sf2 file2.sf3 /path/to/directory` (um diretório é aberto no [explorador de soundfonts](manual/soundfont-explorer.md)).
Se o Polyphone já estiver aberto, os arquivos são abertos na instância em execução.
Algumas [outras opções](#doc_other) estão disponíveis para redefinir a configuração ou exibir a ajuda e a versão.

A lista de todas as opções é exibida com :command:`polyphone --help`.
A ordem dos argumentos não importa.
As opções das [versões anteriores](#doc_legacy) continuam aceitas.

**Observação:** no Windows, é preciso escrever o caminho completo do Polyphone em vez de apenas :command:`polyphone`, por exemplo :command:`C:/Program files/Polyphone/polyphone.exe`.


## Conversão {#doc_conversion}


### Linha de comando


```
polyphone --convert <formato> <arquivos/a/converter> -d <diretório/de/saída> -o <arquivo/de/saída> -f
```


### Argumentos


--convert
: Formato da conversão: “sf2”, “sf3”, “sfz”, “csv” ou “dls”; as opções próprias de cada formato são descritas abaixo.
  Este argumento é opcional se um arquivo de saída for informado com sua extensão, por exemplo :command:`polyphone file.sf2 -o file.sf3`.

-d, --output-dir
: Diretório de saída, que deve existir.
  Este argumento é opcional; por padrão, é o diretório de cada arquivo de entrada.

-o, --output
: Arquivo de saída, com ou sem sua extensão (a extensão do formato é adicionada automaticamente).
  Este argumento é opcional; por padrão, o arquivo de saída tem o nome do arquivo de entrada.
  Um nome sozinho é gravado no diretório de saída, um caminho relativo parte do diretório atual (ou do diretório de saída se -d for usado).
  Este argumento não pode ser usado se vários arquivos forem convertidos.

-f, --force
: Substitui um arquivo de saída existente.
  Sem este argumento, a conversão falha se o arquivo de saída já existir.
  O arquivo de entrada pode ser substituído por sua conversão, por exemplo :command:`polyphone --convert sf2 file.sf2 -f` regrava :file:`file.sf2`.

Vários arquivos podem ser convertidos com um único comando, cada arquivo de saída tendo o nome de seu arquivo de entrada.
Em caso de falha, os outros arquivos são convertidos mesmo assim.


### Exemplos


```
polyphone --convert sf2 /path/to/file.sfArk
polyphone /path/to/file.sf2 -o /path/to/converted.sf3
polyphone --convert sf3 *.sf2 -d /path/to/directory
```


## Conversão para sf2 {#doc_sf2}


Nenhuma opção específica é usada para esta conversão.

```
polyphone --convert sf2 /path/to/file.dls
```


## Conversão para sf3 {#doc_sf3}


--quality
: Qualidade de compressão: “low” (baixa), “medium” (média) ou “high” (alta).
  Este argumento é opcional; por padrão, é “medium”.

```
polyphone --convert sf3 /path/to/file.sf2 --quality high
```


## Conversão para sfz {#doc_sfz}


--preset-prefix
: Cada predefinição recebe como prefixo seu número de predefinição.

--bank-dir
: Um diretório por banco é criado.

--gm-sort
: A classificação General MIDI é usada para ordenar as predefinições.

```
polyphone --convert sfz /path/to/file.sf3 --bank-dir --gm-sort
```

A conversão cria um diretório (com o nome do arquivo de saída) contendo os arquivos sfz e um subdiretório :file:`samples`.
Se esse diretório já existir, um número é adicionado ao seu nome: nada é substituído, nem mesmo com -f.


## Conversão para .csv {#doc_csv}


--raw
: São exportados os valores brutos (como estão armazenados no arquivo soundfont) em vez de valores com unidades físicas e mensuráveis.
  Por exemplo, o tempo de liberação exportado será 0,491 (segundos) ou -1229 em sua forma bruta.
  Por padrão, são exportados os valores com unidades físicas.

```
polyphone --convert csv /path/to/file.sf2 --raw
```

A conversão cria um diretório (com o nome do arquivo de saída) contendo o arquivo :file:`samples.csv` e um arquivo por instrumento e por predefinição.
Se esse diretório já existir, um número é adicionado ao seu nome: nada é substituído, nem mesmo com -f.


## Conversão para dls {#doc_dls}


Nenhuma opção específica é usada para esta conversão.

```
polyphone --convert dls /path/to/file.sf2
```

O arquivo é gravado no formato DLS nível 2; as limitações da conversão são descritas [aqui](manual/annexes/the-different-soundfont-formats.md#doc_dls).


## Renderização de um arquivo MIDI {#doc_render}


### Linha de comando


```
polyphone --render <arquivo/MIDI> <soundfont> -d <diretório/de/saída> -o <arquivo/de/saída> -f
```


### Argumentos


--render
: Caminho do arquivo MIDI a tocar (extensão .mid, .midi, .kar ou .rmi): o arquivo MIDI é tocado com a soundfont e o resultado é gravado em um arquivo wav.

-d, --output-dir
: Diretório de saída no qual o arquivo wav será gravado.
  Este argumento é opcional; por padrão, é o diretório do arquivo MIDI.

-o, --output
: Arquivo wav de saída, com ou sem sua extensão.
  Este argumento é opcional; por padrão, o arquivo wav tem o nome do arquivo MIDI.

-f, --force
: Substitui um arquivo wav existente.


### Exemplo


```
polyphone --render /path/to/song.mid /path/to/file.sf2
```

Todos os canais do arquivo MIDI tocam a soundfont, e as predefinições são escolhidas pelas mudanças de programa do arquivo (banco 128 no canal 10, para a percussão).
São usadas as configurações do sintetizador: [som](manual/settings.md#doc_sound) (reverberação, chorus, volume geral), [polifonia](manual/settings.md#doc_general) e [afinação](manual/settings.md#doc_keyboard).
O resultado é um arquivo wav estéreo de 32 bits em ponto flutuante a 48.000 Hz, que termina quando o som das últimas notas se extingue.
As notas de uma predefinição que a soundfont não tem não são tocadas, e um aviso é exibido para essa predefinição.
Na interface gráfica, um arquivo MIDI também pode ser exportado a partir do [reprodutor MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player_settings), onde cada canal pode tocar uma soundfont ou uma predefinição diferente.


## Modo sintetizador {#doc_synth}


### Linha de comando


```
polyphone --synth <arquivo/a/ler> --channel <canal> --multiple-selection --key-selection <modo>
```

![Polyphone aberto com a opção do modo sintetizador](images/synth_mode.png "Polyphone aberto com a opção do modo sintetizador")


### Argumentos


-s, --synth
: O Polyphone é aberto em modo sintetizador.

--channel
: Número do canal MIDI (entre 1 e 16) a escutar para tocar.
  O valor “all” indica que todos os canais serão usados e tratados como um único canal (valor padrão).
  O valor “each” escuta cada canal separadamente, o que é um pré-requisito, por exemplo, para tocar a saída MIDI de um sequenciador (um arquivo MIDI também pode ser tocado diretamente com o [reprodutor MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player)).

--multiple-selection
: Várias predefinições podem ser selecionadas para tocar.

--key-selection
: Com o valor “on”, as predefinições são selecionadas pelas teclas correspondentes ao número da predefinição (a tecla 0 ativa a predefinição 0, por exemplo).
  Com o valor “toggle”, uma tecla também desmarca sua predefinição.

Estes argumentos são opcionais, e as configurações podem depois ser alteradas pela interface gráfica.


### Exemplo


```
polyphone --synth /path/to/file.sf2 --channel 1 --multiple-selection --key-selection toggle
```


## Servidor MCP {#doc_mcp}


### Linha de comando


```
polyphone --mcp <arquivos/a/abrir> --mcp-port <porta>
```


### Argumentos


--mcp
: O servidor MCP, que permite a um assistente de IA navegar, editar e analisar as soundfonts abertas, é iniciado para esta sessão mesmo que esteja desativado nas [configurações](manual/settings.md).
  O endereço do servidor é exibido no console.
  Uma nova instância do Polyphone é aberta, mesmo que o Polyphone já esteja aberto.

--mcp-port
: Porta do servidor MCP (este argumento implica --mcp).
  Este argumento é opcional; por padrão, é a porta das configurações (8642).

Alterar as configurações do servidor MCP na interface gráfica substitui estas opções.


### Exemplo


```
polyphone --mcp-port 8700 /path/to/file.sf2
```


## Outras opções {#doc_other}


-h, --help
: Exibe a lista de todas as opções.

-v, --version
: Exibe a versão do Polyphone.

-r, --reset-config
: Redefine a configuração do Polyphone (configurações, geometria das janelas, histórico etc.).
  Isso pode ser útil em caso de problemas.

```
polyphone --reset-config
```


## Mensagens e códigos de retorno {#doc_exit}


O carregamento e a gravação são registrados no console, e os erros são escritos na saída de erros.
As mensagens e a ajuda estão no idioma escolhido nas [configurações](manual/settings.md).

O código de retorno é:

0
: Sucesso.

1
: Um arquivo de entrada ou o diretório de saída não existe, ou o arquivo de saída já existe (sem -f).

2
: Os argumentos não são válidos: opção desconhecida, valor incorreto, opção não usada com o modo escolhido, formato de arquivo de entrada não suportado etc.

3
: Um arquivo de entrada não pode ser lido (soundfont, arquivo MIDI).

4
: A saída não pode ser gravada.

Quando vários arquivos são convertidos, o código de retorno é o da primeira falha.


## Opções das versões anteriores {#doc_legacy}


As opções das versões anteriores do Polyphone continuam aceitas:

-1, -2, -3, -4, -6
: Conversão para sf2, sf3, sfz, csv e dls, como com --convert.

-5
: Renderização de um arquivo MIDI, cujo caminho é informado com -c.

-i
: Arquivo de entrada, que também pode ser escrito sem esta opção.

-c
: Configuração, composta de valores separados pelo caractere “|”:
  
  * conversão para sf3: qualidade de compressão, “0” para baixa, “1” para média, “2” para alta;
  * conversão para sfz: 3 valores, “1” ou “0”, para --preset-prefix, --bank-dir e --gm-sort, por exemplo “0|1|1”;
  * conversão para csv: “raw” para --raw;
  * renderização de um arquivo MIDI (-5): caminho do arquivo MIDI;
  * modo sintetizador: canal (“all”, “each” ou de 1 a 16, sendo 0 equivalente a “all” e -1 a “each”), seleção múltipla (“on” ou “off”) e seleção por teclas (“on”, “off” ou “toggle”), por exemplo “all|on|toggle”.

```
polyphone -2 -i /path/to/file.sf2 -c 2
polyphone -s -i /path/to/file.sf2 -c 'all|on|toggle'
```
