Usa queste istruzioni per compilare Polyphone per Windows con l'ultima versione dei sorgenti su <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>. Avrai il software al suo livello di sviluppo più avanzato, con le funzionalità più recenti, ma forse anche con qualche instabilità.


## Prerequisiti


### Installazione di Qt

Polyphone è scritto in C++ con l'ambiente di sviluppo Qt (versione 6). Per installare Qt, vai su questa <a href="https://www.qt.io/download-qt-installer-oss" target="_blank">pagina</a> e scarica il programma di installazione. Prima dovrai creare un account.

Poi avvia il programma di installazione e seleziona i seguenti componenti:

![Componenti da selezionare](images/select-qt-components.png "Componenti da selezionare")

Scegli l'ultima versione disponibile se le opzioni non sono esattamente le stesse.


### Installazione del compilatore e delle dipendenze

Qt ha bisogno di un compilatore per compilare i sorgenti di Polyphone. È necessario usare MinGW per i seguenti motivi:

* innanzitutto per un motivo pratico, perché le dipendenze indicate sotto sono compatibili con MinGW,
* ma anche per consentire una corretta importazione / esportazione dei file in formato sfArk (è necessaria l'opzione di compilazione ``-ffloat-store``).

Il compilatore e le librerie usate da Polyphone si installano con <a href="https://www.msys2.org/" target="_blank">MSYS2</a>.
Installalo nella cartella predefinita :file:`C:/msys64` (il file di progetto :file:`polyphone.pro` cerca le librerie in :file:`C:/msys64/mingw64`), apri il terminale :program:`MSYS2 MINGW64` ed esegui questo comando:

```
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-zlib mingw-w64-x86_64-libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-openssl mingw-w64-x86_64-flac mingw-w64-x86_64-libsndfile
```

Poi vai nelle impostazioni di sistema di Windows per aggiungere la cartella :file:`C:/msys64/mingw64/bin` al Path:

* apri il menu di Windows,
* digita «Variabili d'ambiente»,
* apri le proprietà del sistema e fai clic su «Variabili d'ambiente»,
* fai doppio clic su «Path» nelle variabili di sistema,
* e aggiungi :file:`C:/msys64/mingw64/bin` alle cartelle elencate nella variabile «Path».

In questo modo Qt può trovare gli eseguibili e le librerie di MinGW.


## Compilazione


### Sorgenti

Inizia scaricando i sorgenti di Polyphone <a href="software" target="_blank">qui</a> o da <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> ed estrai i file, ad esempio, sul desktop di Windows.

### Compilare con Qt Creator

Prosegui ora con questo tutorial per compilare Polyphone con Qt Creator: [Compilare Polyphone con Qt Creator](development/using-qt-creator-to-build-polyphone.md). Non sarà necessario modificare il file :file:`polyphone.pro`.

### Compilare da riga di comando

Apri il terminale :program:`MSYS2 MINGW64`, aggiungi Qt al Path e compila il progetto in una nuova cartella (adatta prima i percorsi):

```
export PATH="/c/Qt/6.6.2/mingw_64/bin:$PATH"
mkdir build
cd build
qmake ../sources/polyphone.pro CONFIG+=release
mingw32-make -j4
```

Se la compilazione è andata a buon fine, dovresti trovare il file eseguibile :file:`polyphone.exe` nella cartella :file:`build/release`.


### Avviare Polyphone

Perché l'eseguibile possa funzionare in modo autonomo, la sua cartella deve essere completata con le librerie di Qt.
Esegui il comando seguente in un terminale (adatta prima i percorsi):

```
C:\Qt\6.6.2\mingw_64\bin\windeployqt.exe "C:\Users\User\Desktop\build\release\polyphone.exe"
```

Compariranno le librerie di Qt (ad esempio :file:`Qt6Core`) e un doppio clic sul file :file:`polyphone.exe` avvierà il software.
Per poter avviare Polyphone su un altro computer Windows, servono anche questi file di :file:`C:\msys64\mingw64\bin` per completare la cartella:

* libgcc_s_seh-1.dll
* libstdc++-6.dll
* libwinpthread-1.dll
* libssp-0.dll
* libsndfile-1.dll
* libFLAC.dll
* libogg-0.dll
* libvorbis-0.dll
* libvorbisenc-2.dll
* libvorbisfile-3.dll
* libopus-0.dll
* libmp3lame-0.dll
* libmpg123-0.dll
* libcrypto-3-x64.dll
* libssl-3-x64.dll
* zlib1.dll


## Risoluzione dei problemi


Questa [categoria](forum/support-bug-reports) del forum ti darà aiuto, se necessario.
