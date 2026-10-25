Polyphone è stato progettato per supportare le traduzioni, in modo da renderlo più accessibile.
Chiunque può contribuire: una traduzione può essere creata da zero, oppure una traduzione esistente può essere completata o migliorata.
Dalla versione 2.6, tutte le traduzioni incomplete sono state completate con l'aiuto di un'IA e meritano di essere riviste da madrelingua.


## Prerequisiti


### File di traduzione


Scarica il pacchetto di traduzione corrispondente alla lingua che vuoi tradurre da questa [pagina](software/translations).
Se la lingua non è ancora nell'elenco, usa il modulo di [contatto](contact) per richiederla (basta indicare la nuova lingua desiderata).
Riceverai rapidamente un nuovo pacchetto di traduzione.

I file di traduzione sono disponibili anche nel repository <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>, nella cartella :file:`sources` (un file :file:`polyphone_XX.ts` per lingua).


### Qt Linguist


L'unico software necessario per tradurre è :program:`Qt Linguist`: uno strumento per generare file di traduzione.
Questo software viene installato con Qt (vedi le [pagine sullo sviluppo](development/index.md)) e si trova anche <a href="https://github.com/thurask/Qt-Linguist/releases" target="_blank">qui</a> per gli utenti Windows o nei repository delle distribuzioni Linux (pacchetto :program:`linguist-qt6` su Debian e Ubuntu, :program:`qt6-linguist` su Fedora).


## Aprire il file di traduzione


Estrai il pacchetto di traduzione appena scaricato.
Troverai un file .ts :file:`polyphone_XX.ts`, dove :file:`XX` corrisponde alla tua lingua.
Apri :file:`polyphone_XX.ts` con :program:`Qt Linguist` e potrai iniziare la traduzione.


## Tradurre


Ogni voce di ogni contesto deve essere tradotta.
Basta esaminare gli elementi uno a uno e inserire la traduzione.
A seconda del tuo grado di sicurezza, si possono presentare tre casi.

* Se non sai come tradurre, non esitare a saltare l'elemento.
  Non è un problema: altre persone potranno completarlo.
* Pensi di avere la traduzione giusta ma non ne sei sicuro: scrivi la tua proposta e poi usa :kbd:`Ctrl`+:kbd:`J`.
  La voce non verrà convalidata, ma la traduzione verrà comunque usata nell'interfaccia tradotta.
  Segnala semplicemente a chi migliorerà la traduzione che non eri sicuro.
* Hai la traduzione giusta: scrivila e usa :kbd:`Ctrl`+:kbd:`Enter`.
  La voce verrà convalidata.

Una traduzione richiede attenzione (in un mondo in cui tutto è fatto per <a href="https://medium.com/thrive-global/how-technology-hijacks-peoples-minds-from-a-magician-and-google-s-design-ethicist-56d62ef5edf3" target="_blank">catturarla</a>).
Fai attenzione a:

* la punteggiatura,
* gli spazi iniziali e finali,
* le maiuscole e le minuscole,
* le forme singolari e plurali.

Un ultimo consiglio: **prenditi il tuo tempo!**
E non dimenticare di salvare il lavoro con :kbd:`Ctrl`+:kbd:`S`.


## Provare il proprio lavoro


Puoi verificare il tuo lavoro in qualsiasi momento.
In :program:`Qt Linguist` vai su :menuselection:`File --> Release As…` e crea un file .qm corrispondente alla traduzione che stai preparando.
Segui le istruzioni del pacchetto di traduzione per installarlo (file :file:`readme.txt`), poi apri Polyphone.
Vai nelle [impostazioni](manual/settings.md#doc_interface), seleziona la tua traduzione nell'elenco delle lingue e riavvia Polyphone.
Ora potrai rivedere la tua traduzione.

**Nota:** l'operazione :guilabel:`Release As…` potrebbe tradurre solo il testo privo di accenti. Se si verifica questo bug, apri un terminale e usa il comando seguente: :command:`lrelease polyphone_XX.ts` (adattalo al tuo file).
Un file di traduzione :file:`polyphone_XX.qm` può anche essere caricato da una cartella chiamata :file:`translations` (nella cartella corrente o nella cartella di configurazione di Polyphone), invece di essere incorporato nel software.


## Inviare la traduzione


Quando hai finito (o ti sei stancato!), invia il file .ts (non il .qm!) che stavi modificando a <contact@polyphone.io>.
Il pacchetto di traduzione di questa [pagina](software/translations) verrà aggiornato.

Se hai dimestichezza con GitHub, puoi anche proporre il tuo file .ts con una pull request sul <a href="https://github.com/davy7125/polyphone" target="_blank">repository</a> (vedi la guida per i contributori).
