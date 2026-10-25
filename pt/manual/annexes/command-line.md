O Polyphone pode ser executado pela linha de comando para converter soundfonts no formato [sf2](#doc_sf2), [sf3](#doc_sf3), [sfz](#doc_sfz) ou [csv](#doc_csv). Os formatos de arquivo suportados para a conversão são [sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) e [organ](manual/annexes/the-different-soundfont-formats.md#doc_organ).

Também é possível usar uma opção para abrir o Polyphone em [modo sintetizador](#doc_synth).

Arquivos e diretórios também podem ser passados como argumentos para abri-los na interface gráfica, por exemplo :command:`polyphone file1.sf2 file2.sf3 /path/to/directory` (um diretório é aberto no [explorador de soundfonts](manual/soundfont-explorer.md)).
Algumas [outras opções](#doc_other) estão disponíveis para redefinir a configuração ou exibir a ajuda.

Em uma conversão, o carregamento e a gravação são registrados no console.
O código de retorno é 0 se a conversão for bem-sucedida, e outro valor em caso de falha (o arquivo de entrada não existe, o arquivo não pode ser lido ou gravado, já existe um arquivo de saída para os formatos sf2 e sf3 etc.).


## Conversão para sf2 {#doc_sf2}


### Linha de comando


```
polyphone -1 -i <file/to/convert> -d <output/directory> -o <output_file_name>
```

**Observação:** no Windows, é preciso escrever o caminho completo do Polyphone em vez de apenas :command:`polyphone`, por exemplo :command:`C:/Program files/Polyphone/polyphone.exe`.


### Argumentos


-1
: O modo “1” é “converter para sf2”.

-d
: Diretório de saída em que o arquivo de entrada será convertido.
  Este argumento é opcional; por padrão, é o mesmo diretório do arquivo de entrada.

-o
: Nome de saída do arquivo convertido, sem a extensão (a extensão “.sf2” será adicionada automaticamente).
  Este argumento é opcional; por padrão, é o mesmo nome do arquivo de entrada.


### Exemplo


```
polyphone -1 -i /path/to/file.sfArk
```


## Conversão para sf3 {#doc_sf3}


### Linha de comando


```
polyphone -2 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**Observação:** no Windows, é preciso escrever o caminho completo do Polyphone em vez de apenas :command:`polyphone`, por exemplo :command:`C:/Program files/Polyphone/polyphone.exe`.


### Argumentos


-2
: O modo “2” é “converter para sf3”.

-d
: Diretório de saída em que o arquivo de entrada será convertido.
  Este argumento é opcional; por padrão, é o mesmo diretório do arquivo de entrada.

-o
: Nome de saída do arquivo convertido, sem a extensão (a extensão “.sf3” será adicionada automaticamente).
  Este argumento é opcional; por padrão, é o mesmo nome do arquivo de entrada.

-c
: Configuração da conversão.
  É possível especificar a qualidade da compressão: “0” é baixa, “1” é média, “2” é alta.
  Este argumento é opcional; por padrão, é “1” (qualidade média).


### Exemplo


```
polyphone -2 -i /path/to/file.sf2 -c 2
```


## Conversão para sfz {#doc_sfz}


### Linha de comando


```
polyphone -3 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**Observação:** no Windows, é preciso escrever o caminho completo do Polyphone em vez de apenas :command:`polyphone`, por exemplo :command:`C:/Program files/Polyphone/polyphone.exe`.


### Argumentos


-3
: O modo “3” é “converter para sfz”.

-d
: Diretório de saída em que o arquivo de entrada será convertido.
  Este argumento é opcional; por padrão, é o mesmo diretório do arquivo de entrada.

-o
: Nome de saída do arquivo convertido, sem a extensão (a extensão “.sfz” será adicionada automaticamente).
  Este argumento é opcional; por padrão, é o mesmo nome do arquivo de entrada.

-c
: Configuração da conversão, composta de 3 valores separados pelo caractere “|”. Este argumento é opcional; por padrão, é “0|0|0”.
  
  * O primeiro valor é “1” se cada predefinição deve receber como prefixo seu número de predefinição, “0” caso contrário.
  * O segundo valor é “1” se deve ser criado um diretório por banco, “0” caso contrário.
  * O terceiro valor é “1” se a classificação General MIDI deve ser usada para organizar as predefinições, “0” caso contrário.


### Exemplo


```
polyphone -3 -i /path/to/file.sf3 -c '0|1|1'
```

A conversão cria um diretório (com o nome de saída) contendo os arquivos sfz e um subdiretório :file:`samples`. Se esse diretório já existir, um número é acrescentado ao seu nome.


## Conversão para .csv {#doc_csv}


### Linha de comando


```
polyphone -4 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**Observação:** no Windows, é preciso escrever o caminho completo do Polyphone em vez de apenas :command:`polyphone`, por exemplo :command:`C:/Program files/Polyphone/polyphone.exe`.


### Argumentos


-4
: O modo “4” é “converter para .csv”.

-d
: Diretório de saída em que o arquivo de entrada será convertido.
  Este argumento é opcional; por padrão, é o mesmo diretório do arquivo de entrada.

-o
: Nome de saída do arquivo convertido, sem a extensão (a extensão “.csv” será adicionada automaticamente).
  Este argumento é opcional; por padrão, é o mesmo nome do arquivo de entrada.

-c
: Configuração da conversão.
  A opção “raw” pode ser usada para exportar os valores brutos (tal como são armazenados no arquivo de soundfont) em vez de valores com unidades físicas mensuráveis. Por exemplo, o tempo de release exportado será 0.491 (segundos), ou -1229 em sua forma bruta. Por padrão, são exportados os valores com unidades físicas.


### Exemplo


```
polyphone -4 -i /path/to/file.sf2 -c 'raw'
```

A conversão cria um diretório (com o nome de saída) contendo o arquivo :file:`samples.csv` e um arquivo por instrumento e por predefinição. Se esse diretório já existir, um número é acrescentado ao seu nome.


## Modo sintetizador {#doc_synth}


### Linha de comando


```
polyphone -s -i <file/to/read> -c <configuration>
```

**Observação:** no Windows, é preciso escrever o caminho completo do Polyphone em vez de apenas :command:`polyphone`, por exemplo :command:`C:/Program files/Polyphone/polyphone.exe`.

![Polyphone aberto com a opção do modo sintetizador](images/synth_mode.png "Polyphone aberto com a opção do modo sintetizador")


### Argumentos


-s
: O modo “s” é “abrir o Polyphone em modo sintetizador”.

-c
: Configuração do sintetizador, composta de 3 valores separados pelo caractere “|”. Este argumento é opcional; por padrão, é “0|off|off”. Esses parâmetros podem depois ser alterados pela interface gráfica.
  
  * O primeiro valor é o número do canal MIDI (entre 1 e 16) a escutar para tocar. Um valor de 0 ou “all” indica que todos os canais serão usados e tratados como um único canal. Um valor de -1 ou “each” escuta cada canal separadamente, o que é um pré-requisito, por exemplo, para [tocar arquivos MIDI](tutorials/read-midi-file-with-soundfont.md).
  * O segundo valor é “off” ou “on”, desativando ou ativando a seleção de várias predefinições para tocar.
  * O terceiro valor é “off”, “on” ou “toggle”, desativando ou ativando a seleção das predefinições pelas teclas correspondentes ao número da predefinição (a tecla 0 ativa a predefinição 0, por exemplo).


### Exemplo


```
polyphone -s -i /path/to/file.sf2 -c '1|on|toggle'
```


## Outras opções {#doc_other}


-h
: Exibe uma breve mensagem de ajuda.

-r
: Redefine a configuração do Polyphone (configurações, geometria das janelas, histórico etc.).
  Isso pode ser útil em caso de problemas.

```
polyphone -r
```
