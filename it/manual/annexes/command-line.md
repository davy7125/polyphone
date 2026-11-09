Polyphone può essere eseguito da riga di comando per convertire soundfont nel formato [sf2](#doc_sf2), [sf3](#doc_sf3), [sfz](#doc_sfz), [csv](#doc_csv) o [dls](#doc_dls). I formati di file supportati per la conversione sono [sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [dls](manual/annexes/the-different-soundfont-formats.md#doc_dls), [sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) e [organ](manual/annexes/the-different-soundfont-formats.md#doc_organ).

Un [file MIDI può essere riprodotto](#doc_render) con un soundfont e salvato in un file wav.
È anche possibile richiamare delle opzioni per aprire Polyphone in [modalità sintetizzatore](#doc_synth) o con il [server MCP](#doc_mcp) avviato.

File e cartelle possono anche essere passati come argomenti per aprirli nell'interfaccia grafica, ad esempio :command:`polyphone file1.sf2 file2.sf3 /path/to/directory` (una cartella viene aperta nell'[esploratore di soundfont](manual/soundfont-explorer.md)).
Se Polyphone è già aperto, i file vengono aperti nell'istanza in esecuzione.
Sono disponibili alcune [altre opzioni](#doc_other) per reimpostare la configurazione o mostrare la guida e la versione.

L'elenco di tutte le opzioni viene mostrato con :command:`polyphone --help`.
L'ordine degli argomenti non ha importanza.
Le opzioni delle [versioni precedenti](#doc_legacy) sono ancora accettate.

**Nota:** su Windows devi scrivere il percorso completo di Polyphone invece di :command:`polyphone`, ad esempio :command:`C:/Program files/Polyphone/polyphone.exe`.


## Conversione {#doc_conversion}


### Riga di comando


```
polyphone --convert <formato> <file/da/convertire> -d <cartella/di/uscita> -o <file/di/uscita> -f
```


### Argomenti


--convert
: Formato della conversione: «sf2», «sf3», «sfz», «csv» o «dls»; le opzioni specifiche di ogni formato sono descritte più avanti.
  Questo argomento è facoltativo se viene indicato un file di uscita con la sua estensione, ad esempio :command:`polyphone file.sf2 -o file.sf3`.

-d, --output-dir
: Cartella di uscita, che deve esistere.
  Questo argomento è facoltativo; per impostazione predefinita è la cartella di ogni file di ingresso.

-o, --output
: File di uscita, con o senza estensione (l'estensione del formato viene aggiunta automaticamente).
  Questo argomento è facoltativo; per impostazione predefinita il file di uscita ha il nome del file di ingresso.
  Un nome da solo viene scritto nella cartella di uscita, un percorso relativo parte dalla cartella corrente (o dalla cartella di uscita se si usa -d).
  Questo argomento non può essere usato se vengono convertiti più file.

-f, --force
: Sostituisce un file di uscita esistente.
  Senza questo argomento, la conversione non riesce se il file di uscita esiste già.
  Il file di ingresso può essere sostituito dalla sua conversione, ad esempio :command:`polyphone --convert sf2 file.sf2 -f` riscrive :file:`file.sf2`.

Più file possono essere convertiti con un solo comando, e ogni file di uscita ha il nome del suo file di ingresso.
In caso di errore, gli altri file vengono comunque convertiti.


### Esempi


```
polyphone --convert sf2 /path/to/file.sfArk
polyphone /path/to/file.sf2 -o /path/to/converted.sf3
polyphone --convert sf3 *.sf2 -d /path/to/directory
```


## Conversione in sf2 {#doc_sf2}


Per questa conversione non viene usata alcuna opzione particolare.

```
polyphone --convert sf2 /path/to/file.dls
```


## Conversione in sf3 {#doc_sf3}


--quality
: Qualità di compressione: «low» (bassa), «medium» (media) o «high» (alta).
  Questo argomento è facoltativo; per impostazione predefinita vale «medium».

```
polyphone --convert sf3 /path/to/file.sf2 --quality high
```


## Conversione in sfz {#doc_sfz}


--preset-prefix
: Ogni preset ha come prefisso il suo numero di preset.

--bank-dir
: Viene creata una cartella per ogni banco.

--gm-sort
: La classificazione General MIDI viene usata per ordinare i preset.

```
polyphone --convert sfz /path/to/file.sf3 --bank-dir --gm-sort
```

La conversione crea una cartella (con il nome del file di uscita) che contiene i file sfz e una sottocartella :file:`samples`.
Se questa cartella esiste già, al suo nome viene aggiunto un numero: nulla viene sostituito, nemmeno con -f.


## Conversione in .csv {#doc_csv}


--raw
: Vengono esportati i valori grezzi (così come sono memorizzati nel file soundfont) invece dei valori con unità fisiche e misurabili.
  Ad esempio, il tempo di rilascio esportato sarà 0,491 (secondi) oppure -1229 nella sua forma grezza.
  Per impostazione predefinita vengono esportati i valori con unità fisiche.

```
polyphone --convert csv /path/to/file.sf2 --raw
```

La conversione crea una cartella (con il nome del file di uscita) che contiene il file :file:`samples.csv` e un file per ogni strumento e per ogni preset.
Se questa cartella esiste già, al suo nome viene aggiunto un numero: nulla viene sostituito, nemmeno con -f.


## Conversione in dls {#doc_dls}


Per questa conversione non viene usata alcuna opzione particolare.

```
polyphone --convert dls /path/to/file.sf2
```

Il file viene scritto nel formato DLS livello 2; le limitazioni della conversione sono descritte [qui](manual/annexes/the-different-soundfont-formats.md#doc_dls).


## Rendering di un file MIDI {#doc_render}


### Riga di comando


```
polyphone --render <file/MIDI> <soundfont> -d <cartella/di/uscita> -o <file/di/uscita> -f
```


### Argomenti


--render
: Percorso del file MIDI da riprodurre (estensione .mid, .midi, .kar o .rmi): il file MIDI viene riprodotto con il soundfont e il risultato viene scritto in un file wav.

-d, --output-dir
: Cartella di uscita in cui verrà scritto il file wav.
  Questo argomento è facoltativo; per impostazione predefinita è la cartella del file MIDI.

-o, --output
: File wav di uscita, con o senza estensione.
  Questo argomento è facoltativo; per impostazione predefinita il file wav ha il nome del file MIDI.

-f, --force
: Sostituisce un file wav esistente.


### Esempio


```
polyphone --render /path/to/song.mid /path/to/file.sf2
```

Tutti i canali del file MIDI suonano il soundfont, e i preset vengono scelti dai cambi di programma del file (banco 128 sul canale 10, per le percussioni).
Vengono usate le impostazioni del sintetizzatore: [suono](manual/settings.md#doc_sound) (riverbero, chorus, volume generale), [polifonia](manual/settings.md#doc_general) e [accordatura](manual/settings.md#doc_keyboard).
Il risultato è un file wav stereo a 32 bit in virgola mobile a 48.000 Hz, che termina quando il suono delle ultime note si è spento.
Le note di un preset che il soundfont non contiene non vengono suonate, e viene mostrato un avviso per quel preset.
Nell'interfaccia grafica, un file MIDI può anche essere esportato dal [lettore MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player_settings), dove ogni canale può suonare un soundfont o un preset diverso.


## Modalità sintetizzatore {#doc_synth}


### Riga di comando


```
polyphone --synth <file/da/leggere> --channel <canale> --multiple-selection --key-selection <modalità>
```

![Polyphone aperto con l'opzione della modalità sintetizzatore](images/synth_mode.png "Polyphone aperto con l'opzione della modalità sintetizzatore")


### Argomenti


-s, --synth
: Polyphone viene aperto in modalità sintetizzatore.

--channel
: Numero del canale MIDI (tra 1 e 16) da ascoltare per suonare.
  Il valore «all» indica che verranno usati tutti i canali, elaborati come un unico canale (valore predefinito).
  Il valore «each» ascolta ogni canale separatamente, il che è un prerequisito ad esempio per suonare l'uscita MIDI di un sequencer (un file MIDI può anche essere riprodotto direttamente con il [lettore MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player)).

--multiple-selection
: È possibile selezionare più preset da suonare.

--key-selection
: Con il valore «on», i preset vengono selezionati tramite i tasti corrispondenti al numero di preset (ad esempio, il tasto 0 attiva il preset 0).
  Con il valore «toggle», un tasto deseleziona anche il suo preset.

Questi argomenti sono facoltativi, e le impostazioni possono poi essere modificate tramite l'interfaccia grafica.


### Esempio


```
polyphone --synth /path/to/file.sf2 --channel 1 --multiple-selection --key-selection toggle
```


## Server MCP {#doc_mcp}


### Riga di comando


```
polyphone --mcp <file/da/aprire> --mcp-port <porta>
```


### Argomenti


--mcp
: Il server MCP, che permette a un assistente IA di esplorare, modificare e analizzare i soundfont aperti, viene avviato per questa sessione anche se è disattivato nelle [impostazioni](manual/settings.md).
  L'indirizzo del server viene mostrato nella console.
  Viene aperta una nuova istanza di Polyphone, anche se Polyphone è già aperto.

--mcp-port
: Porta del server MCP (questo argomento implica --mcp).
  Questo argomento è facoltativo; per impostazione predefinita è la porta delle impostazioni (8642).

Modificare le impostazioni del server MCP nell'interfaccia grafica sostituisce queste opzioni.


### Esempio


```
polyphone --mcp-port 8700 /path/to/file.sf2
```


## Altre opzioni {#doc_other}


-h, --help
: Mostra l'elenco di tutte le opzioni.

-v, --version
: Mostra la versione di Polyphone.

-r, --reset-config
: Reimposta la configurazione di Polyphone (impostazioni, geometria della finestra, cronologia, ecc.).
  Può essere utile in caso di problemi.

```
polyphone --reset-config
```


## Messaggi e codici restituiti {#doc_exit}


Il caricamento e la scrittura vengono registrati nella console, e gli errori vengono scritti nell'uscita degli errori.
I messaggi e la guida sono nella lingua scelta nelle [impostazioni](manual/settings.md).

Il codice restituito è:

0
: Successo.

1
: Un file di ingresso o la cartella di uscita non esiste, oppure il file di uscita esiste già (senza -f).

2
: Gli argomenti non sono validi: opzione sconosciuta, valore errato, opzione non usata con la modalità scelta, formato del file di ingresso non supportato, ecc.

3
: Un file di ingresso non può essere letto (soundfont, file MIDI).

4
: L'uscita non può essere scritta.

Quando vengono convertiti più file, il codice restituito è quello del primo errore.


## Opzioni delle versioni precedenti {#doc_legacy}


Le opzioni delle versioni precedenti di Polyphone sono ancora accettate:

-1, -2, -3, -4, -6
: Conversione in sf2, sf3, sfz, csv e dls, come con --convert.

-5
: Rendering di un file MIDI, il cui percorso viene indicato con -c.

-i
: File di ingresso, che può anche essere scritto senza questa opzione.

-c
: Configurazione, composta da valori separati dal carattere «|»:
  
  * conversione in sf3: qualità di compressione, «0» per bassa, «1» per media, «2» per alta;
  * conversione in sfz: 3 valori, «1» o «0», per --preset-prefix, --bank-dir e --gm-sort, ad esempio «0|1|1»;
  * conversione in csv: «raw» per --raw;
  * rendering di un file MIDI (-5): percorso del file MIDI;
  * modalità sintetizzatore: canale («all», «each» o da 1 a 16, dove 0 equivale ad «all» e -1 a «each»), selezione multipla («on» o «off») e selezione tramite i tasti («on», «off» o «toggle»), ad esempio «all|on|toggle».

```
polyphone -2 -i /path/to/file.sf2 -c 2
polyphone -s -i /path/to/file.sf2 -c 'all|on|toggle'
```
