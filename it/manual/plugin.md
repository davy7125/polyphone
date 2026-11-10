Polyphone esiste anche come plugin per le workstation audio digitali (DAW) e gli altri host di plugin audio.
Il plugin suona i soundfont nei formati sf2, sf3, sfz e dls con il sintetizzatore di Polyphone: un soundfont suona allo stesso modo in un host e nell'editor.
Suona i soundfont ma non li modifica: le modifiche si fanno in Polyphone, dopodiché il soundfont viene riaperto nel plugin.

Il plugin è disponibile nei formati VST3, CLAP e LV2 su Linux, Windows e macOS, e anche Audio Unit (AU) su macOS.


![Plugin di Polyphone](images/plugin.png "Plugin di Polyphone")


## Installazione {#doc_install}


Il plugin è distribuito in un archivio per ciascun sistema operativo, disponibile nella sezione «[Software](software)».
Copia i formati usati dal tuo host nelle cartelle in cui cerca i plugin, poi avvia nell'host una nuova scansione dei plugin.

Linux
: :file:`Polyphone.vst3` in :file:`~/.vst3/`,\
  :file:`Polyphone.clap` in :file:`~/.clap/`,\
  :file:`Polyphone.lv2` in :file:`~/.lv2/`.

macOS
: :file:`Polyphone.vst3` in :file:`~/Library/Audio/Plug-Ins/VST3/`,\
  :file:`Polyphone.clap` in :file:`~/Library/Audio/Plug-Ins/CLAP/`,\
  :file:`Polyphone.component` in :file:`~/Library/Audio/Plug-Ins/Components/` (formato Audio Unit, usato da Logic Pro e GarageBand),\
  :file:`Polyphone.lv2` in :file:`~/Library/Audio/Plug-Ins/LV2/`.

Windows
: :file:`Polyphone.vst3` in :file:`C:\Program Files\Common Files\VST3`,\
  :file:`Polyphone.clap` in :file:`C:\Program Files\Common Files\CLAP`,\
  :file:`Polyphone.lv2` in :file:`%APPDATA%\LV2`.

Su macOS i plugin non sono notarizzati: se l'host si rifiuta di caricarli, rimuovi l'attributo di quarantena dei file scaricati, ad esempio:

```
xattr -dr com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/Polyphone.vst3
```


## Soundfont {#doc_soundfont}


La parte superiore della finestra mostra il nome del soundfont e il suo percorso.
Un soundfont si apre:

* con il pulsante :guilabel:`Apri...`,
* con la freccia alla sua destra, che elenca gli ultimi 10 soundfont aperti,
* trascinando un file sulla finestra.

I file sfz e dls vengono convertiti nello stesso modo in cui Polyphone li importa, e i campioni di un file sfz sono i file audio a cui fa riferimento.
L'intero soundfont viene letto all'apertura, e i campioni di un file sf3 vengono decompressi: durante l'esecuzione non viene letto nulla dal disco.
Quando più istanze del plugin suonano lo stesso file (una per traccia, ad esempio), il file viene letto e tenuto in memoria una sola volta.
Se il file è cambiato, ad esempio dopo essere stato salvato di nuovo in Polyphone, viene riletto.


## Modalità di esecuzione {#doc_modes}


Tre pulsanti sopra l'elenco dei preset scelgono il modo in cui vengono suonati i preset:

:guilabel:`Un preset`
: Il preset selezionato nell'elenco viene suonato sui 16 canali MIDI.
  I preset sono mostrati con il loro numero di banco e di preset, ad esempio «000:024 Guitar».

:guilabel:`16 canali`
: Ogni canale MIDI suona il proprio preset, scelto nella sua riga.
  La scelta :guilabel:`Cambio programma` suona il preset scelto dai messaggi di bank select e di program change ricevuti dal canale, come fa un sintetizzatore General MIDI: il banco 128 viene allora usato sul canale 10, per la batteria.
  Il preset suonato è mostrato a destra di ogni riga.
  Questa modalità è comoda per suonare un file MIDI arrangiato per più strumenti.

