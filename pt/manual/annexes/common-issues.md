Esta página lista os defeitos típicos encontrados em soundfonts (que podem ser apontados em avaliações) e, para cada um deles, apresenta técnicas e links para a documentação que ajudam a corrigi-los:

* [amostras de baixa qualidade ou mal preparadas](#doc_sample),
* [afinação incorreta](#doc_tuning),
* [envelope ADSR inadequado](#doc_adsr),
* [loops ausentes ou imprecisos](#doc_loop),
* [padrão MIDI não respeitado](#doc_standard).


## Amostras de baixa qualidade ou mal preparadas {#doc_sample}


Pode acontecer de uma soundfont não oferecer instrumentos convincentes e, se a causa estiver em amostras de baixa qualidade, a única solução é substituí-las:

* procurar outras amostras na internet,
* gravar com um microfone de melhor qualidade (ou usar ferramentas externas de processamento de sinal),
* criar amostras por outros meios.

Às vezes, o problema pode ser resolvido preparando ou usando melhor as amostras existentes.

### Qualidade sonora ruim, especialmente nos agudos

Normalmente, uma única amostra de áudio não basta para criar um instrumento de alta qualidade. Uma amostra tem sua própria altura (exceto as percussões não melódicas), e usá-la em uma faixa ampla de notas distorce o som, especialmente nas notas mais agudas. Para corrigir esse problema, é preciso usar várias amostras ou, se apenas uma amostra estiver disponível, você pode:

* duplicar a amostra,
* transpor a amostra duplicada em uma oitava, por exemplo, com esta [ferramenta](manual/soundfont-editor/tools/sample-tools.md#doc_transpose),
* filtrar os harmônicos agudos com esta [ferramenta](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq) ou [esta outra](manual/soundfont-editor/tools/sample-tools.md#doc_filter).

As amostras devem então ser distribuídas nos alcances de notas em que sua reprodução não altera o timbre original.

### O som contém “cliques” regulares

O problema provavelmente vem de um ponto de loop mal posicionado, assunto tratado em outra [seção](#doc_loop).

### Amostras mal cortadas

Muitas vezes as amostras são usadas apenas parcialmente, e remover o excesso pode reduzir o tamanho do arquivo:

* a parte útil de uma amostra às vezes fica antes do ponto final do loop: é então possível excluir a parte da amostra após o fim do loop, e esta [ferramenta](manual/soundfont-editor/tools/sample-tools.md#doc_trimloop) permite fazer isso em poucos cliques;
* as amostras podem começar com um silêncio, que deve ser removido para reduzir a latência ao disparar uma nota (também existe uma [ferramenta](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank) para fazer isso rapidamente);
* por fim, algumas amostras podem simplesmente não ser usadas: este [resumo de configuração](manual/soundfont-editor/configuration-summaries.md#doc_sample) as lista, e esta [ferramenta](manual/soundfont-editor/tools/global-tools.md#doc_unused) as remove automaticamente.


## Afinação incorreta {#doc_tuning}


Afinar um instrumento virtual envolve várias etapas, e é importante passar por todas elas, na ordem.

### Afinação no nível da amostra

Esta é a etapa de afinação mais importante, descrita nesta [página](tutorials/how-to-prepare-a-sample.md#doc_tuning).

### Afinação no nível do instrumento

4 parâmetros, descritos nesta [página](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table), servem para definir a transposição com que uma amostra é tocada: **nota base**, **afinação (semitons)**, **afinação (cents)** e **afinação (escala)**. Esses valores podem ser modificados durante a execução do instrumento por [moduladores](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator), que também devem ser verificados com atenção.

Se nenhum parâmetro ou modulador estiver definido para a divisão do instrumento vinculada a uma amostra, é importante verificar se há parâmetros ou moduladores definidos na divisão global do instrumento (essa divisão define a configuração padrão de todas as outras divisões).

### Afinação no nível da predefinição

No nível da predefinição, 3 parâmetros afetam a transposição de uma amostra: **afinação (semitons)**, **afinação (cents)** e **afinação (escala)**. Esses valores se somam à configuração dos instrumentos usados nas divisões da predefinição.

Assim como os instrumentos, as predefinições podem incluir moduladores, e sua divisão global funciona como uma configuração padrão que não deve ser esquecida.


## Envelope ADSR inadequado {#doc_adsr}


O envelope ADSR representa como o volume evolui quando uma nota é tocada, mas também quando essa mesma nota é solta. Cada uma das quatro letras do termo ADSR corresponde a uma fase específica, e o envelope de volume usado nas soundfonts conta na verdade com 6 fases:

* delay,
* attack (A),
* hold,
* decay (D),
* sustain (S),
* release (R).

Essas fases são descritas em mais detalhes nesta [página](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope), e abaixo estão listados alguns problemas possíveis relacionados ao envelope.

### Um som começa de forma abrupta demais

Para suavizar o início de uma nota, especifique ou aumente a **duração do attack** da nota no nível do [instrumento](manual/soundfont-editor/editing-pages/instrument-editor.md) ou da [predefinição](manual/soundfont-editor/editing-pages/preset-editor.md).

### Um som começa suave demais

Ao contrário do caso anterior, diminua ou remova a **duração do attack** da nota.

### Sente-se um atraso antes de o som começar

Três soluções possíveis e complementares:

* remover a **duração do delay** se ela estiver especificada no nível do [instrumento](manual/soundfont-editor/editing-pages/instrument-editor.md) ou da [predefinição](manual/soundfont-editor/editing-pages/preset-editor.md),
* remover o silêncio no início da amostra de áudio (veja o corte de amostras nesta [página](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph) ou a ferramenta de remoção de silêncio [aqui](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)),
* por fim, a latência percebida pode ser causada pelo servidor de áudio usado no seu computador (nesse caso, não é um defeito da soundfont em si, e você pode alterar a saída de áudio nas [configurações](manual/settings.md#doc_general)).

### O som para de repente

Se o som para de repente quando deveria continuar enquanto a nota é mantida, veja a seção sobre os [loops](#doc_loop).

Se o som para bruscamente quando uma nota é solta, especifique ou aumente a **duração do release** no nível do [instrumento](manual/soundfont-editor/editing-pages/instrument-editor.md) ou da [predefinição](manual/soundfont-editor/editing-pages/preset-editor.md). Esta [ferramenta](manual/soundfont-editor/tools/instrument-tools.md#doc_release) permite configurar essa duração para que soe natural em toda a extensão, do grave ao agudo.


## Loops ausentes ou imprecisos {#doc_loop}


Os loops permitem produzir um som de duração infinita a partir de uma amostra de comprimento fixo, sem que o ouvido perceba as repetições. Os loops são essenciais para certos tipos de instrumentos, como as flautas, mas não para outros, como as percussões. Os loops também podem ser usados em amostras de piano para simular um pedal de sustain com uma amostra curta.

### O som para antes de a nota ser solta

Se o som para quando deveria continuar tocando, as causas mais prováveis são:

* o loop da amostra não está definido — nesse caso, você pode consultar esta [página](tutorials/how-to-prepare-a-sample.md#doc_loop) para configurar um loop,
* a reprodução em loop não está ativada — nesse caso, é preciso alterar o parâmetro **reprodução em loop** no nível do [instrumento](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).

### O som contém “cliques” regulares

O loop provavelmente está incorreto, e esta [página](tutorials/how-to-prepare-a-sample.md#doc_loop) explica como criar loops adequados.


## Padrão MIDI não respeitado {#doc_standard}


Os padrões MIDI impõem uma numeração específica aos instrumentos contidos nas soundfonts. Isso permite que diferentes soundfonts que seguem o mesmo padrão sejam intercambiáveis, sem diferenças significativas nos timbres tocados: por exemplo, um piano e uma flauta serão chamados da mesma forma. Se uma avaliação indicar que um padrão não está sendo respeitado, há duas soluções possíveis:

* remover a informação de que a soundfont é compatível com um padrão,
* ou verificar se todas as predefinições da soundfont estão corretamente atribuídas ao tipo de instrumento adequado (veja esta [página](manual/annexes/midi-standards.md) para a lista de instrumentos) e se todos os instrumentos exigidos pelo padrão estão presentes.
