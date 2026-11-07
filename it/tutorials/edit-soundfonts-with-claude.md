Polyphone include un server che segue il Model Context Protocol (MCP), uno standard che permette agli assistenti IA di usare altri programmi.
Una volta attivato, un assistente come Claude può lavorare sui soundfont aperti in Polyphone: descrivi ciò che vuoi con parole tue, e l'assistente esplora i soundfont, li modifica, usa gli strumenti di Polyphone e misura il risultato.

Ogni modifica compare subito nell'editor e può essere annullata, come se l'avessi fatta tu.
Questo tutorial mostra come collegare Claude a Polyphone, poi dà esempi di ciò che si può chiedere, con file audio per ascoltare i risultati.


## Cosa può fare l'assistente {#doc_principles}


L'assistente ha accesso a tutto ciò che è aperto in Polyphone:

* **esplorazione**: i soundfont, i loro campioni, strumenti e preset, i parametri di ogni divisione e gli elementi selezionati nell'[albero](manual/soundfont-editor/tree.md) (in modo che «questo campione» o «gli strumenti selezionati» vengano capiti),
* **modifica**: creare, rinominare, duplicare o eliminare elementi, cambiare parametri, intervalli di note e di velocità, punti di loop, modulatori…
* **strumenti**: la maggior parte degli strumenti del [menu delle funzioni](manual/soundfont-editor/tools/index.md), come l'auto loop, la modifica del volume, l'auto-posizionamento dei campioni o il release naturale,
* **analisi del suono**: misure di loudness, altezza, armonici, attacco, qualità del loop… e il rendering di una nota suonata dal sintetizzatore.

Le funzioni di analisi sono essenziali: **l'assistente non può sentire**.
Al loro posto misura il suono prima e dopo una modifica, come un tecnico leggerebbe un analizzatore di spettro.
Per questo può verificare che un loop non faccia clic, che le note siano intonate o che il volume sia uniforme su tutta la tastiera, ma l'ultima parola spetta sempre alle tue orecchie.

Ogni modifica fatta dall'assistente è un passo nella cronologia del soundfont: :kbd:`Ctrl`+:kbd:`Z` in Polyphone la annulla.
Nulla viene scritto sul disco finché non chiedi all'assistente di salvare il soundfont, o finché non lo salvi tu.


## Configurazione {#doc_setup}


### Attivare il server in Polyphone


