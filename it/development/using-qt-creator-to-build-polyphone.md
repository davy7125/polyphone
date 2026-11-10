Queste istruzioni spiegano come compilare Polyphone con Qt Creator.
Si presuppone che sul tuo computer siano già installati Qt Creator con il framework Qt, un compilatore e le dipendenze necessarie per compilare Polyphone (vedi le pagine relative).


## Prerequisiti


Sono necessari i seguenti elementi:

* un compilatore C++ (GCC, Clang, MinGW…),
* Qt Creator con il suo framework (Qt 6, o Qt 5),
* :program:`CMake` (versione 3.16 o superiore), che può essere installato con Qt Creator,
* le dipendenze di Polyphone.

Le istruzioni per installarli dipendono dal sistema operativo che usi; vedi le relative pagine della documentazione.


## Compilare Polyphone


### Copiare i sorgenti


Procurati i sorgenti di Polyphone <a href="software" target="_blank">qui</a> o da <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


### Aprire e configurare il progetto


Avvia Qt Creator e apri il file :file:`CMakeLists.txt` situato nella cartella :file:`sources`.


![Aprire Qt Creator](images/open-Qt-Creator.png "Aprire Qt Creator")


Poi configura il progetto scegliendo un kit, cioè una versione di Qt e un compilatore.

![Configurare il progetto](images/configure-project.png "Configurare il progetto")


Qt Creator esegue quindi :program:`CMake`, che verifica che le dipendenze vengano trovate.
I messaggi sono mostrati nel pannello :guilabel:`General Messages`: se manca una libreria, installala ed esegui di nuovo :program:`CMake` con la voce :guilabel:`Run CMake` del menu :guilabel:`Build`.

Le opzioni del progetto si trovano nella sezione :guilabel:`Projects` della colonna di sinistra, sotto :guilabel:`Build` → :guilabel:`CMake`.
Su Linux, al posto delle librerie del sistema si possono usare le copie di :program:`RtAudio` e :program:`RtMidi` incluse nei sorgenti: attiva le opzioni ``POLYPHONE_LOCAL_RTAUDIO`` e ``POLYPHONE_LOCAL_RTMIDI``, poi fai clic su :guilabel:`Run CMake`.
Questo non è necessario con Windows e macOS, dove vengono sempre usate le versioni incluse.
Nella stessa sezione, seleziona la configurazione di compilazione (è preferibile :guilabel:`Release`).


### Compilare il progetto


Infine, fai clic sull'azione :guilabel:`Build Project "Polyphone"` nel menu :guilabel:`Build`. Durante la compilazione del progetto potresti vedere degli avvisi, ma non dovrebbero esserci errori.

Ora dovresti avere l'eseguibile nella sottocartella :file:`bin` della cartella di compilazione creata da Qt Creator (ad esempio :file:`build/Desktop_Qt_6_…-Release` nella cartella :file:`sources`):

* con Linux, il file :file:`polyphone`,
* con Windows, il file :file:`polyphone.exe`,
* con macOS, il bundle :file:`Polyphone.app`.


## Risoluzione dei problemi


Consulta questa [categoria](forum/support-bug-reports) se ti servono altre informazioni o un aiuto.


<p class="endpage">Un ringraziamento speciale a <a href="dashboard/820-tomotello">Tomotello</a> per questo articolo.</p>
