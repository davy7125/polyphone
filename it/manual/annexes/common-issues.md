Questa pagina elenca i difetti tipici dei soundfont (che possono essere segnalati nelle recensioni) e, per ciascuno di essi, fornisce tecniche e link alla documentazione per correggerli:

* [campioni di scarsa qualità o mal preparati](#doc_sample),
* [accordatura errata](#doc_tuning),
* [inviluppo ADSR inadeguato](#doc_adsr),
* [loop mancanti o imprecisi](#doc_loop),
* [standard MIDI non rispettato](#doc_standard).


## Campioni di scarsa qualità o mal preparati {#doc_sample}


Può capitare che un soundfont non offra strumenti convincenti; se la causa sono campioni di scarsa qualità, l'unica soluzione è sostituirli:

* cercare altri campioni online,
* registrare con un microfono di qualità migliore (o usare software esterni di elaborazione del segnale),
* creare i campioni con altri mezzi.

A volte il problema si può risolvere preparando o usando meglio i campioni esistenti.

### Qualità sonora scarsa, soprattutto negli acuti

Di solito un solo campione audio non basta per creare uno strumento di alta qualità. Un campione ha una propria altezza (tranne le percussioni non intonate), e usarlo su un'ampia estensione di note distorce il suono, soprattutto nelle note acute. Per risolvere questo problema si dovrebbero usare più campioni; se è disponibile un solo campione, puoi:

* duplicare il campione,
* trasporre il campione duplicato di un'ottava, ad esempio con questa [funzione](manual/soundfont-editor/tools/sample-tools.md#doc_transpose),
* filtrare le armoniche acute con questa [funzione](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq) o con [quest'altra](manual/soundfont-editor/tools/sample-tools.md#doc_filter).

I campioni andrebbero poi assegnati a intervalli di note in cui la loro riproduzione non altera il timbro originale.

### Il suono contiene «clic» regolari

Il problema deriva probabilmente da un punto di loop posizionato male, argomento trattato in un'altra [sezione](#doc_loop).

### Campioni tagliati male

Spesso i campioni vengono usati solo in parte, e rimuovere l'eccesso può ridurre la dimensione del file:

* la parte utile di un campione si trova talvolta prima del punto finale del loop: è quindi possibile eliminare la parte del campione successiva alla fine del loop, e questa [funzione](manual/soundfont-editor/tools/sample-tools.md#doc_trimloop) permette di farlo in pochi clic;
* i campioni possono iniziare con del silenzio, che andrebbe rimosso per ridurre la latenza quando si suona una nota (esiste anche una [funzione](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank) per farlo rapidamente);
* infine, alcuni campioni potrebbero semplicemente non essere usati: questo [riepilogo della configurazione](manual/soundfont-editor/configuration-summaries.md#doc_sample) li elenca e questa [funzione](manual/soundfont-editor/tools/global-tools.md#doc_unused) li rimuove automaticamente.


## Accordatura errata {#doc_tuning}


L'accordatura di uno strumento virtuale prevede diversi passaggi, ed è importante seguirli tutti, nell'ordine.

### Accordatura a livello di campione

È il passaggio più importante dell'accordatura ed è descritto in questa [pagina](tutorials/how-to-prepare-a-sample.md#doc_tuning).

### Accordatura a livello di strumento

4 parametri, descritti in questa [pagina](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table), definiscono la trasposizione con cui un campione viene riprodotto: **nota di base**, **accordatura (semitoni)**, **accordatura (centesimi)** e **accordatura (scala)**. Questi valori possono essere modificati durante l'esecuzione dello strumento dai [modulatori](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator), che vanno quindi anch'essi controllati con attenzione.

Se per la divisione di strumento collegata a un campione non sono definiti parametri o modulatori, è importante verificare se ne sono definiti nella divisione globale dello strumento (questa divisione stabilisce la configurazione predefinita per tutte le altre divisioni).

### Accordatura a livello di preset

A livello di preset, 3 parametri influiscono sulla trasposizione di un campione: **accordatura (semitoni)**, **accordatura (centesimi)** e **accordatura (scala)**. Questi valori si sommano alla configurazione degli strumenti usati nelle divisioni del preset.

Come gli strumenti, anche i preset possono contenere modulatori, e la loro divisione globale funge da configurazione predefinita da non trascurare.


## Inviluppo ADSR inadeguato {#doc_adsr}


L'inviluppo ADSR rappresenta come evolve il volume quando si suona una nota, ma anche quando la stessa nota viene rilasciata. Ciascuno dei quattro caratteri della sigla ADSR corrisponde a una fase specifica, e l'inviluppo del volume usato nei soundfont comprende in realtà 6 fasi:

* delay,
* attack (A),
* hold,
* decay (D),
* sustain (S),
* release (R).

Queste fasi sono descritte più in dettaglio in questa [pagina](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope); di seguito sono elencati alcuni possibili problemi legati all'inviluppo.

### Un suono inizia in modo troppo brusco

Per ammorbidire l'inizio di una nota, specifica o aumenta la **durata dell'attack** della nota a livello di [strumento](manual/soundfont-editor/editing-pages/instrument-editor.md) o di [preset](manual/soundfont-editor/editing-pages/preset-editor.md).

### Un suono inizia in modo troppo morbido

Al contrario del caso precedente, riduci o elimina la **durata dell'attack** della nota.

### Si percepisce un ritardo prima che il suono inizi

Tre soluzioni possibili e complementari:

* eliminare la **durata del delay** se è specificata a livello di [strumento](manual/soundfont-editor/editing-pages/instrument-editor.md) o di [preset](manual/soundfont-editor/editing-pages/preset-editor.md),
* rimuovere il silenzio all'inizio del campione audio (vedi il taglio dei campioni in questa [pagina](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph) o la funzione di rimozione del silenzio [qui](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)),
* infine, la latenza percepita può essere causata dal server audio usato sul computer (in questo caso non si tratta di un difetto del soundfont, e puoi cambiare l'uscita audio nelle [impostazioni](manual/settings.md#doc_general)).

### Il suono si interrompe all'improvviso

Se il suono si interrompe all'improvviso mentre dovrebbe continuare finché la nota è tenuta, vedi la sezione sul [loop](#doc_loop).

Se il suono si interrompe bruscamente quando si rilascia una nota, specifica o aumenta la **durata del release** a livello di [strumento](manual/soundfont-editor/editing-pages/instrument-editor.md) o di [preset](manual/soundfont-editor/editing-pages/preset-editor.md). Questa [funzione](manual/soundfont-editor/tools/instrument-tools.md#doc_release) permette di configurare questa durata in modo che suoni naturale su tutta l'estensione, dai gravi agli acuti.


## Loop mancanti o imprecisi {#doc_loop}


I loop permettono di produrre un suono di durata infinita a partire da un campione di lunghezza fissa, senza che l'orecchio si accorga delle ripetizioni. I loop sono indispensabili per alcuni tipi di strumenti, come i flauti, ma non per altri, come le percussioni. I loop possono essere usati anche nei campioni di pianoforte per simulare un pedale di risonanza con un campione breve.

### Il suono si interrompe prima del rilascio della nota

Se il suono si interrompe quando dovrebbe continuare, le cause più probabili sono:

* il loop del campione non è definito: in questo caso puoi fare riferimento a questa [pagina](tutorials/how-to-prepare-a-sample.md#doc_loop) per impostare un loop,
* la riproduzione in loop non è attiva: in questo caso devi modificare il parametro **riproduzione in loop** a livello di [strumento](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).

### Il suono contiene «clic» regolari

Molto probabilmente il loop non è corretto; questa [pagina](tutorials/how-to-prepare-a-sample.md#doc_loop) spiega come creare loop corretti.


## Standard MIDI non rispettato {#doc_standard}


Gli standard MIDI impongono una numerazione specifica per gli strumenti contenuti nei soundfont. Ciò permette di scambiare soundfont diversi che seguono lo stesso standard senza differenze significative nei timbri suonati: ad esempio, un pianoforte e un flauto vengono richiamati nello stesso modo. Se una recensione indica che uno standard non è rispettato, ci sono due soluzioni possibili:

* rimuovere l'informazione secondo cui il soundfont è conforme a uno standard,
* oppure verificare che tutti i preset del soundfont siano assegnati correttamente al tipo di strumento appropriato (vedi questa [pagina](manual/annexes/midi-standards.md) per l'elenco degli strumenti) e che siano presenti tutti gli strumenti richiesti dallo standard.
