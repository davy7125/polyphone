Usa queste istruzioni se non riesci a installare Polyphone sul tuo Mac con i programmi di installazione disponibili nella sezione «[Software](software)».


## Prerequisiti


Sono necessarie le seguenti applicazioni:

* <a href="https://www.qt.io/download-open-source" target="_blank">Qt Creator con il suo framework</a> (versione 6),
* Xcode o solo i «Command Line Tools» (la versione da usare è indicata sul sito di Qt, ad esempio <a href="https://doc.qt.io/qt-6/supported-platforms.html" target="_blank">qui</a>).

Ti servono anche i sorgenti di Polyphone, disponibili <a href="software" target="_blank">qui</a> o su <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.

Per aprire correttamente un progetto con Qt Creator, occorre prima concedergli l'autorizzazione a leggere tutti i file del computer (in «Privacy e sicurezza» => «Accesso completo al disco» => aggiungi «Qt Creator.app», che si trova nella sua cartella di installazione).

Lo strumento di compilazione :program:`CMake` e le librerie usate da Polyphone si installano facilmente con <a href="https://brew.sh/" target="_blank">Homebrew</a>. Dopo aver installato questo strumento, esegui questo comando:

```
brew install cmake pkgconf libsndfile libogg libvorbis flac lame mpg123 opus openssl
```

Il file :file:`CMakeLists.txt` della cartella :file:`sources` cerca gli header e le librerie in :file:`/opt/homebrew` (Apple Silicon), in :file:`/usr/local` (Intel) e nella cartella :file:`macos` situata accanto alla cartella :file:`sources`.
È anche possibile estrarre l'archivio di librerie statiche disponibile [qui](files/macos.zip) per ottenere la cartella :file:`macos`, ma il suo contenuto potrebbe non essere aggiornato.

Se alcune librerie non sono aggiornate o non sono compatibili con la versione del tuo Mac, come ad esempio SSL, sarà necessario cercarne i sorgenti su internet e ricompilarle abilitando il collegamento statico:

```
configure --enable-static --disable-shared --prefix=...
make
make install
```

Poi copia tutti i file .h (header) in :file:`macos/include` e tutti i file .a (librerie statiche) in :file:`macos/lib`.


## Compilazione


Apri il file :file:`CMakeLists.txt` della cartella :file:`sources` con :program:`Qt Creator` (vedi [Compilare Polyphone con Qt Creator](development/using-qt-creator-to-build-polyphone.md)) e compila il progetto.
Nella sottocartella :file:`bin` della cartella di compilazione dovrebbe comparire un bundle :file:`Polyphone.app`.

La compilazione può essere eseguita anche da riga di comando, nella radice del progetto (la cartella che contiene :file:`sources`), indicando il percorso di Qt (prima adattalo!):

```
cmake -S sources -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/Path/To/Qt/6.7.3/macos
cmake --build build --parallel 4
```

Il bundle :file:`Polyphone.app` si trova allora nella cartella :file:`build/bin`.

Se il compilatore segnala che alcune funzionalità non sono disponibili con la versione di macOS di destinazione specificata, puoi aumentare la versione minima di macOS aggiungendo questa opzione al primo comando (il valore predefinito è 12.0):

```
-DCMAKE_OSX_DEPLOYMENT_TARGET=13.0
```

Questo valore va scritto anche nel file :file:`packaging/mac/polyphone.plist`, sotto «LSMinimumSystemVersion».


## Completare il bundle


Esegui il comando seguente per includere librerie e framework nel bundle (prima adatta il percorso!):

```
/Path/To/Qt/6.7.3/macos/bin/macdeployqt /Path/To/build/bin/Polyphone.app
```

Su un Mac con processore Apple Silicon, il bundle deve anche essere firmato (anche con una firma ad hoc) per poter essere eseguito:

```
xattr -cr /Path/To/build/bin/Polyphone.app
codesign --force --deep --sign - /Path/To/build/bin/Polyphone.app
```

A questo punto puoi eseguire il programma o comprimerlo in un file .zip o .dmg per condividerlo.


## Risoluzione dei problemi


Consulta questa [discussione](forum/support-bug-reports/8-success-build-polyphone-on-osx-10-11-6-qt-5-7) se ti servono altre informazioni o un aiuto.
