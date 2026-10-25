Queste istruzioni spiegano come compilare Polyphone con Qt Creator.
Si presuppone che sul tuo computer siano già installati Qt Creator con il framework Qt, un compilatore e le dipendenze necessarie per compilare Polyphone (vedi le pagine relative).


## Prerequisiti


Sono necessari i seguenti elementi:

* un compilatore C++ (VC++, Clang, GCC, …),
* Qt Creator con il suo framework (Qt 6),
* le dipendenze di Polyphone.

Le istruzioni per installarli dipendono dal sistema operativo che usi; vedi le relative pagine della documentazione.


## Compilare Polyphone


### Copiare i sorgenti


Procurati i sorgenti di Polyphone <a href="software" target="_blank">qui</a> o da <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


### Aprire e configurare il progetto


Per prima cosa, apri il file :file:`polyphone.pro` e modificalo per usare eventualmente le versioni incluse di :program:`rtmidi` e :program:`stk` se il tuo sistema operativo non le fornisce (la versione inclusa di :program:`rtaudio` viene usata per impostazione predefinita).
Togli il commento dalle righe seguenti eliminando il «#» iniziale:

```
DEFINES += USE_LOCAL_RTMIDI
DEFINES += USE_LOCAL_STK
```

Puoi saltare queste modifiche se vuoi usare librerie già installate sul tuo computer (assicurati di averle!).
Questo passaggio non è necessario con Windows e macOS, dove vengono sempre usate le versioni incluse.

Avvia Qt Creator e apri il file :file:`polyphone.pro`.


![Aprire Qt Creator](images/open-Qt-Creator.png "Aprire Qt Creator")


Poi configura il progetto.

![Configurare il progetto](images/configure-project.png "Configurare il progetto")


### Eseguire qmake e compilare il progetto


Vai nel menu :guilabel:`Build` e scegli la voce :guilabel:`Run qmake`.


![Eseguire qmake](images/run-qmake.png "Eseguire qmake")


L'esecuzione di :command:`qmake` è terminata quando la barra di avanzamento nell'angolo in basso a destra ha raggiunto il 100% in verde.


![qmake terminato](images/qmake-finished.png "qmake terminato")


Nella sezione :guilabel:`Project` della colonna di sinistra, poi nella sezione :guilabel:`Build&Run`, seleziona la configurazione di compilazione (è preferibile :guilabel:`Release`).

Infine, fai clic sull'azione :guilabel:`Build project Polyphone` nel menu :guilabel:`Build`. Durante la compilazione del progetto potresti vedere degli avvisi, ma non dovrebbero esserci errori.

Ora dovresti avere l'eseguibile nella cartella di compilazione creata da Qt Creator (ad esempio :file:`build-XXX` accanto ai sorgenti), a seconda della scelta fatta nella configurazione di compilazione di Qt Creator:

* con Linux, il file :file:`polyphone` si trova nella sottocartella :file:`bin`,
* con Windows, il file :file:`polyphone.exe` si trova nella sottocartella :file:`release` o :file:`debug`,
* con macOS, il bundle :file:`Polyphone.app` si trova nella cartella :file:`macos`, situata accanto alla cartella :file:`sources`.


## Risoluzione dei problemi


Consulta questa [categoria](forum/support-bug-reports) se ti servono altre informazioni o un aiuto.


<p class="endpage">Un ringraziamento speciale a <a href="dashboard/820-tomotello">Tomotello</a> per questo articolo.</p>
