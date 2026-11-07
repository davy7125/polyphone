O Polyphone inclui um servidor que segue o Model Context Protocol (MCP), um padrão que permite que assistentes de IA usem outros programas.
Depois de ativado, um assistente como o Claude pode trabalhar nas soundfonts abertas no Polyphone: você descreve o que quer com suas próprias palavras, e o assistente explora as soundfonts, modifica-as, usa as ferramentas do Polyphone e mede o resultado.

Cada alteração aparece imediatamente no editor e pode ser desfeita, como se você mesmo a tivesse feito.
Este tutorial mostra como conectar o Claude ao Polyphone e depois dá exemplos do que pode ser pedido, com arquivos de áudio para ouvir os resultados.


## O que o assistente pode fazer {#doc_principles}


O assistente tem acesso a tudo o que está aberto no Polyphone:

* **navegação**: as soundfonts, suas amostras, instrumentos e presets, os parâmetros de cada divisão e os elementos selecionados na [árvore](manual/soundfont-editor/tree.md) (para que “esta amostra” ou “os instrumentos selecionados” sejam compreendidos),
* **edição**: criar, renomear, duplicar ou remover elementos, alterar parâmetros, alcances de notas e de velocidades, pontos de loop, moduladores…
* **ferramentas**: a maioria das ferramentas do [menu de ferramentas](manual/soundfont-editor/tools/index.md), como o loop automático, a mudança de volume, o posicionamento automático das amostras ou o release natural,
* **análise do som**: medidas de loudness, altura, harmônicos, ataque, qualidade do loop… e a renderização de uma nota tocada pelo sintetizador.

As funções de análise são essenciais: **o assistente não pode ouvir**.
Em vez disso, ele mede o som antes e depois de uma alteração, como um engenheiro leria um analisador de espectro.
É assim que ele pode verificar que um loop não estala, que as notas estão afinadas ou que o volume é uniforme em todo o teclado — mas a palavra final é sempre dos seus ouvidos.

Cada alteração feita pelo assistente é uma etapa no histórico da soundfont: :kbd:`Ctrl`+:kbd:`Z` no Polyphone a desfaz.
Nada é gravado no disco enquanto você não pedir ao assistente que salve a soundfont, ou enquanto você mesmo não a salvar.


## Configuração {#doc_setup}


### Ativar o servidor no Polyphone


