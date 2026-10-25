Polyphone supporta diversi formati di soundfont:

* [sf2](#doc_sf2),
* [sf3](#doc_sf3),
* [sfz](#doc_sfz),
* [sfArk](#doc_sfark),
* [organ](#doc_organ).


## Soundfont sf2 {#doc_sf2}


I soundfont sf2 sono i file principali modificati da Polyphone.
Ogni file sf2 contiene uno o più strumenti musicali virtuali, composti da campioni audio e da numerosi parametri.
I parametri definiscono come i campioni devono essere riprodotti sull'intera tastiera, eventualmente modulati da segnali predefiniti ([modulatori](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator)).
I file sf2 sono costruiti secondo una struttura a 3 livelli:

* i [campioni](manual/soundfont-editor/editing-pages/sample-editor.md), provenienti ad esempio da una tromba o da un pianoforte,
* gli [strumenti](manual/soundfont-editor/editing-pages/instrument-editor.md), composti da campioni,
* i [preset](manual/soundfont-editor/editing-pages/preset-editor.md), composti da strumenti.

Oltre alla definizione e all'impostazione di questi elementi, un file sf2 contiene anche [informazioni generali](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md) (ad esempio l'autore, il copyright o la data di modifica).

I soundfont sono usati da sintetizzatori software, come <a href="http://www.fluidsynth.org/" target="_blank">FluidSynth</a>, controllati da segnali MIDI.
È anche possibile usare i soundfont sf2 per ascoltare file MIDI con <a href="https://sourceforge.net/projects/timidity" target="_blank">TiMidity</a>, <a href="https://www.mindwerks.net/projects/wildmidi" target="_blank">WildMIDI</a> o QuickTime e per ascoltare partiture con <a href="https://musescore.org/" target="_blank">MuseScore</a>.
Anche dispositivi hardware come campionatori, schede audio o persino sintetizzatori possono supportare il formato .sf2.

Ulteriori informazioni sui soundfont si trovano ad esempio su <a href="https://it.wikipedia.org/wiki/SoundFont" target="_blank">Wikipedia</a>.
Le specifiche sf2 sono disponibili [qui](files/sf_specifications_v2.01.pdf) per la versione 2.01 e [qui](files/sf_specifications_v2.04.pdf) per la versione 2.04.


## Soundfont sf3 {#doc_sf3}


Il formato sf3, sviluppato da <a href="https://musescore.org/" target="_blank">MuseScore</a>, è in tutto simile al formato sf2, tranne per il fatto che i [campioni](manual/soundfont-editor/editing-pages/sample-editor.md) possono essere memorizzati nel formato OGG (simile all'MP3 ma open source) invece che come dati grezzi.
Di conseguenza, il formato sf3 è circa 10 volte più leggero del formato sf2 a parità di qualità.
In Polyphone, la compressione si attiva nelle [informazioni generali](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md#doc_edit) del soundfont, e ogni campione può poi essere compresso nell'[editor dei campioni](manual/soundfont-editor/editing-pages/sample-editor.md#doc_info) (la qualità della compressione è selezionabile).

I campioni compressi di un soundfont esportato in questo formato non sono destinati a essere modificati in seguito, perché compressioni successive porterebbero a una qualità sonora inferiore.
Questo formato va considerato un prodotto finale ed è molto interessante per tutti gli utenti finali di soundfont, perché:

* i download da internet sono più semplici,
* si risparmia spazio sui computer,
* la combinazione MIDI + sf3 offre una soluzione leggera e potente per riprodurre colonne sonore (videogiochi, applicazioni mobili, …).

A differenza del formato <a href="http://www.fmjsoft.com/fmt/sf2pack.htm" target="_blank">sf2pack</a>, che risponde agli stessi problemi, il formato sf3 è interamente open source.
Il codice sorgente è disponibile, il che ne garantisce la sostenibilità nel tempo (evitiamo gli errori fatti con sfArk o sfPack!).


## Soundfont sfz {#doc_sfz}


Il formato sfz ha lo stesso obiettivo del formato sf2: creare uno strumento musicale disponendo e configurando dei campioni sulla tastiera.
La differenza principale è che il formato sf2 è un unico file che contiene tutto, mentre il formato sfz è un file di testo fornito insieme a una serie di campioni .wav.
Essendo pensato per essere modificabile da una persona, il vantaggio principale era poter modificare il file senza un editor complesso.
Tuttavia, per strumenti di grandi dimensioni è comunque fortemente consigliato un editor, perché la quantità di parametri può scoraggiare rapidamente.
Inoltre, il formato sfz non è definito in modo rigoroso come il formato sf2: possono emergere differenze nel modo di modificare e nel modo di riprodurre uno strumento sfz. Allo stesso tempo, le possibilità degli strumenti sfz sono state ampliate rispetto allo standard sf2.


### Limitazioni dell'importazione sfz


* Vengono importati solo gli opcode che hanno un equivalente nelle [tabelle](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).
* Poiché nei file sf2 l'inviluppo applicato all'intonazione e al filtro è lo stesso (Mod env), non è possibile importare inviluppi diversi per l'intonazione e per il filtro (opcode ``fileg_*`` e ``pitcheg_*``).
* Poiché nei file sf2 l'LFO applicato al volume e al filtro è lo stesso (Mod LFO), non è possibile importare LFO diversi per il volume e per il filtro (opcode ``amplfo_*`` e ``fillfo_*``).
* L'unico filtro che può essere importato è il filtro passa-basso del secondo ordine (``fil_type=lpf_2p``).
* Per essere importati correttamente, gli opcode ``group`` e ``off_by`` devono contenere lo stesso valore all'interno dello stesso gruppo.

**Attenzione:** gli opcode ``width`` e ``position`` potrebbero non essere interpretati correttamente.


### Limitazioni dell'esportazione sfz


* I [modulatori](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) attualmente non vengono esportati.
* Poiché il formato sfz (versione 1) consente una sola modulazione dell'intonazione, i due parametri :guilabel:`Vib LFO → pitch (c)` e :guilabel:`Mod LFO → pitch (c)` non possono essere esportati contemporaneamente.
* Un loop 0-1 può essere esportato, ma viene ignorato dalla maggior parte dei lettori sfz.
* I campioni a 24 bit potrebbero non essere supportati da alcuni sintetizzatori: consulta le loro specifiche.


### In generale


La corrispondenza dei livelli sonori (in dB) durante la conversione sfz / sf2 è stata verificata con il software Sforzando.
Possono tuttavia restare alcune differenze.

Inoltre, il modo in cui i parametri variano in funzione della nota (tramite :guilabel:`Key → Vol env hold / decay` e :guilabel:`Key → Mod env hold / decay`) non ha una corrispondenza esatta.
Il formato sf2 usa una legge esponenziale per definire le durate di decay e di hold in funzione della nota, mentre il formato sfz usa una legge lineare.


## Archivi sfArk {#doc_sfark}


Gli archivi sfArk, come il formato sf3, sono un formato compresso.
Ciò significa che la qualità dei suoni è leggermente ridotta (ma spesso in modo impercettibile).
Questo formato tende a diventare obsoleto, motivo per cui Polyphone permette solo di importare gli archivi sfArk (non di esportarli).


## Disposizioni GrandOrgue {#doc_organ}


GrandOrgue è un programma il cui scopo principale è emulare strumenti come gli organi a canne. Un organo è definito da una serie di file wav, ciascuno contenente il suono di una canna, e da un file con estensione «.organ» che contiene tutti i parametri necessari per creare un organo a partire da tutti i file wav. In pratica, tutti i campioni sono associati a note specifiche e raggruppati per registri, che possono essere attivati o disattivati suonando l'organo. Polyphone è in grado di creare soundfont basati su questa associazione, ma non supporta la conversione dei soundfont nel formato GrandOrgue.

### Limitazione dell'importazione

Le trasposizioni attualmente non sono supportate.


*[MIDI]: Musical Instrument Digital Interface
