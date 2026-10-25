I riepiloghi della configurazione offrono una panoramica di tutti gli elementi contenuti in un soundfont.
Sono accessibili tramite le intestazioni :guilabel:`Campioni`, :guilabel:`Strumenti` e :guilabel:`Preset` nell'[albero](manual/soundfont-editor/tree.md) e permettono di visualizzare gli elementi corrispondenti.

Le informazioni raccolte in queste pagine possono essere utili per verificare soundfont di grandi dimensioni.

Ogni elemento è mostrato su una riga di una tabella.
Un clic sull'intestazione di una colonna ordina la tabella in base a quella colonna, e un doppio clic su una riga apre l'elemento corrispondente nella sua [pagina di modifica](manual/soundfont-editor/editing-pages/index.md).
I valori che meritano attenzione sono scritti in grassetto, in giallo (avviso) o in rosso (errore).


## Campioni {#doc_sample}


Facendo clic sull'intestazione :guilabel:`Campioni` di un soundfont nell'[albero](manual/soundfont-editor/tree.md), compare il riepilogo della configurazione dei campioni.


![Riepilogo della configurazione dei campioni](images/conf_summary_sample.png "Riepilogo della configurazione dei campioni")


Per ogni campione, le colonne sono: :guilabel:`Utilizzato`, :guilabel:`Durata totale`, :guilabel:`Durata del loop`, :guilabel:`Nota di base`, :guilabel:`Correzione`, :guilabel:`Tipo di campione`, :guilabel:`Campione collegato`, :guilabel:`Frequenza di campionamento` e :guilabel:`Compressione`.
Questa pagina permette ad esempio di effettuare i seguenti controlli:

* Il campione è utilizzato?
* La sua durata totale (ed eventualmente la durata del loop) è sufficiente?
* Il loop è corretto? (un loop che potrebbe produrre un clic viene evidenziato)
* È stata fatta l'accordatura?
* È collegato correttamente in stereo? (i due campioni di una coppia stereo dovrebbero avere la stessa nota di base)
* La frequenza di campionamento è abbastanza alta per una buona qualità audio?
* È stato compresso?


## Strumenti {#doc_instrument}


Facendo clic sull'intestazione :guilabel:`Strumenti` di un soundfont nell'[albero](manual/soundfont-editor/tree.md), compare il riepilogo della configurazione degli strumenti.


![Riepilogo della configurazione degli strumenti](images/conf_summary_instrument.png "Riepilogo della configurazione degli strumenti")


Per ogni strumento, le colonne sono: :guilabel:`Utilizzato`, :guilabel:`Numero di campioni`, :guilabel:`Numero di parametri`, :guilabel:`Numero di modulatori`, :guilabel:`Intervallo massimo di note`, :guilabel:`Intervallo massimo di velocità`, :guilabel:`Attenuazione`, :guilabel:`Riproduzione in loop`, :guilabel:`Chorus` e :guilabel:`Riverbero`.
Questa pagina permette ad esempio di effettuare i seguenti controlli:

* Lo strumento è utilizzato?
* Il numero di campioni collegati, di parametri e di modulatori è coerente?
* Gli intervalli di note e di velocità sono corretti?
* I campioni sono stati attenuati correttamente?
* La riproduzione in loop è attiva?
* Sono state dimenticate le impostazioni di chorus e riverbero?


## Preset {#doc_preset}


Facendo clic sull'intestazione :guilabel:`Preset` di un soundfont nell'[albero](manual/soundfont-editor/tree.md), compare il riepilogo della configurazione dei preset.


![Riepilogo della configurazione dei preset](images/conf_summary_preset.png "Riepilogo della configurazione dei preset")


Per ogni preset, le colonne sono: :guilabel:`Banco - Preset`, :guilabel:`Numero di strumenti`, :guilabel:`Numero di parametri`, :guilabel:`Numero di modulatori`, :guilabel:`Intervallo massimo di note`, :guilabel:`Intervallo massimo di velocità`, :guilabel:`Attenuazione`, :guilabel:`Chorus` e :guilabel:`Riverbero`.
Questa pagina permette ad esempio di effettuare i seguenti controlli:

* I numeri di banco e di preset sono stati compilati correttamente?
* Il numero di strumenti collegati, di parametri e di modulatori è coerente?
* Gli intervalli di note e di velocità sono corretti?
* Gli strumenti sono stati attenuati con precisione?
* Sono state dimenticate le impostazioni di chorus e riverbero (come offset)?