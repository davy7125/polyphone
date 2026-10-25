Quando nell'[albero](manual/soundfont-editor/tree.md) sono selezionati uno strumento o le sue divisioni, il [menu delle funzioni](manual/soundfont-editor/tools/index.md) offre funzioni suddivise in 4 categorie:

* [analisi](#doc_cat_analyze),
* [modifica veloce](#doc_cat_fast),
* [modulatori](#doc_cat_mod),
* [trasformazione](#doc_cat_transformation).

Fatta eccezione per la creazione di accordi e di mixture e per la visualizzazione dei parametri, è possibile usare le funzioni su più strumenti contemporaneamente (con una selezione multipla).


## Analisi {#doc_cat_analyze} 


### Visualizza parametri {#doc_display}


Questa funzione mostra come varia un parametro in funzione della nota.
La legenda distingue i valori definiti nella divisione globale, i valori definiti in ciascuna divisione e il valore predefinito del parametro.
È disponibile una scala logaritmica (attenzione ai valori minori o uguali a 0!).


![Funzione Visualizza parametri](images/tool_display_parameters.png "Funzione Visualizza parametri")


## Modifica veloce {#doc_cat_fast}


### Cambia attenuazioni {#doc_attenuation}


Questa funzione calcola l'attenuazione minima e massima di tutti gli strumenti selezionati e permette di aggiungere un offset a tutti i valori di attenuazione, senza superare i limiti superiore e inferiore.

Ad esempio, se l'attenuazione più bassa tra le divisioni degli strumenti selezionati è di 16 dB e si vuole un'attenuazione minima di 5 dB (per aumentare il volume complessivo), è possibile ridurre di 11 dB l'attenuazione di tutte le divisioni di tutti gli strumenti.


![Funzione Cambia attenuazioni](images/tool_change_attenuation.png "Funzione Cambia attenuazioni")


### Scordatura {#doc_detune}


Questa funzionalità scorda leggermente le note di uno strumento per creare, insieme a uno strumento accordato correttamente, un effetto ondulante (voce celeste).
L'intensità dell'effetto si esprime in battimenti al secondo, ed è possibile regolare:

* il numero di battimenti al secondo sulla nota 60 (il «Do» al centro della tastiera; il segno di questo numero determina la direzione della scordatura),
* la variazione del numero di battimenti in funzione della nota: per ogni ottava, il numero di battimenti viene diviso o moltiplicato per un certo fattore (con il valore «1», il numero di battimenti resta costante su tutta la tastiera).


![Funzione Scordatura](images/tool_detune.png "Funzione Scordatura")


Esempio di uno strumento <a href="files/celeste_tuning_off.mp3" download>accordato</a>.

![](files/celeste_tuning_off.mp3)

Lo stesso strumento combinato con la sua versione <a href="files/celeste_tuning_on.mp3" download>scordata</a>.

![](files/celeste_tuning_on.mp3)

Il file .sf2 corrispondente può essere scaricato <a href="files/celeste_tuning.sf2" download>qui</a>.


### Configurazione basata su tasti {#doc_global}


Con questa funzione si possono modificare insieme i parametri di tutte le divisioni di uno strumento, in funzione dell'intervallo di note di ciascuna divisione.


![Funzione Configurazione basata su tasti](images/tool_keybased_configuration.png "Funzione Configurazione basata su tasti")


Il grafico rappresenta l'intensità della modifica: il primo valore a sinistra corrisponde alla modifica applicata alla divisione che comprende la nota 0, l'ultimo valore a destra a quella applicata alla divisione che comprende la nota 127.
Il minimo e il massimo del grafico sono regolabili.
Cambiando il modello si può modificare la curva:

Manuale
: La curva si disegna con il mouse.

Lineare ascendente / Lineare discendente
: La curva è una retta.

Esponenziale ascendente / Esponenziale discendente
: La curva è un'esponenziale la cui curvatura è definita dal parametro :guilabel:`Rigidità`.

Casuale
: I valori vengono generati casualmente, secondo il parametro :guilabel:`Distribuzione`.

Con un andamento lineare o esponenziale, un clic sinistro sul grafico definisce l'inizio dell'area lineare o esponenziale e un clic destro ne definisce la fine.

Il parametro da modificare si sceglie in :guilabel:`Parametro` e il tipo di modifica in :guilabel:`Modifica`:

Addizione
: Il valore della modifica viene sommato al valore del parametro selezionato.

Moltiplicazione
: Il valore del parametro selezionato viene moltiplicato per il valore della modifica.

Sostituzione
: Il valore della modifica sostituisce il valore del parametro selezionato.

È anche possibile applicare la modifica solo alle divisioni il cui intervallo di velocità rientra in un certo intervallo, ad esempio nel caso di un pianoforte con più livelli di velocità.


### Release naturale {#doc_release}


Questa funzione crea un release per ogni divisione di uno strumento.
Il tempo di release viene calcolato in base ai primi due parametri (il tempo di release sulla nota 36 e il fattore tra ottave adiacenti, :guilabel:`Divisione all'ottava successiva`); con il terzo parametro (:guilabel:`Scordatura indotta (semitoni)`) si può anche aggiungere una variazione di intonazione.


![Funzione Release naturale](images/tool_natural_release.png "Funzione Release naturale")


### Auto-posizionamento campione {#doc_position}


Il software definisce gli intervalli di note di tutte le divisioni di uno strumento in modo da sfruttare al meglio la tastiera.
Ciò avviene anche automaticamente quando si collegano a uno strumento vuoto più campioni con note di base diverse.


### Spazializzazione del suono {#doc_spatialization}


Questa funzionalità distribuisce i suoni nello spazio in funzione della loro altezza, il che è utile ad esempio per un pianoforte (dai gravi agli acuti, da sinistra a destra).


![Funzione Spazializzazione del suono](images/tool_sound_spatialization.png "Funzione Spazializzazione del suono")


Ogni barra del grafico rappresenta una corda o una canna, la cui lunghezza è legata all'altezza del suono.
La barra più corta corrisponde al suono più acuto, la più lunga al suono più grave.

Ogni nota viene collocata nello spazio: l'estrema sinistra corrisponde a un suono che si sente da sinistra, l'estrema destra a un suono che si sente da destra.
Sono disponibili i seguenti modelli:

Crescente
: Gli acuti si trovano a sinistra, i gravi a destra.

Decrescente
: I gravi si trovano a sinistra, gli acuti a destra.

Cavo
: I gravi si trovano ai lati, gli acuti al centro.

Punta
: Gli acuti si trovano ai lati, i gravi al centro.

Casuale
: Acuti e gravi vengono distribuiti casualmente.

Sono disponibili altre opzioni:

* il numero di ripetizioni del modello (:guilabel:`Numero di divisioni`),
* l'intensità dell'effetto (:guilabel:`Diffusione (%)`),
* il riempimento delle divisioni (:guilabel:`Riempimento (%)`),
* lo spostamento a sinistra o a destra se la diffusione è inferiore al 100% (:guilabel:`Offset (0-100)`),
* :guilabel:`Inversioni`.

Il grafico si aggiorna a ogni modifica delle impostazioni, così il risultato è facile da vedere.
Passando il mouse sul grafico si vede la corrispondenza tra il numero di nota e il pan.


### Trasponi {#doc_transpose}


La funzione Trasponi permette di trasporre un intero strumento.
Il suono dello strumento resta lo stesso, ma cambia la sua posizione sulla tastiera.

Come dato in ingresso è richiesto uno spostamento in semitoni.
Per rendere il suono più acuto, lo spostamento deve essere positivo.
Al contrario, per rendere il suono più grave, lo spostamento deve essere negativo.
La funzione esegue le seguenti operazioni per ogni divisione dello strumento:

* regolazione della nota di base,
* modifica dell'accordatura fine se la trasposizione è di un numero non intero di semitoni,
* spostamento dell'intervallo di note in base al numero di semitoni, se l'opzione :guilabel:`Adatta l'intervallo di note` è spuntata.


![Funzione Trasponi](images/tool_transpose_inst.png "Funzione Trasponi")


## Modulatori {#doc_cat_mod}


### Sovrascrivi un modulatore predefinito {#doc_default_mod}


A ogni strumento vengono assegnati automaticamente dei modulatori predefiniti.
Questa funzione mostra l'elenco di questi modulatori e permette di creare modulatori che sovrascrivono o disattivano quelli selezionati (pulsanti :guilabel:`Sovrascrivi` e :guilabel:`Disabilita`).


![Funzione di sovrascrittura](images/tool_default_mod.png "Funzione di sovrascrittura")


### Rimuovi modulatori {#doc_remove_mod}


Tutti i modulatori di tutti gli strumenti selezionati vengono rimossi.
Un messaggio indica quanti modulatori sono stati rimossi.

**Nota:** i modulatori predefiniti continuano a essere applicati.
L'unico modo per disattivarli è sovrascriverli con la funzione «[Sovrascrivi un modulatore predefinito](#doc_default_mod)».


## Trasformazione {#doc_cat_transformation}


### Crea accordi {#doc_chords}


Questa funzione permette di creare, a partire da uno strumento, tutti gli accordi possibili.
È utile, ad esempio, per creare cori o pad.

Gli accordi si definiscono scegliendo le note che contengono (fondamentale, terza, quinta, settima, nona) e il loro tipo (maggiore, minore, giusta, diminuita, aumentata), il numero di rivolti e l'ottava.
I campioni contenenti gli accordi vengono creati per ogni nota, ogni 3 note oppure ogni 6 note, con o senza auto loop, in mono o in stereo.


![Funzione Crea accordi](images/tool_create_chords.png "Funzione Crea accordi")


Esempio. <a href="files/chord_base.mp3" download>Prima</a>:

![](files/chord_base.mp3)

<a href="files/chord_done.mp3" download>Dopo</a>:

![](files/chord_done.mp3)

Un file .sf2 di esempio basato su un flauto può essere scaricato <a href="files/chords.sf2" download>qui</a>.


### Duplicazione divisione {#doc_division}


Questa operazione duplica tutte le divisioni di uno strumento.
Ad esempio, se un campione collegato a uno strumento ha l'intervallo di note 36-38, il software duplicherà questa divisione in modo che il campione sia assegnato agli intervalli di note 36-36, 37-37 e 38-38.
Ciò permette di modificare finemente, nota per nota, i parametri di un campione, invece di modificare un'intera divisione con gli stessi parametri.

Le divisioni possono essere duplicate anche in base agli intervalli di velocità.
Gli strumenti che non contengono campioni non sono compatibili con questa funzione.


![Funzione Duplicazione divisione](images/tool_division_duplication.png "Funzione Duplicazione divisione")


### Creazione di mixture {#doc_mixture}


Questa funzione, dedicata agli organi, permette di creare una mixture a partire da uno strumento esistente.
Compare una finestra in cui si specificano le divisioni della mixture e, per ciascuna di esse, l'elenco delle file.
I tipi di file possibili sono molto vari e vanno dall'ottava alla 27ª armonica.


![Funzione Creazione di mixture](images/tool_mixture.png "Funzione Creazione di mixture")


La funzione lavora con tutti i campioni associati allo strumento selezionato, gli intervalli definiti per ciascun campione e le relative attenuazioni.
I campioni devono essere accordati indipendentemente dalla definizione dello strumento (nell'[editor dei campioni](manual/soundfont-editor/editing-pages/sample-editor.md)) e la nota di base indicata non deve riflettere il registro a cui appartengono (ad esempio nessuna trasposizione, anche se il suono è un principale 4').

Un'opzione permette di creare campioni con auto loop, un'altra di definire ogni quante note creare i campioni (per ogni nota, ogni 3 note, ogni 6 note).
Infine si può scegliere il tipo di suono creato (mono o stereo).

Esempio. <a href="files/mixture_base.mp3" download>Prima</a>:

![](files/mixture_base.mp3)

<a href="files/mixture_done.mp3" download>Dopo</a>:

![](files/mixture_done.mp3)

Un file .sf2 di esempio basato su un registro d'organo può essere scaricato <a href="files/mixture.sf2" download>qui</a>.