:guilabel:`MPE`
: Il preset selezionato viene suonato da un controller MPE (MIDI Polyphonic Expression): ogni nota ha il proprio canale, e quindi il proprio pitch bend, la propria pressione e il proprio timbro (CC 74).
  Viene usata una zona inferiore di 15 canali finché il controller non invia la sua configurazione (RPN 6).
  Il canale master di una zona agisce su tutte le sue note: il suo pitch bend si somma a quello di ciascuna nota e i suoi controller si applicano a tutte le note.
  L'estensione del pitch bend è di 48 semitoni per le note e di 2 semitoni per il canale master, e può essere modificata dal controller (RPN 0).


![Plugin nella modalità «16 canali»](images/plugin_channels.png "Plugin nella modalità «16 canali»")


## Parametri {#doc_parameters}


I parametri a destra sono quelli del sintetizzatore di Polyphone, che si ritrovano nelle sue impostazioni [Suono](manual/settings.md#doc_sound) e [Tastiera](manual/settings.md#doc_keyboard):

:guilabel:`Sintetizzatore`
: :guilabel:`Guadagno` (da -50 a +50 dB), :guilabel:`Trasposizione` (da -24 a +24 semitoni, la batteria non viene trasposta), :guilabel:`Accordatura` (intonazione di riferimento del La4, da 400 a 480 Hz) e :guilabel:`Temperamento`, le cui intonazioni fini sono relative alla tonalità scelta sotto l'elenco.

:guilabel:`Riverbero`
: :guilabel:`Livello`, :guilabel:`Dimensione`, :guilabel:`Ampiezza` e :guilabel:`Smorzamento`.

:guilabel:`Chorus`
: :guilabel:`Livello`, :guilabel:`Profondità` e :guilabel:`Frequenza`.

Tutti questi parametri possono essere automatizzati dall'host.


## Tastiera e stato {#doc_keyboard}


La tastiera nella parte inferiore della finestra mostra le note ricevute e può essere suonata con il mouse.
Sotto di essa sono indicati il numero di voci in esecuzione e la quota del processore occupata dal plugin.

Il pulsante :guilabel:`Panico` ferma tutti i suoni e reimposta i controller, il che è utile se una nota resta bloccata.


## Impostazioni {#doc_settings}


Il pulsante :guilabel:`Impostazioni` apre un menu con le seguenti opzioni:

:guilabel:`Polifonia`
: Numero massimo di voci, da 64 a 4096 (1024 per impostazione predefinita).
  Oltre questo numero, le note più deboli si dissolvono.

:guilabel:`Tastiera`
: Canale MIDI delle note suonate con la tastiera della finestra.

:guilabel:`Lingua`
: Lingua del plugin, per impostazione predefinita quella del sistema.
  Questa scelta è condivisa da tutte le istanze del plugin.
  I nomi dei parametri e delle uscite forniti all'host sono quelli della lingua usata al caricamento del plugin (sempre l'inglese con LV2).

:guilabel:`Salva il soundfont nel progetto`
: Il soundfont viene salvato nel progetto dell'host, che può quindi essere aperto su un altro computer senza il file.
  Non è possibile con un file sfz, i cui campioni sono altri file.

:guilabel:`Canali ed effetti sulle proprie uscite`
: Oltre all'uscita principale, le uscite da «Canale 1» a «Canale 16» ricevono le note del loro canale senza effetti, e l'uscita «Effetti» riceve la parte inviata al riverbero e al chorus.
  Un canale la cui uscita non è attivata dall'host resta nell'uscita principale.
  Quando questa opzione è disattivata (impostazione predefinita), tutto viene mixato nell'uscita principale.


## Uso in un progetto {#doc_project}


Il progetto dell'host salva i parametri, i preset suonati, la polifonia e il percorso del soundfont (o il suo contenuto, se è salvato nel progetto).
All'apertura di un progetto, il soundfont viene letto prima che venga ripristinato il resto, in modo che un'esportazione offline possa iniziare subito.

Una nota inizia esattamente nella posizione del suo messaggio MIDI, qualunque sia la dimensione del buffer dell'host.
Durante un'esportazione offline, le voci non sono mai limitate dal carico del processore.
