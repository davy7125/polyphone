Le impostazioni del software sono accessibili dalla [schermata iniziale](manual/index.md) o dal [menu](manual/menu.md#doc_shortcuts) (scorciatoia :kbd:`Ctrl`+:kbd:`,`).
Sono suddivise in cinque sezioni:

* [Generale](#doc_general)
* [Interfaccia](#doc_interface)
* [Suono](#doc_sound)
* [Tastiera](#doc_keyboard)
* [Archivio online](#doc_repository)

Tutte le modifiche vengono salvate immediatamente.


## Generale {#doc_general}


La sezione :guilabel:`Generale` permette di modificare i seguenti parametri:

Uscita audio
: Specifica il sistema audio e il dispositivo usati per riprodurre i suoni.
  I sistemi disponibili dipendono dal sistema operativo (ad esempio WASAPI e ASIO su Windows, ALSA e JACK su Linux, Core Audio su macOS).
  Scegliendo «-» si disattiva l'uscita audio.

Dimensione del buffer
: Specifica la dimensione del buffer audio.
  Un buffer piccolo comporta una latenza ridotta, ma il suono può diventare graffiante.
  Con il valore :guilabel:`Predefinito`, la dimensione dipende dal sistema audio.

Polifonia
: Numero massimo di voci suonate contemporaneamente, da 64 a 4096 (1024 per impostazione predefinita).
  Una voce è un campione in riproduzione: una nota può usare più voci, ad esempio due con un campione stereo.
  Oltre questo numero, alcune note si dissolvono: prima quelle rilasciate, poi le più deboli.
  Le note appena iniziate, così come la nota tenuta più acuta e quella più grave, vengono mantenute il più a lungo possibile.

Carico del processore
: Quota massima del tempo del processore usata per calcolare le voci, dal 20 al 100% (60% per impostazione predefinita).
  Oltre questa quota, alcune note si dissolvono allo stesso modo.
  Un valore elevato permette di suonare più note contemporaneamente, ma il suono può diventare graffiante.

Ingresso MIDI
: Specifica l'ingresso MIDI che controlla il sintetizzatore e la tastiera virtuale.

Inoltre, è possibile spuntare le seguenti opzioni:

Modifica stereo: modifica il campione collegato
: Quando si modifica un lato di un campione stereo, l'altro lato viene modificato automaticamente allo stesso modo.
  Lo stesso vale per la modifica di una divisione stereo di uno strumento: la divisione corrispondente all'altro canale (con gli stessi intervalli di note e di velocità) viene modificata allo stesso modo.

Importazione campioni: taglia alla fine del loop
: Quando si importa un campione, i dati situati dopo la fine del loop vengono rimossi automaticamente (stesso effetto della funzione «[Taglia alla fine del loop](manual/soundfont-editor/tools/sample-tools.md#doc_trimloop)»).

Importazione campioni: rimuovi il silenzio all'inizio
: Quando si importa un campione, qualsiasi zona vuota situata prima dell'inizio dell'attacco viene rimossa automaticamente (stesso effetto della funzione «[Rimuovi silenzio all'inizio](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)»).


![Impostazioni, generale](images/settings_general.png "Impostazioni, generale")


## Interfaccia {#doc_interface}


Questa sezione mostra le opzioni relative all'interfaccia del software:

Lingua
: Qui si può cambiare la lingua.

Nomi delle note
: Le note possono essere indicate con il loro numero (secondo la specifica MIDI) oppure in base al nome del Do centrale (C3, C4 o C5).
  I nomi delle note possono contenere diesis o bemolle.

Istanza unica
: Apre tutti i file in un'unica finestra.

Vista all'avvio
: Seleziona la vista da mostrare all'apertura di un soundfont tra le seguenti: :guilabel:`informazioni generali` (predefinita), :guilabel:`elenco campioni`, :guilabel:`primo campione`, :guilabel:`elenco strumenti`, :guilabel:`primo strumento`, :guilabel:`elenco preset`, :guilabel:`primo preset`.

Ordina le divisioni
: Qui si può cambiare l'ordine delle divisioni all'interno di uno strumento o di un preset: :guilabel:`per intervallo di nota`, :guilabel:`per intervallo di velocità`, :guilabel:`in ordine alfabetico` o :guilabel:`nessuna` (l'ordine di creazione).

Stile
: Lo stile degli elementi dell'interfaccia.
  Lo stile :guilabel:`Fusion` è quello predefinito; a seconda del sistema operativo possono esserne proposti altri (Adwaita, Windows, macOS…).

Colori
: Sono disponibili diversi set di colori predefiniti per personalizzare l'aspetto di Polyphone.
  È anche possibile scegliere manualmente i colori del testo, della finestra, dei pulsanti, della selezione e degli elenchi.
  I colori si possono cambiare solo con gli stili :guilabel:`Fusion` e :guilabel:`Windows`.

Bordi delle finestre
: Mostra o nasconde i bordi delle finestre, per risparmiare spazio sullo schermo (non disponibile su macOS).

Decorazioni
: Le decorazioni di sfondo in alcuni elenchi possono essere disattivate per migliorare la leggibilità.

Una modifica della lingua, dei nomi delle note, dello stile, dei colori, dei bordi delle finestre o delle decorazioni viene applicata al successivo avvio del software.


![Impostazioni, interfaccia](images/settings_interface.png "Impostazioni, interfaccia")


## Suono {#doc_sound}


La sezione :guilabel:`Suono` permette di modificare il riverbero (livello, dimensione della stanza, smorzamento e ampiezza) e il chorus (livello, profondità e frequenza) del sintetizzatore, oltre al volume generale.
Se è collegata una tastiera esterna, il volume generale può essere modificato con il controller MIDI del volume.

Poiché il modulatore predefinito che modifica la frequenza di taglio del filtro è cambiato tra le versioni 2.01 e 2.04 della specifica sf2, qui si può stabilire quale versione usare:

* :guilabel:`SoundFont 2.01`: usa la regola «abbassa la frequenza del filtro in base alla velocità SE la velocità è inferiore a 64» (tra le velocità 63 e 64 si sente un salto netto e problematico);
* :guilabel:`SoundFont 2.04` (predefinito): usa la regola «abbassa la frequenza del filtro in base alla velocità» (nessun salto);
* :guilabel:`disattivato`: questo modulatore predefinito non viene impostato.


![Impostazioni, suono](images/settings_sound.png "Impostazioni, suono")


## Tastiera {#doc_keyboard}


I tasti della tastiera del computer usati per controllare la [tastiera virtuale](manual/soundfont-editor/toolbar.md#doc_keyboard) sono definiti nella sezione :guilabel:`Tastiera`.
L'altezza può essere alzata o abbassata di un'ottava alla volta con l'impostazione :guilabel:`Primo Do`.

Per i musicisti più esperti, qui si può cambiare l'accordatura:

Intonazione di riferimento
: La frequenza del La situato sopra il Do centrale (nota 69), tra 390 e 470 Hz (440 Hz per impostazione predefinita).

Temperamento
: Durante l'esecuzione si può applicare un temperamento diverso dal temperamento equabile.
  Quando si seleziona un temperamento, si può scegliere anche la nota che ne costituisce il riferimento (la sua tonica).

L'intonazione di riferimento e il temperamento possono essere cambiati anche con messaggi MIDI RPN (master fine tuning, tuning program select e tuning bank select).


![Impostazioni, tastiera](images/settings_keyboard.png "Impostazioni, tastiera")


## Archivio online {#doc_repository}


Qui si possono inserire l'indirizzo e-mail e la password di un account per collegare Polyphone a questo account; un [account Premium](premium) sblocca l'intero [archivio online](manual/online-repository.md).
Dopo una connessione riuscita, Polyphone si collega automaticamente all'account a ogni avvio, finché non si usa il pulsante :guilabel:`Disconnetti`.

Qui si può anche cambiare la cartella usata per scaricare i soundfont dall'[archivio online](manual/online-repository.md).


![Impostazioni, archivio](images/settings_repository.png "Impostazioni, archivio")



*[JACK]: JACK Audio Connection Kit
*[ASIO]: Audio Stream Input/Output
*[MIDI]: Musical Instrument Digital Interface
