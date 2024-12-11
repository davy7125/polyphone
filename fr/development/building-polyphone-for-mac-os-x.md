Utilisez ces instructions si vous ne pouvez pas installer Polyphone sur votre Mac avec les installeurs disponibles dans la section «&nbsp;[Download](download)&nbsp;».


## Prérequis


Les éléments suivants seront nécessaires&nbsp;:

* <a href="https://www.qt.io/download-open-source" target="_blank">Qt Creator avec son framework</a>,
* Xcode ou seulement les «&nbsp;Command Line Tools"&nbsp;» (la version à utiliser est indiquée sur le site de Qt - <a href="https://doc.qt.io/qt-6/supported-platforms.html" target="_blank">ici</a> par exemple).

Vous aurez également besoin des sources de Polyphone, disponibles <a href="download" target="_blank">ici</a> ou sur <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>, ainsi que les dépendances téléchargeables [ici](files/macos.zip).

Pour pouvoir ouvrir correctement un projet avec Qt Creator, il faut au préalable lui donner la permission de lire tous les fichiers (dans "Sécurité et confidentialité" => "Accès complet au disque" => ajouter "Qt creator.app" qui se trouve dans son répertoire d'installation).

Si certaines des dépendances précédemment fournies ne sont pas à jour ou pas compatibles avec la version de Mac, comme SSL par exemple, il faudra aller chercher leurs sources sur internet et les recompiler en autorisant une liaison statique&nbsp;:
```
configure --enable-static --disable-shared --prefix=...
make
make install
```
Ensuite copier tous les .h (entêtes) et .a (bibliothèques statiques).

Une autre manière pour récupérer des dépendances déjà compilées est d'utiliser <a href="https://brew.sh/" target="_blank">Homebrew</a>. Après l'installation de cet outil, il est possible de récupérer "libsndfile" ainsi que ses dépendances par cette commande&nbsp;:
```
arch -x86_64 brew install libsndfile
```
Les fichiers .h et .a se trouvent ensuite dans "/usr/local/Cellar".


## Compilation


Décompressez l'archive des dépendances et placez le répertoire :file:`macos` juste à côté du dossier :file:`sources`.

Ouvrez le fichier :file:`polyphone.pro` avec :program:`Qt Creator`.
Compiler le projet, un bundle :file:`polyphone.app` devrait apparaître dans le répertoire :file:`lib_mac`.

Si le chemin du SDK ne peut être résolu, essayez de modifier cette variable dans le fichier :file:`polyphone.pro` pour cibler la version de votre SDK (qui dépend de la version d'XCode installée)&nbsp;:

```
QMAKE_MAC_SDK = macosx10.15

```

Si le compilateur indique à présent que certaines fonctionnalités ne sont pas disponibles dans la cible MacOS spécifiée, vous pouvez augmenter la version de cette variable dans le fichier .pro&nbsp;:
```
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15
```
Cette valeur est à reporter dans le fichier "sources/contrib/mac/polyphone.plist" sous "LSMinimumSystemVersion".


## Compléter le bundle


Lancez la commande suivante pour intégrer les dépendances et le framework Qt dans le bundle (ajustez les chemins au prélable&nbsp;!)&nbsp;:

```
/Path/To/Qt/5.2.0/clang_64/bin/macdeployqt /Path/To/lib_mac/polyphone.app
```

Vous pourvez ensuite exécuter le programme grâce à un double-clic sur le bundle ou le compresser en fichier .zip ou .dmg pour le partager.


## Dépannage


Voir ce [sujet](forum/support-bug-reports/8-success-build-polyphone-on-osx-10-11-6-qt-5-7) pour plus d'information ou pour obtenir de l'aide.