Apri le [impostazioni](manual/settings.md#doc_ai_assistant) e vai alla sezione :guilabel:`Assistente IA`.
Seleziona :guilabel:`Consenti agli assistenti IA di accedere ai soundfont aperti`: la riga sottostante mostra allora l'indirizzo del server, http://127.0.0.1:8642/mcp per impostazione predefinita.
Se la porta 8642 è già usata da un altro programma, scegline un'altra con :guilabel:`Porta`.

Il server accetta solo connessioni provenienti dal tuo computer.
La stessa pagina elenca tutte le funzioni offerte all'assistente, raggruppate in quattro categorie: :guilabel:`Esplorazione`, :guilabel:`Modifica`, :guilabel:`Strumenti` e :guilabel:`Analisi del suono`.


### Collegare Claude Code


<a href="https://claude.com/claude-code" target="_blank">Claude Code</a> è un assistente creato da Anthropic che funziona in un terminale (esiste anche come estensione per gli editor VS Code e JetBrains).
Una volta installato, copia il comando mostrato nelle impostazioni di Polyphone con il pulsante :guilabel:`Copia` ed eseguilo in un terminale:

```
claude mcp add --transport http polyphone http://127.0.0.1:8642/mcp
```

Per impostazione predefinita, Claude Code usa il server solo quando viene avviato dalla cartella in cui è stato eseguito questo comando.
Per usare Polyphone da qualsiasi cartella, aggiungi l'opzione `--scope user`:

```
claude mcp add --scope user --transport http polyphone http://127.0.0.1:8642/mcp
```

Avvia poi una nuova sessione di Claude Code: le funzioni di Polyphone vengono caricate all'avvio di una sessione, e Polyphone deve essere già aperto con il server attivato.
Il comando `/mcp` in Claude Code indica se il server «polyphone» è connesso.
La prima volta che l'assistente vuole usare una funzione di Polyphone, Claude Code ti chiede il permesso, e puoi scegliere di non essere più interpellato per quella funzione.

**Nota:** altri assistenti compatibili con MCP tramite il trasporto HTTP possono collegarsi allo stesso indirizzo.


### Prime domande


Inizia con domande semplici per vedere come l'assistente esplora i soundfont:

* *«Quali soundfont sono aperti in Polyphone?»*
* *«Cosa contiene questo soundfont? Ci sono campioni che nessuno strumento usa?»*
* *«Dammi l'elenco dei preset con il loro numero di banco e di preset.»*

Quando selezioni un elemento nell'albero di Polyphone, l'assistente può farvi riferimento: *«Abbassa di 6 dB il volume del campione selezionato»* o *«Rinomina questi strumenti con il prefisso “Organo”»*.
Per le piccole modifiche, il risultato è immediato e visibile nell'editor.


## Esempio: controllare e correggere uno strumento {#doc_check}


Quando si costruisce uno strumento con molti campioni è facile commettere errori, e alcuni sono difficili da notare: una nota leggermente stonata, un campione un po' più forte dei vicini, un loop che fa clic solo dopo qualche secondo…
L'assistente può controllare uno strumento intero in pochi secondi.

Per provare tu stesso, scarica <a href="files/flute-with-mistakes.sf2" download>flute-with-mistakes.sf2</a> e aprilo in Polyphone.
Contiene uno strumento di flauto in cui sono stati introdotti di proposito alcuni errori.
Ascolta una scala da Do4 a Fa6 seguita da un lungo Sol6:

![](files/ai-assistant-flute-before.mp3)

Poi chiedi:

*«Controlla lo strumento “Flute”: ci sono note stonate, troppo forti o troppo deboli, tasti senza suono o loop difettosi?»*

L'assistente analizza lo strumento (una sola funzione misura la loudness, l'altezza, il timbro e il loop di ogni divisione), poi esamina nel dettaglio i campioni sospetti.
La sua risposta elenca i problemi:

* il tasto Mi4 non suona nulla: la divisione di «Flute 63» si ferma a Re♯4 e la successiva inizia a Fa4,
* «Flute 69» è un semitono troppo alto: suona un La4 (440 Hz) ma la sua nota di base è Sol♯4,
* «Flute 78» è 30 cent troppo alto: la sua correzione è di +30 cent, mentre la registrazione è intonata,
* la divisione di «Flute 84» è 6 dB più forte delle altre (tasti da Si5 a Do♯6),
* il loop di «Flute 90» fa clic: le forme d'onda non coincidono nel punto di loop e la lunghezza del loop non è un numero intero di periodi.

Chiedi allora: *«Correggi questi problemi.»*

L'assistente estende l'intervallo di note di «Flute 63» fino a Mi4, imposta la nota di base di «Flute 69» su La4 e la correzione di «Flute 78» su 0, dà un'attenuazione di 6 dB alla divisione di «Flute 84» (il campione stesso non viene modificato) e usa lo strumento [Auto loop](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop) su «Flute 90».
Una nuova analisi conferma il risultato: nessun tasto muto, nessuna nota stonata, lo stesso volume ovunque e un loop giudicato «buono».
La stessa scala dopo le correzioni:

![](files/ai-assistant-flute-after.mp3)

L'assistente spiega cosa fa, e spesso deve fare delle scelte: qui ha attenuato una divisione invece di modificare i dati audio del campione.
Se preferisci un'altra soluzione, basta dirlo, e puoi rivedere ogni modifica nell'editor o annullarla.


## Esempio: creare uno strumento da zero {#doc_create}


Un assistente come Claude Code può anche eseguire programmi sul tuo computer, ad esempio per calcolare dei campioni.
Insieme alle funzioni di Polyphone, questo permette di costruire uno strumento intero a partire da una descrizione.
Il flauto dell'esempio precedente è stato realizzato così, a partire da due richieste:

