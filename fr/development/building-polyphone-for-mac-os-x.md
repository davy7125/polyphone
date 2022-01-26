Utilisez ces instructions si vous ne pouvez pas installer Polyphone sur votre Mac OS X avec les installeurs disponibles dans la section «&nbsp;[Download](download)&nbsp;».


## Prérequis


Les éléments suivants seront nécessaires&nbsp;:

* Xcode (disponible dans l'Apple Store),
* <a href="https://www.qt.io/download-open-source" target="_blank">Qt Creator avec son framework</a>.

Vous aurez également besoin des sources de Polyphone, disponibles <a href="download" target="_blank">ici</a> ou sur <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>, ainsi que les dépendances téléchargeables [ici](downloads/lib_mac.zip).

Pour pouvoir ouvrir correctement un projet avec Qt Creator, il faut au préalable lui donner la permission de lire tous les fichiers (dans "Sécurité et confidentialité => Accès complet au disque => ajouter "Qt creator.app" qui se trouve dans son répertoire d'installation).

Si certaines des dépendances précédemment fournies ne sont pas à jour ou pas compatibles avec la version de Mac, comme PortAudio par exemple, il faudra aller chercher leurs sources sur internet et les recompiler en autorisant une liaison statique&nbsp;:
```
configure --enable-static --disable-shared --prefix=...
make
make install
```
Ensuite copier tous les .h (entêtes) et .a (bibliothèques statiques).


## Compilation


Décompressez l'archive des dépendances et placez le répertoire :file:`lib_mac` juste à côté du dossier :file:`sources`.

Ouvrez le fichier :file:`polyphone.pro` avec :program:`Qt Creator`.
Compiler le projet, un bundle :file:`polyphone.app` devrait apparaître dans le répertoire :file:`lib_mac`.

Si le chemin du SDK ne peut être résolu, essayez de modifier le fichier :file:`polyphone.pro` pour y faire apparaître ces variables (après avoir ajusté la version de Mac OSX)&nbsp;:

```
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
QMAKE_MAC_SDK = macosx10.11
```

## Compléter le bundle


Lancez la commande suivante pour intégrer les dépendances et le framework Qt dans le bundle (ajustez les chemins au prélable&nbsp;!)&nbsp;:

```
/Path/To/Qt/5.2.0/clang_64/bin/macdeployqt /Path/To/lib_mac/polyphone.app
```

Copiez the framework Jackmp dans :file:`lib_mac` et collez-le dans :file:`lib_mac/polyphone.app/Contents/Frameworks`.
Pour naviguer à l'intérieur du bundle, clic droit dessus et sélection de :guilabel:`View content` (un double-clic sur le bundle l'exécutera et ce n'est pas ce que nous voulons ici).

Enfin, lancez la commande suivante sur une seule ligne (ajustez les chemins au prélable&nbsp;!)&nbsp;:

```
install_name_tool -change /System/Library/Frameworks/Jackmp.framework/Versions/A/Jackmp @executable_path/../Frameworks/Jackmp.framework/Versions/A/Jackmp /Path/To/lib_mac/polyphone.app/Contents/MacOS/Polyphone
```

Vous pourvez ensuite exécuter le programme grâce à un double-clic sur le bundle ou le compresser en fichier .zip pour le partager.


## Dépannage


Voir ce [sujet](forum/support-bug-reports/8-success-build-polyphone-on-osx-10-11-6-qt-5-7) pour plus d'information ou pour obtenir de l'aide.
