Uno strumento costruito in un soundfont è composto da campioni, provenienti ad esempio dalle registrazioni di uno strumento reale.
Questo tutorial descrive come aggiungere e preparare un campione in un soundfont.


## Importazione {#doc_import}


Per prima cosa devi avere dei campioni sul tuo computer.
Su internet si trovano molti campioni gratuiti, registrati da strumenti reali (flauto, chitarra, violoncello, …) oppure creati con diversi tipi di sintesi audio.

Per questo tutorial viene fornito questo campione di flauto: <a href="files/flute.wav" download>flute.wav</a>.

Una volta ottenuto, crea un nuovo soundfont con Polyphone (:menuselection:`Menu --> Nuovo`) e importa il campione (:guilabel:`Aggiungi un campione` nella [barra degli strumenti](manual/soundfont-editor/toolbar.md#doc_edit)).


![Importazione del campione](images/import-sample.png "Importazione del campione")


## Loop {#doc_loop}


### Introduzione


Un loop è una zona all'interno di un campione che può essere ripetuta per allungare la durata del campione.
Supponiamo che un campione duri solo 0,5 secondi: con un buon loop il suono può essere prolungato fino a 10 secondi o più.
Questo è utile solo per i campioni di strumenti non percussivi, come flauti, trombe, organi o violini.
Al contrario, la batteria non richiede loop.
Lo stesso può valere per un pianoforte o una chitarra, a seconda di come vengono sintetizzati questi suoni.

Questa sezione è quindi necessaria solo se senti il bisogno di allungare la durata originale di un campione.
Altrimenti, passa direttamente alla sezione successiva «Accordatura».


### Configurare un loop


Nell'albero, fai clic sul campione che vuoi mettere in loop e concentrati sui parametri del loop.


![Loop del campione: parametri del loop](images/loop-sample-1.png "Loop del campione: parametri del loop")


Se sei fortunato, questi valori erano già inclusi nel campione .wav e i campi sono già compilati.
In caso contrario, o se sono configurati male (può succedere…), fai clic a piacere a sinistra e a destra sul grafico (area di visualizzazione WAV) per definire l'inizio e la fine del loop.
Di solito il loop si colloca nella fase stabile del suono, non durante l'attacco o il release.
Poi ingrandisci attorno all'inizio del loop e cerca di sovrapporre la curva verde (o rossa) e la curva blu.


![Loop del campione: allineamento delle onde](images/loop-sample-2.png "Loop del campione: allineamento delle onde")


### Ascoltare il risultato


Quando il loop è configurato, premi :guilabel:`Riproduci` nel lettore con l'opzione :guilabel:`loop` attiva per verificare il risultato.


![Loop del campione: verifica del risultato](images/loop-sample-3.png "Loop del campione: verifica del risultato")


Se non senti la transizione quando viene raggiunta la fine del loop, il loop è buono.
Altrimenti, lascia il lettore in funzione e sposta leggermente la fine e / o l'inizio del loop finché non trovi dei punti con una transizione il più fluida possibile.
Ascolta le registrazioni seguenti per sentire la differenza tra un loop cattivo e uno buono:

* <a href="files/bad-loop.mp3" download>loop cattivo</a>

![](files/bad-loop.mp3)

* <a href="files/good-loop.mp3" download>loop buono</a>

![](files/good-loop.mp3)


**Nota:** esiste una funzione per «barare» se non riesci a trovare facilmente un loop: la funzione [Auto loop](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop).
Crea automaticamente un loop tagliando il campione e sovrapponendo due sezioni, in modo che la fine della sezione corrisponda esattamente al suo inizio.
Per maggiori informazioni consulta la relativa documentazione.


## Accordatura {#doc_tuning}


Tutti i campioni devono essere accordati se non vuoi uno strumento stonato.
Indicando nel soundfont l'altezza esatta di tutti i campioni importati, il soundfont potrà adattarli ai tasti.
Alcuni campioni possono già contenere questa informazione: in tal caso non dovresti dover modificare i valori di :guilabel:`Nota di base` e :guilabel:`Correzione`.
Per precauzione, però, è sempre bene controllare.


![Accordatura del campione: informazioni sull'altezza](images/tune-sample-1.png "Accordatura del campione: informazioni sull'altezza")


Attiva il loop se il campione ne ha uno e premi :guilabel:`Riproduci` nel lettore.
Poi attiva la calibrazione :guilabel:`sinusoide` e regola il volume in modo da sentire entrambi i suoni il più chiaramente possibile.


![Accordatura del campione: riproduzione del campione insieme al segnale di calibrazione](images/tune-sample-2.png "Accordatura del campione: riproduzione del campione insieme al segnale di calibrazione")


Inizia l'accordatura cambiando la nota di base finché i due suoni non si avvicinano il più possibile.
Continua regolando la correzione.
In questo passaggio, presta attenzione agli eventuali battimenti e fai in modo che siano il più lenti possibile.
Ecco alcuni esempi con il suono di un flauto e una sinusoide di calibrazione:

* <a href="files/very-bad-tune.mp3" download>accordatura pessima</a> (la nota di base non corrisponde)

![](files/very-bad-tune.mp3)

* <a href="files/bad-tune.mp3" download>accordatura cattiva</a> (la correzione non è giusta e si sentono battimenti)

![](files/bad-tune.mp3)

* <a href="files/good-tune.mp3" download>accordatura buona</a> (entrambi i valori sono giusti)

![](files/good-tune.mp3)


**Nota:** l'[analisi delle frequenze](manual/soundfont-editor/editing-pages/sample-editor.md#doc_freq) può essere un buon riferimento per iniziare l'accordatura. Ricorda però che nessun algoritmo è perfetto per questo compito e che solo le tue orecchie troveranno l'accordatura giusta.


![Accordatura del campione: analisi delle frequenze](images/tune-sample-3.png "Accordatura del campione: analisi delle frequenze")


## Per andare oltre {#doc_further}


Nel menu :guilabel:`Toolbox` della [barra degli strumenti](manual/soundfont-editor/toolbar.md#doc_edit) sono disponibili diverse funzioni.
Puoi modificare i campioni per:

* [rimuovere il silenzio](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank) all'inizio,
* [equalizzare](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq) il suono,
* [trasporre](manual/soundfont-editor/tools/sample-tools.md#doc_transpose),
* [normalizzare](manual/soundfont-editor/tools/sample-tools.md#doc_volume) il volume,
* [regolare il bilanciamento](manual/soundfont-editor/tools/sample-tools.md#doc_balance) dei campioni stereo.


## Nota sui campioni stereo {#doc_stereo}


I campioni stereo sono composti da due campioni mono.
Il collegamento è memorizzato nelle informazioni di un campione.


![Campione stereo](images/stereo-sample.png "Campione stereo")


Gestire due campioni mono può rendere più difficile creare il loop, perché un loop può suonare bene su un lato e non sull'altro.
In questo caso non esitare a «barare» configurando due loop diversi sul lato destro e su quello sinistro.
La qualità del suono non ne risentirà.
