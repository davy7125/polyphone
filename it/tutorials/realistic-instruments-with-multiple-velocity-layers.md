Molti soundfont riproducono un solo campione per nota: qualunque sia la forza con cui si preme un tasto, si sente sempre la stessa registrazione, solo più forte o più piano.
Gli strumenti acustici non si comportano così.
Un tasto di pianoforte premuto delicatamente dà un suono rotondo e morbido, mentre lo stesso tasto premuto con forza dà un suono più brillante, più ricco di armonici, con un attacco più percussivo.

I livelli di velocità portano questa ricchezza in un soundfont: ogni nota viene registrata con diverse dinamiche, e la registrazione riprodotta viene scelta in base alla velocità della nota.
Questo tutorial prende come esempio il pianoforte, ma il metodo vale per ogni strumento il cui timbro cambia con la forza dell'esecuzione: chitarre, batterie, ottoni, archi…
Si conclude con altri dettagli che rendono uno strumento ancora più realistico.


## Perché un solo campione non basta {#doc_why}


Ascolta il tasto Do4 premuto quattro volte, dal piano al forte (velocità 30, 60, 90 e 120):

* con un <a href="files/velocity-layers-one-layer.mp3" download>solo campione</a>, registrato con una dinamica media,

![](files/velocity-layers-one-layer.mp3)

* con <a href="files/velocity-layers-four-layers.mp3" download>quattro livelli</a>, con la registrazione scelta in base alla velocità.

![](files/velocity-layers-four-layers.mp3)

Con un solo campione, le note piano suonano come note forti sentite da lontano e alla nota più forte manca brillantezza.
Il sintetizzatore abbassa leggermente la frequenza di taglio di un filtro per le note piano (è uno dei modulatori predefiniti), ma siamo lontani dal cambiamento di timbro di un vero pianoforte.
Con quattro livelli, il timbro segue la forza dell'esecuzione.

La differenza è evidente anche in un contesto musicale, qui con arpeggi sempre più forti:

* <a href="files/velocity-layers-phrase-one-layer.mp3" download>un solo campione</a>,

![](files/velocity-layers-phrase-one-layer.mp3)

* <a href="files/velocity-layers-phrase-four-layers.mp3" download>quattro livelli</a>.

![](files/velocity-layers-phrase-four-layers.mp3)


## Preparare i campioni {#doc_samples}


### Registrare i livelli


Ogni nota deve essere registrata con diverse dinamiche, ad esempio *pp* (pianissimo), *mp* (mezzo-piano), *mf* (mezzo-forte) e *ff* (fortissimo).
Con 3 o 4 livelli il miglioramento è già evidente, mentre le migliori librerie di pianoforte usano da 8 a 16 livelli per transizioni più morbide.
Tutte le registrazioni devono essere fatte nelle stesse condizioni (microfoni, distanza, guadagno): deve cambiare solo la forza dell'esecuzione.

L'esempio di questo tutorial usa 4 livelli, con un campione ogni 3 tasti su tutta l'estensione del pianoforte.


### Importazione


Dai ai file un nome con la nota e la dinamica, ad esempio :file:`C4 pp.wav`, :file:`C4 mp.wav`, :file:`C4 mf.wav` e :file:`C4 ff.wav`.
Quando la nota di base non è memorizzata in un file audio, Polyphone la trova nel nome del file se questo contiene il nome della nota in notazione inglese (C4 per il Do4, tasto 60) o il numero del tasto.
La dinamica scritta nel nome sarà utile nel passaggio successivo.