*«Puoi farmi un suono di flauto per il tasto 60?»*

*«Adesso fai un flauto completo con diversi livelli di intensità, in un nuovo soundfont.»*

Ecco cosa ha fatto l'assistente, passo dopo passo:

1. ha misurato un flauto registrato in un soundfont General MIDI per conoscere il timbro dello strumento in ogni registro (armonici, rumore del soffio, attacco, vibrato),
2. ha scritto un programma che sintetizza 39 campioni: 13 note da Do4 a Do7, una ogni 3 tasti, ciascuna in tre dinamiche; *p* è più scuro e più soffiato con un attacco morbido, *f* è più ricco di armonici con un attacco più deciso,
3. ha importato i campioni in un nuovo soundfont, ha impostato le loro note di base e i loro loop, e ha creato uno strumento per dinamica con un release di 0,4 s,
4. ha sovrapposto i tre strumenti in un preset (banco 0, preset 73, il flauto dello standard General MIDI) con gli intervalli di velocità 0-49, 50-89 e 90-127, come descritto nel tutorial «[Strumenti realistici con più livelli di velocità](tutorials/realistic-instruments-with-multiple-velocity-layers.md)»,
5. ha fatto il rendering di note ai due lati di ogni limite di velocità: il volume cambia solo da 0,1 a 0,4 dB, così che da un livello al successivo cambia solo il timbro.

L'ultimo passaggio ha rivelato un punto debole: nell'ottava più acuta, i tre livelli avevano quasi lo stesso timbro.
L'assistente ha modificato il suo programma perché le note forti diventino più brillanti anche nel registro acuto, poi ha generato di nuovo i campioni e ricostruito lo strumento.

Ascolta il tasto Do4 suonato con le velocità 35, 70 e 110, una nota per livello:

![](files/ai-assistant-flute-layers.mp3)

E una frase sempre più forte, in cui il timbro segue la dinamica:

![](files/ai-assistant-flute-phrase.mp3)

Un flauto sintetizzato non eguaglierà mai una buona registrazione: questo esempio mostra il metodo più che il suono.
Con le tue registrazioni vale lo stesso tipo di richiesta: *«Importa i file wav di questa cartella, trova la loro nota di base, mettili in loop e costruisci uno strumento con un livello per dinamica.»*


## Altre idee {#doc_ideas}


Ecco altre richieste che funzionano bene:

* *«Imposta un release naturale per tutti gli strumenti, più lungo nei bassi.»*
* *«Le note acute di questo strumento sono troppo brillanti: addolciscile gradualmente a partire da Do6.»*
* *«Crea un registro di voce celeste a partire da questo strumento d'organo, con 3 battimenti al secondo sul Do4.»*
* *«Converti tutti i campioni stereo in mono e aggiorna gli strumenti.»*
* *«Confronta questi due campioni: perché il secondo suona più spento?»*
* *«Spiega in parole semplici i modulatori di questo preset.»*
* *«Elimina i campioni inutilizzati e i duplicati, poi salva il soundfont in formato sf3.»*


## Consigli {#doc_tips}


* **Precisa l'ambito**: «solo lo strumento “Piano”», «non modificare i campioni, solo gli strumenti»…
* **Chiedi delle misure**: «verifica il risultato» o «confronta prima e dopo» porta l'assistente a controllare il suo lavoro con le funzioni di analisi.
* **Procedi per gradi** per le modifiche importanti, e ascolta tra un passaggio e l'altro.
* **Tieni una copia** del tuo soundfont e salva solo quando sei soddisfatto: chiedi all'assistente di salvare, oppure usa :kbd:`Ctrl`+:kbd:`S`.
* **I soundfont grandi** richiedono più tempo per l'analisi: punta a uno strumento o a un preset piuttosto che all'intero soundfont.
* **Riservatezza**: l'assistente riceve i nomi, i parametri, le misure e immagini delle forme d'onda, non l'audio stesso; questi dati vengono però inviati al servizio dell'assistente, come il resto della conversazione.
