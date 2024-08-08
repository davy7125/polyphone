Utilisez ces instructions pour compiler Polyphone sur Windows avec la dernière version des sources disponibles sur <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>. Vous aurez ainsi le logiciel à son niveau de développement le plus avancé, incluant les dernières fonctionnalités mais peut-être aussi des instabilités.


## Prérequis


### Installation de Qt

Polyphone est écrit en C++ avec l'environnement de développement Qt. Pour installer Qt, rendez-vous sur cette <a href="https://www.qt.io/download-qt-installer-oss" target="_blank">page</a> pour télécharger l'installateur. Il vous faudra au préalable créer un compte.

Lancer ensuite l'installateur et sélectionnez les éléments suivants&nbsp;:

![Éléments à sélectionner](images/select-qt-components.png "Éléments à sélectionner")

Prenez les versions les plus à jour si ces choix ne sont pas exactement les mêmes.


### Installation du compilateur

Qt a besoin d'un compilateur pour compiler les sources de Polyphone. Il est nécessaire d'utiliser MinGW pour ces raisons&nbsp;:

* tout d'abord une raison pratique car les dépendances ci-dessous sont compatibles avec MinGW,
* mais également pour permettre un import / export correct des fichiers au format sfArk.

Téléchargez MinGW sur cette <a href="https://wiki.qt.io/MinGW" target="_blank">page</a> en prenant la version correspondant à ce qui a été sélectionné au paragraphe précédent.

Désarchivez l'ensemble dans le répertoire :file:`C:/mingw64` par exemple.

Dirigez-vous ensuite dans les configurations du système Windows pour ajouter le dossier :file:`C:/mingw64/bin` dans le Path&nbsp;:

* ouvrez le menu Windows,
* cherchez "variables d'environnement",
* ouvrez les propriétés systèmes et cliquez sur "Variables d'environnement",
* double-cliquez sur "Path" dans les variables système,
* et ajoutez :file:`C:/mingw64/bin` parmi les répertoires listés dans la variable "Path".

De cette manière, les exécutables de mingw64 pourront être appelés par Qt.


### Dépendances de Polyphone

Téléchargez les dépendances du projet disponibles [ici](files/lib_windows.zip). Ce sont des éléments supplémentaires utilisés par Polyphone et qui ne sont pas fournis par Qt.


## Compilation


### Sources

Tout d'abord, téléchargez les sources de Polyphone <a href="download" target="_blank">ici</a> ou depuis <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> et désarchivez l'ensemble sur votre bureau Windows par exemple.

Ensuite, désarchivez les dépendences supplémentaires de Polyphone dans un dossier :file:`lib_windows` qui doit se trouver à côté du dossier :file:`sources`.

Continuez maintenant avec ce tutoriel pour la compilation de Polyphone avec Qt Creator&nbsp;: [Compiler avec Qt Creator](development/using-qt-creator-to-build-polyphone.md). Aucune modification du fichier :file:`polyphone.pro` ne sera nécessaire.

### Lancer Polyphone sans Qt Creator

Placez-vous dans le répertoire :file:`lib_windows/64bits`. Si la compilation s'est bien déroulée, vous devriez y voir l'exécutable :file:`polyphone.exe`. Pour rendre possible son exécution de manière indépendante, il est nécessaire de compléter ce dossier. Exécutez la commande suivante dans un terminal (adaptez les chemins au préalable)&nbsp;:

```
C:\Qt\6.7.2\mingw_64\bin\windeployqt.exe "C:\Users\User\Desktop\polyphone\lib_windows\64bits\polyphone.exe"
```

Vous verrez apparaitre les bibliothèques Qt (:file:`Qt6Core` par exemple) et un double-clic sur :file:`polyphone.exe` permettra de lancer le logiciel.
Pour permettre le lancement de Polyphone sur un autre PC Windows, ces fichiers en provenance de :file:`C:\mingw64\bin` sont égalements nécessaires pour compléter le dossier&nbsp;:

* libgcc_s_seh-1.dll
* libssp-0.dll
* libstdc++-6.dll
* libwinpthread-1.dll


## Dépannage


Cette [section](forum/support-bug-reports/) dans le forum vous permettra d'obtenir de l'aide.