Abra as [configurações](manual/settings.md#doc_ai_assistant) e vá até a seção :guilabel:`Assistente de IA`.
Marque :guilabel:`Permitir que assistentes de IA acessem as soundfonts abertas`: a linha abaixo mostra então o endereço do servidor, http://127.0.0.1:8642/mcp por padrão.
Se a porta 8642 já estiver sendo usada por outro programa, escolha outra com :guilabel:`Porta`.

O servidor só aceita conexões vindas do seu próprio computador.
A mesma página lista todas as funções oferecidas ao assistente, agrupadas em quatro categorias: :guilabel:`Navegação`, :guilabel:`Edição`, :guilabel:`Ferramentas` e :guilabel:`Análise do som`.


### Conectar o Claude Code


O <a href="https://claude.com/claude-code" target="_blank">Claude Code</a> é um assistente criado pela Anthropic que funciona em um terminal (ele também existe como extensão para os editores VS Code e JetBrains).
Depois de instalá-lo, copie o comando mostrado nas configurações do Polyphone com o botão :guilabel:`Copiar` e execute-o em um terminal:

```
claude mcp add --transport http polyphone http://127.0.0.1:8642/mcp
```

Por padrão, o Claude Code só usa o servidor quando é iniciado a partir da pasta em que esse comando foi executado.
Para usar o Polyphone a partir de qualquer pasta, adicione a opção `--scope user`:

```
claude mcp add --scope user --transport http polyphone http://127.0.0.1:8642/mcp
```

Em seguida, inicie uma nova sessão do Claude Code: as funções do Polyphone são carregadas quando uma sessão começa, e o Polyphone já precisa estar aberto com o servidor ativado.
O comando `/mcp` no Claude Code mostra se o servidor “polyphone” está conectado.
Na primeira vez que o assistente quiser usar uma função do Polyphone, o Claude Code pedirá sua permissão, e você pode escolher não ser mais consultado para essa função.

**Observação:** outros assistentes compatíveis com MCP pelo transporte HTTP podem se conectar ao mesmo endereço.


### Primeiras perguntas


Comece com perguntas simples para ver como o assistente explora as soundfonts:

* *“Quais soundfonts estão abertas no Polyphone?”*
* *“O que esta soundfont contém? Há amostras que não são usadas por nenhum instrumento?”*
* *“Me dê a lista dos presets com seus números de banco e de preset.”*

Quando você seleciona um elemento na árvore do Polyphone, o assistente pode se referir a ele: *“Abaixe em 6 dB o volume da amostra selecionada”* ou *“Renomeie estes instrumentos com o prefixo ‘Órgão’”*.
Nas pequenas alterações, o resultado é imediato e visível no editor.


## Exemplo: verificar e corrigir um instrumento {#doc_check}


É fácil cometer erros ao montar um instrumento com muitas amostras, e alguns são difíceis de perceber: uma nota ligeiramente desafinada, uma amostra um pouco mais forte que as vizinhas, um loop que só estala depois de alguns segundos…
O assistente pode verificar um instrumento inteiro em poucos segundos.

Para experimentar você mesmo, baixe <a href="files/flute-with-mistakes.sf2" download>flute-with-mistakes.sf2</a> e abra o arquivo no Polyphone.
Ele contém um instrumento de flauta no qual alguns erros foram introduzidos de propósito.
Ouça uma escala de Dó4 a Fá6 seguida de um Sol6 longo:

![](files/ai-assistant-flute-before.mp3)

Depois peça:

*“Verifique o instrumento ‘Flute’: há notas desafinadas, fortes ou fracas demais, teclas sem som ou loops ruins?”*

O assistente analisa o instrumento (uma única função mede o loudness, a altura, o timbre e o loop de cada divisão) e depois examina em detalhe as amostras suspeitas.
A resposta lista os problemas:

* a tecla Mi4 não toca nada: a divisão de “Flute 63” termina em Ré♯4 e a seguinte começa em Fá4,
* “Flute 69” soa um semitom acima: ela toca um Lá4 (440 Hz), mas sua nota base é Sol♯4,
* “Flute 78” está 30 cents acima: sua correção é de +30 cents, enquanto a gravação está afinada,
* a divisão de “Flute 84” é 6 dB mais forte que as outras (teclas Si5 a Dó♯6),
* o loop de “Flute 90” estala: as formas de onda não coincidem no ponto de loop e o comprimento do loop não é um número inteiro de períodos.

Peça então: *“Corrija esses problemas.”*

O assistente estende o alcance de notas de “Flute 63” até Mi4, define a nota base de “Flute 69” como Lá4 e a correção de “Flute 78” como 0, dá uma atenuação de 6 dB à divisão de “Flute 84” (a amostra em si não é alterada) e usa a ferramenta [Loop automático](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop) em “Flute 90”.
Uma nova análise confirma o resultado: nenhuma tecla sem som, nenhuma nota desafinada, o mesmo volume em toda parte e um loop avaliado como “bom”.
A mesma escala depois das correções:

![](files/ai-assistant-flute-after.mp3)

O assistente explica o que faz, e muitas vezes precisa fazer escolhas: aqui, ele atenuou uma divisão em vez de alterar os dados de áudio da amostra.
Se você preferir outra solução, basta dizer — e você pode revisar cada alteração no editor, ou desfazê-la.


## Exemplo: criar um instrumento do zero {#doc_create}


Um assistente como o Claude Code também pode executar programas no seu computador, por exemplo para calcular amostras.
Combinado com as funções do Polyphone, isso permite montar um instrumento inteiro a partir de uma descrição.
A flauta do exemplo anterior foi feita assim, a partir de dois pedidos:

*“Você pode me fazer um som de flauta para a tecla 60?”*

*“Agora faça uma flauta completa com vários níveis de intensidade, em uma nova soundfont.”*

Veja o que o assistente fez, passo a passo:

1. mediu uma flauta gravada em uma soundfont General MIDI para conhecer o timbre do instrumento em cada registro (harmônicos, ruído de sopro, ataque, vibrato),
2. escreveu um programa que sintetiza 39 amostras: 13 notas de Dó4 a Dó7, uma a cada 3 teclas, cada uma em três dinâmicas — *p* é mais escura e com mais sopro, com um ataque suave, *f* é mais rica em harmônicos, com um ataque mais marcado,
3. importou as amostras em uma nova soundfont, ajustou suas notas base e seus loops, e criou um instrumento por dinâmica com um release de 0,4 s,
4. empilhou os três instrumentos em um preset (banco 0, preset 73, a flauta do padrão General MIDI) com os alcances de velocidades 0-49, 50-89 e 90-127, como descrito no tutorial “[Instrumentos realistas com várias camadas de velocidade](tutorials/realistic-instruments-with-multiple-velocity-layers.md)”,
5. renderizou notas dos dois lados de cada limite de velocidade: o volume muda apenas de 0,1 a 0,4 dB, de modo que só o timbre muda de uma camada para a seguinte.

A última etapa revelou um ponto fraco: na oitava mais aguda, as três camadas tinham quase o mesmo timbre.
O assistente modificou seu programa para que as notas fortes também fiquem mais brilhantes no registro agudo, e depois gerou as amostras e montou o instrumento novamente.

Ouça a tecla Dó4 tocada com as velocidades 35, 70 e 110 — uma nota por camada:

![](files/ai-assistant-flute-layers.mp3)

E uma frase cada vez mais forte, em que o timbre acompanha a dinâmica:

![](files/ai-assistant-flute-phrase.mp3)

Uma flauta sintetizada nunca vai se igualar a uma boa gravação: este exemplo mostra mais o método do que o som.
Com as suas próprias gravações, vale o mesmo tipo de pedido: *“Importe os arquivos wav desta pasta, encontre a nota base de cada um, crie os loops e monte um instrumento com uma camada por dinâmica.”*


## Outras ideias {#doc_ideas}


Aqui estão outros pedidos que funcionam bem:

* *“Defina um release natural para todos os instrumentos, mais longo nos graves.”*
* *“As notas agudas deste instrumento estão brilhantes demais: suavize-as gradualmente a partir do Dó6.”*
* *“Crie um registro de voz celeste a partir deste instrumento de órgão, com 3 batimentos por segundo no Dó4.”*
* *“Converta todas as amostras estéreo em mono e atualize os instrumentos.”*
* *“Compare estas duas amostras: por que a segunda soa mais abafada?”*
* *“Explique os moduladores deste preset em palavras simples.”*
* *“Remova as amostras não usadas e as duplicadas, depois salve a soundfont no formato sf3.”*


## Dicas {#doc_tips}


* **Seja preciso quanto ao escopo**: “só o instrumento ‘Piano’”, “não altere as amostras, só os instrumentos”…
* **Peça medidas**: “verifique o resultado” ou “compare antes e depois” faz o assistente conferir o próprio trabalho com as funções de análise.
* **Avance passo a passo** nas alterações grandes, e ouça entre as etapas.
* **Mantenha uma cópia** da sua soundfont e só salve quando estiver satisfeito: peça ao assistente para salvar, ou use :kbd:`Ctrl`+:kbd:`S`.
* **Soundfonts grandes** levam mais tempo para ser analisadas: concentre-se em um instrumento ou em um preset em vez da soundfont inteira.
* **Privacidade**: o assistente recebe os nomes, os parâmetros, as medidas e imagens de formas de onda, não o áudio em si; mas esses dados são enviados ao serviço do assistente, como o resto da conversa.
