Utilisez ces instructions pour compiler Polyphone avec Qt Creator.
Il est supposé que vous avez déjà installé sur votre ordinateur Qt Creator avec les framework Qt, un compilateur et les dépendances nécessaires pour compiler Polyphone (voir les pages correspondantes dans la documentation).


## Prérequis


Les éléments suivants sont requis&nbsp;:

* un compilateur C++ (VC++, Clang, GCC, …),
* Qt Creator avec son framework,
* les dépendances de Polyphone.

Les instructions pour les installer dépendent de votre système d'exploitation, voir les pages correspondantes dans la documentation technique pour cela.


## Compiler Polyphone


### Copier les sources


Téléchargez les sources de Polyphone <a href="download" target="_blank">ici</a> ou depuis <a href="https://github.com/davy7125/polyphone" target="_blank">Github</a>.


### Ouvrir et configurer le projet


Au préalable, ouvrez le fichier :file:`polyphone.pro` et éditez le de manière à éventuellement utiliser les versions embarquées de :program:`rtmidi`, :program:`stk` et :program:`qcustomplot` si votre système d'exploitation ne les fournit pas.
Décommentez les lignes suivantes en enlevant le premier caractère ‘#’&nbsp;:

```
DEFINES += USE_LOCAL_RTMIDI
DEFINES += USE_LOCAL_STK
DEFINES += USE_LOCAL_QCUSTOMPLOT
```

Vous pouvez sauter cette étape si vous souhaitez utiliser les dépendances déjà installées sur votre ordinateur (assurez-vous juste de les avoir&nbsp;!).

Démarrez Qt Creator et ouvrez le fichier :file:`polyphone.pro`.


![Ouverture de Qt Creator](images/open-Qt-Creator.png "Ouverture de Qt Creator")


Vient ensuite la configuration du projet.


![Configuration du projet](images/configure-project.png "Configuration du projet")


### Utiliser qmake et compiler le projet


Allez dans le menu :guilabel:`Build` et choisissez l'élément :guilabel:`Run qmake`.


![Utilisation de qmake](images/run-qmake.png "Utilisation de qmake")


La commande :command:`qmake` se termine correctement lorsque la barre de progression en bas à droite de Qt Creator atteint 100% et reste verte.


![qmake s'est bien terminé](images/qmake-finished.png "qmake s'est bien terminé")


Dans la section :guilabel:`Project` dans la colonne de gauche, puis dans la section :guilabel:`Build&Run`, sélectionnez une configuration de compilation (:guilabel:`Release` est préféré).

Enfin, cliquez sur l'action :guilabel:`Build project Polyphone` dans le menu :guilabel:`Build`.


![Compilation du projet](images/build-project.png "Compilation du projet")


Pendant la compilation du projet il est possible de voir des avertissements mais normalement pas d'erreur.

À présent vous devriez trouver le fichier exécutable :file:`polyphone` dans le sous-dossier :file:`RELEASE` ou :file:`DEBUG` du répertoire source, selon le choix de la configuration de compilation dans QtCreator.


## Dépannage


Voir le [forum](forum/polyphone/support-bug-reports) pour plus d'information ou pour obtenir de l'aide.


<p class="endpage">Un grand merci à <a href="dashboard/820-tomotello">Tomotello</a> pour cet article.</p>