Importa tutti i file con :guilabel:`Aggiungi un campione` nella [barra degli strumenti](manual/soundfont-editor/toolbar.md#doc_edit), oppure con un [trascinamento](manual/soundfont-editor/tree.md#doc_dragdrop) nell'albero.
Un file stereo dà due campioni, i cui nomi terminano con «L» e «R».
Controlla poi l'accordatura di ogni campione come spiegato nel tutorial «[Come preparare un campione](tutorials/how-to-prepare-a-sample.md#doc_tuning)».
I campioni di pianoforte non hanno bisogno di alcun loop: il suono si spegne in modo naturale.


## Uno strumento per livello {#doc_instruments}


Ogni livello diventa uno strumento che contiene i campioni di una sola dinamica.
Per ogni dinamica:

1. scrivi il suo nome (ad esempio «pp») nella [barra di ricerca](manual/soundfont-editor/tree.md#doc_search) dell'albero, in modo che vengano mostrati solo i campioni di questo livello,
2. seleziona tutti questi campioni: fai clic sul primo e poi sull'ultimo tenendo premuto il tasto :kbd:`Shift`,
3. fai clic su :guilabel:`Aggiungi uno strumento` nella [barra degli strumenti](manual/soundfont-editor/toolbar.md#doc_edit), dai un nome allo strumento (ad esempio «Piano pp») e lascia selezionata l'opzione :guilabel:`Collega i campioni selezionati`.

I campioni stereo vengono posizionati automaticamente a sinistra e a destra, e gli intervalli di note vengono distribuiti sulla tastiera in base alle note di base (vedi lo strumento «[Auto-posizionamento campione](manual/soundfont-editor/tools/instrument-tools.md#doc_position)»).
Ogni strumento può già essere suonato da solo con la [tastiera virtuale](manual/soundfont-editor/toolbar.md#doc_keyboard).

I parametri comuni a tutti i livelli possono essere modificati in una volta sola: quando i quattro strumenti sono selezionati nell'albero, la [tabella](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) mostra le loro divisioni globali una accanto all'altra, e un valore digitato dopo aver selezionato le quattro celle di una riga viene scritto in tutte.
Nell'esempio, il parametro :guilabel:`Vol env release (s)` è impostato a 1 s, in modo che il suono non si interrompa bruscamente quando si rilascia un tasto.


## Sovrapporre i livelli in un preset {#doc_preset}


Seleziona i quattro strumenti nell'albero e fai clic su :guilabel:`Aggiungi un preset`, con l'opzione :guilabel:`Collega gli strumenti selezionati` selezionata.
Il preset contiene allora quattro divisioni, ognuna delle quali copre tutta la tastiera.
I livelli si sovrappongono dando a ogni divisione del preset un :guilabel:`Intervallo di velocità` diverso nell'[editor dei preset](manual/soundfont-editor/editing-pages/preset-editor.md):

* 0-39 per il livello *pp*,
* 40-69 per il livello *mp*,
* 70-99 per il livello *mf*,
* 100-127 per il livello *ff*.

Gli intervalli devono coprire tutte le velocità da 0 a 127, senza buchi (alcune velocità resterebbero mute) né sovrapposizioni (si sentirebbero due livelli contemporaneamente).
I limiti non devono per forza essere regolari: possono essere spostati in base alle registrazioni.
I valori della riga :guilabel:`Attenuazione (dB)` sono spiegati nella sezione successiva.


![Livelli di velocità nella tabella del preset](images/velocity_layers_table.png "Livelli di velocità nella tabella del preset")


Nell'[editor degli intervalli](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range), ogni livello è una fascia orizzontale: la divisione selezionata (qui *mp*) è tratteggiata.
Questa vista mostra rapidamente un buco o una sovrapposizione.


![Livelli di velocità nell'editor degli intervalli](images/velocity_layers_ranges.png "Livelli di velocità nell'editor degli intervalli")


**Nota:** gli intervalli di velocità possono anche essere definiti in un unico strumento che contiene tutti i campioni, e ogni divisione ha il proprio intervallo di velocità.
In questo caso, la scelta :guilabel:`per intervallo di velocità` dell'opzione :guilabel:`Ordina le divisioni` nelle [impostazioni](manual/settings.md#doc_interface) mantiene insieme le divisioni di uno stesso livello.


## Bilanciare i livelli {#doc_balance}


All'interno di un livello, il volume cambia già con la velocità: se ne occupa un altro modulatore predefinito, definito dal formato sf2.
Ma a ogni limite il volume aumenta di colpo, perché le registrazioni del livello superiore sono più forti: circa da 5 a 7 dB nell'esempio.
L'obiettivo è che passando da un livello al successivo cambi solo il timbro, con un volume che aumenta gradualmente.

Usa la [tastiera virtuale](manual/soundfont-editor/toolbar.md#doc_keyboard) per confrontare i due lati di un limite: la velocità di una nota dipende dalla posizione verticale del clic, e le frecce :kbd:`↑` e :kbd:`↓` suonano di nuovo l'ultimo tasto con una velocità aumentata o diminuita di 1.
Poi, per ogni limite, dal basso verso l'alto:

1. suona un tasto con l'ultima velocità di un livello (ad esempio 39) e poi con la prima velocità del livello successivo (40),
2. aumenta l'attenuazione del livello superiore nella tabella del preset finché le due note non hanno all'incirca lo stesso volume,
3. passa al limite successivo: le attenuazioni si sommano, e ogni livello è attenuato almeno quanto il livello inferiore.

Nell'esempio, le attenuazioni sono di 0, 6, 11 e 16 dB, come si vede nella tabella qui sopra.
Controlla diversi tasti su tutta la tastiera: se un registro resta sbilanciato, l'attenuazione delle divisioni interessate può essere affinata nello strumento del livello, ad esempio con lo strumento «[Configurazione basata su tasti](manual/soundfont-editor/tools/instrument-tools.md#doc_global)».

La gamma dinamica complessiva dello strumento, dalla nota più piano alla più forte, è quindi data dal modulatore predefinito.
Se le note piano sono troppo deboli o non abbastanza, questo modulatore può essere modificato con lo strumento «[Sovrascrivi un modulatore predefinito](manual/soundfont-editor/tools/instrument-tools.md#doc_default_mod)».

Infine, niente di meglio di un vero brano musicale per provare il risultato: suona il soundfont con una tastiera MIDI o con il [lettore MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player) (vedi il tutorial «[Riprodurre un file MIDI con un soundfont](tutorials/read-midi-file-with-soundfont.md)»).


## Più dettagli per più realismo {#doc_details}


I livelli di velocità sono un grande passo avanti, ma uno strumento reale ha altre caratteristiche che un soundfont può riprodurre.
Eccone due per il pianoforte.


### Rumori al rilascio


Quando si rilascia un tasto, lo smorzatore ricade sulle corde e la meccanica produce un piccolo rumore.
Alcune librerie di campioni forniscono questi rumori: quella usata nell'esempio ne ha uno per tasto.

Vengono messi in un quinto strumento, «Piano release», aggiunto al preset su tutto l'intervallo di velocità:

* seleziona la modalità di riproduzione ![release](images/sample_mode_release.png "release"){.inline} (release) nella riga :guilabel:`Riproduzione in loop`, in modo che un campione venga riprodotto quando si rilascia il tasto,
* usa un valore lungo per :guilabel:`Vol env release (s)` (5 s nell'esempio), in modo che il rumore venga riprodotto per intero,
* attenua molto lo strumento (40 dB nell'esempio): il rumore deve restare discreto.

Grazie al modulatore predefinito che collega la velocità al volume, il rumore è più forte quando la nota è stata suonata con più forza.

**Nota:** la modalità di riproduzione «release» non è documentata dal formato sf2 e potrebbe non essere supportata da altri lettori.
Il tutorial «[Usare release personalizzati in uno strumento](tutorials/using-custom-releases-in-an-instrument.md)» descrive un metodo che funziona con qualsiasi lettore.


### Corde senza smorzatori


Su un pianoforte, le corde più acute non hanno smorzatori (all'incirca l'ultima ottava e mezza): continuano a suonare dopo il rilascio del tasto.
In ogni strumento di livello, alle divisioni di questi tasti (dal Fa6, tasto 89, nell'esempio) viene quindi dato un valore più lungo di :guilabel:`Vol env release (s)`: 3 s invece di 1 s.

Ascolta note brevi al centro della tastiera, poi nell'ottava più acuta e infine un accordo nei bassi:

* <a href="files/velocity-layers-without-details.mp3" download>senza questi dettagli</a>,

![](files/velocity-layers-without-details.mp3)

* <a href="files/velocity-layers-with-details.mp3" download>con i rumori al rilascio e le corde senza smorzatori</a>.

![](files/velocity-layers-with-details.mp3)


### Altre idee


Vale la pena provare altri miglioramenti:

* registrare ogni tasto invece di un tasto su tre: su un pianoforte ogni tasto ha le proprie corde, e un campione trasposto non è mai esattamente la nota vicina,
* usare registrazioni stereo, che danno ampiezza al suono,
* calcolare tempi di release che cambiano lungo la tastiera con lo strumento «[Release naturale](manual/soundfont-editor/tools/instrument-tools.md#doc_release)»,
* regolare il volume o il filtro per registro con lo strumento «[Configurazione basata su tasti](manual/soundfont-editor/tools/instrument-tools.md#doc_global)».

E lo stesso approccio funziona per altri strumenti: una corda di chitarra pizzicata piano o forte, un rullante tra ghost note e accenti, una tromba che passa da un suono rotondo a un suono squillante…
Ogni dettaglio preso dallo strumento reale rende il soundfont più vivo.
