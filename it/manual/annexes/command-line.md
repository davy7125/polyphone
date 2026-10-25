Polyphone può essere eseguito da riga di comando per convertire soundfont nel formato [sf2](#doc_sf2), [sf3](#doc_sf3), [sfz](#doc_sfz) o [csv](#doc_csv). I formati di file supportati per la conversione sono [sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) e [organ](manual/annexes/the-different-soundfont-formats.md#doc_organ).

È anche possibile richiamare un'opzione per aprire Polyphone in [modalità sintetizzatore](#doc_synth).

File e cartelle possono anche essere passati come argomenti per aprirli nell'interfaccia grafica, ad esempio :command:`polyphone file1.sf2 file2.sf3 /path/to/directory` (una cartella viene aperta nell'[esploratore di soundfont](manual/soundfont-explorer.md)).
Sono disponibili alcune [altre opzioni](#doc_other) per reimpostare la configurazione o mostrare la guida.

Durante una conversione, il caricamento e la scrittura vengono registrati nella console.
Il codice restituito è 0 se la conversione è riuscita e un altro valore in caso di errore (il file di ingresso non esiste, il file non può essere letto o scritto, esiste già un file di uscita per i formati sf2 e sf3, ecc.).


## Conversione in sf2 {#doc_sf2}


### Riga di comando


```
polyphone -1 -i <file/to/convert> -d <output/directory> -o <output_file_name>
```

**Nota:** su Windows devi scrivere il percorso completo di Polyphone invece di :command:`polyphone`, ad esempio :command:`C:/Program files/Polyphone/polyphone.exe`.


### Argomenti


-1
: La modalità «1» corrisponde a «converti in sf2».

-d
: Cartella di uscita in cui verrà convertito il file di ingresso.
  Questo argomento è facoltativo; per impostazione predefinita è la stessa cartella del file di ingresso.

-o
: Nome di uscita del file convertito, senza estensione (l'estensione «.sf2» viene aggiunta automaticamente).
  Questo argomento è facoltativo; per impostazione predefinita è lo stesso nome del file di ingresso.


### Esempio


```
polyphone -1 -i /path/to/file.sfArk
```


## Conversione in sf3 {#doc_sf3}


### Riga di comando


```
polyphone -2 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**Nota:** su Windows devi scrivere il percorso completo di Polyphone invece di :command:`polyphone`, ad esempio :command:`C:/Program files/Polyphone/polyphone.exe`.


### Argomenti


-2
: La modalità «2» corrisponde a «converti in sf3».

-d
: Cartella di uscita in cui verrà convertito il file di ingresso.
  Questo argomento è facoltativo; per impostazione predefinita è la stessa cartella del file di ingresso.

-o
: Nome di uscita del file convertito, senza estensione (l'estensione «.sf3» viene aggiunta automaticamente).
  Questo argomento è facoltativo; per impostazione predefinita è lo stesso nome del file di ingresso.

-c
: Configurazione della conversione.
  È possibile specificare la qualità della compressione: «0» è bassa, «1» media, «2» alta.
  Questo argomento è facoltativo; per impostazione predefinita vale «1» (qualità media).


### Esempio


```
polyphone -2 -i /path/to/file.sf2 -c 2
```


## Conversione in sfz {#doc_sfz}


### Riga di comando


```
polyphone -3 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**Nota:** su Windows devi scrivere il percorso completo di Polyphone invece di :command:`polyphone`, ad esempio :command:`C:/Program files/Polyphone/polyphone.exe`.


### Argomenti


-3
: La modalità «3» corrisponde a «converti in sfz».

-d
: Cartella di uscita in cui verrà convertito il file di ingresso.
  Questo argomento è facoltativo; per impostazione predefinita è la stessa cartella del file di ingresso.

-o
: Nome di uscita del file convertito, senza estensione (l'estensione «.sfz» viene aggiunta automaticamente).
  Questo argomento è facoltativo; per impostazione predefinita è lo stesso nome del file di ingresso.

-c
: Configurazione della conversione, composta da 3 valori separati dal carattere «|». Questo argomento è facoltativo; per impostazione predefinita vale «0|0|0».
  
  * Il primo valore è «1» se ogni preset deve essere preceduto dal suo numero di preset, «0» altrimenti.
  * Il secondo valore è «1» se deve essere creata una cartella per ogni banco, «0» altrimenti.
  * Il terzo valore è «1» se per ordinare i preset deve essere usata la classificazione General MIDI, «0» altrimenti.


### Esempio


```
polyphone -3 -i /path/to/file.sf3 -c '0|1|1'
```

La conversione crea una cartella (con il nome di uscita) contenente i file sfz e una sottocartella :file:`samples`. Se questa cartella esiste già, al suo nome viene aggiunto un numero.


## Conversione in .csv {#doc_csv}


### Riga di comando


```
polyphone -4 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**Nota:** su Windows devi scrivere il percorso completo di Polyphone invece di :command:`polyphone`, ad esempio :command:`C:/Program files/Polyphone/polyphone.exe`.


### Argomenti


-4
: La modalità «4» corrisponde a «converti in .csv».

-d
: Cartella di uscita in cui verrà convertito il file di ingresso.
  Questo argomento è facoltativo; per impostazione predefinita è la stessa cartella del file di ingresso.

-o
: Nome di uscita del file convertito, senza estensione (l'estensione «.csv» viene aggiunta automaticamente).
  Questo argomento è facoltativo; per impostazione predefinita è lo stesso nome del file di ingresso.

-c
: Configurazione della conversione.
  L'opzione «raw» può essere usata per esportare i valori grezzi (così come sono memorizzati nel file soundfont) invece di valori con unità fisiche misurabili. Ad esempio, il tempo di release esportato sarà 0,491 (secondi), oppure -1229 nella sua forma grezza. Per impostazione predefinita vengono esportati i valori con unità fisiche.


### Esempio


```
polyphone -4 -i /path/to/file.sf2 -c 'raw'
```

La conversione crea una cartella (con il nome di uscita) contenente il file :file:`samples.csv` e un file per ogni strumento e per ogni preset. Se questa cartella esiste già, al suo nome viene aggiunto un numero.


## Modalità sintetizzatore {#doc_synth}


### Riga di comando


```
polyphone -s -i <file/to/read> -c <configuration>
```

**Nota:** su Windows devi scrivere il percorso completo di Polyphone invece di :command:`polyphone`, ad esempio :command:`C:/Program files/Polyphone/polyphone.exe`.

![Polyphone aperto con l'opzione della modalità sintetizzatore](images/synth_mode.png "Polyphone aperto con l'opzione della modalità sintetizzatore")


### Argomenti


-s
: La modalità «s» corrisponde ad «apri Polyphone in modalità sintetizzatore».

-c
: Configurazione del sintetizzatore, composta da 3 valori separati dal carattere «|». Questo argomento è facoltativo; per impostazione predefinita vale «0|off|off». Questi parametri possono poi essere modificati tramite l'interfaccia grafica.
  
  * Il primo valore è il numero del canale MIDI (tra 1 e 16) da ascoltare per suonare. Un valore di 0 o «all» indica che verranno usati tutti i canali, elaborati come un unico canale. Un valore di -1 o «each» ascolta ogni canale separatamente, il che è un prerequisito ad esempio per [riprodurre file MIDI](tutorials/read-midi-file-with-soundfont.md).
  * Il secondo valore è «off» o «on» e disattiva o attiva la selezione di più preset da suonare.
  * Il terzo valore è «off», «on» o «toggle» e disattiva o attiva la selezione dei preset tramite i tasti corrispondenti al numero di preset (ad esempio, il tasto 0 attiva il preset 0).


### Esempio


```
polyphone -s -i /path/to/file.sf2 -c '1|on|toggle'
```


## Altre opzioni {#doc_other}


-h
: Mostra un breve messaggio di aiuto.

-r
: Reimposta la configurazione di Polyphone (impostazioni, geometria della finestra, cronologia, ecc.).
  Può essere utile in caso di problemi.

```
polyphone -r
```
