Utilisez ces instructions pour compiler Polyphone avec Qt Creator.
Il est supposé que vous avez déjà installé sur votre ordinateur Qt Creator avec les framework Qt, un compilateur et les dépendances nécessaires pour compiler Polyphone (voir les pages correspondantes dans la documentation).


## Prérequis


Les éléments suivants sont requis&nbsp;:

* un compilateur C++ (GCC, Clang, MinGW…),
* Qt Creator avec son framework (Qt 6, ou Qt 5),
* :program:`CMake` (version 3.16 ou ultérieure), qui peut être installé avec Qt Creator,
* les dépendances de Polyphone.

Les instructions pour les installer dépendent de votre système d'exploitation, voir les pages correspondantes dans la documentation technique pour cela.


## Compiler Polyphone


### Copier les sources


Téléchargez les sources de Polyphone <a href="software" target="_blank">ici</a> ou depuis <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


### Ouvrir et configurer le projet


Démarrez Qt Creator et ouvrez le fichier :file:`CMakeLists.txt` situé dans le répertoire :file:`sources`.


![Ouverture de Qt Creator](images/open-Qt-Creator.png "Ouverture de Qt Creator")


Vient ensuite la configuration du projet, qui consiste à choisir un kit, c'est-à-dire une version de Qt et un compilateur.


![Configuration du projet](images/configure-project.png "Configuration du projet")


Qt Creator exécute alors :program:`CMake`, qui vérifie que les dépendances sont trouvées.
Les messages sont affichés dans le panneau :guilabel:`General Messages`&nbsp;: si une bibliothèque est manquante, installez-la et relancez :program:`CMake` avec l'élément :guilabel:`Run CMake` du menu :guilabel:`Build`.

Les options du projet se trouvent dans la section :guilabel:`Projects` de la colonne de gauche, sous :guilabel:`Build` → :guilabel:`CMake`.
Sous Linux, les copies de :program:`RtAudio` et :program:`RtMidi` incluses dans les sources peuvent être utilisées à la place des bibliothèques du système&nbsp;: activez les options ``POLYPHONE_LOCAL_RTAUDIO`` et ``POLYPHONE_LOCAL_RTMIDI``, puis cliquez sur :guilabel:`Run CMake`.
Ce n'est pas nécessaire avec Windows et macOS, les versions embarquées étant toujours utilisées.
Dans la même section, sélectionnez la configuration de compilation (la configuration :guilabel:`Release` est préférée).


### Compiler le projet


Enfin, cliquez sur l'action :guilabel:`Build Project "Polyphone"` dans le menu :guilabel:`Build`. Pendant la compilation du projet, il est possible de voir des avertissements mais il ne devrait pas y avoir d'erreur.

À présent vous devriez trouver l'exécutable dans le sous-répertoire :file:`bin` du répertoire de compilation créé par Qt Creator (par exemple :file:`build/Desktop_Qt_6_…-Release` dans le répertoire :file:`sources`)&nbsp;:

* avec Linux, le fichier :file:`polyphone`,
* avec Windows, le fichier :file:`polyphone.exe`,
* avec macOS, le bundle :file:`Polyphone.app`.


## Dépannage


Voir cette [section](forum/support-bug-reports) du forum pour plus d'informations ou pour obtenir de l'aide.


<p class="endpage">Un grand merci à <a href="dashboard/820-tomotello">Tomotello</a> pour cet article.</p>