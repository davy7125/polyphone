Utilisez ces instructions pour compiler Polyphone avec Qt Creator.
Il est supposé que vous avez déjà installé sur votre ordinateur Qt Creator avec les framework Qt, un compilateur et les dépendances nécessaires pour compiler Polyphone (voir les pages correspondantes dans la documentation).


## Prérequis


Les éléments suivants sont requis&nbsp;:

* un compilateur C++ (VC++, Clang, GCC, …),
* Qt Creator avec son framework (Qt 6),
* les dépendances de Polyphone.

Les instructions pour les installer dépendent de votre système d'exploitation, voir les pages correspondantes dans la documentation technique pour cela.


## Compiler Polyphone


### Copier les sources


Téléchargez les sources de Polyphone <a href="software" target="_blank">ici</a> ou depuis <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


### Ouvrir et configurer le projet


Au préalable, ouvrez le fichier :file:`polyphone.pro` et éditez-le de manière à éventuellement utiliser les versions embarquées de :program:`rtmidi` et :program:`stk` si votre système d'exploitation ne les fournit pas (la version embarquée de :program:`rtaudio` est utilisée par défaut).
Décommentez les lignes suivantes en enlevant le premier caractère ‘#’&nbsp;:

```
DEFINES += USE_LOCAL_RTMIDI
DEFINES += USE_LOCAL_STK
```

Vous pouvez sauter cette étape si vous souhaitez utiliser les dépendances déjà installées sur votre ordinateur (assurez-vous juste de les avoir&nbsp;!).
Cette étape n'est pas nécessaire avec Windows et macOS, les versions embarquées étant toujours utilisées.

Démarrez Qt Creator et ouvrez le fichier :file:`polyphone.pro`.


![Ouverture de Qt Creator](images/open-Qt-Creator.png "Ouverture de Qt Creator")


Vient ensuite la configuration du projet.


![Configuration du projet](images/configure-project.png "Configuration du projet")


### Utiliser qmake et compiler le projet


Allez dans le menu :guilabel:`Build` et choisissez l'élément :guilabel:`Run qmake`.


![Utilisation de qmake](images/run-qmake.png "Utilisation de qmake")


La commande :command:`qmake` se termine correctement lorsque la barre de progression en bas à droite de Qt Creator atteint 100&nbsp;% et reste verte.


![qmake s'est bien terminé](images/qmake-finished.png "qmake s'est bien terminé")


Dans la section :guilabel:`Project` de la colonne de gauche, puis dans la section :guilabel:`Build&Run`, sélectionnez une configuration de compilation (la configuration :guilabel:`Release` est préférée).

Enfin, cliquez sur l'action :guilabel:`Build project Polyphone` dans le menu :guilabel:`Build`. Pendant la compilation du projet, il est possible de voir des avertissements mais il ne devrait pas y avoir d'erreur.

À présent vous devriez trouver l'exécutable dans le répertoire de compilation créé par Qt Creator (par exemple :file:`build-XXX` à côté des sources), selon le choix de la configuration de compilation dans Qt Creator&nbsp;:

* avec Linux, le fichier :file:`polyphone` se trouve dans le sous-répertoire :file:`bin`,
* avec Windows, le fichier :file:`polyphone.exe` se trouve dans le sous-répertoire :file:`release` ou :file:`debug`,
* avec macOS, le bundle :file:`Polyphone.app` se trouve dans le répertoire :file:`macos` situé à côté du répertoire :file:`sources`.


## Dépannage


Voir cette [section](forum/support-bug-reports) du forum pour plus d'informations ou pour obtenir de l'aide.


<p class="endpage">Un grand merci à <a href="dashboard/820-tomotello">Tomotello</a> pour cet article.</p>